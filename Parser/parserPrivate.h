#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include "parser.h"

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

enum
{
	TYPE_OP_INVALID = 0,
	TYPE_OP_ALLOC = 0x1,
	TYPE_OP_CALCUL = 0x2,
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
	bool insertSublevelEntry(Entity entry);
	bool setFunction(std::string name);
};

complexType getNumber(std::string string);
complexType combineComplexParser(complexType a, complexType b);
void printMonome(monome input);
bool isVariable(std::string input);

bool checkString(std::string input);
int syntaxAnalysis(std::string input);

monome parseMonome(std::string str, bool & error);