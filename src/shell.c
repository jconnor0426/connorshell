#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include "shell.h"
#include "parse.h"


char history[1000][512];
int curr_history = 0;

char buff[512];
int curr_pos;

int curr_cmd = 0;

void redraw_buff()
{
	int i;
	printf("\r");
	printf("%s ", PROMPT );

	for (i=0; i<MAX_LINE_LEN; i++) printf(" ");
	printf("\r");
	buff[curr_pos] = '\0';
	printf("%s %s", PROMPT, buff); fflush(0);
}

int main()
{
	char ch;
	struct termios save_termios;
	struct termios torestore_termios;
	int val;
	int curr_h;
	int start;
	parseInfo *parsedline;

	if (tcgetattr(STDIN_FILENO, &torestore_termios) < 0) /* get whatever is there */
		perror("getattr");

	if (tcgetattr(STDIN_FILENO, &save_termios) < 0) /* get whatever is there */
		perror("getattr");

	/* save_termios.c_lflag &= ~ICANON;*/ /* set non canonical mode */
	save_termios.c_lflag &= ~(ECHO | ICANON); /* set non canonical mode, turn off echo */
	save_termios.c_cc[VMIN] = 0;  /* one byte at a time */
	save_termios.c_cc[VTIME] = 0; /* no timer */

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &save_termios) < 0) 
		perror("setattr");

	curr_pos = 0;
	parsedline = NULL;
	start = 1;

	printf("%s ", PROMPT);fflush(0);
	while (1) {    
		if (read(STDIN_FILENO, &ch, 1) > 0) {
			start = 0;
			if (ch == '\n') {
				putchar(ch);
				fflush(0);
				buff[curr_pos ++] = '\0';
				strcpy(history[curr_history++], buff);
				curr_h = curr_history;
				curr_cmd++;

				/*Parse Commands Here*/
				parsedline = parse(buff);
				if(  parsedline )
				{
					print_info( parsedline );
					free_info( parsedline );
				}else
				{
					printf( "[main] error parsing line buffer\n");
				}
				/*Execute Parse Structure Here*/


				printf("%s ", PROMPT);fflush(0);
				curr_pos = 0;
			}else if (ch == 1) { /* ctrl-A */ 
				if (curr_pos > 0) {
					curr_pos --;
					redraw_buff();
				}
			} else if (ch == 16) { /* ctrl-P */
				if (curr_h > 0) {
					strcpy(buff, history[curr_h-1]);
					curr_h--;
					curr_pos = strlen(buff);
					redraw_buff();
				}
			} else if(curr_pos<MAX_LINE_LEN) {
				buff[curr_pos ++] = ch;
				putchar(ch);fflush(0);
			}else if( ch == 3 || ch == 4) {
				//Do nothing here for now. Maybe flash? 
				printf(" GOODBYE\n");
				break;
			}
		} else if( ch == EOF )
		{
			printf(" GOODBYE\n");
			break;
		}
	 }

	 //restore previous terminal attributes
	 if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &torestore_termios) < 0) 
		perror("setattr");

}








