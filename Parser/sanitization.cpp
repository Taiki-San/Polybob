#include "parserPrivate.h"

//Perform basic consistency checks, ensure parenthesis and brackets are properly set, and that there is no illegal operator combinaison
bool checkString(std::string input)
{
	std::string::const_iterator iterator;
	int parenthesisCount = 0, bracketCount = 0, braceCount = 0;
	char last = 0;
	bool inconsistency = false;

	//Initial check, we will simply check coherency in (), [] and incompatible operators
	for(iterator = input.begin(); iterator != input.end() && !inconsistency; ++iterator)
	{
		switch(*iterator)
		{
			case '(':
			{
				parenthesisCount++;

				if(last == '^')
					inconsistency = true;
				else
					last = '(';

				break;
			}

			case ')':
			{
				if(--parenthesisCount < 0)
					inconsistency = true;

				else if(last != 0)
					inconsistency = true;

				else
					last = ')';

				break;
			}

			case '{':
			{
				if(++bracketCount > 1)
					inconsistency = true;

				last = '{';

				break;
			}

			case '}':
			{
				if(--bracketCount < 0)
					inconsistency = true;
				else
					last = '}';
				break;
			}

			case '[':
			{
				if(last != 0)
					inconsistency = true;
				else
				{
					braceCount++;
					last = '[';
				}
				break;
			}

			case ']':
			{
				if(--braceCount < 0)
					inconsistency = true;
				else
					last = ']';
			}


			case '*':
			case '/':
			{
				if(last == '+' || last == '-' || last == '^' || last == '(' || last == '[')
					inconsistency = true;
				else
					last = *iterator == '*' ? '*' : '/';
				break;
			}

			case '-':
			{
				if(last == '+')
					inconsistency = true;
				else
					last = '-';
				break;
			}

			case '+':
			case '^':
			{
				if(last != 0)
					inconsistency = true;
				else
					last = *iterator == '+' ? '+' : '^';
				break;
			}

			default:
			{
				last = 0;
			}
		}
	}

	if(inconsistency)
	{
		if(parenthesisCount < 0)
			std::cout << "Invalid parenthesis () combinaison\n";
		else if(bracketCount < 0 || bracketCount > 1)
			std::cout << "Invalid bracket [] combinaison\n";
		else if(braceCount < 0)
			std::cout << "Invalid brace {} combinaison\n";
		else
			std::cout << "Invalid operand combinaison: " << last << " before " << *--iterator << '\n';
	}

	return !inconsistency;
}