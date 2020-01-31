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
	int pid = fork();
	if (pid == 0) {
		// child process
		if (execvp(file, argv) == -1) { // third arg is char *const envp[]
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("execvp %s failed with %s\n", file, strerror(err));
			exit(errno);
		} 
	} else if (pid > 0) { // parent process
		int status; 
		struct rusage usage; 
		// TODO temp stuff
		if (strcmp("sleep", file) == 0) {
			printf("Executing wait3\n");
			pid_t pid = wait3(&status, WNOHANG, &usage);
			printf("pid: %ld\n", (long) pid);
			printf("Finished!\n\n");
		}
		else {
			wait3(&status, 0, &usage);
		}
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
	else {
		printf("Fork failed");
	}
}


void checkwaitcommands() {
	int status; 
	struct rusage usage; 
	// TODO temp stuff
	printf("executing wait3 as general check:\n");
	pid_t pid = wait3(&status, WNOHANG, &usage);
	printf("PID: %ld\n", (long) pid);
	printf("status: %i\n", status);
	printf("Finished!\n");
}


void execute_boring_commander() {
	char* argv1[] = {"sleep", "2", NULL};
	execute_command("sleep", 2, argv1); 
	checkwaitcommands();
	char* argv2[] = {"last", NULL};
	execute_command("last", 1, argv2); 
	checkwaitcommands();
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command("ls", 3, argv3);
	checkwaitcommands();
	char* argv4[] = {"sleep", "1", NULL};
	execute_command("sleep", 2, argv1); 
	checkwaitcommands();
}


int main() {
	execute_boring_commander();
	return 0;
}


