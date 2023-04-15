#include "func_discover.h"
#include "func_ls.h"
#include "func_pwd.h"

#define MAX_word_count 1000
#define MAX_his_store 5
#define MAX_BUFFER 1000

char _home_dir[MAX_word_count];
char curr_dir[MAX_word_count];

char *_flags[MAX_word_count];

char *tar_dir_to_search_in;
char *file_name_to_find;

static int provided_file_name = 0;
static int provied_tar_dir = 0;

static int flag_count = 0;
int want_home_flag = 0;
int printing_dir = 0;
int layer_1 =0;

void discover_formating(char **cmd_words, int count_words, char *home)
{
    strcpy(_home_dir, home);
    strcpy(curr_dir, ".");

    // // geting current dir
    // get_pwd(curr_dir);

    int flag_d = 0;
    int flag_f = 0;
    int word_count = 0;
    // char **words = (char **) malloc(sizeof(char*) * MAX_word_count);
    char BUFFER[MAX_BUFFER];

    for (size_t i = 1; i < count_words; i++)
    {
        // printf("\n\n<%s>(%ld)\n",cmd_words[0],strlen(cmd_words[0]));

        if (cmd_words[i][0] == '-')
        {
            _flags[flag_count++] = cmd_words[i];
        }
        else if (cmd_words[i][0] == '\"' && cmd_words[i][strlen(cmd_words[i]) - 1] == '\"')
        {

            if (provided_file_name)
            {
                perror("ERROR : only one target file to be enter as argument\n");
                return;
            }

            char temp[MAX_BUFFER];
            for (size_t i = 1; i < strlen(cmd_words[i]) - 1; i++)
            {
                temp[i - 1] = cmd_words[i][i];
            }
            temp[strlen(cmd_words[i]) - 2] = '\0';
            strcpy(file_name_to_find, temp);
            provided_file_name = 1;
        }
        else if(cmd_words[i][0] == '\"' || cmd_words[i][strlen(cmd_words[i]) - 1] == '\"'){
            perror("ERROR : discover wrong inout form");
            return ;
        }
        else
        {
            if (provied_tar_dir)
            {
                printf("ERROR : only one target dir to be enter as argument\n");
            }
            tar_dir_to_search_in = cmd_words[i];
            provied_tar_dir = 1;
        }
    }
    // printf("---\n");
    for (size_t i = 0; i < flag_count; i++)
    {
        // printf("->%s.\n",_flags[i]);
        if (strcmp(_flags[i], "-f") == 0)
        {
            flag_f = 1;
        }
        else if (strcmp(_flags[i], "-d") == 0)
        {
            flag_d = 1;
        }
        else if (strcmp(_flags[i], "-df") == 0)
        {
            flag_f = 1;
            flag_d = 1;
        }
        else
        {
            printf("ERROR : \"%s\" flag is not recognised \n ", _flags[i]);
            return;
        }
    }

    int success = 0;

    // home case
    if (provided_file_name && strcmp(tar_dir_to_search_in, "~") == 0)
    {
        stpcpy(curr_dir, _home_dir);
        want_home_flag = 1;
    }
    else if (provied_tar_dir)
    {
        // printf("tar_dir = %s\n",tar_dir_to_search_in);
        if (tar_dir_to_search_in[0] != '.' && strlen(tar_dir_to_search_in) == 1)
        {
            strcat(curr_dir, "/");
            strcat(curr_dir, tar_dir_to_search_in);
        }

        if (tar_dir_to_search_in[0] == '.' && tar_dir_to_search_in[1] != '.')
        {
            strcpy(curr_dir,tar_dir_to_search_in);
        }
    }

    // printf("--> tar dir (%d)<%s>\n", provided_file_name,tar_dir_to_search_in);
    // printf("--> cur dir (%d)<%s>\n", provided_file_name,file_name_to_find);
    // printf("--> _flags (%d|%d)<%s>\n", flag_d,flag_f,curr_dir);

    layer_1 = 1;

    if(provided_file_name && provied_tar_dir)
    {
        success = discover(curr_dir, file_name_to_find, flag_d, flag_f);
    }
    else if(provied_tar_dir)
    {
        success = discover_without_target(curr_dir, flag_d, flag_f);
    }
    else if(flag_d || flag_f){
        success = discover_without_target(curr_dir, flag_d, flag_f);
    }
    else{
        success = discover_without_target(curr_dir, 1, 1);
    }

    if (!success)
    {
        printf("ERROR : discovere failed to execute correctly \n ");
    }

    // free all usless things
    flag_count = 0;
    provided_file_name = 0;
    want_home_flag = 0;
    printing_dir = 0;
    layer_1 = 0;
    flag_d = 0;
    flag_f = 0;
}

