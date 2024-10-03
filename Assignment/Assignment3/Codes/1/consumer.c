#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd;             
    int sum = 0;
    int buf[20];   
    fd=dup(0);
    read(fd,buf,sizeof(buf));

    for(int i = 0;i < 20;i++)
    {  
	// input the interger array
        if(i == 0)
            printf("Contents of file numbers.txt:%d,",buf[i]);
        else if(i > 0 && i< 19)
            printf("%d,",buf[i]);
        else
            printf("%d\n",buf[i]);
    }

    for (int i = 0; i < 20; i++)
    {
	// check odd or even
        if(buf[i]&1 == 1)
            sum += buf[i];      
        else
            printf("the even numbers:%d\n",buf[i]); // output even
    }
    printf("the sum of the odd numbers:%d\n",sum);  // output the sum of odd
    close(fd);

    return 0;
}

