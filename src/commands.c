#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "parse.h"
#include "shell.h"

extern struct termios torestore_termios;

int run( parseInfo* parsedline )
{
	int rtstatus;
	//Check for Builtin Commands in the first pipe	
	if( strncmp( parsedline->CommArray[0].command, "exit", 4 ) == 0 ) //Exit Command
	{
		rtstatus = cmd_exit( &(parsedline->CommArray[0]) );
	}
	else if( strncmp( parsedline->CommArray[0].command, "pwd", 3 ) == 0 ) //pwd Command
	{
		rtstatus = cmd_pwd( &(parsedline->CommArray[0]) );
	}


	return rtstatus;
}
int cmd_exit( struct commandType* cmd )
{
	printf( "\r%s GOODBYE!\n\nprocess exited\n", PROMPT );
	//Restore Terminal Settings
	if( isatty( STDIN_FILENO) ){
		 //restore previous terminal attributes
		 if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &torestore_termios) < 0) 
			perror("setattr");
	}
	//Exit Process
	exit(0);

	//If this returns then exit somehow failed
	return -1;
}

int cmd_pwd( struct commandType* cmd )
{
	char pwd_buffer[256];
	if( getcwd( pwd_buffer, 256) )
		printf( "%s\n", pwd_buffer);
	else
		perror( "PWD FAIL");
	
	//After notifying the fail of getcwd don't pass the error up
	return 0;
}