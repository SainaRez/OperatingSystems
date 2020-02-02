#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


// This function takes the start time and rusage as inputs and returns
//the time elapsed and page faults

void get_statistics(struct rusage ruse_start, struct timeval start_time) {
	// Measure pagefaults:
	struct rusage rusage_end;
	getrusage(RUSAGE_CHILDREN, &rusage_end);
	long major_faults = rusage_end.ru_majflt - ruse_start.ru_majflt;
	long minor_faults = rusage_end.ru_minflt - ruse_start.ru_minflt;

	// measure endtime
	struct timeval end_timeval;
	gettimeofday(&end_timeval, NULL);

	// diff time
	struct timeval delta_timeval;
	timersub(&end_timeval, &start_time, &delta_timeval);

	//print statistics:
	printf("\n");
	printf("-- Statistics ---\n");
	printf("Time elapsed: %ld.%06ld seconds\n", delta_timeval.tv_sec, delta_timeval.tv_usec); // TODO change to milliseconds
	printf("Page Faults: %ld\n", major_faults);
	printf("Page Faults (reclaimed): %ld\n", minor_faults);
	printf("-- End of Statistics --\n");
	printf("\n");

	return;
}


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
	// TODO max path length is 256 chars?
	char curr_path[100]; 

	if(strcmp(argv[0], "ccd") == 0) {
		if(chdir(argv[1]) == -1) {
			err = errno;
			printf("chdir %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			wait(NULL);
			printf("Changed to directory: %s\n", getcwd(curr_path, 100));
		}
	}
	else if(strcmp(argv[0], "cpwd") == 0) {
		char buffer[255];
		long size;
		if(getcwd(buffer, sizeof(buffer)) == NULL) {
			err = errno;
			printf("getcwd %s failed with %s\n", argv[0], strerror(err));
		}
		else {
			wait(NULL);
			printf("Current directory: %s\n", buffer);
		}
	}
	else {
		int pid = fork();
		if (pid == 0) {
			if (execvp(argv[0], argv) == -1) { // third arg is char *const envp[]
			err = errno;
			printf("execvp %s failed with %s\n", argv[0], strerror(err));
			exit(err);
			}
		}			
		else if (pid > 0) { // parent process
		wait(NULL);
		get_statistics(rusage_start, start_timeval);
		}
		else {
			printf("Fork Failed!");
		}
	}
	return;
}


void execute_boring_commander() {
	char* argv1[] = {"whoami", NULL};
	execute_command(1, argv1);
	char* argv2[] = {"last", NULL};
	execute_command(1, argv2);
	char* argv3[] = {"ls", "-al", "/home", NULL};
	execute_command(3, argv3);
	char* argv4[] = {"ccd", "/", NULL};
	execute_command(2, argv4);
};

void loop_repl() {
	
	FILE *file; 
	file = fopen("custom.txt", "r");
	char cmd[129];
	int line_count = 0;
	
	while((fgets(cmd, 129, file) != NULL) && (line_count < 32)) {
		
		//remove new line char 
		cmd[strcspn(cmd, "\n")] = 0;
        
		char* token = strtok(cmd, " ");
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

		// Execute the  command
		execute_command(counter, argv);
	
		printf("\n");
		line_count++;
 	}
	return;
}


int main() {
	loop_repl();
	return 0;
}


