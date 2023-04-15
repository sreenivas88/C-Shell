#include "func_process_maker.h"

#define MAX_word_count 1000
#define MAX_pids 1000
#define MAX_list_count 100
#define MAX_BUFFER 100

int is_fg ;
int fg_pid = -1;
char fg_name[MAX_BUFFER];

int count_pipe = 0;

static int proc_array[MAX_pids];
static int bg_count = 0;

char * names[MAX_pids];
char * storage_path;
char *_ter;

int time_taken ;
int start_time ;

int add_proc_in_bg(char *name ,int pid){
    char * word = (char *)malloc(sizeof(char) * strlen(name));

    strcpy(word,name);
    proc_array[bg_count] = pid;
    names[bg_count++] = word;
}

int removing_bg_from_list(pid_t pid,int not_success){
    int is_bg_child = 0;
    int is_bg_index = 0;
    for (size_t i = 0; i < bg_count; i++)
    {
        // printf("#%d<%s>\n",proc_array[i],names[i]);
        if(proc_array[i] == pid){
            is_bg_child = 1;
            is_bg_index = i;
            // printf(" -FOUND\n");
            // break;
        }
    }
    // printf("\x1B[31m--\n\n\033[0m");

    if(is_bg_child){
        if(not_success){
            fprintf(stderr,"\n%s with pid = %d exited abnormally\n",names[is_bg_index],pid);
        }
        else
        {
            fprintf(stderr,"\n%s with pid = %d exited normally\n",names[is_bg_index],pid);
        }

        // printf("\x1B[31mUpdating porc ---\n");
        // changing the storage of bg
        int new_bg_count = 0;   
        int only_once = 0; 
        int new_array[MAX_pids];
        char * new_array2[MAX_pids];

        // printf("+\n\n\033[0m");

        for (size_t i = 0; i < bg_count; i++)
        {
            // printf("\x1B[31m- %d\n",proc_array[i]);
            if(proc_array[i] != pid){
                new_array[new_bg_count] = proc_array[i];
                new_array2[new_bg_count++] = names[i];
            }
            else{
                free(names[is_bg_index]);
            }
        }
        bg_count--;
        for (size_t i = 0; i < bg_count; i++)
        {
            proc_array[i] = new_array[i] ;
            names[i] = new_array2[i] ;
        }

            printf("\033[0;36m"); //Set the text to the color red
            printf("%s",_ter);
            printf("\033[0m");
            fflush(stdout);
        
        // printf("\n\n");
    }
    else{
        is_fg = -1;
        // printf("\x1B[34mIt was a foregrong process\n\n");
    }    
}

int get_list_ptr(char * list_par[],int proc_array_par[]){
    for (size_t i = 0; i < bg_count; i++)
    {
        list_par[i] = names[i];
        proc_array_par[i] = proc_array[i];
    }
    
}

int get_bg_count(){
    return bg_count;
}

int get_name_fg(char * dest){
    if(fg_pid != -1){
        strcpy(dest,fg_name);
        return 1;
    }
    return 0;
}

int get_pid_of_fg(){
    return fg_pid;
}

int handler1(){

}

int handler2(){
    
}

int exec_fg(pid_t pid){
    int status;
    setpgid(pid, getpgid(0));
    int start_time = time(NULL);

    // signal(SIGINT,handler1);
    // signal(SIGTSTP,handler2);
    
    tcsetpgrp(0, pid);

    if (kill(pid, SIGCONT) < 0) perror("Error");

    waitpid(pid, &status, WUNTRACED);
    
    tcsetpgrp(0, getpgid(0));

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    int end_time = time(NULL) - start_time;

    removing_bg_from_list(pid,status);

    return end_time;
}

void exec_bg(pid_t pid){
    int flag = kill(pid,SIGCONT);
    if(flag == -1){
        perror("ERROR : FAILED to exec bg");
    }
    // printf("fucked up onot|%d|\n",flag);
    // printf("grp %d - pid %d \n",getpgid(pid),pid);
    // printf("grp %d - pid %d \n",getgid(),getpid());
}

void exec_sig(pid_t pid,int sig){
    int flag = kill(pid,sig);
    if(flag == -1){
        perror("ERROR : FAILED to exec sig");
    }
    // printf("fucked up onot|%d|\n",flag);
    // printf("grp %d - pid %d \n",getpgid(pid),pid);
    // printf("grp %d - pid %d \n",getgid(),getpid());

}

int create_list_arg(char ** cmd_words,int words_count,char *list[],int is_bg){
    int list_size = 0;
    char BUFFER[MAX_BUFFER];

    for (size_t i = 0; i < words_count; i++)
    {        
        char *word = (char *) malloc(sizeof(char) * 100);
        strcpy(word,cmd_words[i]);
        list[list_size++] = word;

        if(strcmp(word,"|") == 0){
            count_pipe++;
        }
    }
    is_fg = 1 - is_bg ;
    list[list_size++] = NULL;

    if(0){
        for (size_t i = 0; i < list_size; i++)
        {
            printf(">><%s>\n", list[i]);
        }
        printf("ppied %d times \n",count_pipe);
    }

 
    return list_size;
}

void check_wether_proc_bf_or_fg(){
    int not_success;
    pid_t pid = waitpid(-1,&not_success,WNOHANG);

    removing_bg_from_list(pid,not_success);

    // printf("\x1B[31ma proc with pid = %d died stored(%d)\n",pid,bg_count);
    // printf("\x1B[31mSeraching for porc ---\n");
    //checking if pid is child that died which we created 
    
}

int start_process(char * list[],int list_size){   
    int f_pid_ret = fork();
    
    if(f_pid_ret < 0){
        perror("ERROR : in fork\n");
    }

    // child
    else if(f_pid_ret == 0){
        setpgid(0, getpgid(f_pid_ret));

        if (execvp(list[0], list) < 0) {
            perror("ERROR : in execvp\n");
            exit(1);
        }
    }

    // parent
    else{
        if(!is_fg){
            
            names[bg_count] = (char *)malloc(sizeof(char ) * strlen(list[0]));
            
            strcpy(names[bg_count],list[0]);
            for (size_t i = 1; i < list_size - 1; i++)
            {
                strcat(names[bg_count]," ");
                strcat(names[bg_count],list[i]);
            }
            bg_count++;
            
            printf("[%d] %d\n",bg_count,getpid());
            fflush(stdout);
            proc_array[bg_count-1] = f_pid_ret;

            return -1;
        }
        else{
            fg_pid = f_pid_ret;

            strcpy(fg_name,list[0]);
            for (size_t i = 1; i < list_size - 1; i++)
            {
                strcat(fg_name," ");
                strcat(fg_name,list[i]);
            }
            start_time = time(NULL);

            waitpid(f_pid_ret,NULL,WUNTRACED);
            
            time_taken = time(NULL) - start_time;
            return time_taken;
        }
    }
}

int make_process(char ** cmd_words,int count_cmd_words,char* _ter_pass,int is_bg){
    is_fg = 1;

    char * list[MAX_list_count];

    _ter = _ter_pass;

    int list_size = create_list_arg(cmd_words,count_cmd_words,list,is_bg);

    // if(is_fg){
    //     // printf("%s is fg \n\n>",list[0]);
    //     return foreground(list,list_size);

    // }
    // else{

    //     // printf("%s is bg \n\n>",list[0]);
    //     background(list,list_size);
    //     return -100;
    // }

    return start_process(list,list_size);
}






