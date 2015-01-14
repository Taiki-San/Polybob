#include "wrapper.h"

using namespace std;

char* rmSuperscript(char* line)
{
	string str = line;
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
