#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>


#define BUFFER_SIZE 100
#define MAX_NO_OF_ARGUMENTS 10
#define MAX_LEN_OF_AN_ARGUMENT 20

//General Purpose Buffer
char buf[BUFFER_SIZE];

//Function Definitions
void showPrompt();
char* getCmd();
void storeHistory();
void printHistory();
char** parseCmd(char* cmd);
int execCmd(char** args);
int runOtherCmd(char** args);

int main(){
    using_history();
    stifle_history(10);
    while(1){
        showPrompt();
        char* cmd=getCmd(); 
        storeHistory(cmd);
        char** args=parseCmd(cmd);
        execCmd(args);
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
char** parseCmd(char* cmd){
    int len=strlen(cmd);
    int ptr=0;
    int wptr=0;
    int c=0;
    char** args=(char**)malloc(MAX_NO_OF_ARGUMENTS*sizeof(char*));
    while(ptr<len){
        char* word=(char*)malloc(MAX_NO_OF_ARGUMENTS*sizeof(char));
        while(cmd[ptr]!=' ' && ptr<len)word[wptr++]=cmd[ptr++];
        word[ptr]='\0';
        args[c++]=word;
        wptr=0;
        ptr++;
    }
    return args;
}
int execCmd(char** args){
    if(!strcmp(args[0],"cd")){                                                    //if cd is entered
        if(chdir(args[1]))printf("cd error: Directory not found\n");
    }else if(!strcmp(args[0],"history")){                                         //if history is entered
        printHistory();
    }else if(!strcmp(args[0],"exit")){                                            //if exit is entered
        exit(EXIT_SUCCESS);
    }else if(!strcmp(args[0],"kill")){                                            //if kill is entered
        int pid=0;
        int i=0;
        while(args[1][i]!='\0')pid=pid*10+(args[1][i++]-'0');
        printf("\n%d\n",pid);
        if(!(kill(pid,SIGTERM)==0)){
            printf("\nkill error: Either pid is invalid or you do not have permission to kill the process\n");
            return 0;
        }else{
            printf("\nKilling Successful\n");
        }
        
    }else if(!strcmp(args[0],"job")){                                             //if job is entered

    }else{
        runOtherCmd(args);
    }
   
}

int runOtherCmd(char** args){
    
}
/*
Things to experiment:
1. Memory Management of the cwd variable in each loop... is free() necessary?
2. 

*/
