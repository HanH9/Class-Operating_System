#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
	int p[2];
	int pid;
	int status;


	if(pipe(p) < 0)
	{
		perror("Fatal Error");
		exit(1);
	}

	pid = fork();
	if(pid < 0)
	{
		perror("Problem forking");
		exit(1);
	}

	if(pid > 0)
	{
		FILE* fp;
		close(p[0]);
		fp = fopen("numbers.txt", "w");
        	if(fp == -1)
        	{
                	perror("File Error");
                	exit(1);
        	}
        	printf("File numbers.txt fd is:%d\n", fp);


		for (int i = 0; i <20; i++) 
		{
			fprintf(fp, "%d ", rand() % 100); // 在文件中写入随机数
		}

		write(p[1], fp, sizeof(fp);

		wait(&status);	
	}
	else
	{
		FILE* fp;
		close(p[1]);
		read(p[0], fp, sizeof(fp));

		exit(1);
	}

}
