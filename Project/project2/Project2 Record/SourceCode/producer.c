/** 
 * @ Program: producer.c
 * @ The producer for project2
 * @ Author:H9
 * @ Date: 2023.06.04
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

sem_t read_sem; // Semaphore for read thread
sem_t change_sem; // Semaphore for for replace character thread
sem_t upper_sem; // Semaphore for toUpper thread
sem_t write_sem; // Semaphore for write thread

// Flag to remark if each thread finished
int readerFinished = 0; 
int charReplaceFinished = 0;
int toUpperFinished = 0;
int writerFinished = 0;

// Pointers for buffers
struct Buffer* read_buffer;
struct Buffer* change_buffer;
struct Buffer* upper_buffer;

/**
 * Description:struct for node
 * @ value Stored string
 * @ next Pointer to the next node
 */
struct Node
{
	char value[256];
	struct Node* next;
};

/**
 * Description: struct for buffer
 * @ haed The head node
 * @ tail The tail node
 */
struct Buffer
{
	int size;
	struct Node* head;
	struct Node* tail;
};

/**
 * Function name: tryPush
 * Description: Push the node into the buffer
 * Parament:
 * @ value String for the node
 * @ buffer Pointer to the buffer
 */
int tryPush(char* value, struct Buffer* buffer)
{
	if(buffer->size < 10)
	{
		struct Node* newChar = (struct Node*)malloc(sizeof(struct Node));
		strcpy(newChar->value, value);
		//printf("%s\n", newChar->value);
		newChar->next = NULL;

		if(buffer->head == NULL && buffer->tail == NULL)
		{
			buffer->head = newChar;
			buffer->tail = newChar;
		}
		else
		{
			struct Node* oldTail = buffer->tail;
			oldTail->next = newChar;
			buffer->tail = newChar;
		}
		buffer->size++;
		return 1;
	}

	return -1;
}

/**
 * Function name: pop
 * Description: pop the node from buffer
 * Parament:
 * @ buffer Pointer to the buffer
 */
char* pop(struct Buffer* buffer)
{
	if(buffer->size != 0)
	{
		struct Node* oldHead = buffer->head;
		struct Node* newHead = buffer->head->next;
		buffer->head = newHead;
		buffer->size--;
		return oldHead->value;
	}
	return NULL;
}

/**
 * Function name: error
 * Description: out put the error
 * Parament:
 * @ msg The information about error
 */
void error(const char *msg)
{
        perror(msg);
        exit(1);
}

void *readFile(void *filePath)
{
	FILE *fp = fopen(filePath, "r");
	char line[256]; // Buffer for line

	// Buffers will be created by the reader thread
	read_buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	change_buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	upper_buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	
	// Read context one line at a time from file
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		//printf("%d ", line[i]);
		// When buffer is full			
		while((tryPush(line, read_buffer) != 1))
		{	
			// Character replace thread grab from the queue
			sem_post(&change_sem);

			// Wait for character replace thread finish grabing
			sem_wait(&read_sem);
		}
	}

	// Finish reader thread
	//printf("read arrive first\n");
	readerFinished = 1;
	sem_post(&change_sem);
}

/**
 * Function name: charReplace
 * Description: Replace the specific character from the buffer of reader
 * Parament:
 * @ c The specific character
 */
void *charReplace(void* c)
{
	char line[256];// Buffer for the line
	char character = ((char*)c)[0]; // The character supplied by the client
	
	// Wait for permission or reader thread fill the buffer
	sem_wait(&change_sem);

	while(read_buffer->size > 0)
	{
		// Get the line from queue of read thread
		strcpy(line,pop(read_buffer));
		for(int i = 0; i < strlen(line); i++)
		{
			// Exchange space
			if(line[i] == ' ')
			{
				line[i] = character;
			}
		}
		
		// Put new line into the queue of character replace thread
		while((tryPush(line, change_buffer) != 1))
		{
			// Run toUpper thread when queue if full
			sem_post(&upper_sem);
			// Wait for toUpper thread finish grabing
			sem_wait(&change_sem);
		}

		// If the reader thread finished, just keep running character replace thread
		if(readerFinished != 1)
		{
			// Tell the reader thread that its queue is not full anymore and ready for running again
			sem_post(&read_sem);
			sem_wait(&change_sem);
		}	
	}

	// Finish character replace  thread
	charReplaceFinished = 1;
	sem_post(&upper_sem);
}

/**
 * Function name: toUpper
 * Description: Convert all lowcase in reader buffer to upper case
 */