int discover(char *curr_dir, char *tar_file, int mode_d, int mode_f)
{
    // printf("in discovery 1 :\t>scanning in dir %s\n \t>looking for file %s\n ",curr_dir,tar_file);
    // printf("home1---\n");

    DIR *dir;
    struct dirent *entry;
    int success = 0;

    if ((dir = opendir(curr_dir)) == NULL)
    {
        perror("opendir() error");
        return 0;
    }

    else
    {
        char path[MAX_BUFFER];
        strcpy(path, curr_dir);
        char *list[MAX_BUFFER];
        int list_size = 0;

        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
                list[list_size++] = entry->d_name;
        }
        sort(list, list_size);
        for (size_t i = 0; i < list_size; i++)
        {
            char ind_path[MAX_BUFFER];

            strcpy(ind_path, path);
            strcat(ind_path, "/");
            strcat(ind_path, list[i]);

            if (isDir(ind_path))
            {
                discover(ind_path, tar_file, mode_d, mode_f);
            }
            else
            {
                if (strcmp(list[i], tar_file) == 0)
                    if (want_home_flag)
                    {
                        char temp[MAX_BUFFER] = "~";
                        // printf("> :<|%s|>\n",_home_dir,strlen(_home_dir));
                        // printf("> :<|%s|>\n",ind_path,strlen(ind_path));

                        for (size_t i = strlen(_home_dir); i < strlen(ind_path); i++)
                        {
                            temp[i - strlen(_home_dir) + 1] = ind_path[i];
                        }
                        // printf("= :<|%s|>\n",temp,strlen(temp));
                        printf(" %s\n", temp);
                    }
                    else
                        printf(" %s\n", ind_path);
            }
        }

        closedir(dir);
    }

    return 1;
}

int discover_without_target(char *curr_dir, int mode_d, int mode_f)
{
    // printf("in discovery 2 :\n\t>scanning in dir %s\n \t>looking for file %d\n ", curr_dir, printing_dir);

    DIR *dir;
    struct dirent *entry;
    int success = 0;

    if ((dir = opendir(curr_dir)) == NULL)
    {
        perror("opendir() error");
    }

    else
    {
        char path[MAX_BUFFER];
        strcpy(path, curr_dir);
        char *list[MAX_BUFFER];
        int list_size = 0;

        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
                list[list_size++] = entry->d_name;
        }
        sort(list, list_size);
        for (size_t i = 0; i < list_size; i++)
        {
            char ind_path[MAX_BUFFER];

            strcpy(ind_path, path);
            strcat(ind_path, "/");
            strcat(ind_path, list[i]);

            if (isDir(ind_path))
            {
                if(mode_d){
                    if(i == 0 && layer_1 && strcmp(tar_dir_to_search_in,".") == 0){
                        printf(" .\n");
                        layer_1 = 0;
                    }
                    printf(" %s\n", ind_path);
                }
                discover_without_target(ind_path,mode_d, mode_f);
            }
            else if(mode_f)
            {

                if (want_home_flag)
                {
                    printf("+++\n");
                    char temp[MAX_BUFFER] = "~";
                    // printf("> :<|%s|>\n",_home_dir,strlen(_home_dir));
                    // printf("> :<|%s|>\n",ind_path,strlen(ind_path));

                    for (size_t i = strlen(_home_dir); i < strlen(ind_path); i++)
                    {
                        temp[i - strlen(_home_dir) + 1] = ind_path[i];
                    }
                    // printf("= :<|%s|>\n",temp,strlen(temp));
                    printf(" %s\n", temp);
                }
                else
                    printf(" %s\n", ind_path);
            }
        }

        closedir(dir);
    }

    return 1;
}
