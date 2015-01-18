#include "interface.h"
#include "wrapper.h"

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
	else
	{
		char * suggestion = getSuggestion(buf);
		if(suggestion != NULL)
		{
			linenoiseAddCompletion(lc, suggestion);
			free(suggestion);
		}
	}
}

void parseCommand(const char *command)
{
	if(!strcmp(command, "help"))
	{
		printf("Bienvenue dans l'asistance de Polybob!\n");
		printf("Notre syntaxe est la suivante, les opérateurs standards sont supportés\n");
		printf("Vous pouvez utiliser des variables en tapant {nom de la variable}\n");
		printf("Attention, les espaces seront ignorés\n\n");
		printf("Un certain nombre de fonctions sont supportés, vous pouvez les appeler de la manière suivante:\n");
		printf("nomDeLaFonction[argument1, argument2]\n\n");
		printf("Liste des fonctions:\n");
		printf("	- expand 	 : développe un polynôme\n");
		printf("	- factor 	 : factorise un polynôme\n");
		printf("	- evaluate 	 : évalue un polynome pour une valeur\n");
		printf("	- interpolate	 : calcul un polynôme passant par les valeurs réelles données\n");
		printf("	- composition	 : compose deux polynômes\n");
		printf("	- division	 : réalise la division euclidienne de deux polynômes\n");
		printf("			   Attention: / ne réalise pas une division euclidienne\n\n");
		printf("Nous espérons que Polybob pourra vous satisfaire.\n");
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