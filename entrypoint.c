#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include <iostream>
#include <string>
#include "parser.h"
#include "CLI/wrapper.h"

#include "CLI/linenoise.h"
#include "CLI/interface.h"

#define HIST_FILENAME ".polyBobHistory"

int main(int argc, char **argv)
{
	char* line;
	unsigned int promptNb = 1;
	char promptMsg[100];
	bool end = false;
	
	srand(time(NULL));
    printLogo();

	/* Set the completion callback. This will be called every time the
	 * user uses the <tab> key. */
	linenoiseSetCompletionCallback(completion);

	/* Load history from file.*/
	linenoiseHistoryLoad(HIST_FILENAME); /* Load the history at startup */

	snprintf(promptMsg, 100, "%s[%d]: ", "\033[0m", promptNb); 
	while(!end && (line = linenoise(promptMsg)) != NULL)
	{    
		linenoiseHistoryAdd(line); /* Add to the history. */
		linenoiseHistorySave(HIST_FILENAME); /* Save the history on disk. */
		/* Do something with the string. */
        rmSuperscript(line);

		if(line[0] == '/')
			parseCommand(&(line[1]));

		else if(!strcmp(line, "exit") || !strcmp(line, "quit"))
			end = true;

		else if(line[0] != '\0')
			simpleParserAPI(line);

		free(line);
		snprintf(promptMsg, 100, "[%d]: ", ++promptNb);
	}
	return 0;
}