#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>

int main() {
    
    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
    }
    else if (pid == 0) {
        printf("I am child\n");
    }
    else {
        int rc_wait = wait(NULL);
        printf("I am parent\n");
    }
    
    return 0;

}