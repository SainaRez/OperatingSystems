#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFSIZE 32

/**
 * <BS>
 *
 * struct timeval {
                 time_t      tv_sec;     /* seconds 
                 suseconds_t tv_usec;    /* microseconds
 * 	                  };
 */
int main() {
	char cmd[BUFSIZE];
	int err;

	while(1) {
		// Input next command from fgets
		if (fgets(cmd, BUFSIZE, stdin) == NULL) {
			printf("EOF Detected, Exiting.../n");
			exit(0);
		}

		// "Moving where the null terminator is by one to avoid the new line problem"
		cmd[strcspn(cmd, "\n")] = 0;
		printf("Received: %s\n", cmd); // TEMP logging

		// Measure start of time
		struct timeval start_timeval;
		gettimeofday(&start_timeval, NULL);

		if (fork() == 0) {
			// child process
			if (execl(cmd, cmd, NULL) == -1) {
				err = errno;
				printf("exec %s failed with %s\n", cmd, strerror(err));
				exit(errno);
			} 
		} else { // parent process
			wait(NULL);

			// measure endtime
			struct timeval end_timeval;
			gettimeofday(&end_timeval, NULL);

			// diff time
			struct timeval delta_timeval;
			timersub(&end_timeval, &start_timeval, &delta_timeval);

			// Measure pagefaults:

			// print statistics:
			printf("Time elapsed: %ld.%06ld\n seconds", delta_timeval.tv_sec, delta_timeval.tv_usec); // TODO change to milliseconds
			printf("Page Faults: %ld", major_faults);
			printf("Page Faults (reclaimed): %ld", minor_faults);

		}	
	}
}

/*
 * note: don't use /bin/sh -c ls, that's wasteful
 */
