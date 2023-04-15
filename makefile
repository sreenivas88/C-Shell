CC=gcc
CFLAGS=-g -trigraphs

all: a.out clean


#--------------------------------------
a.out:main.o func_cd.o func_echo.o func_pwd.o display_req.o func_history.o func_ls.o func_discover.o func_process_maker.o func_pinfo.o func_autocomp.o display_jobs.o
	$(CC) $(CFLAGS) main.o func_cd.o func_echo.o func_pwd.o display_req.o func_history.o func_ls.o func_discover.o func_process_maker.o  func_pinfo.o func_autocomp.o display_jobs.o -o main

main.o: main.c 
	$(CC) $(CFLAGS)  -c main.c


func_cd.o: ./specs/func_cd.c ./specs/func_cd.h
	$(CC) $(CFLAGS)  -c ./specs/func_cd.c

display_req.o: ./specs/display_req.c ./specs/display_req.h
	$(CC) $(CFLAGS)  -c ./specs/display_req.c

func_echo.o: ./specs/func_echo.c ./specs/func_echo.h
	$(CC) $(CFLAGS)  -c ./specs/func_echo.c

func_pwd.o: ./specs/func_pwd.c ./specs/func_pwd.h
	$(CC) $(CFLAGS)  -c ./specs/func_pwd.c

func_history.o: ./specs/func_history.c ./specs/func_history.h
	$(CC) $(CFLAGS)  -c ./specs/func_history.c


func_ls.o: ./specs/func_ls.c ./specs/func_ls.h
	$(CC) $(CFLAGS)  -c ./specs/func_ls.c

func_discover.o: ./specs/func_discover.c ./specs/func_discover.h
	$(CC) $(CFLAGS)  -c ./specs/func_discover.c

func_process_maker.o: ./specs/func_process_maker.c ./specs/func_process_maker.h
	$(CC) $(CFLAGS)  -c ./specs/func_process_maker.c

func_pinfo.o: ./specs/func_pinfo.c ./specs/func_pinfo.h
	$(CC) $(CFLAGS)  -c ./specs/func_pinfo.c

func_autocomp.o: ./specs/func_autocomp.c ./specs/func_autocomp.h
	$(CC) $(CFLAGS)  -c ./specs/func_autocomp.c

display_jobs.o: ./specs/display_jobs.c ./specs/display_jobs.h
	$(CC) $(CFLAGS)  -c ./specs/display_jobs.c

#--------------------------------------


clean: 
	-rm -f *.o 

cleanmain:
	-rm *.out