#include "parserPrivate.h"

complexType getNumber(std::string string)
{
	complexType complex;
	size_t size = string.size();
	double value;
	char haveI = 0;
	bool isComplex;

	if(string.at(0) == 'i')
	{
		haveI++;
		string.erase(0, 1);
		size--;
	}

	if(string.at(size-1) == 'i')
	{
		haveI++;
		string.erase(size-1, 1);
		size--;
	}

	if(string.at(0) == '[')
	{
		value = 0;
	}
	else
		value = atof(string.c_str());

	if(haveI == 1)
		isComplex = true;
	else
	{
		isComplex = false;
		if(haveI == 2)
			value *= -1;
	}

	if(isComplex)
	{
		complex.coefReal = 0;
		complex.coefComplex = value;
	}
	else
	{
		complex.coefReal = value;
		complex.coefComplex = 0;
	}

	return complex;
}

complexType combineComplexParser(complexType a, complexType b)
{
	if(a.coefReal == 0 && b.coefReal == 0)
	{
		a.coefReal = a.coefComplex * b.coefComplex;
		a.coefComplex = 0;
	}

	else if(a.coefComplex == 0 && b.coefComplex == 0)
		a.coefReal *= b.coefReal;

	else if(a.coefReal == 0)
		a.coefComplex *= b.coefComplex;

	else
	{
		a.coefComplex = a.coefReal * b.coefComplex;
		a.coefReal = 0;
	}

	return a;
}

//Parser utils
bool havePlusOnLevel(std::string level, std::vector<uint> & positions)
{
	bool inBlock = false, haveFoundSomething = false;
	char exitChar = 0, currentChar, previousChar = 0;
	uint index = 0;
	
	for(std::string::const_iterator currentCharIt = level.begin(); currentCharIt != level.end(); ++currentCharIt, index++)
	{
		currentChar = *currentCharIt;
		
		if(inBlock)
		{
			if(currentChar != exitChar)
				continue;
			else
				inBlock = false;
		}
		else if(currentChar == '+' || currentChar == '-' || (currentChar == '^' && previousChar != 'x'))
		{
			haveFoundSomething = true;
			positions.push_back(index);
		}

		//Ignored blocks
		else if(currentChar == '(')
		{
			inBlock = true;
			exitChar = ')';
		}
		else if(currentChar == '[')
		{
			inBlock = true;
			exitChar = ']';
		}
		else if(currentChar == '{')
		{
			inBlock = true;
			exitChar = '}';
		}

		previousChar = currentChar;
	}
	
	if(haveFoundSomething)
		positions.push_back((uint)level.length());
	
	return haveFoundSomething;
}

bool haveMultOnLevel(std::string level, std::vector<uint> & positions)
{
	bool inBlock = false, haveFoundSomething = false, indexedLastRun = false;
	char exitChar = 0, currentChar, previousChar = 0;
	uint index = 0;
	
	for(std::string::const_iterator currentCharIt = level.begin(); currentCharIt != level.end(); ++currentCharIt, index++)
	{
		indexedLastRun = false;
		currentChar = *currentCharIt;
		
		if(inBlock)
		{
			if(currentChar != exitChar)
				continue;
			else
			{
				inBlock = false;
				if(exitChar == ')')		//The equation may end with this, and if so, we don't want to add an other marker afterward
				{
					haveFoundSomething = true;
					indexedLastRun = true;
					positions.push_back(index);
				}
			}
		}
		else if(currentChar == '*' || currentChar == '/')
		{
			haveFoundSomething = true;
			positions.push_back(index);
		}
		else if(currentChar == '^' && previousChar != 'x')
		{
			haveFoundSomething = true;
			positions.push_back(index);			
		}
		else if(currentChar == '(')
		{
			inBlock = true;
			exitChar = ')';
			haveFoundSomething = true;
			positions.push_back(index);
		}
		
		//Ignored blocks
		else if(currentChar == '[')
		{
			inBlock = true;
			exitChar = ']';
		}
		else if(currentChar == '{')
		{
			inBlock = true;
			exitChar = '}';
		}
		
		previousChar = currentChar;
	}
	
	if(haveFoundSomething && !indexedLastRun)
		positions.push_back((uint)level.length());
	
	return haveFoundSomething;
}

bool isFunction(std::string level, uint & functionCode, bool & error)
{
	//Not finishing as a function
	if(level[level.length()-1] != ']')
		return false;
	
	//Containing several function
	size_t index = level.find(']');
	
	//We may contain several functions, but we're not sure, let's check
	if(index != level.length() - 1)
	{
		char c;
		bool stillMayBeInVar = true;
		while((c = level[++index]) != ']')
		{
			//We enter a new variable name (proving we were indeed the end of an early function) or a new function (proving that there is more than one function)
			if((stillMayBeInVar && c == '{') || (!stillMayBeInVar && c == '['))
				return false;
			
			//Ok, we were in a variable name, but there still could be a function after us (eg: f[{lol]42}]+2g[1337])
			else if(c == '}')
				stillMayBeInVar = false;
		}
		
		//We never encountered any var, so there was indeed two+ functions
		if(stillMayBeInVar)
			return false;
		
		//Okay, some dumbass named a variable with a ] inside, congrats...
	}

	index = level.find('[');
	
	level = level.substr(0, index);
	
	//We have our function name, let's see if it does exist
	functionCode = Catalog::getIDOfFunction(level);
	
	if(functionCode == INVALID_FUNCTION_ID)
		return false;
	
	return true;
}

uint8_t getPreviousOP(char operand)
{
	switch (operand)
	{
		case '+':
			return OP_PLUS;
			
		case '-':
			return OP_MINUS;
			
		case '/':
			return OP_DIV;
			
		case '^':
			return OP_POWER;
	}
	return OP_MULT;
}