#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void ls_type(char ** cmd_words,int count_cmd_words,char * home);

void ls(int mode);
void ls_l(int mode);


// sortig alpahbetical order
int compare( const void *a, const void * b);
void sort(char* line[],int n);


int isDir(const char *path) ;
int isExe(const char *path) ;
int isFile(const char *path) ;


// returns 1 for file 0 for dir and -1 for error
int get_stat(char * dir_name);

