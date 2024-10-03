#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/wait.h>

int main()
{
	int p[2]; //pipe
	int pid;
	int status;
	
	// Generates pipe
	if(pipe(p) < 0)
	{
		perror("Pipe Error");
		exit(1);
	}

	// Generates child process
	pid = fork();
	if(pid < 0)
	{
		perror("Problem forking");
		exit(1);
	}

	// Child process
	if(pid == 0)
	{
		close(p[1]); // Close the write pipe
		char FileName[20] = {0}; // To store the file name

		printf("Child Process: My pid = %d. My parent pid = %d.\n", getpid(), getppid());

		read(p[0], FileName, 11); // Gets file name from parent process
		execl("./consumer", "consumer", FileName, NULL); // Executes consumer
                exit(0);	
	}
	else
	{	
		close(p[0]); // Close the read pipe
		char FileName[] = "numbers.txt";
		int fd;

		printf("Parent Process: My pid = %d. I created child pid = %d.\n", getpid(),pid);

		// Creates numbers.txt
		fd = open("numbers.txt", O_WRONLY|O_CREAT, 0777);
        	if(fd == -1)
        	{
                	perror("Producer File Error");
                	exit(1);
        	}
        	printf("File numbers.txt fd is:%d\n", fd);

		// Generates 20 integers into file
		char s[sizeof(int)] = {0};
                for(int i = 0; i < 20; i++)
                {
                       sprintf(s, "%d", i);
                      write(fd, s, sizeof(int));
                }

		write(p[1], FileName, sizeof(FileName)); // Pass the name of file to child
		wait(&status);
	}
	return 0;
}

