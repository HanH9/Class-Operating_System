/**
 * @File name: server.c
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
#include <ctype.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Converts the content of a file to uppercase.
 * @param filename The name of the file to be converted.
 */
void changeToUpper(char *filename)
{
    // Opens the input file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        error("ERROR opening file");
    }

    // Creates the output file
    FILE *outfile = fopen("fileUpper.txt", "w");
    if (outfile == NULL)
    {
        error("ERROR creating output file");
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Converts each character in the line to uppercase and write to the output file
        for (int i = 0; line[i] != '\0'; i++)
        {
            fputc(toupper(line[i]), outfile);
        }
    }

    // Closes the files
    fclose(file);
    fclose(outfile);
}

/**
 * Counts the occurrences of a specific character in a file.
 * @param targetChar The character to be counted.
 * @param filename The name of the file to be searched.
 */
void countCharacter(char targetChar, char *filename)
{
    // Opens the input file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        error("ERROR opening file");
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Counts the occurrences of the target character in each line
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == targetChar)
            {
                count++;
            }
        }
    }

    // Closes the file
    fclose(file);

    // Creates the output file
    FILE *outfile = fopen("fileChar.txt", "w");
    if (outfile == NULL)
    {
        error("ERROR creating output file");
    }

    // Writes the count to the output file
    fprintf(outfile, "%d", count);
    fclose(outfile);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char request[256]; // The request
    char filename[256]; // The target file name
    char targetChar; // The target character


    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    fprintf(stdout, "Run client by providing host and port\n");

    // Creates a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Clears the server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    // Sets up the server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Binds the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Listens for client connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Accepts a client connection
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // Clears the buffer
    bzero(buffer, 256);

    // Reads the request from the client
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");

    // Parses the request from the buffer
    sscanf(buffer, "%s", request);

    if (strcmp(request, "toUpper") == 0)
    {
        sscanf(buffer, "%*s < %s >", filename);
        changeToUpper(filename);

        // Opens the output file
        FILE *fileUpper = fopen("fileUpper.txt", "r");
        if (fileUpper == NULL)
        {
            error("ERROR opening fileUpper.txt");
        }

        char line[256];
        while (fgets(line, sizeof(line), fileUpper) != NULL)
        {
            // Writes each line to the client
            n = write(newsockfd, line, strlen(line));
            if (n < 0)
                error("ERROR writing to socket");
        }

        // Closes the file
        fclose(fileUpper);
    }
    else if (strcmp(request, "count") == 0)
    {
        sscanf(buffer, "%*s < %c, %s >", &targetChar, filename);
        countCharacter(targetChar, filename);

        // Opens the output file
        FILE *fileChar = fopen("fileChar.txt", "r");
        if (fileChar == NULL)
        {
            error("ERROR opening fileChar.txt");
        }

        char line[256];
        while (fgets(line, sizeof(line), fileChar) != NULL)
        {
            // Writes each line to the client
            n = write(newsockfd, line, strlen(line));
            if (n < 0)
                error("ERROR writing to socket");
        }

        // Closes the file
        fclose(fileChar);
    }
    else
    {
        error("ERROR invalid request");
    }

    // Closes the client socket
    close(newsockfd);

    // Closes the server socket
    close(sockfd);

    return 0;
}

