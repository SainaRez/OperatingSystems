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
 * Executes a command using execvp, and measures the time of the 
 * execution of the command, and the pagefaults of the execution of the command
 *
 * @param const char *file
 * 	The file name (i.e., "ls")
 * @param int argc
 * 	The number of elements in the array argv
 * @param char *const argv[]
 * 	The args. Kind of assumes argv[0] is == the file name
 */
void execute_command(const char *file, int argc, char *const argv[]) {
	// Prints the 'running command' log
	printf("Running command: ");
	int i; 
	for(i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");

	// Measure start of time
	struct timeval start_timeval;
	gettimeofday(&start_timeval, NULL);

	// Measure pagefaults at start:
	struct rusage rusage_start;
	getrusage(RUSAGE_CHILDREN, &rusage_start);

	int err;
	if (fork() == 0) {
		// child process
		if (execvp(file, argv) == -1) { // third arg is char *const envp[]
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("execvp %s failed with %s\n", file, strerror(err));
			exit(errno);
		} 
	} else { // parent process
		wait(NULL);
		printf("\n");

		// Measure pagefaults:
		struct rusage rusage_end;
		getrusage(RUSAGE_CHILDREN, &rusage_end);
		long major_faults = rusage_end.ru_majflt - rusage_start.ru_majflt;
		long minor_faults = rusage_end.ru_minflt - rusage_start.ru_minflt;

		// measure endtime
		struct timeval end_timeval;
		gettimeofday(&end_timeval, NULL);

		// diff time
		struct timeval delta_timeval;
		timersub(&end_timeval, &start_timeval, &delta_timeval);

		// print statistics:
		printf("-- Statistics ---\n");
		printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); // TODO change to milliseconds
		printf("Page Faults: %ld\n", major_faults);
		printf("Page Faults (reclaimed): %ld\n", minor_faults);
		printf("-- End of Statistics --\n");
		printf("\n");
	}
}


void execute_boring_commander() {
	char* argv1[] = {"whoami", NULL};
	execute_command("whoami", 1, argv1); // TODO, this can't be NULL! (Unix expects it argv[0] to be the name of the program or something weird. See https://stackoverflow.com/questions/36673765/why-can-the-execve-system-call-run-bin-sh-without-any-argv-arguments-but-not
	char* argv2[] = {"last", NULL};
	execute_command("last", 1, argv2); // TODO something else is bugging out here
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command("ls", 3, argv3);
};

/**
 * Loops the Read:Eval:Print:Loop
 */
void loop_repl() {
	char cmd[BUFSIZE];

	while(1) {
		// Input next command from fgets
		if (fgets(cmd, BUFSIZE, stdin) == NULL) {
			printf("EOF Detected, Exiting.../n");
			exit(0);
		}

		// TODO, use the strtok function to parse the string in an array of char* argv[]. (as per hw guidelines)

		// "Moving where the null terminator is by one to avoid the new line problem"
		cmd[strcspn(cmd, "\n")] = 0;
		printf("Received: %s\n", cmd); // TEMP logging

		execute_command(cmd, 1, NULL); // TODO, null should be argv[], 1 should be argc
	}

}


int main() {
	execute_boring_commander();
	// loop_repl();
}


