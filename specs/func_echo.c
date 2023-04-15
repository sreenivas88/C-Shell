#include "func_echo.h"

#define MAX_word_count 1000
#define MAX_BUFFER 1000



void echo(char ** cmd_words,int count){   
    // and printing it directly
    for (size_t i = 1; i < count; i++)
    {
        printf("%s ",cmd_words[i]);
    }
    printf("\n");
}
