#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "parse.h"
#include "commands.h"
#include "shell.h"

extern struct termios torestore_termios;
extern char** environ;

int run( parseInfo* parsedline )
{
	int rtstatus;
	int pid;
	int stat;
	int fd;
	int fd_pipe[2];
	int i;
	int input_fd, ouput_fd;

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
	else if( strncmp( parsedline->CommArray[0].command, "env", 3 ) == 0 ) //env Command
	{
		rtstatus = cmd_env( &(parsedline->CommArray[0]) );
	}
	else if( strncmp( parsedline->CommArray[0].command, "set", 3 ) == 0 ) //env Command
	{
		rtstatus = cmd_set( &(parsedline->CommArray[0]) );
	}else //Attempt to process the command
	{
		pid = fork();

		if( pid == 0 ) //Process Child Stuff
		{
			//Input Redirection
			if( parsedline->boolInfile )
			{
				fd = open( parsedline->inFile, O_RDONLY, 1 );
				if( fd != -1 )
				{
					dup2( fd, STDIN_FILENO );
					close( fd );
				}else
					perror( "failed opening file" );
			}
			//Output Redirection
			if( parsedline->boolOutfile )
			{
				fd = open( parsedline->outFile, O_RDWR | O_CREAT, 0666 );
				if( fd != -1 )
				{
					dup2( fd, STDOUT_FILENO );
					close( fd );
				}else
					perror( "failed opening file" );
			}

			
			printf( "[RUN] %d\n", parsedline->pipeNum );

			//If there are more than one pipes set them up
			if( parsedline->pipeNum )
			{
				input_fd = 0;	//Start with the first fd being STDIN

				for( i = 0; i < parsedline->pipeNum; i++ )
				{
					pipe( fd_pipe ); //Create the pipe
					ouput_fd = fd_pipe[1];

					if( fork() == 0) 
					{
						dup2( input_fd, STDIN_FILENO );
						close( input_fd);

						dup2( ouput_fd, STDOUT_FILENO );
						close( ouput_fd );

						execvp( parsedline->CommArray[i].command, parsedline->CommArray[i].VarList ); 

					}else
					{
						close( ouput_fd );
						input_fd = fd_pipe[0];
					}
				}

				dup2( input_fd, STDIN_FILENO );
				execvp( parsedline->CommArray[i].command, parsedline->CommArray[i].VarList ); 
				exit(0);
			}else
			{
				if(execvp( parsedline->CommArray[0].command, parsedline->CommArray[0].VarList ))
					perror("problem executing command" );
				exit(0);
			}
		}else
		{
			pid = wait( &stat );
		}
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

int cmd_env( struct commandType* cmd )
{
	int i = 0;
	char* current_var;
	
	current_var = environ[0];
	while( current_var )
	{
		printf( "%s\n", current_var);
		current_var = environ[++i];
	}

	return 0;
}

int cmd_set( struct commandType* cmd )
{
	char key[MAX_LINE_LEN];
	char value[MAX_LINE_LEN];
	int cur_pos;
	int i;
	int rtstatus;


	//if no arguments just return
	if( cmd->VarNum < 2 )
		return CMD_SUCCESS;

	//Just for the first argument
	//Get Key

	i = 0;
	cur_pos = 0;
	while( cmd->VarList[1][i] != '='  && cmd->VarList[1][i] != '\0'  )
	{
		key[cur_pos++] = cmd->VarList[1][i++];
	}

	if( cmd->VarList[1][i] == '\0' )
	{
		printf( "[SET] no value specified using empty string");
		value[0] = '\0' ;
	}else
	{
		i++; //Increment past the = sign
		cur_pos = 0;
		while( !isspace(cmd->VarList[1][i]) && cmd->VarList[1][i] != '\0'  )
		{
			value[cur_pos++] = cmd->VarList[1][i++];
		}	
	}

	rtstatus = setenv( key, value, 1 ); //key=value and 1 to set the overwrite
	if( rtstatus )
		perror( "setenv" );
	
	
	return CMD_SUCCESS;
}