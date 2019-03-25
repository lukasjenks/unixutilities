#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

pid_t childpid;			//to track pid of the child
int rc;				//to process fork and identify child processes

//signal handler to kill a child process upon ^C
static void sigint_handler(int signal, siginfo_t * t, void *arg)
{
	if (rc == 0) {		//if in the child fork loop,
		kill(childpid, SIGKILL);	//kill the child process
	}
}

int main(void)
{
	char *cmdline;		//for storing the cmdline input
	char *dirname;		//for storing the pwd path
	char *token = NULL;	//for splitting the cmdline
	pid_t pid;		//to track pid of the child
	char *args[100];	//to keep track of the cmdline input (splitted into arguments)
	char *history[1000];	//to hold past cmdline inputs for built-in history cmd
	int i = 0;		//for indexing through args when parsing cmdline
	int hindex = 0;		//for iterating through and populating the history array
	int status;		//for getting the exit status of the child process
	int x = 0;		//for indexing through the args array
	int y = 0;

	//C struct to define behaviour for sigint signals
	//(Changes default behaviour of SIGINT encounter to be
	// to call the sigint_handler function)
	struct sigaction sa_sigint;
	memset(&sa_sigint, 0, sizeof(sa_sigint));
	sigemptyset(&sa_sigint.sa_mask);
	sa_sigint.sa_sigaction = sigint_handler;
	sa_sigint.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &sa_sigint, NULL);

	while (1) {		//loops forever (until exit is called)

		//reset args array index counter and other array index counters
		i = 0;
		x = 0;
		y = 0;

		//allocate memory
		cmdline = calloc(1, 1024);
		dirname = calloc(1, 1024);

		//print prompt
		getcwd(dirname, 200);
		printf("%s $ ", dirname);

		//parse command line - (note: based on shell.c from lab 2)
		fgets(cmdline, 1024, stdin);
		history[hindex] = strdup(cmdline);
		hindex++;
		token = strtok(cmdline, "\n ");
		args[0] = token;
		while (token != NULL) {
			args[i++] = strdup(token);
			token = strtok(NULL, "\n ");
		}
		args[i] = NULL;

		// ENVIRONMENT VARIABLE SUBSTITUTION
		for (x = 0; x < i; x++) {
			if (args[x][0] == '$') {
				char *potentialEnvVar = args[x] + 1;
				char *var = getenv(potentialEnvVar);
				if (!var) {
					printf
					    ("Failed to access environment variable: %s\n",
					     potentialEnvVar);
				} else {
					args[x] = var;
				}
			}
		}

		//remove \t characters from args so that we split solely on whitespace regardless of tabs

		//EXIT BUILT IN
		if (strcmp(args[0], "exit") == 0) {
			free(cmdline);
			free(dirname);
			printf("\n%s\n", "Exiting shell...");
			return 0;
		}
		//CD BUILT IN
		else if (strcmp(args[0], "cd") == 0) {
			if (args[1] == NULL) {
				chdir(getenv("HOME"));
			} else {
				chdir(args[1]);
			}
		}
		//HISTORY BUILT IN
		else if (strcmp(args[0], "history") == 0) {
			for (int j = 0; j < hindex; j++) {
				printf("%s", history[j]);
			}
		}

		else {
			//fork processes
			rc = fork();
			if (rc < 0) {
				fprintf(stderr, "failed to fork process\n");
				exit(1);
				//child - execvp
			} else if (rc == 0) {
				childpid = getpid();
				//if args[0] os a built-in...
				execvp(args[0], args);
				//child should end here. If not, process has failed.
				exit(-1);
				//parent - wait for child
			} else {
				//wait for child process
				pid = waitpid(rc, &status, 0);
				if (pid == -1) {
					printf
					    ("\nProcess interrupted by sigint signal : exit status: %d\n",
					     pid);
				} else if (status == 0) {
					printf
					    ("([%d] terminated successfully)\n",
					     pid);
				} else {
					printf
					    ("([%d] terminated with non-zero exit status: %d)\n",
					     pid, WEXITSTATUS(status));
				}
			}
		}

		//free allocated memory
		free(cmdline);
		free(dirname);
	}
	return 0;
}
