#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <malloc.h>

int main(void)
{
	// Get the size of a page
	int pagesize = sysconf(_SC_PAGE_SIZE);
	// Allocate the same amount of memory as the size of
	// a page at the appropriate address
	char *array = memalign(pagesize, pagesize);

	// set memory in array to write and execute
	if (mprotect(array, pagesize, PROT_EXEC | PROT_WRITE) != 0) {
		// if the above if statement was true mprotect failed
		free(array);
		printf("ERROR\n");
		exit(1);

	} else {
		// sleep to give enough time for wx_scan to
		// detect this process
		sleep(15);
		free(array);
		return 0;
	}
}
