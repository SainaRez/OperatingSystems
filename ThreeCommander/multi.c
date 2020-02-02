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
	int pid;
} background_command;

background_command background_command_array[MAX_COMMANDS];

/**
 * Iterates through background_command_array and sets all background_id's to -1.
 */
void initialize_background_command_array() {
	for (int i = 0; i < MAX_COMMANDS; i++) {
		background_command_array[i].background_id = -1;
	}
}


/**
 * Lists all background processes currently active
 * 
 * Output looks as such:
 * -- Background Processes --
 *  [0] sleep 10
 *  [1] sleep 3
 */
void print_active_background_processes() {
	for (int i = 0; i < MAX_COMMANDS; i++) {
		background_command cmd = background_command_array[i];
		if (cmd.background_id != -1) {
			printf("[%i] ", cmd.background_id);
			printf("%s\n", cmd.command);
		}
	}
	printf("\n");
}


/**
 * Checks argv to see if the given command is one of 'ccd', 'cpwd', or 'cproclist', 
 * and if so runs the command and exits using exit(0)
 * 
 * @param int argc
 * 	The number of elements in the array argv
 * @param char *const argv[]
 * 	The args. Assumes argv[0] is == the file name
 */
void check_special_commands(int argc, char *const argv[]) {
	int err;                                                                                                                // TODO max path length is 256 chars?
	char curr_path[100]; 


	if (strcmp(argv[0], "ccd") == 0) {
		if(chdir(argv[1]) == -1) {
			err = errno;
			printf("chdir %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			wait(NULL);
			printf("Changed to directory: %s\n", getcwd(curr_path, 100));
		}     
		exit(0);
	}

	else if (strcmp(argv[0], "cpwd") == 0) {
		if(chdir(argv[1]) == -1) {
			err = errno;
			printf("chdir %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			wait(NULL);
			printf("Changed to directory: %s\n", getcwd(curr_path, 100));
		}     
		exit(0);
	}

	else if (strcmp(argv[0], "cproclist") == 0) {
		print_active_background_processes();
		exit(0);
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
		check_special_commands(argc, argv);
		// child process
		if (execvp(argv[0], argv) == -1) {
			err = errno;
			printf("execvp %s failed with %s\n", argv[0], strerror(err));
			exit(errno);
		} 
	} else if (pid > 0) { // parent process
		wait4(pid, NULL, 0, NULL);
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
	printf("Background: ID [%i]: ", background_id);
	for(i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n\n");

	int err;
	int pid = fork();
	if (pid == 0) { // Child process

		int pid2 = fork();

		// Measure start of time
		struct timeval start_timeval;
		gettimeofday(&start_timeval, NULL);

		// Measure pagefaults at start:
		struct rusage rusage_start;
		getrusage(RUSAGE_CHILDREN, &rusage_start);


		if (pid2 == 0) { // Grandchild process
			// run process
			check_special_commands(argc, argv);
			if (execvp(argv[0], argv) == -1) {
				err = errno;
				printf("execvp %s failed with %s\n", argv[0], strerror(err));
				exit(errno);
			} 
		} else if (pid2 > 0) { // Not the Grandchild process
			wait4(pid2, NULL, 0, NULL);

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


			// Print Job Complete Log
			printf("-- Job Complete [%i: ", background_id);
			for(i = 0; i < argc; i++)
				printf("%s ", argv[i]);
			printf("] --\n");
			printf("Process ID: %i\n\n", pid2);



			// print statistics:
			printf("-- Statistics ---\n");
			// TODO change to milliseconds
			printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); 
			printf("Page Faults: %ld\n", major_faults);
			printf("Page Faults (reclaimed): %ld\n", minor_faults);
			printf("-- End of Statistics --\n");
			printf("\n");
			exit(0);
		}
	} else if (pid > 0) { // parent process
		// TODO not sure if this logic is necessary anymore.
		background_command cmd = background_command_array[background_id];
		cmd.pid = pid;
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
 * Searches background_command_array for the command with has the given PID,
 * and returns the background_id of the first command found.
 *
 * Returns -1 if no PID is found.
 *
 * @param int pid
 * 	The process id, such as 1243, of a given background process.
 */
int get_command_of_pid(int pid) {
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (background_command_array[i].pid == pid) {
			return background_command_array[i].background_id;
		}
	}

	return -1;
}


/**
 * Marks any background process in the array, background_command_array, which has
 * the PID of wait_pid as being disabled. Does so by setting it's background_id to -1.
 */
void disable_background_process_by_pid(int wait_pid) {
	for (int i = 0; i < MAX_COMMANDS; i++) {
		if (background_command_array[i].pid == wait_pid) {
			background_command_array[i].background_id = -1;
		}
	}
}

/**
 * Waits for any child processes, and disables their active status if they have completed.
 *
 * Returns true if there are more processes to be waited for.
 */
bool wait_for_process() {
	int wait_pid = wait3(NULL, WNOHANG, NULL);
	if (wait_pid == 0) { return true; }
	else if (wait_pid > 0) {
		int background_id = get_command_of_pid(wait_pid);
		background_command cmd = background_command_array[background_id];
		disable_background_process_by_pid(wait_pid);
		return true;
	}

	return false;
}

int valueinarray(int val, int arr[], int argc) {
	int i;
	for(i = 0; i < argc; i++) {
		if(arr[i] == val)
			return 1;
	}
	return 0;
}


void process_text_file(const char *filename, int multi_threaded_line_numbers[], int argc) {
	FILE *file; 
	file = fopen(filename, "r");
	char cmd[128];
	int file_line_number = 1; // 1 because linenumbers start at 1
	int background_id_counter = 0;
	while(fgets(cmd, 128, file) != NULL) {
		cmd[strcspn(cmd, "\n")] = 0; // Remove newline suffix from command string
		printf("Processing CMD: %s, line %i\n", cmd, file_line_number);
		char cmd_copy[128]; // Create a copy of cmd before tokenizing it.
		strcpy(cmd_copy, cmd);
		char* token = strtok(cmd, " ");
		char* argv[128]; 
		int arg_counter = 0;

		while(token != NULL) {
			argv[arg_counter] = token;
			token = strtok(NULL, " ");
			arg_counter++;
		}
		argv[arg_counter] = NULL; // Make sure our argv is null terminated

		if (valueinarray(file_line_number, multi_threaded_line_numbers, argc)) {
			// Create a struct for the command and store it background_command_array
			background_command command;
			strcpy(command.command, cmd_copy);
			command.background_id = background_id_counter;
			background_command_array[background_id_counter] = command;

			// Execute the command
			execute_multi_command(arg_counter, argv, background_id_counter);
			background_id_counter++;
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


int main(int argc, char *argv[]) {
	// execute_boring_commander();
	initialize_background_command_array();

	// Acquire multi-threaded command line numbers
	int multi_threaded_line_numbers[MAX_COMMANDS];
	for (int i = 1; i < argc; i++) {
		multi_threaded_line_numbers[i] = atoi(argv[i]);
	}
	multi_threaded_line_numbers[argc] = '\0';

	process_text_file("multi.txt", multi_threaded_line_numbers, argc);
	return 0;
}


