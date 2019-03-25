#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

// File entry filter function for use in scandir in main() that takes
// a pointer to a dirent struct and determines if entry->d_name is
// a string of digits, which represents a pid, and if so, returns 1,
// and returns 0 in all other cases.
int isdigit_filter(const struct dirent *entry)
{

	// Check if string entry->d_name is a string of digits
	int index = 0;
	int isdigit = 1;

	while (index < strlen(entry->d_name)) {
		// If this if statement is ever entered,
		// entry->d_name is not a string of digits
		if (!isdigit(entry->d_name[index]))
			isdigit = 0;
		index = index + 1;
	}

	// Return 1 if string of digits
	if (isdigit == 1) {
		return 1;
		// Return 0 if not
	} else if (isdigit == 0) {
		return 0;
	}
	return 0;
}

// This function checks if the maps file at the location "path"
// contains any memory with the "wx" permissions set by scanning
// the file for one of the appropriate strings that represent this.
// returns 1 if wx permissions are found, 0 in all other cases.
int string_search(char *path)
{

	FILE *mapsfile = fopen(path, "r");
	// Allocate memory for where we'll store a read line from the maps file
	char *line = (char *)malloc(300 * sizeof(char));
	// Make a copy pointer to do operations on so that we can free(line)
	// properly at the end of this function
	char *linecopy = line;

	linecopy[0] = '\0';
	// fopen will have returned null if it was unable to read the mapsfile
	if (mapsfile == NULL) {
		free(line);
		return 0;
	}
	while (fgets(linecopy, 35, mapsfile)) {
		if ((strstr(linecopy, " rwxp ") != NULL) ||
		    (strstr(linecopy, " rwx- ") != NULL) ||
		    (strstr(linecopy, " -wxp ") != NULL) ||
		    (strstr(linecopy, " -wx- ") != NULL)) {
			fclose(mapsfile);
			free(line);
			return 1;
		}
	}
	fclose(mapsfile);
	free(line);

	return 0;
}

// Checks if a directory entry for a directory in /proc/ that represents
// a pid contains "wx" permissions in its maps file by calling string_search
// and returns 1 if the "wx" permission is found, 0 otherwise.
int wx_filter(const struct dirent *entry)
{

	// Check if "rwxp","rwx-" "-wxp" or "-wx-" is to be found in the file
	// "/proc/[entry->d_name]/maps"
	char *path = calloc(30, sizeof(char));
	char *pathcopy = path;

	strcpy(pathcopy, "/proc/");
	strcat(pathcopy, entry->d_name);
	strcat(pathcopy, "/maps");

	// pathcopy now = "/proc/[pid]/maps"
	int has_wx_perms = string_search(pathcopy);

	if (has_wx_perms == 1) {
		free(path);
		return 1;
	}

	free(path);
	return 0;
}

int main(int argc, char **argv)
{

	// PARSE COMMAND LINE ARGUMENTS, IF ANY:
	int kflag = 0;

	if (argc > 2) {
		fprintf(stderr, "Error; too many arguments given to wx_scan\n");
		exit(-1);
	} else if (argc == 2) {
		if (strcmp("-k", argv[1]) == 0) {
			kflag = 1;
		} else {
			fprintf(stderr,
				"Error; invalid flag passed to wx_scan\n");
		}
	}
	//GET RELEVANT INFO FROM PROC
	struct dirent **entryarray;

	int numentries;
	int has_wx_perms;
	pid_t pid;

	numentries = scandir("/proc/", &entryarray, isdigit_filter, alphasort);
	if (numentries < 0) {
		fprintf(stderr, "ERROR CALLING SCANDIR\n");
		exit(-1);
	} else {
		// Subtract 1 to account for null terminator from numentries
		// to get the number of elements within entryarray, so that we
		// can iterate through the elements within entryarray
		numentries = numentries - 1;
		// Print newline at the beginning of output to match
		// the output on the assignment spec
		printf("\n");
		while (numentries >= 0) {
			has_wx_perms = wx_filter(entryarray[numentries]);
			if (has_wx_perms == 1) {

				// Build a string which is the path to the
				// status file, so we can extract the executable
				// name from it
				char *statusfilepath =
				    calloc(1024, sizeof(char));
				char *line = calloc(1024, sizeof(char));
				char *linecopy = line;

				strcpy(statusfilepath, "/proc/");
				strcat(statusfilepath,
				       entryarray[numentries]->d_name);
				strcat(statusfilepath, "/status");

				// Open status file and extract the executable
				// name from the first line
				FILE *statusfile = fopen(statusfilepath, "r");

				fgets(linecopy, 1000, statusfile);
				linecopy = strtok(linecopy, ":");
				linecopy = strtok(NULL, "\t");
				// (Now linecopy is executable name)

				fclose(statusfile);
				free(statusfilepath);

				// Regular mode (non-kill mode)
				if (kflag == 0) {
					printf("%s %s\n",
					       entryarray[numentries]->d_name,
					       linecopy);
					free(line);

					// Kill mode (k flag passed)
				} else if (kflag == 1) {
					printf("%s %s",
					       entryarray[numentries]->d_name,
					       linecopy);
					printf("Killing %s\n",
					       entryarray[numentries]->d_name);
					pid =
					    atoi(entryarray
						 [numentries]->d_name);
					// kill process
					kill(pid, SIGKILL);
					free(line);
				}
			}
			free(entryarray[numentries]);
			numentries = numentries - 1;
		}
		free(entryarray);
	}
	return 0;
}
