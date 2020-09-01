#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define BUFFER_SIZE 100

//General Purpose Buffer
char buf[BUFFER_SIZE];

//Function Definitions
void showPrompt();
char* getCmd();
void storeHistory();
void printHistory();
int execCmd();
int main(){
    using_history();
    stifle_history(10);
    while(1){
        showPrompt();
        char* cmd=getCmd(); 
        
        storeHistory(cmd);
        execCmd(cmd);
        
        
    }
    

    exit(EXIT_SUCCESS);
}
void showPrompt(){
    getcwd(buf,BUFFER_SIZE);
    printf("$~%s $$",buf);
}

char* getCmd(){
    char* x=readline(""); //Allocated with Malloc, so no need to take care of size or \0
    return x;
}
void storeHistory(char* cmd){
    if (cmd && *cmd) add_history (cmd);
}
void printHistory(){
    printf("\n\nThe previous commands are (a maximum of 10 are shown):\n");
    struct _hist_entry** list=history_list();
    int count=0;
    while(list[count]!=NULL){
        printf("$ %d: %s\n",count+1,list[count]->line);
        count++;
    }
}
int execCmd(char* cmd){
    int len=strlen(cmd);
    int ptr=0;
    char x[BUFFER_SIZE]; 
    while(cmd[ptr]!=' ' && ptr<len){x[ptr]=cmd[ptr];ptr++;}; 
    x[ptr]='\0';
    printf("%s\n",x);
    if(!strcmp(x,"cd\0")){
        char folder[BUFFER_SIZE];
        int i=0;
        ptr++;
        while(ptr<len)folder[i++]=cmd[ptr++];
        folder[i]='\0';
        if(chdir(folder))printf("cd error: Directory not found\n");
    }else if(!strcmp(x,"history")){
        printHistory();
    }else if(!strcmp(x,"exit")){
        exit(EXIT_SUCCESS);
    }


    
}
/*
Things to experiment:
1. Memory Management of the cwd variable in each loop... is free() necessary?
2. 

*/
