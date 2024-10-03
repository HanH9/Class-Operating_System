#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
        pid_t pid;
        int statu = 0;
        pid = fork();
        char* argv[] = {"whoami", NULL};

        if(pid < 0)
        {
                perror("fork()");
        }
        else if(pid > 0)
        {
                printf("Parent: My pid = %d. My child has pid = %d\n", getpid(), pid);
                wait(&statu);
        }
        else
        {
                printf("Child: My pid = %d. My parent has pid = %d\n", getpid(), getppid());
                printf("Child: The current user is:");
		fflush(stdout);
                execv("/bin/whoami", argv);
        }
        return 0;
}

