#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>


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
		if (execvp(argv[0], argv) == -1) {
			err = errno;
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
/**
 * Executes a command using execvp, and measures the time of the 
 * execution of the command, and the pagefaults of the execution of the command
 *
 * @param int argc
 * 	The number of elements in the array argv
 * @param char *const argv[]
 * 	The args. Assumes argv[0] is == the file name
 * @param int background_id
 * 	The project-specific "background ID" of the argument.
 */
void execute_multi_command(int argc, char *const argv[], int background_id) {
	// Prints the 'running command' log
	printf("Running background command: ");
	int i; 
	for(i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");
	// TODO print "Background: ID [%i]: <command>\n\n", background_id

	int err;
	int pid = fork();
	if (pid == 0) { // Child process

		int pid2 = fork();

		// Measure start of time
		struct timeval start_timeval;
		gettimeofday(&start_timeval, NULL);

		if (pid2 == 0) { // Grandchild process
			// run process
			if (execvp(argv[0], argv) == -1) {
				err = errno;
				printf("execvp %s failed with %s\n", argv[0], strerror(err));
				exit(errno);
			} 
		} else if (pid2 > 0) { // Not the Grandchild process
			wait(NULL); // TODO maybe use wait0
			// measure endtime
			struct timeval end_timeval;
			gettimeofday(&end_timeval, NULL);

			// diff time
			struct timeval delta_timeval;
			timersub(&end_timeval, &start_timeval, &delta_timeval);

			// print time
			printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); 
			exit(0);
		}
	} else if (pid > 0) { // parent process
		// TODO
		printf("Returning from fork. There is now a child running with PID %i\n", 42);
		return;
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

bool wait_for_process() {
	int status = 0;
	struct rusage usage;
	int wait_pid = wait3(&status, WNOHANG, &usage);
	if (wait_pid == 0) { return true; }
	else if (wait_pid > 0) {
		printf("PID %i returned!\n", wait_pid);
	}

	return false;
}

typedef struct {
	char command[128];
	int background_id;
} background_command;

typedef struct node {
	struct background_command;
	struct node * next;
} background_list;

void process_text_file(const char *filename) {
	FILE *file; 
	file = fopen(filename, "r");
	char cmd[128];
	int file_line_number = 1; // 1 because linenumbers start at 1
	int background_id_counter = 0;
	while(fgets(cmd, 128, file) != NULL) {
		cmd[strcspn(cmd, "\n")] = 0; // Remove newline suffix from command string
		printf("Processing CMD: %s, line %i\n", cmd, file_line_number);
		char* token = strtok(cmd, " ");
		char* argv[32]; 
		int arg_counter = 0;

		while(token != NULL) {
			argv[arg_counter] = token;
			token = strtok(NULL, " ");
			arg_counter++;
		}
		argv[arg_counter] = NULL; // Make sure our argv is null terminated

		// Branch based on multi_thread_line_numbers
		if (file_line_number == 2) { // TODO temp
			execute_multi_command(arg_counter, argv, background_id_counter++);

			// keep track of background commands in a struct
		} else {
			execute_command(arg_counter, argv);
		}

		wait_for_process();
		file_line_number++;
	}

	// Now just wait for all multi-threaded processes to finish
	while(wait_for_process()) {
		// pass
	}
}

void parse_line_number_arguments() {
	// TODO
}

int main(int argc, char *argv[]) {
	// execute_boring_commander();

	// TODO
	int multi_threaded_line_numbers[2] = {1,3};
	parse_line_number_arguments();

	process_text_file("multi.txt");
	return 0;
}


