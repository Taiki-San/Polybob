#include "parserPrivate.h"

//Perform basic consistency checks, ensure parenthesis and brackets are properly set, and that there is no illegal operator combinaison
void checkString(std::string input)
{
	std::string::const_iterator iterator = input.begin();
	std::vector<int> parenthesisCountInBracket;
	int parenthesisCount = 0, bracketCount = 0;
	char last = 0, equalCount = 0;
	bool inconsistency = false, inBraces = false;
	
	if(*iterator == '+' || *iterator == '*' || *iterator == '/' || *iterator == '^' || *iterator == '=')
		inconsistency = true;

	//Initial check, we will simply check coherency in (), [] and incompatible operators
	for(; iterator != input.end() && !inconsistency; ++iterator)
	{
		if(inBraces)
		{
			if(*iterator == '{')
				inconsistency = true;
			else if(*iterator == '}')
			{
				inBraces = false;
				last = '}';
			}

			continue;
		}

		switch(*iterator)
		{
			case '{':
			{
				inBraces = true;
				break;
			}

			case '(':
			{
				if(bracketCount == 0)
				{
					parenthesisCount++;

					if(last == '^')
						inconsistency = true;
					else
						last = '(';
				}
				else
					parenthesisCountInBracket[bracketCount - 1]++;

				break;
			}

			case ')':
			{
				if(bracketCount == 0)
				{
					if(--parenthesisCount < 0)
						inconsistency = true;

					else if(last != 0)
						inconsistency = true;

					else
						last = ')';
				}
				else
				{
					if(--parenthesisCountInBracket[bracketCount - 1] < 0)
						inconsistency = true;
				}

				break;
			}

			case '[':
			{
				if(last != 0)
					inconsistency = true;
				else
				{
					bracketCount++;
					parenthesisCountInBracket.insert(parenthesisCountInBracket.end(), 0);
					last = '[';
				}
				break;
			}

			case ']':
			{
				if(--bracketCount < 0 || parenthesisCountInBracket[bracketCount] != 0)
					inconsistency = true;
				else
				{
					parenthesisCountInBracket.pop_back();
					last = ']';
				}

				break;
			}
				
			case ',':
			{
				if(!bracketCount || last == ',')
					inconsistency = true;
				else
					last = ',';

				break;
			}
				
			case '=':
			{
				if((!equalCount || (equalCount < 2 && last == '=')) && bracketCount == 0 && parenthesisCount == 0)
				{
					equalCount++;
					last = '=';
				}
				else
					inconsistency = true;
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
				if(last != 0 && last != ')')
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
		std::stringstream error;

		if(parenthesisCount < 0)
			error << "Invalid parenthesis () combinaison";
		else if(bracketCount < 0 || bracketCount > 1)
			error << "Invalid bracket [] combinaison";
		else if(inBraces)
			error << "Invalid brace {} combinaison";
		else if(last == '=')
			error << "Invalid use of equality, only 1 equal per equation, or two together to perform comparaison";
		else
			error << "Invalid operand combinaison: " << last << " before " << *--iterator;

		throw std::invalid_argument(error.str());
	}
	else if(last != 0 && last != '}' && last != ']' && last != ')')	//Obvious invalid end
	{
		throw std::invalid_argument("Invalid end of the request");
	}
}

int syntaxAnalysis(std::string input)
{
	size_t index;
	
	//We can't end on a =, so we can safely access the next letter
	if((index = input.find('=')) != -1)
	{
		if(input.at(index + 1) == '=')	//Comparaison
			return TYPE_OP_COMPARE;

		else
		{
			std::string substring = input.substr(0, index);
			if(Catalog::isVariable(substring))
				return TYPE_OP_ALLOC;
		}
		
		throw std::invalid_argument("Invalid request");
	}
	
	return TYPE_OP_CALCUL;
}

