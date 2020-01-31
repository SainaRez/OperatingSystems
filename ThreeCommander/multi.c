#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


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
		// TODO change to milliseconds
		printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); 
		printf("Page Faults: %ld\n", major_faults);
		printf("Page Faults (reclaimed): %ld\n", minor_faults);
		printf("-- End of Statistics --\n");
		printf("\n");
	}
	else {
		printf("Fork failed");
	}
}

void execute_boring_commander() {
	char* argv1[] = {"whoami", NULL};
	execute_command("whoami", 1, argv1);
	char* argv2[] = {"last", NULL};
	execute_command("last", 1, argv2);
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command("ls", 3, argv3);
};

void execute_custom.txt() {
	FILE *file; 
	file = fopen("custom.txt", "r");
	char cmd[128];
	int loopnum = 0;
	while(fgets(cmd, 128, file) != NULL) {
		
		char* token = strtok(cmd, " ");
		char* argv[32]; 
		argv[0] = token;
		int counter = 1;

		while(token != NULL) {
			token = strtok(NULL, " ");
			argv[counter] = token;
 			counter++;
		}
		int i = 0;
		while(i < counter) {
			printf("printing the argv array: %s \n", argv[i]);
			i++;
		}
		execvp(argv[0], argv);
		
		
		printf("THE LOOP NUMBER: %int\n",  loopnum);
		loopnum++;
 	}
}

int main() {
	execute_boring_commander();
	loop_repl();
	return 0;
}


