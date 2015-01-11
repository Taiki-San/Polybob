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

void printMonome(monome input)
{
	std::cout << "Success:\n  coefficient: ";
	if(input.coef.coefReal == 0 && input.coef.coefComplex == 0)
	{
		std::cout << "0\n";
		return;
	}

	else if(input.coef.coefComplex == 0)
		std::cout << input.coef.coefReal;

	else if(input.coef.coefReal == 0)
		std::cout << input.coef.coefComplex << 'i';

	else
		std::cout << '(' << input.coef.coefReal << '+' << input.coef.coefComplex << "i)";

	if(input.exponent > 1)
		std::cout << "x^" << input.exponent << '\n';
	else if(input.exponent == 1)
		std::cout << "x\n";
	else
		std::cout << '\n';
}