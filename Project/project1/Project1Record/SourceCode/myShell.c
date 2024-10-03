/********************************************************************************
* @File name: myShell.c
* @Author: H9
* @Version: 1.3
* @Date: 2022-4.28
* @Description: Program for the project1, a simple shell
********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
# define max 4

/********************************************************
* Function name ：parsePipe
* Description  : Parses initial command according to pipe
* Parameter  ：
* @input Initial command
* @pipes Parsed commands accroding to pipe
* @numbers The number of pipes
**********************************************************/
void parsePipe(char* input, char* pipes[], int* numbers)
{
	// parses the commands accroding to pipe identifier
	char* ptr = strtok(input,"|");
    	int size = 0;
    	while(ptr != NULL)
    	{
		*numbers = size;
        	pipes[size++] = ptr;
        	ptr = strtok(NULL,"|");
    	}

}

/********************************************************
* Function name ：exec
* Description  : Parses and executes command
* Parameter  ：
* @initPipes Section command after pipe parsing
* @num The index about the section command
**********************************************************/
void exec(char* initPipes[], int num)
{
	char* argv[10] = {};
    	char* ptr = strtok(initPipes[num]," ");
    	int size = 0;

	// parses commands of thie section
    	while(ptr != NULL)
    	{
        	argv[size++] = ptr;
       		ptr = strtok(NULL," ");
    	}
    	argv[size] = NULL;

	// removes \' or \" from the command
	for(int i = 0; i < size; i++)
        {
                if((argv[i][0] == '\'') || (argv[i][0] == '\"'))
                {
                        int len = strlen(argv[i]) - 1;
                        char temp[1024] = {0};
                        for(int j = 0; j < len - 1; j++)
                        {
                                temp[j] = argv[i][j+1];
                        }
                        temp[len] = '\0';
                        argv[i] = temp;
                }
        }

	// checks if this section commands need to output to special file 
	if(size >= 3)
	{
		if(strcmp(argv[size - 2], ">") == 0)
		{
			int fd = open(argv[size-1], O_WRONLY|O_CREAT, 0777);
			if(fd < 0)
			{
				perror("Open file error:");
				exit(-1);
			}
		
			dup2(fd, 1); // redircts the standerd output to the file
	
			argv[size-1] = NULL;
			argv[size-2] = NULL;
			size -= 2;
		}
	}

	execvp(argv[0], argv); // executes this section commands after command parsing
}

/********************************************************
* Function name ：execPipes
* Description  : Executes commands that parsed by pipe
* Parameter  ：
* @initPipes Parsed commands accroding to pipe
* @pipeNum The number of pipes
**********************************************************/
void execPipes(char* initPipes[], int pipeNum) {
    	int pipes[pipeNum][2];
    	pid_t pid[pipeNum+1];
    	
	// Creates pipes
    	for (int i = 0; i < pipeNum; i++) 
	{
        	if(pipe(pipes[i]) < 0) 
		{
           		perror("Create pipes error:");
            		exit(-1);
        	}
    	}
    	

	// Creates pipeNum+1 processes
    	for (int i = 0; i < pipeNum+1; i++) 
	{
        	if ((pid[i] = fork()) == 0) 
		{
            		if (i == 0) 
			{
                		// First process
                		dup2(pipes[i][1], 1);
            		} 
			else if(i == pipeNum)
			{
                		// Last process
                		dup2(pipes[i - 1][0], 0);
            		} 
			else 
			{
                		// Middle process
                		dup2(pipes[i - 1][0], 0);
                		dup2(pipes[i][1], 1);
            		}

			// closes all of the descirptiors of pipes to save the resource
            		for (int i = 0; i < pipeNum; i++) 
			{
                		close(pipes[i][0]);
                		close(pipes[i][1]);
            		}

            		exec(initPipes, i); // execute the command
			perror("pipe execution wrong");
            		exit(-1);
        	}

    	}

    	// Closes all of the pipes
    	for (int i = 0; i < pipeNum; i++) 
	{
        	close(pipes[i][0]);
        	close(pipes[i][1]);
    	}
    	
	// Waits for all of the child process to avoid zombie process
    	for (int i = 0; i < pipeNum+1; i++) 
	{
        	waitpid(pid[i], NULL, 0);
    	}
}

int main()
{
	char input[1024] = {0}; // Stores the initial command
	int pipeNum = 0; // The number of pipe
	char* initPipes[max];//Stores commands after pipe parsing



	printf("MyShPrompt>");
	fflush(stdout);

	fgets(input, 1024, stdin);// Gets the initial command
	input[strlen(input)-1] = '\0';


	parsePipe(input, initPipes, &pipeNum); // Parses the initial command to pipes

	if(pipeNum > 3)
	{
		printf("To many pipes!\n"); // There are up to three pipes
		exit(-1);
	}
	else if(pipeNum == 0)
	{
		exec(initPipes, 0); // No pipe -- execute the command dircetly
	}	
	else
	{
		execPipes(initPipes, pipeNum);// Has pipe -- execute the command accroding to the order

	}

}

