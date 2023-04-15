#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void func_pwd();
void get_home_dir(char * dest);
void get_pwd(char * dest);
