#include<stdio.h>
#include<assert.h>
#include<string.h>
#include <pwd.h>
#include <fcntl.h> 
#include <termios.h>
#include <ctype.h>

#include"./specs/display_req.h"
#include"./specs/func_pwd.h"
#include"./specs/func_echo.h"
#include"./specs/func_cd.h"
#include"./specs/func_ls.h"
#include"./specs/func_pinfo.h"
#include"./specs/func_discover.h"
#include"./specs/func_process_maker.h"
#include"./specs/func_autocomp.h"
#include"./specs/display_jobs.h"




#define MAX_user_name 100
#define MAX_ter 1000
#define MAX_nam 100

#define MAX_BUFFER 1000
#define MAX_cmd_line 1000
#define MAX_word_count 1000

int dbg = 0;
int was_fg_executed = 0;
char * ter;

int is_pipe = 0;
int was_pipe_executed = 0;

int terminate = 0;



extern int make_process(char ** cmd_words,int count_cmd_words,char * ter,int is_bg);

// extern int make_process(char * cmd_line,char *storge_file,char *storge_children);


extern struct passwd *display();
extern void ls_type(char ** cmd_words,int count_cmd_words,char * home);


extern void discover_formating(char ** cmd_line,int index_to_continue_from,char *home);

extern void func_pwd();
extern void check_wether_proc_bf_or_fg();

extern void echo(char ** cmd_line,int count);
extern int func_cd(char * dirctory_provided,char * ter);

extern void store_in_histroy(char ** cmd_line,int count,char * storage_path);
extern void print_history(char * storage_path);
extern void pinfo_wrapper(char ** cmd_line,int index_of_str_start,int pid_given);

struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */


// signals for spec  
void ctlChandler(){
    // printf("\n|CTL+C|\n");
    // int fg_pid = get_pid_of_fg();

    // if(fg_pid != -1){
    //     kill(fg_pid,SIGINT);
    //     printf(" [%d] fg is killed\n",fg_pid);
    // }
    return;
}

void ctlZhandler(){

    pid_t fg_pid = get_pid_of_fg();

    if(fg_pid == -1){
        return;
    }
    // printf("|CTL+Z|");

    kill(fg_pid,SIGSTOP);

    char name[MAX_BUFFER];
    get_name_fg(name);

    add_proc_in_bg(name,fg_pid);

    return;
}

