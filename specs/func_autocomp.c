#include "func_ls.h"
#include "func_pwd.h"

#define MAX_word_count 1000
#define MAX_his_store 5
#define MAX_BUFFER 1000

char tar_dir[MAX_word_count];
int tar_dir_given = -1;

int oldest_cmd_size = 0;

char prefix_str[MAX_word_count];

int min(int x , int y){
    if(x == -1){
        return y;
    }
    if(y == -1){
        return x;
    }
    if(x < y)
        return x;
    return y;
}

void print_promt_after_autocomp(char * ter){
    
    // printing the ter at start

    printf("\033[0;36m"); //Set the text to the color red
    printf("%s",ter);
    printf("\033[0m"); // reset back to original color
}

// returns 1 for file 0 for dir and -1 for error
int get_stat_in_auto_comp(char * path){
    // printf("<\"%s\">\n",path);
    if(isDir(path)){
        return 0;
    }
    else if(isFile(path)){
        return 1;
    }
    return -1;
}

// assuming cmd is already allocated
int get_cmd_words_auto(char * cmd_line,char ** cmd_words){

    int word_count = 0; 
    char BUFFER[MAX_BUFFER] ; 

    for (int i = 0; i < strlen(cmd_line); i++)
    {

        int word_size =0;  

        if(0){
            printf("\t\t\t----%c(%d)\n",cmd_line[i],i);
        }

        if(cmd_line[i] == '\t' || cmd_line[i] == ' '){
            continue;
        }

        else{
            do
            {
                if(0){
                    printf("\t\t-_-%c(%d)\n",cmd_line[i],i);        
                }
                
                if(cmd_line[i] == ';' || cmd_line[i] == '&' ){
                    i--;
                    break;
                }

                BUFFER[word_size++] = cmd_line[i++];   
            } 
            while (cmd_line[i] != '\t' && cmd_line[i] != ' ' && i < strlen(cmd_line));
        }



        BUFFER[word_size++] = '\0';
        
        char *word = (char *) malloc(sizeof(char) * word_size);
        strcpy(word,BUFFER);
        cmd_words[word_count++] = word;

        if(0){
            printf("\n\n<%s>(%ld)\n",word,strlen(word));
        }


        if(i >= strlen(cmd_line)){
            break;
        }
    } 
    return word_count;
    
}

// return list of only match 0 for no match 
int search_for_matches(char * list[],char * cmd_line){
    DIR *dir;
    struct dirent *entry;
    int list_size = 0;
    int copy_from_index = -1;



    if ((dir = opendir(tar_dir)) == NULL){
        perror("opendir() error in tab");
    }

    else {
        while ((entry = readdir(dir)) != NULL){
            if(entry->d_name[0] != '.'){
                if(strlen(entry->d_name) >= strlen(prefix_str)){
                    int matched_flag = 1;
                    // printf("<%s>(%ld) == <%s>(%ld)\n",entry->d_name,strlen(entry->d_name),prefix_str,strlen(prefix_str));
                    
                    for (size_t i = 0; i < strlen(prefix_str); i++)
                    {
                        // printf("\t-<%c> == <%c>\n",entry->d_name[i],prefix_str[i]);

                        if(entry->d_name[i] != prefix_str[i]){
                            matched_flag = 0;
                            copy_from_index = min(copy_from_index, strlen(prefix_str));
                            break;
                        }
                    }
                    
                    if(matched_flag){
                        list[list_size++] = entry->d_name; 
                        // printf("--<>--%d\n\n",copy_from_index);
                    }
                }
            }
        }
        
        closedir(dir);
    }

    // for (size_t i = 0; i < list_size; i++)
    // {
    //     printf("---><%s>\n",list[i]);
    // }

    if(list_size == 0){
        return 0;
    }

    int resultant_size = strlen(cmd_line) + strlen(list[0]) - copy_from_index;
    int org_size = strlen(cmd_line);


    if(list_size == 1 && copy_from_index != -1){
        // printf("<%s>(%d)\n<%s>(%ld)\n",cmd_line,org_size,list[0],strlen(list[0]));

        for (size_t i = copy_from_index; i < strlen(list[0]); i++)
        {
            cmd_line[org_size++] = list[0][i];
        } 

        char path[MAX_BUFFER] ;
        strcpy(path,tar_dir);
        strcat(path,"/");
        strcat(path,list[0]);

        int flag = get_stat_in_auto_comp(path);

        if(flag){
            cmd_line[org_size++] = ' ';
        }
        else{
            cmd_line[org_size++] = '/';
        }

        cmd_line[org_size++] = '\0';

    }



    else if(list_size > 0){
        int size_prefix = strlen(prefix_str);
        int store_size_prefix = size_prefix;
        // printf("////%d\n\n",size_prefix);

        while(1){
            int exit_flag = 0;
            for (size_t i = 0; i < list_size - 1; i++)
            {
                if(list[i][size_prefix] != list[i+1][size_prefix]){
                    exit_flag = 1;
                    break;
                }
            }
            if(exit_flag){
                break;
            }
            prefix_str[size_prefix] = list[0][size_prefix];
            size_prefix++;
        }

        // printf("--?<%s>\n",prefix_str);

        

        for (size_t i = store_size_prefix; i < strlen(prefix_str); i++)
        {
            cmd_line[org_size++] = prefix_str[i];
        } 
        cmd_line[org_size++] = '\0'; 
    }
    // printf("\n453(%d) : <%s>\n",copy_from_index,cmd_line);
    

    return list_size;
}


