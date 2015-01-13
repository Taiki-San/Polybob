#include "parserPrivate.h"

int parserEntrypoint()
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
			entity.print(0);
		else
			std::cout << "Hum, something went wrong, sorry :/\n";
	}

	std::cout << "Bye... :-) \n\n";
	return 0;
}

int main()
{
	return parserEntrypoint();
}