int inputHandling(char *piece,char **cmd_words,int count_cmd_words,char *storage_path,char *home_dir,char *ter,int is_bg){
    char pipe1[MAX_BUFFER];
    char pipe2[MAX_BUFFER];


    int sub_proc = 1;
    int time_taken_for_fg = 0;


    int out_rederict = 0;
    int prev_stdout;
    
    int in_rederict = 0;
    int prev_stdin;
    
    int flag = 0;

    int new_min_read_word_count = count_cmd_words;

    for (size_t i = 0; i < count_cmd_words; i++)
    {
        int append = 0;
        if(cmd_words[i][0] == '>' && (strlen(cmd_words[i]) == 2 || strlen(cmd_words[i]) == 1) ){
            
            if(strcmp(cmd_words[i],">>") == 0){
                append = O_APPEND;
            }
            else if(strcmp(cmd_words[i],">") == 0){
                append =  O_TRUNC;
            }
            else{
                perror("Case not handdled for < without matchong\n\n");
                break;
            }

            // printf(">> : |%d|\n",append);

            out_rederict = open(cmd_words[i+1], O_CREAT | append | O_WRONLY ,0644);
            if(out_rederict < 0){
                perror("ERROR: out redirst failed");
                return 0;
            }

            prev_stdout = dup(STDOUT_FILENO);
            dup2(out_rederict,STDOUT_FILENO);

            new_min_read_word_count = (i < new_min_read_word_count) ? i : new_min_read_word_count;

        }

        else if(strcmp(cmd_words[i],"<") == 0){

            // fprintf(stdout,"found < symbol\n");
            
            in_rederict = open(cmd_words[i+1], O_RDONLY  ,0644);
            if(in_rederict < 0){
                perror("ERROR: out redirst failed");
                return 0;
            }
            prev_stdin = dup(STDIN_FILENO);
            dup2(in_rederict,STDIN_FILENO);
            new_min_read_word_count = (i < new_min_read_word_count) ? i : new_min_read_word_count;
        }

    }

    count_cmd_words = new_min_read_word_count;


    // doing pipe redirecton 
    strcpy(pipe1,storage_path);
    strcpy(pipe2,storage_path);
    strcat(pipe1,"/pipe1.txt");
    strcat(pipe2,"/pipe2.txt");

    if(was_pipe_executed){
        in_rederict = open(pipe2, O_RDONLY  ,0644);
        if(in_rederict < 0){
            perror("ERROR: out redirst failed");
            return 0;
        }
        prev_stdin = dup(STDIN_FILENO);
        dup2(in_rederict,STDIN_FILENO);
        printf("f\b");
    }

    if(is_pipe){
        out_rederict = open(pipe1, O_CREAT | O_TRUNC | O_WRONLY ,0644);
        if(out_rederict < 0){
            perror("ERROR: out redirst failed");
            return 0;
        }

        prev_stdout = dup(STDOUT_FILENO);
        dup2(out_rederict,STDOUT_FILENO);
    }


    if(0){
        for(size_t i = 0 ; i < count_cmd_words; i++)
        {
            printf("(%ld)<%s>\n",i,cmd_words[i]);
        }
        printf("\b\n\n\n");
        printf("pipe is piped from %d\n",is_pipe);
        printf("pipe is piped from %d\n",was_pipe_executed);
    }
    

    char BUFFER[MAX_BUFFER] ;  
    char temp[MAX_BUFFER] ;  
    
    if(count_cmd_words == 0){
        return 0;
    }

    else if(strcmp(cmd_words[0],"pwd") == 0 && count_cmd_words == 1){
        func_pwd();
    }

    else if(strcmp(cmd_words[0],"cd") == 0){
        if(count_cmd_words ==  1){
            return 0;
                        // strcpy(cmd_words[count_cmd_words++],".");
        }

        else if(count_cmd_words > 2){
            printf("give at least dir after cd command  \\¯_((⇀‸↼))_¯/\n");
        }

        int flag = func_cd(cmd_words[1],ter);

        if(flag == 0){
            printf("ERROR : Someting worng with your input dir for cd(☉ʖ☉)\n");
        }
    }

    else if(strcmp(cmd_words[0],"echo") == 0){
        echo(cmd_words,count_cmd_words);
    }

    else if(strcmp(cmd_words[0],"history") == 0){
        strcpy(temp,storage_path);
        strcat(temp,"/history.txt");
        
        print_history(temp);

    }

    else if(strcmp(cmd_words[0],"jobs") == 0){
        jobs(cmd_words,count_cmd_words);
    }

    else if(strcmp(cmd_words[0],"bg") == 0 && count_cmd_words == 2){
        int pid = atoi(cmd_words[1]);
        func_bg(pid);
    }

    else if(strcmp(cmd_words[0],"fg") == 0  && count_cmd_words == 2){
        int pid = atoi(cmd_words[1]);
        int flagf = func_fg(pid);

        if(flagf >= 0){

            // printf("\\t---\t<%s>is fg (%d) nad time returned (%d)----\n\n",piece,is_bg,flag);
            // forgrong was executed and time has been stored
            was_fg_executed = 1;
            time_taken_for_fg = flagf;
        }
    }

    else if(strcmp(cmd_words[0],"sig") == 0  && count_cmd_words == 3){
        int pid = atoi(cmd_words[1]);
        int sig = atoi(cmd_words[2]);
        func_sig(pid,sig);
    }  

    else if(strcmp(cmd_words[0],"ls") == 0){
        ls_type(cmd_words,count_cmd_words,home_dir);
    }

    else if(strcmp(cmd_words[0],"discover") == 0){
        discover_formating(cmd_words,count_cmd_words,home_dir);
    }

    else if(strcmp(cmd_words[0],"q") == 0 && count_cmd_words == 1){
        terminate = 1;
    }

    else if(strcmp(cmd_words[0],"pinfo") == 0){
    // printf("+.\n");

        if(count_cmd_words == 2){
    // printf("+.\1n");

            int pid_given = atoi(cmd_words[1]);
            // printf("%s -> %d",temp,pid_given);
            pinfo_wrapper(cmd_words,count_cmd_words,pid_given);
        }
        else if(count_cmd_words == 1){
            pinfo_wrapper(cmd_words,count_cmd_words,-1);
        }
        else{
            perror("ERROR : follow pinfo format\n");
        }
        
    }

    else{           
        strcpy(temp,storage_path);
        strcat(temp,"/run_time.txt");
        strcpy(BUFFER,storage_path);
        strcat(BUFFER,"/no_child.txt"); // finding the storage    

        for (size_t i = 0; i < strlen(piece); i++)
        {
            if(piece[i] == '>' || piece[i] == '<'){
                piece[i] = '\0';
                break;
            }
        }
                        
        
        {        
            
            FILE * f =  fopen(BUFFER,"w");
            fprintf(f,"%d",0);
            fclose(f);
            
            // flag = make_process(piece,temp,BUFFER);
            flag = make_process(cmd_words,count_cmd_words,ter,is_bg);
        }

        
        // printf("exe ----%s---------> %d\n-\n",word,flag);

        if(flag >= 0){

            // forgrong was executed and time has been stored
            was_fg_executed = 1;
            time_taken_for_fg = flag;
        }
        else{
            flag = 0;
        }
        
        // if(flag <= 0)
        //     printf("dont know what you are saying ¯\\_(ツ)_/¯\n");
        

    }

    // cloasing and changing all the file to their original I/O streams
    
    if(out_rederict) {
        close(out_rederict);
        dup2(prev_stdout, 1);
        close(prev_stdout);
    }

    // fprintf(stdout,">%d\n",in_rederict);
    if(in_rederict) {
        close(in_rederict);
        dup2(prev_stdin, 0);
        close(prev_stdin);
    }

    if(is_pipe){
        was_pipe_executed = 1;
        is_pipe = 0;

        FILE *fptr1, *fptr2;

        fptr1 = fopen(pipe1, "r");
        if (fptr1 == NULL)
        {
            printf("Cannot open file %s \n", pipe1);
            exit(0);
        }
        // Open another file for writing
        fptr2 = fopen(pipe2, "w");
        if (fptr2 == NULL)
        {
            printf("Cannot open file %s \n", pipe2);
            exit(0);
        }
    
        // Read contents from file
        int c = fgetc(fptr1);
        while (c != EOF)
        {
            fputc(c, fptr2);
            c = fgetc(fptr1);
        }
    
        fclose(fptr1);
        fclose(fptr2);
    }
    else {
        was_pipe_executed = 0;
    }

   
            // printf("\\t---\t<%s>is fg  nad time returned (%d)----\n\n",piece,flag);
    return time_taken_for_fg;
}

