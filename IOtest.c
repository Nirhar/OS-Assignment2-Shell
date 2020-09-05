#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
void main(){
    int fd=creat("tester",0644);
    if(fd==-1){ 
        printf("Cannot open file \n"); 
        return;
    }
    dup2(fd,1);
    printf("Hello world!\n");
    
}
