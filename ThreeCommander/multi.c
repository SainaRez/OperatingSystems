#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>

#define MAX_COMMANDS 32

typedef struct {
	char command[128];
	int background_id;
} background_command;

background_command background_command_array[MAX_COMMANDS];

void initialize_background_command_array() {
	for (int i = 0; i < MAX_COMMANDS; i++) {
		background_command_array[i].background_id = -1;
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

/**
 * Lists all background processes currently active
 * 
 * Output looks as such:
 * -- Background Processes --
 *  [0] sleep 10
 *  [1] sleep 3
 */
void print_active_background_processes() {
	for (int i = 0; i < 32; i++) { // TODO define 32
		//if (background_command_array[i]) {
		printf("B_PID: %i", background_command_array[i].background_id);
	}
	printf("\n");
}

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

int valueinarray(int val, int arr[], int argc)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        if(arr[i] == val)
            return 1;
    }
    return 0;
}


typedef struct {
	char command[128];
	int background_id;
} background_command;

void process_text_file(const char *filename, int multi_threaded_line_numbers[], int argc) {
	FILE *file; 
	file = fopen(filename, "r");
	char cmd[128];
	int file_line_number = 1; // 1 because linenumbers start at 1
	int background_id_counter = 0;
	while(fgets(cmd, 128, file) != NULL) {
		cmd[strcspn(cmd, "\n")] = 0; // Remove newline suffix from command string
		printf("Processing CMD: %s, line %i\n", cmd, file_line_number);
		char* token = strtok(cmd, " ");
		char* argv[128]; 
		int arg_counter = 0;

		while(token != NULL) {
			argv[arg_counter] = token;
			token = strtok(NULL, " ");
			arg_counter++;
		}
		argv[arg_counter] = NULL; // Make sure our argv is null terminated

		// background_command background_command_array[32]; TODO delete

		// Branch based on multi_thread_line_numbers
		// if (multi_threaded_line_numbers.contains(file_line_number)) { // TODO temp
		if (valueinarray(file_line_number, multi_threaded_line_numbers, argc)) { // TODO temp

			execute_multi_command(arg_counter, argv, background_id_counter++);

			// Next, create a struct for the command and store it background_command_array
			background_command command;
			strcpy(command.command, cmd);
			command.background_id = background_id_counter;
			background_command_array[background_id_counter] = command;
		} else {
			execute_command(arg_counter, argv);
		}

		print_active_background_processes();
		wait_for_process();
		file_line_number++;
	}

	// Now just wait for all multi-threaded processes to finish
	while(wait_for_process()) {
		// pass
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
	// execute_boring_commander();
	initialize_background_command_array();
	// TODO
	int multi_threaded_line_numbers[32];
	for (int i = 1; i < argc; i++) {
		multi_threaded_line_numbers[i] = atoi(argv[i]);
	}
	multi_threaded_line_numbers[argc] = '\0';

	process_text_file("multi.txt", multi_threaded_line_numbers, argc-1);
	return 0;
}