void print_promt(char * ter,int time_taken_for_fg){
    
    // printing the ter at start

    printf("\033[0;36m"); //Set the text to the color red
    printf("%s",ter);

    // if fg was executed in last cmd
    if(was_fg_executed){
        was_fg_executed = 0;
        printf("\b |took %d| >",time_taken_for_fg);
    }
    printf("\033[0m"); // reset back to original color
}

// assuming cmd is already allocated
int get_cmd_words(char * cmd_line,char ** cmd_words){

    int word_count = 0; 
    char BUFFER[MAX_BUFFER] ; 

    for (int i = 0; i < strlen(cmd_line); i++)
    {

        if(cmd_line[i] == '\t' || cmd_line[i] == ' '){
            continue;
        }

        int word_size =0;  
        if(dbg){
            printf("\t\t\t----%c(%d)\n",cmd_line[i],i);
        }
        if(cmd_line[i] == '>' && cmd_line[i+1] == '>'){
            BUFFER[word_size++] = cmd_line[i];   
            BUFFER[word_size++] = cmd_line[i]; 
            i++;  
        }

        else if(cmd_line[i] == ';' || cmd_line[i] == '&' || cmd_line[i] == '>' || cmd_line[i] == '<' || cmd_line[i] == '|'){
            BUFFER[word_size++] = cmd_line[i];   
        }

        else{
            do
            {
                if(dbg){
                    printf("\t\t-_-%c(%d)\n",cmd_line[i],i);        
                }
                
                if(cmd_line[i] == ';' || cmd_line[i] == '&' || cmd_line[i] == '|'){
                    i--;
                    break;
                }

                BUFFER[word_size++] = cmd_line[i++];   
            } 
            while (cmd_line[i] != '\t' && cmd_line[i] != ' ' && i < strlen(cmd_line));
        }



        BUFFER[word_size++] = '\0';
        
        char *word = (char *) malloc(sizeof(char) * word_size);
        strcpy(word,BUFFER);
        cmd_words[word_count++] = word;

        // printf("\n\n<%s>(%ld)\n",word,strlen(word));

        if(i >= strlen(cmd_line)){
            break;
        }
    }        



    return word_count;
    
}

