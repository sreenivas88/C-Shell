#include "func_history.h"

#define MAX_word_count 1000
#define MAX_his_store 20
#define MAX_BUFFER 1000

char * history[MAX_his_store] ;
int size = 0;
char* storage_path_glob;

static int is_history_loaded = 0;

void file_print(FILE *p){
    for (size_t i = 0; i < size; i++)
    {
        fprintf(p,"%s\n",history[i]);
    }    
}

void load_history(){ 

    if(is_history_loaded) // dont load again
        return;
    
    FILE * read_file = fopen(storage_path_glob,"r");
    assert(read_file != NULL);

    char temp[MAX_BUFFER];
    char ch;

    while(1){

        int BUFFER_size = 0;

        if(fscanf(read_file,"%c",&ch) == EOF){
            break;
        }

        while( ch != '\n' && !feof(read_file)){
            temp[BUFFER_size++] = ch;
            fscanf(read_file,"%c",&ch);
        }

        temp[BUFFER_size] = '\0';
        char *to_be_stored = (char *) malloc(sizeof(char) * MAX_BUFFER);
        strcpy(to_be_stored,temp);

        history[size++] = to_be_stored;

        // printf("\n==========%s\n",history[size-1]);
    }
    fclose(read_file);
    is_history_loaded = 1;

    return;

}

void store_in_histroy(char ** cmd_words,int cmd_size,char * storage_path){
    storage_path_glob = storage_path;
    load_history();

    char *to_be_stored = (char *) malloc(sizeof(char) * MAX_BUFFER);


    if(cmd_size == 0){
        return;
    }
    
    strcpy(to_be_stored,cmd_words[0]);
    strcat(to_be_stored," ");

    // printf("\t<%s>\n",str);

    for (int i = 1; i < cmd_size ; i++)
    {
        strcat(to_be_stored,cmd_words[i]);
        strcat(to_be_stored," ");
        // printf("<%s>\n",str);
    }
    to_be_stored[strlen(to_be_stored)-1 ] = '\0';

        // three cases of storiung the history
    
    // case 1 it is same as previous 
    if(size != 0 && strcmp(history[size-1],to_be_stored) == 0){
        // printf("---------0---------\n");

        return;
    }

    else if(size >= MAX_his_store)
    {
        // printf("---------1---------\n");

        free(history[0]);

        for (size_t i = 0; i < size-1; i++)
        {
            history[i] = history[i+1];
        }

        size--;
        history[size++] = to_be_stored;
    }

    else{
        // printf("---------2---------\n");
        history[size++] = to_be_stored;
    }

    // reset the storage---------------------------------------
    FILE * write_file = fopen(storage_path_glob,"w");
    file_print(write_file);
    fclose(write_file); 
}

void print_history(char *storage_path){    
    storage_path_glob = storage_path;

    load_history(storage_path_glob);

    
    for (size_t i = 0; i < size; i++)
    {
        printf(" %s\n",history[i]);
    }    
}












