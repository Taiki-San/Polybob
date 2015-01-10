#include "parserPrivate.h"

complexType getNumber(std::string string, bool negative, bool &a)
{
	complexType complex;
	char haveI = 0;

	size_t size = string.size();
	double value;

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
		complex.isComplex = true;
	else
	{
		complex.isComplex = false;
		if(haveI == 2)
			value *= -1;
	}

	if(negative)
		value *= -1;

	complex.coefficient = value;
	return complex;
}
