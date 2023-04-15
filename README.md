# OSN Assignment 3

Submitted by:
Sreenivas B Papireddy
2021111007


## Run the code :

Note: The makefile makes a ./exe which is the executable.

> make

- This will result in an executable with the name `exe`.

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

## Utils:

have competed specfication 1 for redirection of input and output for commands from file and vis versa

>supports ">" ">>" and "<"

>supports "|" along with input output redirection

>implemented jobs sig bg and fg

>singal handling of Ctl + D , Ctl + Z , Ctl + C

>autocomplete tab is sp[ecial char to activate it and will either give list if multiple options present or aut fills the line

---

The End

---