int main(){
    signal(SIGCHLD,check_wether_proc_bf_or_fg);
    signal(SIGINT,ctlChandler); // clt + c
    signal(SIGTSTP,ctlZhandler); // clt + z
    

    // for clear the screen
    printf("\e[1;1H\e[2J");

    char BUFFER[MAX_BUFFER] ;  
    char temp[MAX_BUFFER] ;  

    char *home_dir = (char *) malloc(sizeof(char) * MAX_ter);
    char *storage_path = (char *) malloc(sizeof(char) * MAX_ter);
    char *storage_path_hist = (char *) malloc(sizeof(char) * MAX_ter);
    ter = (char *) malloc(sizeof(char) * MAX_ter);
    char * nam = (char *) malloc(sizeof(char) * MAX_nam);
    
    int time_taken_for_fg = 0;

    //user name and home dir set
    char user_name[MAX_user_name] ;

    struct passwd * detail = display();
    assert(detail != NULL);
    strcpy(user_name,detail->pw_name);
    
    getcwd(BUFFER,MAX_BUFFER);
    strcpy(home_dir,BUFFER);

    strcpy(storage_path,home_dir);
    strcat(storage_path,"/storage");

    strcpy(storage_path_hist,storage_path);
    strcat(storage_path_hist,"/history.txt");


    // seting the default line

    gethostname(nam,MAX_nam);

    strcpy(ter,"<");
    strcat(ter,user_name);
    strcat(ter,"@");
    strcat(ter,nam);
    strcat(ter,":~>");


    assert(home_dir != NULL );
    assert(ter != NULL );
    assert(storage_path != NULL );
    assert(user_name != NULL );
    assert(storage_path_hist != NULL );

    if(dbg){
        printf("--------------------------\n");
        printf("home %s\n",home_dir);
        printf("storge %s\n",storage_path);
        printf("ter %s\n",storage_path_hist);
        printf("user %s\n",user_name);
        printf("ter %s\n",ter); 
    }

    //an infintie loop ----------------------------------
    while (!terminate)
    {
        print_promt(ter,time_taken_for_fg);
        time_taken_for_fg = 0; // resetting time


        // taking input stored in buffer
        char ch;
        char * cmd_line;
        int BUFFER_size = 0;
        int sep_BUFFER_size = 0;

        // raw mode impelmentation!!!!!!!!!
        enableRawMode();

        // reading the input to shell
        do
        {
            scanf("%c",&ch);

            if (iscntrl(ch)) {
                if (ch == 127) { // backspace
                    if (BUFFER_size > 0) {
                        if (BUFFER[BUFFER_size-1] == 9) {
                            for (int i = 0; i < 7; i++) {
                                printf("\b");
                            }
                        }
                        BUFFER[--BUFFER_size] = '\0';
                        printf("\b \b");
                    }
                } 
                else if (ch == 9) { // TAB character
                   // impelenmt auto complete
                //    printf("<|TAB!|>");
                   BUFFER[BUFFER_size++] = '\0';

                   BUFFER_size = auto_complete(BUFFER,ter);
                }
                else if (ch == 10) { // end character
                   printf("\n");
                }
                else if (ch == 4) { // end character
                   printf("\n");
                   exit(0);
                }
                else{
                    // printf("<|%d|>", ch);
                }

            }
            else{
                if(ch != '\n'){
                    BUFFER[BUFFER_size++] = ch;
                }
                printf("%c", ch);
            }





        } while (ch != '\n');
        BUFFER[BUFFER_size++] = '\0';

        // raw mode impelmentation!!!!!!!!!
        disableRawMode();


        cmd_line = (char * )malloc(sizeof(char)* MAX_cmd_line);
        char * token_line = (char * )malloc(sizeof(char)* MAX_cmd_line);
        
        strcpy(cmd_line,BUFFER);
        strcpy(token_line,BUFFER);

        char ** hist_words = (char ** )malloc(sizeof(char *)* MAX_word_count);

        int total_count_words_in_cmd_line = get_cmd_words(cmd_line,hist_words);


        if(0){
            for(size_t i = 0 ; i < total_count_words_in_cmd_line; i++)
            {
                printf("(%ld)<%s>\n",i,hist_words[i]);
            }
            printf("\b\n\n\n");
        }
        


        // starting the reading fo the line----
        char * piece;
        char sep[] = ";&|";
        int i_word_count = 0;
        

        while ((piece = strtok_r(token_line,sep,&token_line))){
            
            int len = strlen(piece);
            int is_bg = 0;

            char ** cmd_words = (char ** )malloc(sizeof(char *)* MAX_word_count);
            int count_cmd_words = get_cmd_words(piece,cmd_words);

            i_word_count += count_cmd_words;


            if(i_word_count < total_count_words_in_cmd_line){
                               
                if(strcmp(hist_words[i_word_count],"&") == 0 ){
                    is_bg = 1;
                    i_word_count++;
                }
                else if(strcmp(hist_words[i_word_count],"|") == 0 ){
                    is_pipe = 1;
                    i_word_count++;
                }
                // printf("is_bg inbn cmd line = %d\ncompatrrd (%s)<%d>\n\n",is_bg,hist_words[i_word_count],i_word_count);
            }


            time_taken_for_fg += inputHandling(piece,cmd_words,count_cmd_words,storage_path,home_dir,ter,is_bg);
        
            // freeing the words gerated-------------------------------------
            for (size_t i = 0; i < count_cmd_words; i++){free(cmd_words[i]);}
            free(cmd_words);
        }

        store_in_histroy(hist_words,total_count_words_in_cmd_line,storage_path_hist);
        // freeing the temp files for hist store
        for (size_t i = 0; i < total_count_words_in_cmd_line; i++){free(hist_words[i]);}
        free(hist_words);
        
        is_pipe = 0;
        was_fg_executed = 0;

    }
    // loop ends ----------------------------------------
    









     // part end free all mallocs---------------------
    free(home_dir);
    free(ter);
    free(nam);
    free(storage_path);

}


