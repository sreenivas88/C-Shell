#ifndef dis_H
#define dis_H

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

void discover_formating(char ** cmd_line,int index_to_continue_from,char *home);
int discover(char * curr_dir,char * tar_file,int mode_d,int mode_f);
int discover_without_target(char * curr_dir,int mode_d,int mode_f);

#endif