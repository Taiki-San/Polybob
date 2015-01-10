#include <iostream>
#include <string>
#include <complex>

typedef struct complex
{
	double coefficient;
	bool isComplex;
}complexType;

enum
{
	OP_NONE,
	OP_POWER,
	OP_MULT,
	OP_DIV,
	OP_PLUS,
	OP_MINUS

} OPERATORS;

bool checkString(std::string input);
void parseMonome(std::string str, bool & error);