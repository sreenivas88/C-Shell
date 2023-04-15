#ifndef func_pinfo_H
#define func_pinfo_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

void pinfo_wrapper(char ** cmd_line,int index_of_str_start,int pid_given);

#endif