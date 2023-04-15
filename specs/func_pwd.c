#include "func_pwd.h"

#define MAX_BUFFER 1000

static int is_stored = 0;
char home_dir[MAX_BUFFER];

void func_pwd(){
    char BUFFER[MAX_BUFFER];
    // str = (char*)malloc(sizeof(char)*MAX_BUFFER);
    getcwd(BUFFER,MAX_BUFFER);
    printf("%s\n",BUFFER);               

}

void get_home_dir(char * dest)
{
    printf("<|%d> : \"%s\"\n",is_stored,home_dir);
    if(!is_stored){
        printf("reloaded\n");
        getcwd(home_dir,MAX_BUFFER);
        is_stored = 1;
    }
    strcpy(dest,home_dir);
    printf("<|%d> : \"%s\"\n",is_stored,home_dir);
}

void get_pwd(char * dest)
{
    char BUFFER[MAX_BUFFER];
    getcwd(BUFFER,MAX_BUFFER);
    strcpy(dest,BUFFER);
}