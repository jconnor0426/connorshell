cc=clang
cflags=
binfolder=../bin
target=$(binfolder)/cnrshl

shell: shell.c shell.h parse.o
	$(cc) -o $(target) shell.c parse.o

parse.o: parse.h parse.c
	$(cc) -c parse.c 
