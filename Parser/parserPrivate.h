#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <algorithm>
#include "parser.h"

#define VERBOSE
#define SPIRIT_DEFAULT_POWER_VALUE 0

#pragma mark Enums

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
	PREV_OP_SUM
};

enum
{
	TYPE_OP_INVALID = 0,
	TYPE_OP_ALLOC = 0x1,
	TYPE_OP_CALCUL = 0x2,
};

#pragma mark Structures

typedef struct complex
{
	double coefReal;
	double coefComplex;
}complexType;

typedef struct monome
{
	complexType coef;
	int exponent;

} monome;

#pragma mark Entity

class Entity
{
	std::vector<Entity> subLevel;

	bool isContainer;
	bool isFunction;

	std::string functionName;
	
public:
	
	monome _monome;

	uint8_t previousOperator;
	int power;
	bool initialized;
	
	Entity();
	bool setMonome(monome entry);
	bool setSublevel(std::vector<Entity> entry);
	bool setFunction(std::string name);
	
	void updatePowerOfLast(int power);
	
	bool isReal() const;
	
	void print(uint depth)	const;
	void print()		const;
	void printMonome()	const;
};

#pragma mark - Declarations -
#pragma mark Parser core

Entity parserCore(std::string input, int opType, bool & error);
Entity _parseEntity(std::string level, bool & error);
std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions, bool & error);
monome parseMonome(std::string str, bool & error);

#pragma mark Parser utils

bool havePlusOnLevel(std::string level, std::vector<uint> & positions);
bool haveMultOnLevel(std::string level, std::vector<uint> & positions);
bool isFunction(std::string level, std::string functionName, bool & error);
uint8_t getPreviousOP(char operand);

#pragma mark Spirit utils

complexType getNumber(std::string string);
complexType combineComplexParser(complexType a, complexType b);

#pragma mark Sanitization

bool checkString(std::string input);
int syntaxAnalysis(std::string input);

