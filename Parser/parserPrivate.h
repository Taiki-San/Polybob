#include <iostream>
#include <string>
#include <vector>
#include <complex>

typedef struct complex
{
	double coefReal;
	double coefComplex;
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

enum
{
	PREV_OP_NONE = 0,
	PREV_OP_MULT,
	PREV_OP_DIV
};

typedef struct monome
{
	complexType coef;
	int exponent;

} monome;

class Entity
{
	std::vector<monome> levelMember;
	std::vector<Entity> subLevel;

	bool isContainer;
	bool isFunction;

	std::string functionName;

	uint8_t previousOperator;

public:
	Entity();
	bool insertMonomeEntry(monome entry);
};

bool checkString(std::string input);
complexType getNumber(std::string string);
void printMonome(monome input);
complexType combineComplexParser(complexType a, complexType b);

monome parseMonome(std::string str, bool & error);