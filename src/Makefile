cc=clang
cflags=
binfolder=../bin
target=$(binfolder)/cnrshl

shell: shell.c shell.h parse.o command.o
	$(cc) -o $(target) shell.c parse.o commands.o

command.o: parse.o commands.c commands.h
	$(cc) -c commands.c

parse.o: parse.h parse.c
	$(cc) -c parse.c 
