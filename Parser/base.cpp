#include "parserPrivate.h"

int main()
{
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Expression parser...\n\n";
	std::cout << "/////////////////////////////////////////////////////////\n\n";
	std::cout << "Type an expression...or [q or Q] to quit\n\n";

	std::string str;
	bool error;
	while (std::getline(std::cin, str))
	{
		if (str.empty() || str[0] == 'q' || str[0] == 'Q')
			break;
		else if(!checkString(str))
			continue;

		monome input = parseMonome(str, error);

		if(!error)
			printMonome(input);
	}

	std::cout << "Bye... :-) \n\n";
	return 0;
}