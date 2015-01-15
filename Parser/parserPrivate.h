#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <complex>
#include <algorithm>
#include "../parser.h"
#include "sharedWithCore.h"

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
	FCODE_EXPAND		= 0,
	FCODE_FACTOR		= 1,
	FCODE_EVALUATE		= 2,
	FCODE_INTERPOLATE	= 3,
	FCODE_COMPOSITION	= 4,
	FCODE_TEST			= 5,
	FCODE_DUMP			= 6,
	INVALID_FUNCTION_ID = UINT_MAX
} FUNCTION_CODES;

enum
{
	FARG_TYPE_POLY_NOFACT 	= 1 << 0,
	FARG_TYPE_FACTORISED	= 1 << 1,
	FARG_TYPE_POLY			= FARG_TYPE_POLY_NOFACT | FARG_TYPE_FACTORISED,
	FARG_TYPE_REAL			= 1 << 2,
	FARG_TYPE_COMPLEX		= 1 << 3,
	FARG_TYPE_NUMBER		= FARG_TYPE_REAL | FARG_TYPE_COMPLEX,
	FARG_TYPE_DIV_RESULT	= 1 << 4
};

enum
{
	TYPE_OP_INVALID = 0,
	TYPE_OP_ALLOC = 0x1,
	TYPE_OP_CALCUL = 0x2,
};

#pragma mark Structures

typedef struct Monomial monome;

#pragma mark Entity

class Entity
{
	std::vector<Entity> subLevel;

	bool isContainer;
	bool isFunction;

	uint functionCode;
	
	void migrateType(uint8_t newType, Polynomial & finalPoly, PolynomialFact & finalFact, Complex::complexN & finalNumber);
	bool checkArgumentConsistency(bool & error) const;
	
public:
	
	//Public content
	monome _monome;
	std::vector<monome> polynome;
	
	//Mature
	uint8_t matureType;
	Polynomial polynomePure;
	PolynomialFact polynomeFact;
	Complex::complexN numberPure;
	divResult divisionResult;

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
	bool monomeCouldBePartFactorisedPoly(uint index) const;
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

#pragma mark - Catalog

struct variableMetadata
{
	uint8_t type;
	uint index;
};

typedef struct variableContainer
{
	Polynomial polynomial;
	PolynomialFact polynomialFact;
	Complex::complexN number;
	
	uint8_t type;
} VARIABLE;

class Catalog
{
	std::string cacheRaw, cacheName;
	bool haveCache;
	
	std::vector<uint> functionCodes;
	std::vector<std::string> functionNames;
	std::vector<uint> functionArgumentNumber;
	std::vector<std::vector<uint>> functionArgumentType;
	std::vector<uint> functionReturnType;
	
	std::vector<std::string> variableNames;
	std::vector<VARIABLE> variableContent;
	
	Catalog();
	Catalog(Catalog const&);

	static bool findSuggestion(std::string begining, std::vector<std::string> source, std::string & suggestion);

public:
	static Catalog& Instance();
	
	static uint getIDOfFunction(std::string name);
	static uint getNbArgsForID(uint ID);
	static std::string getFunctionName(uint ID);
	static std::vector<uint> getArgumentType(uint ID);
	static uint getFuncReturnType(uint ID);
	
	//Variable tools
	static bool variableName(std::string input, std::string & variableName);
	static bool isVariable(std::string input);
	
	//Auto-completion
	static bool haveVariableSuggestion(std::string begining, std::string & suggestion);
	static bool haveFunctionSuggestion(std::string begining, std::string & suggestion);
	
	//Variable manipulation
	static VARIABLE variableValue(std::string variableName, bool & error);
	static void setVariableValue(std::string variableName, VARIABLE content);
	
	static void registerCache(std::string rawInput, std::string variableName);
	static bool getCache(std::string & raw, std::string & name);
};

#pragma mark - Declarations -
#pragma mark Parser core

Entity _parserCore(std::string input, int opType, bool & error);
Entity _parseEntity(std::string level, bool canDiv, bool & error);
std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions, bool canDiv, bool & error);
Entity parseMonome(std::string str, bool & error);

#pragma mark Parser utils

bool havePlusOnLevel(std::string level, std::vector<uint> & positions);
bool haveMultOnLevel(std::string level, std::vector<uint> & positions);
bool isFunction(std::string level, uint & functionCode, bool & error);
void separateFunctionArgs(std::string level, std::vector<uint> & positions);
uint8_t getPreviousOP(char operand);

#pragma mark Spirit utils

VARIABLE getNumber(std::string string, bool & error);

#pragma mark Sanitization

bool checkString(std::string input);
int syntaxAnalysis(std::string input);

