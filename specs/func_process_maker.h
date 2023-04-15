#ifndef proc_maker_H
#define proc_maker_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pwd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

int make_process(char ** cmd_words,int count_cmd_words,char * ter,int is_bg);
int exec_fg(pid_t pid);
void exec_bg(pid_t pid);
void exec_sig(pid_t pid, int sig);

int get_bg_count();
int get_pid_of_fg();
int get_list_ptr(char * list_par[],int proc_array_par[]);

int get_name_fg(char * dest);

int add_proc_in_bg(char *,int);


#endif