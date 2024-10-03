#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>

int main(){
    int fd[2];
    pid_t pid; 

    if(pipe(fd) == -1)
    {
    	perror("pipe error!");
	exit(1);
    }
    
    pid = fork();
    if(pid < 0)
    {
    	perror("fork error");
	exit(1);
    }
    
    if(pid == 0)
    {       
        close(fd[1]);
        dup2(fd[0], 0);
        printf("Child Process: My pid = %d. My parent pid = %d.\n",getpid(),getppid()); 
	fflush(stdin);
        execl("./consumer", "consumer", NULL);
    }
    else{               
        close(fd[0]);
        fd[1] = open("numbers.txt",O_WRONLY|O_CREAT,0777);
        printf("File numbers.txt fd is: %d\n",fd[1]);
        int buf[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
        write(fd[1],buf,sizeof(buf));
        printf("Parent Process: My pid = %d. I created child pid = %d.\n",getpid(),pid);
        close(fd[1]);
    }

    return 0;
}

