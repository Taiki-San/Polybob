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
	OP_NONE		= 1 << 0,
	OP_POWER	= 1 << 1,
	OP_MULT		= 1 << 2,
	OP_DIV		= 1 << 3,
	OP_PLUS		= 1 << 4,
	OP_MINUS	= 1 << 5

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

	uint functionCode;
	
	bool checkArgumentConsistency(bool & error) const;
	
public:
	
	//Public content
	std::vector<monome> polynome;
	monome _monome;

	//Relationship
	uint8_t previousOperator;
	int power;
	bool initialized;

	//Setters
	Entity();
	bool setMonome(monome entry);
	bool setSublevel(std::vector<Entity> entry);
	bool setFunction(uint functionCode);
	
	void updatePowerOfLast(int power);
	
	//Getter
	uint getType() const;
	bool isReal() const;
	bool isFactorisedPoly() const;

	//IO
	void print(uint depth)	const;
	void print()		const;
	void printMonome()	const;
	
	//Maturation
	void maturation(bool & error);
	void executeFunction(bool & error);
	bool isMature;
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
bool isFunction(std::string level, uint & functionCode, bool & error);
void separateFunctionArgs(std::string level, std::vector<uint> & positions);
uint8_t getPreviousOP(char operand);

#pragma mark Spirit utils

complexType getNumber(std::string string);
complexType combineComplexParser(complexType a, complexType b);

#pragma mark Sanitization

bool checkString(std::string input);
int syntaxAnalysis(std::string input);

