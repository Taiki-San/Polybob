#include "parserPrivate.h"

int main()
{
	return parserEntrypoint();
}

int parserEntrypoint()
{
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Expression parser...\n\n";
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Type an expression...or [q or Q] to quit\n\n";

	std::string str;
	bool error;
	int operation;
	Entity entity;
	
	while (std::getline(std::cin, str))
	{
		if (str.empty() || str[0] == 'q' || str[0] == 'Q')
			break;
		
		//We remove every spaces, as they are no-op
		str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
		
		if(!checkString(str))
			continue;
		else if((operation = syntaxAnalysis(str)) == TYPE_OP_INVALID)
		{
			std::cerr << "Invalid request";
			continue;
		}

		entity = parserCore(str, operation, error);

//		if(!error)
//			printMonome(input);
	}

	std::cout << "Bye... :-) \n\n";
	return 0;
}