#include "interface.h"

#define RED  "\033[22;31m"
#define RESET  "\033[0m"

void completion(const char *buf, linenoiseCompletions *lc)
{
	if(buf[0] == '/' && buf[1] == 'h') 
	{   
		linenoiseAddCompletion(lc, "/help");
		linenoiseAddCompletion(lc, "/historylen");
	}
	else if(buf[0] == 'e')
	{
		linenoiseAddCompletion(lc, "exit");
	}
	else if(buf[0] == 'q')
	{
		linenoiseAddCompletion(lc, "quit");
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
	else if(!strncmp(command,"historylen",10))
	{
		int len = atoi(command+10);
		linenoiseHistorySetMaxLen(len);
	}
	else if(!strcmp(command,"fluffle"))
	{
		easterEgg();
	}
    else
        printf("'%s' : Unknown command\n", command);
}

void printLogo()
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int nbSpace = w.ws_col / 2  - 20;
	
    if(nbSpace < 0 && nbSpace > -15)
    {
        printf("%sPolyBob\n\n",RED);
    }
	else if(nbSpace < -15)
	{
		printf("42");
	}
    else
    {
		int i;
		char text[nbSpace + 1];
		
        for(i = 0 ; i < nbSpace ; i++)
            text[i] = ' ';
		
		text[nbSpace] = 0;
		
        printf("%s%s ____       _       ____        _\n%s|  _ \\ ___ | |_   _| __ )  ___ | |__\n%s| |_) / _ \\| | | | |  _ \\ / _ \\| '_ \\ \n%s|  __/ (_) | | |_| | |_) | (_) | |_) |\n%s|_|   \\___/|_|\\__, |____/ \\___/|_.__/\n%s              |___/                  \n%s", RED, text, text, text, text, text, text, RESET);
    }
    fflush(stdout);
}