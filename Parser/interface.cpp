#include "parserPrivate.h"

Entity parserCore(std::string input, bool & error)
{
	int opType = TYPE_OP_INVALID;
	
	input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
	
	if(!checkString(input) || (opType = syntaxAnalysis(input)) == TYPE_OP_INVALID)
	{
		error = true;
		Entity empty;
		return empty;
	}

	return _parserCore(input, opType, error);
}

#if 0

int main()
{
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Expression parser...\n\n";
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Type an expression...or [q or Q] to quit\n\n";
	
	std::string str;
	Entity entity;
	
	while (std::getline(std::cin, str))
	{
		if (str.empty() || str[0] == 'q' || str[0] == 'Q')
			break;
		
		//We remove every spaces, as they are no-op
		bool error = false;
		
		entity = parserCore(str, error);
		
		if(!error)
			entity.print();
		else
			std::cout << "Hum, something went wrong, sorry :/\n";
	}
	
	std::cout << "Bye... :-) \n\n";
	return 0;
}

#endif

#pragma mark - API

void simpleParserAPI(char * cString)
{
	bool error;
	std::string string = cString;
	
	Entity input = parserCore(string, error);
	
	if(!error)
		input.print();
	else
		std::cout << "Hum, something went wrong, sorry :/\n";
}

bool haveVariableSuggestion(std::string begining, std::string & suggestion)
{
	return Catalog::haveVariableSuggestion(begining, suggestion);
}

bool haveFunctionSuggestion(std::string begining, std::string & suggestion)
{
	return Catalog::haveFunctionSuggestion(begining, suggestion);
}