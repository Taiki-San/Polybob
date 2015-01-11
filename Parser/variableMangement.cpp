#include "parserPrivate.h"

Variables& Variables::Instance()
{
	static Variables instance;
	
	return instance;
}

Variables::Variables()
{
	//constructeur normal
}

bool Variables::variableName(std::string input, std::string & variableName)
{
	std::string::const_iterator iterator = input.begin();
	size_t posStart = 0, length = 0, curPos;
	bool entered = false, left = false;
	
	for(; iterator != input.end(); ++iterator, ++curPos)
	{
		switch (*iterator)
		{
			case ' ':
				break;
				
			case '[':
			{
				if(entered)
					return false;
				
				entered = true;
				posStart = curPos + 1;
			}
				
			case ']':
			{
				left = true;
				length = curPos - posStart;
			}
				
			default:
			{
				if(!entered || left)
					return false;
			}
		}
	}
	
	if(*iterator != '[')
		return false;
	
	if(length == 0)
		return false;
	
	variableName = input.substr(posStart, length);
	return true;
}

bool Variables::isVariable(std::string input)
{
	std::string unused;
	return Variables::variableName(input, unused);
}