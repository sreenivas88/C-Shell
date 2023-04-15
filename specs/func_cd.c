#include "func_cd.h"

#define MAX_BUFFER 1000

static char home_dir[MAX_BUFFER] ;
static char temp[MAX_BUFFER] ;
static char prev_dir[MAX_BUFFER] ;
static int first_time = 1;

int func_cd(char * dirctory_provided,char * ter){
    char BUFFER[MAX_BUFFER] ; 
    char curr_temp[MAX_BUFFER] ; 

    // expcetion for cd -
    int prev_time = 0;
    if(first_time){
        getcwd(BUFFER,MAX_BUFFER);
        strcpy(home_dir,BUFFER);
        strcpy(prev_dir,BUFFER);
        first_time = 0;

    } 

    if(dirctory_provided == NULL){
        
    }

    getcwd(curr_temp,MAX_BUFFER);
    // printf(" B>%s(%ld)\n",curr_temp,strlen(curr_temp));  
    // printf(" P<%s(%ld)\n\n",prev_dir,strlen(prev_dir));  
    // printf(" H=%s(%ld)\n\n",home_dir,strlen(home_dir));

    // printf(" H=%s(%ld)\n\n",dirctory_provided,strlen(dirctory_provided));

    getcwd(BUFFER,MAX_BUFFER);//stores the current dir before cd command exe


    // cd - does changes and pre_dir not  it ter
    if(strcmp(dirctory_provided,"-") == 0){

        strcpy(temp,BUFFER);
        strcpy(BUFFER,prev_dir);
        strcpy(prev_dir,temp);
        
        char a[] = "/";
        dirctory_provided = a;
        int flag = chdir(dirctory_provided); // go to root

        dirctory_provided = BUFFER; //then change the destination

        flag = chdir(dirctory_provided);

        if(flag < 0){
            return 0;
        }



        getcwd(BUFFER,MAX_BUFFER);

        prev_time = 1;

        if(flag < 0){
            return 0;
        }
    }
    // for noremal operaation for previos storing of dir
    else{
        getcwd(BUFFER,MAX_BUFFER);
        strcpy(prev_dir,BUFFER);
        // printf(" >%s(%ld)\n\n",BUFFER,strlen(BUFFER));  
        // printf(" =%s(%ld)\n\n",prev_dir,strlen(prev_dir));  
        // printf("HO%s(%ld)\n\n\n",home_dir,strlen(home_dir)); 
    }

    // for cd ~
    if(strcmp(dirctory_provided,"~") == 0){
        int flag = chdir("/");
        flag = chdir(home_dir);

        if(flag < 0){
            return 0;
        }
    }

    // normal dir ops
    else {
        int flag = chdir(dirctory_provided);

        if(flag < 0){
            return 0;
        }
    }

    int i_hom_dir = 0;
    int i_curr_dir = 0;

    getcwd(BUFFER,MAX_BUFFER); // stores current dir  after cd exe

    // to cahnge the command line ter

    if(strcmp(home_dir,BUFFER) == 0){
        for (int i = 0; i < strlen(ter) +1; i++)
        {

            if(ter[i] == ':'){
                i++;
                ter[i++] = '~';

                ter[i++] = '>';
                // i++;
                ter[i++] = '\0';
            }
        }
    }
    else{
       if(strlen(BUFFER) > strlen(home_dir)){
            // printf("1<%c><%d>\n",ter[i],compare_flag); //---------------------------------------------
            for (int i = 0; i < strlen(ter) + 1 ; i++)
            {

                if(ter[i] == ':'){
                    while (BUFFER[i_curr_dir++] == home_dir[i_hom_dir++]);
                    i++;
                    ter[i] = '~';
                    i++;
                    ter[i] = '/';
                    i++;
                    
                    do
                    {
                        ter[i++] = BUFFER[i_curr_dir++];
                    } while (i_curr_dir != strlen(BUFFER));

                    ter[i++] = '>';
                    ter[i++] = '\0';
                }
            }
        } 
        // else if(strlen(BUFFER) < strlen(home_dir)){
        else{
            for (int i = 0; i < strlen(ter) +1; i++)
            {

                if(ter[i] == ':'){
                    i_curr_dir = 0;
                    i++;

                    do
                    {
                        ter[i++] = BUFFER[i_curr_dir++];
                    } while (i_curr_dir != strlen(BUFFER));

                    ter[i++] = '>';
                    ter[i++] = '\0';
                }
            }
        }
        // else{
        // }
    }

    // printf("FUCKED!!!!!\n");
    return 1;
}