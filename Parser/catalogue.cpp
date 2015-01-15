#include "parserPrivate.h"

Catalog& Catalog::Instance()
{
	static Catalog instance;
	
	return instance;
}

#define MAX_FUNC_ID FCODE_DUMP

Catalog::Catalog()
{
	haveCache = false;
	
	functionCodes = {FCODE_EXPAND, FCODE_FACTOR, FCODE_EVALUATE, FCODE_INTERPOLATE, FCODE_COMPOSITION, FCODE_TEST, FCODE_DUMP};
	functionNames = {"expand", "factor", "evaluate", "interpolate", "composition", "test", "dump"};
	functionArgumentNumber = {1, 1, 2, 0, 2};
	functionArgumentType = {{FARG_TYPE_FACTORISED}, {FARG_TYPE_POLY_NOFACT}, {FARG_TYPE_POLY, FARG_TYPE_NUMBER}, {FARG_TYPE_REAL}, {FARG_TYPE_POLY, FARG_TYPE_POLY}};
	functionReturnType = {FARG_TYPE_POLY_NOFACT, FARG_TYPE_FACTORISED, FARG_TYPE_NUMBER, FARG_TYPE_POLY, FARG_TYPE_POLY};
}

#pragma mark Function related

uint Catalog::getIDOfFunction(std::string name)
{
	Catalog & instance = Catalog::Instance();
	uint index = 0;
	for (std::vector<std::string>::const_iterator current = instance.functionNames.begin(); current != instance.functionNames.end(); ++current, ++index)
	{
		if(name == *current)
			return index;
	}
	
	return INVALID_FUNCTION_ID;
}

uint Catalog::getNbArgsForID(uint ID)
{
	if (ID > MAX_FUNC_ID)
		return INVALID_FUNCTION_ID;
	
	return Catalog::Instance().functionArgumentNumber[ID];
}

std::string Catalog::getFunctionName(uint ID)
{
	if (ID > MAX_FUNC_ID)
		return "";
	
	return Catalog::Instance().functionNames[ID];
}

std::vector<uint> Catalog::getArgumentType(uint ID)
{
	if (ID > MAX_FUNC_ID)
	{
		std::vector<uint> empty;
		return empty;
	}
	
	return Catalog::Instance().functionArgumentType[ID];
}

uint Catalog::getFuncReturnType(uint ID)
{
	if(ID > MAX_FUNC_ID)
		return FARG_TYPE_POLY;
		
	return Catalog::Instance().functionReturnType[ID];
}

#pragma mark Variable related

bool Catalog::variableName(std::string input, std::string & variableName)
{
	std::string rawCache, cacheName;
	size_t length;
	
	//We check the cache
	if(Catalog::getCache(rawCache, cacheName) && rawCache == input)
	{
		variableName = cacheName;
		return true;
	}
	
	//If no cache, we get the length, strip the borders
	length = input.length();
	
	if(length <= 2 || input[0] != '{' || input[length - 1] != '}')
		return false;
	
	variableName = input.substr(1, length - 2);

	//We check if there is no illegal character in the variable name
	for(std::string::const_iterator iterator = variableName.begin(); iterator != input.end(); ++iterator)
	{
		if(*iterator == '{' || *iterator == '}')
			return false;
	}
	
	//We register to the cache
	Catalog::registerCache(input, variableName);
	return true;
}

bool Catalog::isVariable(std::string input)
{
	std::string unused;
	return Catalog::variableName(input, unused);
}

//Used by auto-completion
#warning "TODO"

bool Catalog::haveVariableSuggestion(std::string begining, std::string & suggestion)
{
	return false;
}

bool Catalog::haveFunctionSuggestion(std::string begining, std::string & suggestion)
{
	return false;
}

//Now, let's have a look at our variables
bool Catalog::variableExist(std::string variableName)
{
	return false;
}

std::vector<monome> Catalog::variableValue(std::string variableName, bool & error)
{
	std::vector<monome> lol;
	return lol;
}

void Catalog::setVariableValue(std::string variableName, std::vector<monome>)
{
	
}

//Micro caching
void Catalog::registerCache(std::string rawInput, std::string variableName)
{
	Catalog & instance = Catalog::Instance();
	
	instance.cacheRaw = rawInput;
	instance.cacheName = variableName;
	instance.haveCache = true;
}

bool Catalog::getCache(std::string & raw, std::string & name)
{
	Catalog & instance = Catalog::Instance();
	
	if(!instance.haveCache)
		return false;
	
	raw = instance.cacheRaw;
	name = instance.cacheName;
	
	return true;
}
