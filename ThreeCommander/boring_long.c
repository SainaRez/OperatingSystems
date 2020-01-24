#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


void whoami() {

    int pid = fork();

    struct timeval whoami_start;
    gettimeofday(&whoami_start, NULL);
    suseconds_t total_start = ((whoami_start.tv_sec * 1000000) + whoami_start.tv_usec);

    if (pid < 0) {
        printf("oh no fork failed\n");
    }
    else if (pid == 0) {
        printf("Running Command: whoami\n");
        char *myargs[2];
        myargs[0] = strdup("whoami");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
    }
    else {
        int rc_wait = wait(NULL);
        //printf("I am parent process\n");
    }

    printf("\n");
    printf("-- Statistic --");
    struct rusage whoami_rusage;
    getrusage(RUSAGE_CHILDREN, &whoami_rusage);
    printf("Page Faults: %ld\n", whoami_rusage.ru_majflt);
    printf("Page Faults (reclaimed): %ld\n", whoami_rusage.ru_minflt);

    struct timeval whoami_end;
    gettimeofday(&whoami_end, NULL);
    suseconds_t total_end = ((whoami_end.tv_sec * 1000000) + whoami_end.tv_usec);

    suseconds_t whoami_diff = total_end - total_start;
    printf("Elapsed_time: %ld\n", whoami_diff);
    printf("-- End of Statistics --");
    printf("\n");

    return;
}

void last() {
    struct timeval whoami_start;
    gettimeofday(&whoami_start, NULL);
    suseconds_t total_start = ((whoami_start.tv_sec * 1000000) + whoami_start.tv_usec);
    printf("\n");
    printf("Running Command: last\n");
    
    int pid = fork();
    if (pid == 0) {
        
        char *myargs[2];
        myargs[0] = strdup("last");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
    }

    else if (pid > 0) {
        int rc_wait = wait(NULL);
    }
    else {
        printf("oh no fork failed");
    }
   
    printf("\n");
    printf("-- Statistics --");
    struct rusage whoami_rusage;
    getrusage(RUSAGE_CHILDREN, &whoami_rusage);
    printf("Page Faults: %ld\n", whoami_rusage.ru_majflt);
    printf("Page Faults (reclaimed): %ld\n", whoami_rusage.ru_minflt);

    struct timeval whoami_end;
    gettimeofday(&whoami_end, NULL);
    suseconds_t total_end = ((whoami_end.tv_sec * 1000000) + whoami_end.tv_usec);

    suseconds_t whoami_diff = total_end - total_start;
    printf("Elapsed time: %ld\n", whoami_diff);
    printf("-- End of Statistics --\n");
    printf("\n");

    return;
}

void ls() {
    int pid = fork();

    struct timeval whoami_start;
    gettimeofday(&whoami_start, NULL);
    suseconds_t total_start = ((whoami_start.tv_sec * 1000000) + whoami_start.tv_usec);
    
    if (pid == 0) {
    
        printf("Running Command: ls -al /home\n");
        char *myargs[4];
        myargs[0] = strdup("ls");
        myargs[1] = strdup("-al");
        myargs[2] = strdup("/home");
        myargs[3] = NULL;
        execvp(myargs[0], myargs);
    }
    else if (pid > 0) {
        int rc_wait = wait(NULL);
    }
    else {
        printf("oh no fork failed");
    }

    struct timeval tv;
    time_t curtime;
    gettimeofday(&tv, NULL);    

    printf("\n");
    printf("-- Statistics --\n");
    struct rusage whoami_rusage;
    getrusage(RUSAGE_CHILDREN, &whoami_rusage);
    printf("Page Faults: %ld\n", whoami_rusage.ru_majflt);
    printf("Page Faults (reclaimed): %ld\n", whoami_rusage.ru_minflt);

    struct timeval whoami_end;
    gettimeofday(&whoami_end, NULL);
    suseconds_t total_end = ((whoami_end.tv_sec * 1000000) + whoami_end.tv_usec);

    suseconds_t whoami_diff = total_end - total_start;
    printf("Elapsed time: %ld\n", whoami_diff);
    printf("-- End of Statistics --\n");
    printf("\n");

    return;
}

int main() {
    
    whoami();
    last();
    ls();
    
    return 0;
}