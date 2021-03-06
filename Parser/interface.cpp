#include "parserPrivate.h"

Entity parserCore(std::string input)
{
	input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
	
	checkString(input);
	
	return _parserCore(input, syntaxAnalysis(input));
}

void finalProcessing()
{
	MUTEX_DESTROY;
}

#pragma mark - API

void simpleParserAPI(char * cString)
{
	std::string string = cString;
	Entity input;
	
	try
	{
		input = parserCore(string);
	}
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}
}

bool haveVariableSuggestion(std::string begining, std::string & suggestion)
{
	return Catalog::haveVariableSuggestion(begining, suggestion);
}
