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
void execute_command(int argc, char *const argv[]) {

	pid_t w;
	int status;

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
	char curr_path[100]; 

	if(strcmp(argv[0], "ccd") == 0) {
		printf("Directory before calling ccd: %s\n", getcwd(curr_path, 100));
		if(chdir(argv[1]) == -1) {
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("chdir %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			printf("Directory after calling ccd: %s\n", getcwd(curr_path, 100));
		}
	}
	else if(strcmp(argv[0], "cpwd") == 0) {
		char* buffer;
		long size;
		if(getcwd(buffer, (size_t)size) == -1) {
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("getcwd %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			printf("The output of cpwd: %s\n", buffer);
			//printf("getwd just ran\n");
		}
	}
	else {
		int pid = fork();
		if (pid == 0) {
			if (execvp(argv[0], argv) == -1) { // third arg is char *const envp[]
			err = errno;
			// TODO/SUGGESTION add args to logging
			printf("execvp %s failed with %s\n", argv[0], strerror(err));
			exit(err);
			}
		}			
		else if (pid > 0) { // parent process
		wait(NULL);

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

		//print statistics:
		printf("\n");
		printf("-- Statistics ---\n");
		printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); // TODO change to milliseconds
		printf("Page Faults: %ld\n", major_faults);
		printf("Page Faults (reclaimed): %ld\n", minor_faults);
		printf("-- End of Statistics --\n");
		printf("\n");
		}
		else {
			printf("Fork Failed!");
		}
	}
	return;
}


void execute_boring_commander() {
	char* argv1[] = {"whoami", NULL};
	execute_command(1, argv1); // TODO, this can't be NULL! (Unix expects it argv[0] to be the name of the program or something weird. See https://stackoverflow.com/questions/36673765/why-can-the-execve-system-call-run-bin-sh-without-any-argv-arguments-but-not
	char* argv2[] = {"last", NULL};
	execute_command(1, argv2); // TODO something else is bugging out here
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command(3, argv3);
	char* argv4[] = {"ccd", "/", NULL};
	execute_command(2, argv4);
};

/**
 * Loops the Read:Eval:Print:Loop
 */
void loop_repl() {
	
	FILE *file; 
	file = fopen("custom.txt", "r");
	char cmd[129];
	int line_count = 0;
	
	while((fgets(cmd, 129, file) != NULL) && (line_count < 32)) {
		
		//remove new line char 
		cmd[strcspn(cmd, "\n")] = 0;
        
		char* token = strtok(cmd, " ");
		printf ("#############################\n");
		printf("\n");
		char* argv[32];
		int counter = 0;

		while(token != NULL) {
			argv[counter] = token;
			token = strtok(NULL, " ");
 			counter++;
		}

		//Add NULL to the end of argv which holds the tokens (command)
		argv[counter] = NULL;

		//printing the argv elements (the command that id being passed for execution)
		int i = 0;
		while(i <= counter) {
			printf("printing the argv array: %s \n", argv[i]);
			i++;
		}

		// Execute the  command
		execute_command(counter, argv);
				
		//printf("Line Number: %int\n", line_count);
		printf("\n");
		line_count++;
 	}
	return;
}

		// char delim = " "; 
		// char* token = strtok(cmd, delim);
		// char* argv[32];
		// argv[0] = token;
		// int counter = 1;

		// while(token != NULL) {
		// 	argv[counter] = strtok(cmd, delim);
		// 	counter++;
		// }
		
		// for(int i = 0; i <= sizeof(argv); i++) {
		// 	printf("%s \n", argv[i]);
		// }
		



		// TODO, use the strtok function to parse the string in an array of char* argv[]. (as per hw guidelines)

		// "Moving where the null terminator is by one to avoid the new line problem"


		//execute_command(cmd, 1, NULL); // TODO, null should be argv[], 1 should be argc



int main() {
	//execute_boring_commander();
	loop_repl();
	return 0;
}


