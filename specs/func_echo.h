#ifndef func_echo_H
#define func_echo_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

void echo(char ** cmd_words,int count);

#endif