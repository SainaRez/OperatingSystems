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
 * @param int argc
 * 	The number of elements in the array argv
 * @param char *const argv[]
 * 	The args. Assumes argv[0] is == the file name
 */
void execute_command(int argc, char *const argv[]) {
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
		if (execvp(argv[0], argv) == -1) { // third arg is char *const envp[]
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("execvp %s failed with %s\n", argv[0], strerror(err));
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
	execute_command(1, argv1);
	char* argv2[] = {"last", NULL};
	execute_command(1, argv2);
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command(3, argv3);
};

void process_text_file(const char *filename) {
	FILE *file; 
	file = fopen(filename, "r");
	char cmd[128];
	int file_line_number = 1; // 1 because linenumbers start at 1
	while(fgets(cmd, 128, file) != NULL) {
		cmd[strcspn(cmd, "\n")] = 0; // Remove newline suffix from command string
		printf("Prcessing CMD: %s, line %i\n", cmd, file_line_number);
		char* token = strtok(cmd, " ");
		char* argv[32]; 
		int arg_counter = 0;

		while(token != NULL) {
			argv[arg_counter] = token;
			token = strtok(NULL, " ");
 			arg_counter++;
		}
		
		file_line_number++;
 	}
}

char* parse_line_number_arguments(int argc, char* argv[], int num_array[]) {

    for (int i = 1; i < argc; i++) {
		num_array[i] = atoi(argv[i]);
	}
	num_array[argc] = '\0';
	return num_array; 
}

int main(int argc, char *argv[]) {
	//execute_boring_commander();
	
	// TODO
	int multi_threaded_line_numbers[32];
	for (int i = 1; i < argc; i++) {
		multi_threaded_line_numbers[i] = atoi(argv[i]);
	}
	multi_threaded_line_numbers[argc] = '\0';

	//process_text_file("multi.txt");
	return 0;
}


