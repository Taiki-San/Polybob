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
		complex.coefComplex = value;
	else
		complex.coefReal = value;

	return complex;
}