void *toUpper()
{
	char line[256]; // Buffer for the line

	// Wait for permission or character thread fill the buffer
	sem_wait(&upper_sem);
	while(change_buffer->size > 0)
	{	

		// Get the line from queue of character replace thread
		strcpy(line, pop(change_buffer));
		for(int i = 0; i < strlen(line); i++)
		{
			// to upper
			if(isalpha(line[i]))
			{
				line[i] = toupper(line[i]);
			}
		}

		// Put new line into the queue of toUpper thread
		while((tryPush(line, upper_buffer) != 1))
		{
			// Run writer thread when queue if full
			sem_post(&write_sem);
			// Wait for writer thread finish grabing
			sem_wait(&upper_sem);
		}
		
		if(charReplaceFinished != 1)
		{
			// Tell the character replace thread that its  queue is not full anymore and ready for running again
			sem_post(&change_sem);
			sem_wait(&upper_sem);
		}
	}

	// Finish character replace  thread
	toUpperFinished = 1;
	sem_post(&write_sem);
}

/**
 * Function name: writer
 * Description: Write the contents in upper buffer to the output file
 */
void *writer()
{
	int outFile;
	outFile = open("output.txt", O_TRUNC | O_CREAT | O_RDWR, 0777);
	char outBuffer[256]; // Buffer for line

	// Wait for permission or toUpper thread fill the buffer
	sem_wait(&write_sem);
	while(upper_buffer->size > 0)
	{
		// Get the line from queue of character replace thread	
		strcpy(outBuffer, pop(upper_buffer));
		// Write the into output file
		write(outFile, outBuffer, strlen(outBuffer));
	
		// Tell the toUpper thread that its queue is not full anymore and ready for running again
		if(toUpperFinished != 1)
		{
			sem_post(&upper_sem);
			sem_wait(&write_sem);
		}
	}

	// Finish writer replace  thread
	close(outFile);
	writerFinished = 1;

	// Destroy buffers by the writer thread.
	free(read_buffer);
	free(change_buffer);
	free(upper_buffer);
}

int main(int argc, char *argv[]) 
{
	int sockfd, newsockfd, portno;
	socklen_t cli_len;
	pthread_t read_thread, change_thread, upper_thread, write_thread;
	char buffer[256];
	char filename[32];
	char fileLocation[256];
	char character[1];
	char currentLoc[256];
	char output[256] = "output.txt ";
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 2) {
		error("not enough arguments");
	}

	getcwd(currentLoc, 255);
	//printf("%s\n", currentLoc);
	
	fprintf(stdout, "Wait for client\n");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd,5);
	cli_len = sizeof(cli_addr);

	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
		if (newsockfd < 0)
			error("ERROR on accept");
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0)
			error("ERROR reading from socket");
		
		// Get file name
		int i = 0;
		while(i < strlen(buffer) && buffer[i] != ' ')
		{
			filename[i] = buffer[i];
			i++;
		}
		filename[i] = '\0';

		// Get position
		i++;
		int j = 0;
		while(i < strlen(buffer) && (buffer[i] != ' '))
		{
			fileLocation[j] = buffer[i];
			i++;
			j++;
		}
		fileLocation[j] = '\0';
		
		
		// Get character
		i++;
		while(i < strlen(buffer) && buffer[i] != ' ' && buffer[i] != '\n')
		{
			character[0] = buffer[i];
			i++;
		}

		//printf("Filename: %s\n", filename);
		//printf("File Location: %s\n", fileLocation);
		//printf("Char: %c\n", character[0]);
		
		sem_init(&read_sem, 0, 0);
        	sem_init(&change_sem, 0, 0);
        	sem_init(&upper_sem, 0, 0);
        	sem_init(&write_sem, 0, 0);

		// Thread Stuff
		pthread_create( &read_thread, NULL, readFile, (void*)fileLocation);
		pthread_create( &change_thread, NULL, charReplace, (void*)character);
		pthread_create( &upper_thread, NULL, toUpper, NULL);
		pthread_create( &write_thread, NULL, writer, NULL);
		
		pthread_join(read_thread, NULL);
		pthread_join(change_thread, NULL);
		pthread_join(upper_thread, NULL);
		pthread_join(write_thread, NULL);

		// Clear the record		
		bzero(filename,32);
		bzero(fileLocation,256);
		character[0] = '\0';
		
		//int a = 0;
		//int b = 0;
		//int c = 0;
		//int d = 0;
		//sem_getvalue(&r, &a);
                //sem_getvalue(&cR, &b);
                //sem_getvalue(&tU, &c);
                //sem_getvalue(&w, &d);
		//printf("%d %d %d %d\n", a, b, c, d);


		//Write to client
		strcat(output, currentLoc);
		strcat(output, "/output.txt");
		//printf("%s\n", output);
		n = write(newsockfd, output, 256);
		if (n < 0)
			error("ERROR writing to socket");
	}
	
	close(newsockfd);
	close(sockfd);
	return 0;
}
