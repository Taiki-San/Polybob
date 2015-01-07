#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "interface.h"

int main(int argc, char **argv)
{
	char *line;
	char *prgname = argv[0];
	unsigned int promptNb = 1;
	char promptMsg[100];

	printLogo();

	/* Set the completion callback. This will be called every time the
	 * user uses the <tab> key. */
	linenoiseSetCompletionCallback(completion);

	/* Load history from file.*/
	linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

	snprintf(promptMsg, 100, "%s[%d]: ", "\033[0m", promptNb); 
	while((line = linenoise(promptMsg)) != NULL)
	{    
		linenoiseHistoryAdd(line); /* Add to the history. */
		linenoiseHistorySave("history.txt"); /* Save the history on disk. */
		/* Do something with the string. */
		if(line[0] == '/')
		{
			parseCommand(line+1);    
		}
		else if(!strcmp(line, "exit"))
		{
			exit(0);
		}
		else if(line[0] != '\0')
		{
			printf("echo: '%s'\n", line);
		}

		free(line);
		promptNb++;
		snprintf(promptMsg, 100, "[%d]: ", promptNb);
	}
	return 0;
}
