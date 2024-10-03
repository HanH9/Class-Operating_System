/** 
 * @ Program: producer.c
 * @ Description: The consumer for project2
 * @ Author: H9
 * @ Date: 2023.06.04
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/**
 * Function name: error
 * Description: out put the error
 * Parament:
 * @ msg The information about error
 */
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
        char buffer[256]; // store output file name and output file location
        char filename[32]; // output file name
        char fileLocation[256]; // output file location
        char output[256]; // store the contents provided by producer

        // Check argument
	if (argc < 3) {
		error("arguments not enough\n");
	}

        // Initiate the socket
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
        
	// Get connection
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		error("NO such host!\n");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		error("ERROR connecting");

        // Input the request
        printf("Input the request: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
		error("ERROR writing to socket");

	// Get the output provided by producer
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0)
		error("ERROR reading from socket");
	else
	{
            	// Get the output file name
		int i = 0;
		while(i < strlen(buffer) && buffer[i] != ' ')
		{
			filename[i] = buffer[i];
			i++;
		}
		filename[i] = '\0';
		
                // Get the output file location
		i++;
		int j = 0;
		while(i < strlen(buffer) && (buffer[i] != ' '))
		{
			fileLocation[j] = buffer[i];
			i++;
			j++;
		}
		fileLocation[j] = '\0';

		//printf("Filename: %s\n", filename);
		//printf("File Location: %s\n", fileLocation);
	}
	
	//  Output the contents of the file provided by the produece
	FILE* fp = fopen(fileLocation, "r");	
	while(fgets(output, sizeof(output), fp) != NULL)
	{
		printf("%s", output);
	}
	
	close(sockfd);
	return 0;
}
