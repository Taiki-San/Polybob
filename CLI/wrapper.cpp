#include "wrapper.h"

char* rmSuperscript(char* line)
{
	std::string str = line;
	char key[][4] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"}, replacement[3];
	int iter;

	replacement[0] = '^';
	replacement[2] = 0;

	for (int j = 0; j < 10; ++j)
	{
		replacement[1] = '0' + j;
		while((iter = str.find(key[j])) != -1)
		{
			str.replace(iter, (j > 3 ? 3 : 2), replacement);		
		}
	}
	
	free(line);
	return strdup(str.c_str());
}

char * getSuggestion(const char * begining)
{
	printf("\nGo\n");
	bool firstPass = true, noVar = false, noFunc = false;
	uint length = strlen(begining);
	
	while(length-- > 0 && (!noVar || !noFunc))
	{
		if(!noVar && begining[length] == '{')
		{
			std::string string = &(begining[length+1]), output;
			
			if(haveVariableSuggestion(string, output))
			{
				return strdup(output.c_str());
			}
			else
				noVar = true;
			
			return NULL;
		}
		else if(!noFunc && (begining[length] < 'a' || begining[length] > 'z'))
		{
			if(!firstPass)
			{
				std::string string = &(begining[length+1]), output;
				
				if(haveFunctionSuggestion(string, output))
				{
					return strdup(output.c_str());
				}
			}
			
			noFunc = true;
		}
		
		firstPass = false;
	}
	
	return NULL;
}
