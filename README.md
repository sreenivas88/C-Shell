# C shell

Submitted by:
Sreenivas B Papireddy


## Run the code :

Note: The makefile makes a ./exe which is the executable.

> make

- This will result in an executable with the name `main`.

> ./exe

## Directory structure of the code:

```
    - exe
    > MakeFile
    - specs
        > func_cd.c
        > func_cd.h
        > func_ls.c
        > func_ls.h
        > func_history.c        
        > func_history.h
        > func_pinfo.c
        > func_pinfo.h
        > func_pwd.c
        > func_pwd.h
        > func_process_maker.c        
        > func_process_maker.h
        > func_discover.c
        > func_discover.h
        > func_echo.c
        > func_echo.h
        > display_req.c
        > display_req.h
    - storage
        > history.txt
        > no_child.txt    
    > exe.c
```
#### features:

- basic features 
    - showing current directory
    - show run time of the foreground command
    - cd
    - ls (with -a -l -al -la)
    - pwd
    - echo
    - history (gets restored on next run)

- System commands like sleeps, emacs, vim, ls, cat, etc. 

- Advanced features
    - Auto Suggestion !!!!!!!!(😤)
    - discover is a custom discover command which emulates the basics of the find command. The command should search for files in a directory hierarchy
    - pinfo 
    - redirection of input and output for commands from file and vis versa
    - piping
    - jobs, sig, bg, fg
    - signal handling of Ctl + D , Ctl + Z , Ctl + C
    - autocomplete tab is special char to activate it and will either give list if multiple options present or aut fills the line


---

The End

---