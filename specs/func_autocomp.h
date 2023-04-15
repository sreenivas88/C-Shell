
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

void print_list(char * list[],char *cmd_line,char * ter);
void update_line(char * cmd_line,char * ter);

int auto_complete(char *cmd_line,char * ter);

// return 1 for list 0 for only match -1 for no match 
int search_for_matches(char * list[],char * cmd_line);



