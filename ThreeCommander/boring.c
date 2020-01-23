#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    
    int pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
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
    
    pid = fork();
    if (pid == 0) {
        printf("########################\n");
        printf("Running Command: last\n");
        printf("-- Statistics --\n");
        char *myargs[2];
        myargs[0] = strdup("last");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
        
    }
    else if (pid > 0) {
        int rc_wait = wait(NULL);
    }
    else {
        printf("oh no you failed");
    }
    printf("-- End of Statistics --\n");
    
    
    pid = fork();
    if (pid == 0) {
        printf("########################\n");
        printf("Running Command: ls -al /home\n");
        printf("-- Statistics --\n");
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
        printf("oh no you failed");
    }
    printf("-- End of Statistics --\n");
    

    return 0;

}