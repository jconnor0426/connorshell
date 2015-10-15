#ifndef COMMANDS_H
#define COMMANDS_H

#include "parse.h"

//RETURN CODES
#define CMD_SUCCESS 0



int run( parseInfo* parsedline );

/*Builtin Commands*/
int cmd_exit( struct commandType* cmd );
int cmd_pwd( struct commandType* cmd );
int cmd_cd( struct commandType* cmd );
int cmd_set( struct commandType* cmd );
int cmd_env( struct commandType* cmd );

#endif