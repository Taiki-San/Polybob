#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "linenoise.h"

#define RED  "\033[22;31m"
#define RESET  "\033[0m"

void completion(const char *buf, linenoiseCompletions *lc)
{
	if(buf[0] == '/' && buf[1] == 'h') 
	{   
		linenoiseAddCompletion(lc,"/help");
		linenoiseAddCompletion(lc,"/historylen");
	}
}

void parseCommand(const char *command)
{
	if(!strcmp(command, "help"))
	{
		char txt[500] = "";
		FILE *help = fopen("HELP", "r");
		if(help == NULL)
			printf("Unable to open help\n");

		while(fgets(txt, 500, help) != NULL)
		{
			printf("%s", txt);
			fflush(stdout);
		}
		printf("\n");
		fclose(help);
	}
	else if (!strncmp(command,"historylen",10))
	{
		int len = atoi(command+10);
		linenoiseHistorySetMaxLen(len);
	}
	else
		printf("'%s' : Unknown command\n", command);
}

int main(int argc, char **argv)
{
	char *line;
	char *prgname = argv[0];
	unsigned int promptNb = 1;
	char promptMsg[100];
	int i;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int nbSpace = (w.ws_col/2)-20;
	char lol[8000] = ""; 
	//printf("%d",nbSpace);
	if(nbSpace<0)
	{
		printf("%sPolyBob\n\n",RED);
	}
	else
	{
		for(i = 0 ; i < nbSpace ; i++)
		{
			strcat(lol, " ");
		}
		printf("%s%s ____       _       ____        _\n%s|  _ \\ ___ | |_   _| __ )  ___ | |__\n%s| |_) / _ \\| | | | |  _ \\ / _ \\| '_ \\ \n%s|  __/ (_) | | |_| | |_) | (_) | |_) |\n%s|_|   \\___/|_|\\__, |____/ \\___/|_.__/\n%s              |___/                  \n%s", RED, lol, lol, lol, lol, lol, lol, RESET);
	}
	fflush(stdout);

	/* Parse options, with --multiline we enable multi line editing. */
	while(argc > 1)
	{
		argc--;
		argv++;
		if(!strcmp(*argv,"--multiline"))
		{
			linenoiseSetMultiLine(1);
			printf("Multi-line mode enabled.\n");
		}
		else if(!strcmp(*argv,"--keycodes"))
		{
			linenoisePrintKeyCodes();
			exit(0);
		}
		else
		{
			fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
			exit(1);
		}
	}

	/* Set the completion callback. This will be called every time the
	 * user uses the <tab> key. */
	linenoiseSetCompletionCallback(completion);

	/* Load history from file. The history file is just a plain text file
	 * where entries are separated by newlines. */
	linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

	/* Now this is the main loop of the typical linenoise-based application.
	 * The call to linenoise() will block as long as the user types something
	 * and presses enter.
	 *
	 * The typed string is returned as a malloc() allocated string by
	 * linenoise, so the user needs to free() it. */
	snprintf(promptMsg, 100, "%s[%d]: ", RESET, promptNb); 
	while((line = linenoise(promptMsg)) != NULL)
	{    
		linenoiseHistoryAdd(line); /* Add to the history. */
		linenoiseHistorySave("history.txt"); /* Save the history on disk. */
		/* Do something with the string. */
		if(line[0] == '/')
		{
			parseCommand(line+1);    
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
