#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>

int main()
{
	int a = 20;
	char s[20];
	int fd;
	int len;
	fd = open("numbers.txt",  O_WRONLY);
	for(int i = 0; i < 5; i++)
	{
		len = sprintf(s, "%d", i);
		write(fd, s, len);
	}
	return 0;
}

