#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "parse.h"

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
	else if( strncmp( parsedline->CommArray[0].command, "cd", 2 ) == 0 ) //cd Command
	{
		rtstatus = cmd_cd( &(parsedline->CommArray[0]) );
	}

	return rtstatus;
}
int cmd_exit( struct commandType* cmd )
{
	printf( "GOODBYE!\n\nprocess exited\n" );
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

int cmd_cd( struct commandType* cmd )
{
	int rtstatus;
	char* home;

	//if arguments are 1 ( just the command) then change directory to ../
	if( cmd->VarNum > 1 )
	{
		printf( "[CMD_CD] Attempting to move to %s\n", cmd->VarList[1] );
		rtstatus = chdir( cmd->VarList[1] );
		if( rtstatus )
			perror( "CD FAILED" );
	}
	else //if arguments are more than 1 just use the path in the second argument
	{
		home = getenv("HOME");
		printf( "[CMD_CD] Attempting to move to %s\n", home );
		rtstatus = chdir( home);
		if( rtstatus)
			perror( "CD FAILED");	
	}	

	//Errors handled within this function
	return 0;
}