void print_list(char * list[],char *cmd_line,char * ter,int list_size){
    printf("\n");
    for (size_t i = 0; i < list_size; i++)
    {
        printf(" %s\n",list[i]);
    }
    
    print_promt_after_autocomp(ter);
    printf("%s",cmd_line);
}

void update_line(char * cmd_line,char * ter){
    for (size_t i = 0; i < oldest_cmd_size; i++)
    {
        printf("\b");   
    }
    printf("%s",cmd_line);
}

int auto_complete(char *cmd_line,char * ter){
    char ** cmd_words = (char **)malloc(sizeof(char*) * MAX_word_count);

    int count_cmd_words = get_cmd_words_auto(cmd_line,cmd_words);
    char * list[MAX_word_count];

    oldest_cmd_size = strlen(cmd_line);


    // for(size_t i = 0 ; i < word_count; i++)
    // {
    //     printf("0101(%ld)<%s>\n",i,cmd_words[i]);
    // }
    // printf("\b\n\n\n");   



    for (int i = strlen(cmd_words[count_cmd_words-1]); i >= 0  ; i--)
    {
        // printf("\n1!!!(%d)\n",i);

        if(cmd_words[count_cmd_words-1][i] == '/'){
            tar_dir_given = i;
            char * last_word = cmd_words[count_cmd_words-1];

            // getting the dir
            for (int j = 0; j < i; j++){
                // printf("2!!!(%d)<%c>\n",i,last_word[j]);
                tar_dir[j] = last_word[j];
            }
            tar_dir[i] = '\0';

            // printf("\n230 : <|%s|>\n",last_word);
            // printf("230 : <|%s|>\n",tar_dir);
            // printf("230 : <|%s|>\n",prefix_str);

            // repfiox of file name
            for(int j = i+1; j < strlen(last_word) ; j++){
                prefix_str[j-i-1] = last_word[j];
                // printf("3!!!(%d)<%c>\n",j-i-1,prefix_str[j-i-1]);
            }
            prefix_str[ strlen(last_word) - i -1] = '\0';
            // printf("3!!!(%ld)<%c>\n", strlen(last_word) - i -1,prefix_str[ strlen(last_word) - i -1] );


            break;
        }
    }


    // check tar_dir is empty
    if(tar_dir_given == -1){
        tar_dir[0] = '.';
        tar_dir[1] = '\0';

        for(int j = 0; j < strlen(cmd_words[count_cmd_words-1]) ; j++){
            prefix_str[j] = cmd_words[count_cmd_words-1][j];
        }
        prefix_str[strlen(cmd_words[count_cmd_words-1])] = '\0';
    }


            // printf("\n targert dir  <%s>(%d)\n",tar_dir,tar_dir_given);
            // printf("\n targert pre  <%s>(%ld)\n--------------\n",prefix_str,strlen(prefix_str));



    int flag = search_for_matches(list,cmd_line);
    
    if (flag == 1){
        update_line(cmd_line,ter);
    }
    else if(flag >= 2){
        int list_size = flag;
        print_list(list,cmd_line,ter,list_size);
    }
    tar_dir_given = -1;

    strcpy(tar_dir,"");
    strcpy(prefix_str,"");
    return strlen(cmd_line);
    // printf("not fucked if you see me!!!\n");
}




