#include"display_jobs.h"
#include"func_process_maker.h"

#define MAX_BUFFER 1000
#define MAX_pids 1000

int proc_pids[MAX_pids];
char * proc_names[MAX_pids];
int proc_sorted_order_index[MAX_pids];

int bg_count = 0;


// sortig alpahbetical order
int compare_jobs( const void *a, const void * b)
{
    int const *p1 = a;
    int const *p2 = b;
    return strcmp(proc_names[*p1],proc_names[*p2]);
}

void sort_in_jobs(int line[],int n)
{
    qsort(line, n, sizeof(int), compare_jobs);
}

// required before getiing info loads the upto date data
void load_data(){
    bg_count = get_bg_count();
    get_list_ptr(proc_names,proc_pids);
}

void func_bg(int ind){
    load_data();    

    if(ind > bg_count){
        perror("ERROR ; bg not presetn in list\n");
    }
    exec_bg(proc_pids[ind-1]);
}

void func_sig(int ind,int sig){
    load_data();    

    if(ind > bg_count){
        perror("ERROR ; bg not presetn in list\n");
    }
    exec_sig(proc_pids[ind-1],sig);
}

int func_fg(int ind){
    load_data();    

    if(ind > bg_count){
        perror("ERROR ; bg not presetn in list\n");
    }
    return exec_fg(proc_pids[ind-1]);
}

void jobs(char ** cmd_words,int count_cmd_words){
    load_data();   

    for (int i = 0; i < bg_count; i++)
    {
        proc_sorted_order_index[i] = i;
    }

    int r_flag = 0;
    int s_flag = 0;
    

    sort_in_jobs(proc_sorted_order_index,bg_count);

    for (size_t i = 1; i < count_cmd_words; i++)
    {
        if(strcmp(cmd_words[i],"-r") == 0){
            r_flag = 1;
        }
        else if(strcmp(cmd_words[i],"-s") == 0){
            s_flag = 1;
        }
        else if(strcmp(cmd_words[i],"-rs") == 0 || strcmp(cmd_words[i],"-sr") == 0 ){
            r_flag = 1;
            s_flag = 1;
        }
        else{
            perror("ERROR : Wrong input fornmat flags");
            return;
        }
    }


    if(r_flag == 0 && s_flag == 0){
        r_flag = s_flag = 1;
    }
    

    for (int i = 0; i < bg_count; i++)
    {
        // line 2 status
        long long int dmp_int;
        char *dmp_chr = (char *)malloc(sizeof(char)*MAX_BUFFER);
        char *exec_name = (char *)malloc(sizeof(char)*MAX_BUFFER);

        char proc_dir[MAX_BUFFER] = "/proc";
        char pid[MAX_BUFFER] ;
        sprintf(pid,"%d",proc_pids[proc_sorted_order_index[i]]);


        strcat(proc_dir,"/");
        strcat(proc_dir,pid);

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

        // printf("process Status : %s\n",dmp_chr);
        fclose(file);



        if(r_flag && dmp_chr[0] != 'T'){
            printf("[%d] %s %s [%d]\n", proc_sorted_order_index[i] + 1, dmp_chr[0] == 'T' ? "Stopped" : "Running",proc_names[proc_sorted_order_index[i]], proc_pids[proc_sorted_order_index[i]]);
        }

        else if(s_flag && dmp_chr[0] == 'T'){
            printf("[%d] %s %s [%d]\n", proc_sorted_order_index[i] + 1, dmp_chr[0] == 'T' ? "Stopped" : "Running",proc_names[proc_sorted_order_index[i]], proc_pids[proc_sorted_order_index[i]]);
        }

    }
}