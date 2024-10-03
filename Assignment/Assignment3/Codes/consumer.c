#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int sum = 0;
	int fd;
	int numbers[20];
        char s[sizeof(int)] = {0};

	// Opens the file
        fd = open(argv[1], O_RDONLY);
        if(fd == -1)
        {
        	perror("Consumer File Error");
                exit(1);
        }

	// Reads 20 integers from file 
	printf("Contents of file numbers.txt:");
        for(int i = 0; i < 20; i++)
        {
                read(fd, s, sizeof(int));
                numbers[i] = atoi(s);
		if(i < 19)
		{
                	printf("%d,",numbers[i]);
		}
		else
		{
			printf("%d\n",numbers[i]);
		}
        }

	// Outputs the even number and caculates the sum of odd numbers
	printf("The even numbers:");
	for(int i = 0; i < 20; i++)
	{
		if((numbers[i] & 1 )== 0)
		{
			printf("%d ", numbers[i]);
		}
		else
		{
			sum += numbers[i];
		}
	}
	printf("\nThe sum of the odd numbers:%d\n", sum);

	return 0;
}

