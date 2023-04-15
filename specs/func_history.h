#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

void store_in_histroy(char ** cmd_line,int count,char * storage_path);
void print_history(char * storage_path);