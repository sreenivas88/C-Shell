#include"func_pinfo.h"

#define MAX_word_count 1000
#define MAX_BUFFER 1000

void pinfo_wrapper(char ** cmd_words,int count_cmd_words,int pid_given){

    char proc_dir[MAX_BUFFER] = "/proc";
    char pid[MAX_BUFFER] ;

    if(pid_given == -1){
        pid_given = getpid();
    }

    // strcpy(pid,ito)
    sprintf(pid,"%d",pid_given);

    long long int dmp_int;
    char *dmp_chr = (char *)malloc(sizeof(char)*MAX_BUFFER);
    char *exec_name = (char *)malloc(sizeof(char)*MAX_BUFFER);

    strcat(proc_dir,"/");
    strcat(proc_dir,pid);
    // printf("\t>%s\n",proc_dir);//---------------------------------------


    // line 1 pid
    printf("pid : %d\n",pid_given);

    // line 2 status
    char stat_dir[MAX_BUFFER];
    strcpy(stat_dir,proc_dir);
    strcat(stat_dir,"/stat");

    // printf("\t>%s\n",stat_dir);//---------------------------------------

    FILE * file = fopen(stat_dir,"r");
    if(!file) {
        perror("Erro: cannot open stat of proc\n");
    }


    fscanf(file,"%lld",&dmp_int);
    fscanf(file,"%s",exec_name);
    fscanf(file,"%s",dmp_chr);
    
    printf("process Status : %s\n",dmp_chr);




    fclose(file);


    // line 3 memory
    char mem_dir[MAX_BUFFER];
    char mem[MAX_BUFFER];
    strcpy(mem_dir,proc_dir);
    strcat(mem_dir,"/statm");

    // printf("\t>%s\n",stat_dir);//---------------------------------------

    file = fopen(mem_dir,"r");
    if(!file) {
        perror("Erro: cannot open statm of proc\n");
    }


    fscanf(file,"%lld",&dmp_int);

    printf("memory : %lld\n",dmp_int);
    fclose(file);

    // line 4 exe path
    char exe_dir[MAX_BUFFER];
    strcpy(exe_dir,proc_dir);
    strcat(exe_dir,"/exe");

    // printf("\t>%s\n",exe_dir);//---------------------------------------

    file = fopen(exe_dir,"r");
    if(!file) {
        perror("Erro: cannot open exe_dir of proc\n");
    }


    int t = readlink(exe_dir,dmp_chr,MAX_BUFFER);
    if (t != -1) {
        dmp_chr[t] = '\0';
    } else {
        //printf("Cannot access the process ii\n");
        //return;
        strcat(dmp_chr,exec_name);
    }
    printf("executable Path : %s\n",dmp_chr);

    fclose(file);

    free(dmp_chr);
}