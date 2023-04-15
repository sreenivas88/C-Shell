#include "func_ls.h"
#include "func_pwd.h"

#define MAX_word_count 1000
#define MAX_his_store 5
#define MAX_BUFFER 1000

char * flags[MAX_word_count];
char __home_dir[MAX_word_count];
char * dir_file[MAX_word_count];

static int flag_count = 0;
static int df_count = 0;


// sortig alpahbetical order
int compare( const void *a, const void * b)
{
    char *const *p1 = a;
    char *const *p2 = b;
    return strcmp(*p1, *p2);
}

void sort(char* line[],int n)
{
    qsort(line, n, sizeof(*line), compare);
}



int isDir(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int isExe(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return statbuf.st_mode & S_IXUSR;
}

int isFile(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISREG(statbuf.st_mode);
}



// returns 1 for file 0 for dir and -1 for error
int get_stat(char * dir_name){
    char path[MAX_BUFFER];
    get_pwd(path);
    strcat(path,"/");
    strcat(path,dir_name);
    // int exists =  ope;

    // printf("<\"%s\">\n",path);
    if(isDir(path)){
        return 0;
    }
    else if(isFile(path)){
        return 1;
    }
    return -1;
}

//mode a 1 for -a activation
int print_dir_list(DIR *dir,char * curr_path,struct dirent *entry,char *sep,int mode_a){
    char * list[MAX_BUFFER];
    char path[MAX_BUFFER];
    int list_size = 0;
    while ((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'  || mode_a)
            list[list_size++] = entry->d_name; 
    }
    sort(list,list_size);
    for (size_t i = 0; i < list_size; i++)
    {
        strcpy(path,curr_path);
        strcat(path,"/"); 
        strcat(path,list[i]);

        // printf("\t>%s\n",path);

        if(isDir(path)){
            printf("\033[0;32m");
            printf(" %s%s", list[i],sep);
            printf("\033[0m");

        }
        else if(isExe(path)){
            printf("\033[0;33m");
            printf(" %s%s", list[i],sep);
            printf("\033[0m");

        }
        else if(isFile(path)){
            printf(" %s%s", list[i],sep);
        }
        else {
            printf("FUCKED ??!!!\n");
        }
        // printf("-|-\n");


    }
}

int print_dir_list_l(DIR *dir,char * curr_path,struct dirent *entry,char *sep,int mode_a){


    char * list[MAX_BUFFER];
    char path[MAX_BUFFER];
    char time_ext[18];
    char time[18];
    int list_size = 0;
    while ((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'  || mode_a)
            list[list_size++] = entry->d_name; 
    }
    sort(list,list_size);
    for (size_t i = 0; i < list_size; i++)
    {
        strcpy(path,curr_path);
        strcat(path,"/"); 
        strcat(path,list[i]);
        struct stat statbuf;
        if (stat(path, &statbuf) != 0)
        return 0;

        printf((S_ISDIR(statbuf.st_mode))? "d":"-");

        printf((statbuf.st_mode & S_IRUSR)? "r":"-");
        printf((statbuf.st_mode & S_IWUSR)? "w":"-");
        printf((statbuf.st_mode & S_IXUSR)? "x":"-");
        // printf("\n");
        printf((statbuf.st_mode & S_IRGRP)? "r":"-");
        printf((statbuf.st_mode & S_IWGRP)? "w":"-");
        printf((statbuf.st_mode & S_IXGRP)? "x":"-");
        // printf("\n");
        printf((statbuf.st_mode & S_IROTH)? "r":"-");
        printf((statbuf.st_mode & S_IWOTH)? "w":"-");
        printf((statbuf.st_mode & S_IXOTH)? "x":"-");

        printf(" %u",(unsigned int)statbuf.st_nlink);

        //getting the user name
        struct passwd *p;
        uid_t  uid;

        if(strcmp(list[i],"..") == 0){ //check for way to find it out not uderstood
            printf(" root      ");
            printf("root      ");
        }
        else if ((p = getpwuid(uid = getuid())) == NULL)
            perror("getpwuid() error");
        else {
            printf(" %s",p->pw_name);
            printf(" %s",p->pw_name);
        }

        strcpy(time_ext,ctime(&statbuf.st_atime));
        time_ext[17] = '\0';

        for (size_t i = 4; i < 18; i++)
        {
            time[i-4] = time_ext[i];
        }
        
        printf(" %7ld",statbuf.st_size);
        printf(" %s",time);
        

        // printf("\t>%s\n",path);

        if(isDir(path)){
            printf("\033[0;32m");
            printf(" %s%s", list[i],sep);
            printf("\033[0m");

        }
        else if(isExe(path)){
            printf("\033[0;33m");
            printf(" %s%s", list[i],sep);
            printf("\033[0m");

        }
        else if(isFile(path)){
            printf("\033[0;37m");
            printf(" %s%s", list[i],sep);
            printf("\033[0m");

        }
        else {
            printf("FUCKED ??!!!\n");
        }
        // printf("\b\b");


    }
}




void ls_type(char ** cmd_words,int count_cmd_words,char * home){

    strcpy(__home_dir,home);

    char BUFFER[MAX_BUFFER] ; 
    
    for (size_t i = 1; i < count_cmd_words; i++)
    {
        // printf("\n\n<%s>(%ld)\n",cmd_words[i],strlen(cmd_words[i]));

        if(cmd_words[i][0] == '-'){
            flags[flag_count++] = cmd_words[i];
        }
        else{
            dir_file[df_count++] = cmd_words[i];
        }
    }        

    // for (size_t i =0; i < flag_count; i++)
    // {
    //     printf("->%s.\n",flags[i]);
    // }
    // printf("---\n");

    // for (size_t i =0; i < df_count; i++)
    // {
    //     printf("~>%s.f\n",dir_file[i]);
    // }
    // printf("\n");

    int flag_a = 0;
    int flag_l = 0;

    for (size_t i = 0; i < flag_count; i++)
    {
        if(strcmp(flags[i],"-a") == 0){
            flag_a = 1;
        }

        else  if(strcmp(flags[i],"-l") == 0){
            flag_l = 1;
        }

        else  if(strcmp(flags[i],"-al") == 0){
            flag_l = 1;
            flag_a = 1;
        }

        else  if(strcmp(flags[i],"-la") == 0){
            flag_a = 1;
            flag_l = 1;
        }

        else{
            printf("ERROR flag \"%s\" is not recongmnisable \n",flags[i]);
            return;
        }
    }

    if( flag_l == 1 && flag_a == 1){
        ls_l(1);
    }

    else if( flag_a == 1){
        ls(1);
    }
    
    else if( flag_l == 1){
        ls_l(0);
    }

    else{
        ls(0);
    }

    flag_count = 0;
    df_count = 0;




}

void ls(int mode_a){
    DIR *dir;
    struct dirent *entry;

    if(df_count == 1){
        char sep[] = "\n";

        //exception
        if(strcmp(dir_file[0],"~") == 0){
            if ((dir = opendir(__home_dir)) == NULL)
                perror("opendir() error");
            else 
            {
                char curr_path[MAX_BUFFER];
                char path[MAX_BUFFER];
                strcpy(curr_path,__home_dir);
                
                print_dir_list(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
            }
            return;
        }
        
        // calling func to chech what it is
        int st = get_stat(dir_file[0]);
        // printf("\t>stat = %d\n",st);
        
        // case one it is a file
        if(st == 1){
            char * list[MAX_BUFFER];
            char curr_path[MAX_BUFFER];
            char path[MAX_BUFFER];

            get_pwd(curr_path);

            strcpy(path,curr_path);
            strcat(path,"/"); 
            strcat(path,dir_file[0]);

            // printf("\t>%s\n",path);

            if(isExe(path)){
                printf("\033[0;33m");
                printf(" %s\n",dir_file[0]);
                printf("\033[0m");

            }
            else if(isFile(path)){
                printf("\033[0;37m");
                printf(" %s\n",dir_file[0]);
                printf("\033[0m");

            }
            
        }

        // case two is a dir
        else if(st == 0){
        // printf("\t\t(1)\n");
            if ((dir = opendir(dir_file[0])) == NULL){
                perror("opendir() error");
            }

            else {
                char curr_path[MAX_BUFFER];
    
                get_pwd(curr_path);
                strcat(curr_path,"/"); 
                strcat(curr_path,dir_file[0]);
                
                print_dir_list(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
            }
        }

        //else doesnt exit
        else{
            perror("ERROR :given name is doesnt exit in dir\n");
        }


    }


    else if(df_count > 1){
        char sep[] = " ";
    // printf("\t\t(2)\n");
        for (size_t i = 0; i < df_count; i++)
        {
            
            if(strcmp(dir_file[i],"~") == 0){
                printf("%s :\n",__home_dir);
                if ((dir = opendir(__home_dir)) == NULL){
                    perror("opendir() error");
                }
                else 
                {
                    char curr_path[MAX_BUFFER];
                    char path[MAX_BUFFER];
                    strcpy(curr_path,__home_dir);
                    
                    print_dir_list(dir,curr_path,entry,sep,mode_a);
                
                    closedir(dir);
                    printf("\n\n");
                    continue;
                }
            }
            
            // calling func to chech what it is
            int st = get_stat(dir_file[i]);
            // printf("\t>stat = %d\n",st);
            
            printf("%s :\n",dir_file[i]);
            // case one it is a file
            if(st == 1){
                char * list[MAX_BUFFER];
                char curr_path[MAX_BUFFER];
                char path[MAX_BUFFER];

                get_pwd(curr_path);

                strcpy(path,curr_path);
                strcat(path,"/"); 
                strcat(path,dir_file[i]);

                // printf("\t>%s\n",path);

                if(isExe(path)){
                    printf("\033[0;33m");
                    printf(" %s\n",dir_file[i]);
                    printf("\033[0m");

                }
                else if(isFile(path)){
                    printf("\033[0;37m");
                    printf(" %s\n",dir_file[i]);
                    printf("\033[0m");

                }
                else{
                    printf("FUCKED ??? !!!\n");
                }
            }

            // case two is a dir
            else if(st == 0){
                if ((dir = opendir(dir_file[i])) == NULL){
                    perror("opendir() error");
                }

                else {

                    char curr_path[MAX_BUFFER];
    
                    get_pwd(curr_path);
                    strcat(curr_path,"/"); 
                    strcat(curr_path,dir_file[i]);
                    
                    print_dir_list(dir,curr_path,entry,sep,mode_a);
                    
                    closedir(dir);
                }
            }

            //else doesnt exit
            else{
                perror("ERROR :given name is doesnt exit in dir\n");
            }

            printf("\n\n");
        }
        
    }
    

    else{
        // printf("\t\t(3)\n");
        char sep[] = "\n";

        if ((dir = opendir(".")) == NULL)
            perror("opendir() error");
        else {
                char curr_path[MAX_BUFFER];
    
                get_pwd(curr_path);
                
                print_dir_list(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
        }
    }

}

void ls_l(int mode_a){
   DIR *dir;
    struct dirent *entry;

    if(df_count == 1){
        char sep[] = "\n";

        //exception
        if(strcmp(dir_file[0],"~") == 0){
            if ((dir = opendir(__home_dir)) == NULL)
                perror("opendir() error");
            else 
            {
                char curr_path[MAX_BUFFER];
                char path[MAX_BUFFER];
                strcpy(curr_path,__home_dir);
                
                print_dir_list_l(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
            }
            return;
        }
        
        // calling func to chech what it is
        int st = get_stat(dir_file[0]);
        // printf("\t>stat = %d\n",st);
        
        // case one it is a file
        if(st == 1){
            char time_ext[18];
                char time[18];
                char * list[MAX_BUFFER];
                char curr_path[MAX_BUFFER];
                char path[MAX_BUFFER];

                get_pwd(curr_path);

                strcpy(path,curr_path);
                strcat(path,"/"); 
                strcat(path,dir_file[0]);

                // printf("\t>%s\n",path);

                struct stat statbuf;
                if (stat(path, &statbuf) != 0)
                return ;

                printf((S_ISDIR(statbuf.st_mode))? "d":"-");

                printf((statbuf.st_mode & S_IRUSR)? "r":"-");
                printf((statbuf.st_mode & S_IWUSR)? "w":"-");
                printf((statbuf.st_mode & S_IXUSR)? "x":"-");
                // printf("\n");
                printf((statbuf.st_mode & S_IRGRP)? "r":"-");
                printf((statbuf.st_mode & S_IWGRP)? "w":"-");
                printf((statbuf.st_mode & S_IXGRP)? "x":"-");
                // printf("\n");
                printf((statbuf.st_mode & S_IROTH)? "r":"-");
                printf((statbuf.st_mode & S_IWOTH)? "w":"-");
                printf((statbuf.st_mode & S_IXOTH)? "x":"-");

                printf(" %u",(unsigned int)statbuf.st_nlink);
                strcpy(time_ext,ctime(&statbuf.st_atime));
                time_ext[17] = '\0';

                for (size_t i = 4; i < 18; i++)
                {
                    time[i-4] = time_ext[i];
                }
                
                printf("\b\b %7ld",statbuf.st_size);
                printf(" %s",time);

                if(isDir(path)){
                    printf("\033[0;32m");
                    printf(" %s%s", dir_file[0],sep);
                    printf("\033[0m");

                }
                else if(isExe(path)){
                    printf("\033[0;33m");
                    printf(" %s%s", dir_file[0],sep);
                    printf("\033[0m");

                }
                else if(isFile(path)){
                    printf("\033[0;37m");
                    printf(" %s%s", dir_file[0],sep);
                    printf("\033[0m");

                }
                else {
                    printf("FUCKED ??!!!\n");
                }
            
        }

        // case two is a dir
        else if(st == 0){
        // printf("\t\t(1)\n");
            if ((dir = opendir(dir_file[0])) == NULL){
                perror("opendir() error");
            }

            else {
                char curr_path[MAX_BUFFER];
    
                get_pwd(curr_path);
                strcat(curr_path,"/"); 
                strcat(curr_path,dir_file[0]);
                
                print_dir_list_l(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
            }
        }

        //else doesnt exit
        else{
            perror("ERROR :given name is doesnt exit in dir\n");
        }


    }


    else if(df_count > 1){
        char sep[] = "\n";
    // printf("\t\t(2)\n");
        for (size_t i = 0; i < df_count; i++)
        {
            
            if(strcmp(dir_file[i],"~") == 0){
                printf("%s :\n",__home_dir);
                if ((dir = opendir(__home_dir)) == NULL){
                    perror("opendir() error");
                }
                else 
                {
                    char curr_path[MAX_BUFFER];
                    char path[MAX_BUFFER];
                    strcpy(curr_path,__home_dir);
                    
                    print_dir_list_l(dir,curr_path,entry,sep,mode_a);
                
                    closedir(dir);
                    printf("\n");
                    continue;
                }
            }
            
            // calling func to chech what it is
            int st = get_stat(dir_file[i]);
            // printf("\t>stat = %d\n",st);
            
            // case one it is a file
            if(st == 1){
                char time_ext[18];
                char time[18];
                char * list[MAX_BUFFER];
                char curr_path[MAX_BUFFER];
                char path[MAX_BUFFER];

                get_pwd(curr_path);

                strcpy(path,curr_path);
                strcat(path,"/"); 
                strcat(path,dir_file[i]);

                // printf("\t>%s\n",path);

                struct stat statbuf;
                if (stat(path, &statbuf) != 0)
                return ;

                printf((S_ISDIR(statbuf.st_mode))? "d":"-");

                printf((statbuf.st_mode & S_IRUSR)? "r":"-");
                printf((statbuf.st_mode & S_IWUSR)? "w":"-");
                printf((statbuf.st_mode & S_IXUSR)? "x":"-");
                // printf("\n");
                printf((statbuf.st_mode & S_IRGRP)? "r":"-");
                printf((statbuf.st_mode & S_IWGRP)? "w":"-");
                printf((statbuf.st_mode & S_IXGRP)? "x":"-");
                // printf("\n");
                printf((statbuf.st_mode & S_IROTH)? "r":"-");
                printf((statbuf.st_mode & S_IWOTH)? "w":"-");
                printf((statbuf.st_mode & S_IXOTH)? "x":"-");

                printf(" %u",(unsigned int)statbuf.st_nlink);
                strcpy(time_ext,ctime(&statbuf.st_atime));
                time_ext[17] = '\0';

                for (size_t i = 4; i < 18; i++)
                {
                    time[i-4] = time_ext[i];
                }
                
                printf("\b\b %7ld",statbuf.st_size);
                printf(" %s",time);

                if(isDir(path)){
                    printf("\033[0;32m");
                    printf(" %s%s", dir_file[i],sep);
                    printf("\033[0m");

                }
                else if(isExe(path)){
                    printf("\033[0;33m");
                    printf(" %s%s", dir_file[i],sep);
                    printf("\033[0m");

                }
                else if(isFile(path)){
                    printf("\033[0;37m");
                    printf(" %s%s", dir_file[i],sep);
                    printf("\033[0m");

                }
                else {
                    printf("FUCKED ??!!!\n");
                }
        
            }

            // case two is a dir
            else if(st == 0){
                printf("%s :\n",dir_file[i]);
                if ((dir = opendir(dir_file[i])) == NULL){
                    perror("opendir() error");
                }

                else {

                    char curr_path[MAX_BUFFER];
    
                    get_pwd(curr_path);
                    strcat(curr_path,"/"); 
                    strcat(curr_path,dir_file[i]);
                    
                    print_dir_list_l(dir,curr_path,entry,sep,mode_a);
                    
                    closedir(dir);
                }
            }

            //else doesnt exit
            else{
                perror("ERROR :given name is doesnt exit in dir\n");
            }

            printf("\n");
        }
        
    }
    

    else{
        // printf("\t\t(3)\n");
        char sep[] = "\n";

        if ((dir = opendir(".")) == NULL)
            perror("opendir() error");
        else {
                char curr_path[MAX_BUFFER];
    
                get_pwd(curr_path);
                
                print_dir_list_l(dir,curr_path,entry,sep,mode_a);
                
                closedir(dir);
        }
    }
 
}