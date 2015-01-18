#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <complex>
#include <algorithm>

#include <unistd.h>

#include "../parser.h"
#include "sharedWithCore.h"

#define SPIRIT_DEFAULT_POWER_VALUE 0

#pragma mark - Multithreading

extern pthread_mutex_t mutex;
#define MUTEX_LOCK pthread_mutex_lock(&mutex)
#define MUTEX_UNLOCK pthread_mutex_unlock(&mutex)
#define MUTEX_DESTROY pthread_mutex_destroy(&mutex)

void createNewThread(void *function, void *arg);

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
	FCODE_DIVISION		= 5,
	MAX_FUNC_ID			= FCODE_DIVISION,
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
	FARG_TYPE_ANY			= FARG_TYPE_POLY | FARG_TYPE_NUMBER,
	FARG_TYPE_DIV_RESULT	= 1 << 4
};

enum
{
	TYPE_OP_ALLOC 			= 0x1,
	TYPE_OP_CALCUL 			= 0x2,
	TYPE_OP_CALCUL_SILENT 	= 0x3,
	TYPE_OP_COMPARE 		= 0x4
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

	void migrateType(uint8_t newType, Polynomial & finalPoly, PolyFact & finalFact, Complex::complexN & finalNumber);
	bool checkArgumentConsistency();

public:

	//Public content
	monome _monome;
	std::vector<monome> polynome;

	//Mature
	uint8_t matureType;
	Polynomial polynomePure;
	PolyFact polynomeFact;
	Complex::complexN numberPure;
	divResult divisionResult;

	//Relationship
	uint8_t previousOperator;
	int power;
	bool initialized;

	bool polyfactOfDegree0;
	bool functionArg;

	//Setters
	Entity();
	bool setMonome(monome entry);
	bool setSublevel(std::vector<Entity> entry);
	bool setFunction(uint functionCode);

	void updatePowerOfLast(int power);
	void resetToOne();

	//Getter
	uint getType() const;
	bool isReal() const;
	bool monomeCouldBePartFactorisedPoly(uint index) const;
	bool isFactorisedPoly() const;

	//IO
	void print()		const;
	void printMonome()	const;

	//Maturation
	void maturation(short threadID);
	void executeFunction();
	bool isMature;
};

#pragma mark - Catalog

typedef struct variableContainer
{
	Polynomial polynomial;
	PolyFact PolyFact;
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
	std::vector<std::vector<uint> > functionArgumentType;
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

	//Variable manipulation
	static VARIABLE variableValue(std::string variableName);
	static void setVariableValue(std::string variableName, VARIABLE content);

	static void registerCache(std::string rawInput, std::string variableName);
	static bool getCache(std::string & raw, std::string & name);
};

#pragma mark - Declarations -
#pragma mark Parser core

Entity _parserCore(std::string input, int opType);
Entity _parseEntity(std::string level);
std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions);
Entity parseMonome(std::string str);

#pragma mark Parser utils

bool havePlusOnLevel(std::string level, std::vector<uint> & positions);
bool haveMultOnLevel(std::string level, std::vector<uint> & positions);
bool isFunction(std::string level, uint & functionCode);
void separateFunctionArgs(std::string level, std::vector<uint> & positions);
uint8_t getPreviousOP(char operand);

#pragma mark Spirit utils

VARIABLE convertSpirit(std::string string);

#pragma mark Sanitization

void checkString(std::string input);
int syntaxAnalysis(std::string input);

