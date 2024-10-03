/**
 * @File name: client.c
 * @Author: H9
 * @Version: 1.1
 * @Date: 2022-5.18
 * @Description: Program for the Assignment5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256]; // Stores the request and the output

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }
    
    // Retrieves the port number
    portno = atoi(argv[2]);

    // Creates a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Gets the server's IP address using the hostname
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    // Clears the server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Sets up the server address structure
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connects to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Clears the buffer
    bzero(buffer, sizeof(buffer));

    // Inputs the request
    printf("Enter the request: ");
    fgets(buffer, sizeof(buffer) - 1, stdin);

    // Sends the request to the server
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    // Clears the buffer
    bzero(buffer, sizeof(buffer));

    // Reads the response from the server
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)
        error("ERROR reading from socket");

    // Print the response from the server
    printf("Server response:\n%s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}

