#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>




#define BUFFER_SIZE 100
#define MAX_NO_OF_ARGUMENTS 10
#define MAX_LEN_OF_AN_ARGUMENT 20


//General Purpose Buffer
char buf[BUFFER_SIZE];

//A struct to store background process information
typedef struct bg_process{
    int pid;
    char* name;
}bg_process;

//Function Definitions
void showPrompt();
char* getCmd();
void storeHistory();
void printHistory();
char** parseCmd(char* cmd,int* nargs);
int execCmd(char** args,int l);
int runOtherCmd(char** args,int nargs);
void updateBgProcessList();
void printBgProcess();
int addBgProcess(int pid, char* name);
int removeBgProcess(int pid);


//List and counters to maintain the list of background processes
bg_process** bglist=NULL;
int bgcounter=0;
int nbgprocess=0;

int main(){
    using_history();
    stifle_history(10);
    while(1){
        showPrompt();
        char* cmd=getCmd();
        if(strlen(cmd)==0){printf("\n");continue;} 
        int nargs;
        char** args=parseCmd(cmd,&nargs);
        int x=execCmd(args,nargs);
        if(x!=2)storeHistory(cmd);  //History commands should not be stored
        //freeMemory(cmd,args,nargs);
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
char** parseCmd(char* cmd, int* nargs){
    int len=strlen(cmd);
    int ptr=0;
    int wptr=0;
    int c=0;
    char** args=(char**)malloc(MAX_NO_OF_ARGUMENTS*sizeof(char*));
    while(ptr<len){
        char* word=(char*)malloc(MAX_NO_OF_ARGUMENTS*sizeof(char));
        while(cmd[ptr]!=' ' && ptr<len)word[wptr++]=cmd[ptr++];
        word[wptr]='\0';
        args[c++]=word;
        wptr=0;
        ptr++;
    }
    *nargs=c;
    return args;
}

int execCmd(char** args, int nargs){
    if(!strcmp(args[0],"cd")){                                                    //if cd is entered                                           
        if(chdir(args[1]))printf("cd error: Directory not found\n");
    }else if(!strcmp(args[0],"history")){                                         //if history is entered
        printHistory();
    }else if(!strcmp(args[0],"exit")){                                            //if exit is entered
        updateBgProcessList();
        if(nbgprocess==0){
            exit(EXIT_SUCCESS);
        }
        printf("The following background processes are running: \n");
        printBgProcess();
        printf("Please kill these processes before exiting\n");
        return 1;
        
    }else if(!strcmp(args[0],"kill")){                                            //if kill is entered
        int pid=0;
        int i=0;
        while(args[1][i]!='\0')pid=pid*10+(args[1][i++]-'0');
        //printf("\n%d\n",pid);
        if(!(kill(pid,SIGTERM)==0)){
            printf("\nkill error: Either pid is invalid or you do not have permission to kill the process\n");
            return 0;
        }else{
            updateBgProcessList();
            printf("\nKilling Successful\n");
        }
        
    }else if(!strcmp(args[0],"job")){                                             //if job is entered
        updateBgProcessList();
        printBgProcess();

    }else if(args[0][0]=='!'){                                                    //if ! is entered
        struct _hist_entry** list=history_list();
        char* cmd=NULL;
        if(args[0][1]!='-'){
            char c=args[0][1]-'0'-1;
            if(c>9 || c<0){printf("'!' error: Invalid Number given"); return 1;}
            cmd=list[c]->line;
        }else if(args[0][1]=='-'){
            HISTORY_STATE* state=history_get_history_state(); 
            int l=state->length;
            char c=args[0][2]-'0'-1;
            if(c>9 || c<0){printf("'!' error: Invalid Number given"); return 1;}
            cmd=list[l-c-1]->line;   
        }else{
            printf("'!' error: Invalid Number given");
            return 1;
        }
        int l;
        char** args=parseCmd(cmd,&l);
        int t=execCmd(args,nargs);
        storeHistory(cmd);
        return 2;
    }else{
        runOtherCmd(args,nargs);
    }
   
}
void updateBgProcessList(){
    if(nbgprocess==0)return; 
    for(int i=0;i<bgcounter;i++){
        if(bglist[i]!=NULL){
            int status;
            int t=waitpid(bglist[i]->pid, &status,WNOHANG);
            if(t==-1){
                printf("Error occured in updatin background process list");
            }else if(t){
                removeBgProcess(bglist[i]->pid);
            }
        }
    }
}
int addBgProcess(int pid, char* name){
    nbgprocess++;
    bg_process* newproc=(bg_process*)malloc(sizeof(bg_process));
    newproc->pid=pid;
    newproc->name=name;
    int i=0;
    while(bgcounter!=0 && i<bgcounter){
        if(bglist[i]==NULL)break;
        i++;
    }
    if(i<bgcounter){
        bglist[i]=newproc;
        return 1;
    }
    bgcounter++;
    bglist=(bg_process**)realloc(bglist,sizeof(bg_process*)*bgcounter);
    bglist[bgcounter-1]=newproc;
    return 1;
}
int removeBgProcess(int pid){
    for(int i=0;i<bgcounter;i++){
        if(bglist[i]->pid==pid){
            bg_process* temp=bglist[i];
            bglist[i]=NULL;
            free(temp);
            nbgprocess--;
            return 1;
        }
    }
    
    return 0;
}
void printBgProcess(){
    if(nbgprocess==0){
        printf("No Background Processes running\n");
        return;
    }
    printf("PID            NAME\n");
    for(int i=0;i<bgcounter;i++){
        if(bglist[i]!=NULL){
            printf("%d          %s\n",bglist[i]->pid,bglist[i]->name);
        }
    }
}
int runOtherCmd(char** args, int nargs){
    int status;
    char* path=args[0];
    int pid=fork();
    if(pid>0){
        if(strcmp(args[nargs-1],"&")) {
            int p=wait(&status);
        }else{
            addBgProcess(pid,path);
            printBgProcess();
            return 1;
        }
    }else if(pid==0){
        int fdI=0;
        for(int i=0;i<nargs;i++){
            if(!strcmp(args[i],"<")){
                fdI=open(args[++i],O_RDONLY);
                if(fdI==-1){
                    printf("Unable to open file\n");
                    return 0;
                }
                dup2(fdI,STDIN_FILENO);
                close(fdI);

            }else if(!strcmp(args[i],">")){
                int fdO=creat(args[++i],0644);
                if(fdO==-1){
                    printf("Unable to Create output file, perhaps it already exists\n");
                    return 0;
                }
                dup2(fdO,STDOUT_FILENO);
                close(fdO);
            }
        }
        int e=execvp(path,args);
        if(e==-1){printf("%s Command not found! Please check...\n",path);return 0;}
    }else{
        printf("Process Creation failed :(\n");
        return 0;
    }
    
}

