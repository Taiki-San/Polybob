#include "parserPrivate.h"

Catalog& Catalog::Instance()
{
	static Catalog instance;
	
	return instance;
}

Catalog::Catalog()
{
	haveCache = false;
	
	functionCodes = {FCODE_EXPAND, FCODE_FACTOR, FCODE_EVALUATE, FCODE_INTERPOLATE, FCODE_COMPOSITION, FCODE_DIVISION};
	functionNames = {"expand", "factor", "evaluate", "interpolate", "composition", "division"};
	functionArgumentNumber = {1, 1, 2, 0, 2, 2};
	functionArgumentType = {{FARG_TYPE_FACTORISED}, {FARG_TYPE_POLY_NOFACT}, {FARG_TYPE_POLY, FARG_TYPE_NUMBER}, {FARG_TYPE_REAL}, {FARG_TYPE_POLY, FARG_TYPE_POLY}, {FARG_TYPE_ANY, FARG_TYPE_ANY}};
	functionReturnType = {FARG_TYPE_POLY_NOFACT, FARG_TYPE_FACTORISED, FARG_TYPE_NUMBER, FARG_TYPE_POLY, FARG_TYPE_POLY, FARG_TYPE_DIV_RESULT};
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
	for(std::string::const_iterator iterator = variableName.begin(); iterator != variableName.end(); ++iterator)
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

bool Catalog::haveVariableSuggestion(std::string begining, std::string & suggestion)
{
	Catalog & instance = Catalog::Instance();
	return findSuggestion(begining, instance.functionNames, suggestion);;
}

bool Catalog::haveFunctionSuggestion(std::string begining, std::string & suggestion)
{
	Catalog & instance = Catalog::Instance();
	return findSuggestion(begining, instance.functionNames, suggestion);
}

bool Catalog::findSuggestion(std::string begining, std::vector<std::string> source, std::string & suggestion)
{
	if(source.size() == 0)
		return false;
	
	uint matchPosition = UINT_MAX, counter = 0;
	
	for(std::vector<std::string>::const_iterator iter = source.begin(); iter != source.end(); ++iter, ++counter)
	{
		if(!iter->compare(0, begining.length(), begining))
		{
			//First match
			if(matchPosition == UINT_MAX)
				matchPosition = counter;
			
			//Not, we stop right now
			else
				return false;
		}
	}
	
	try
	{
		suggestion = source.at(matchPosition);
	}
	catch (const std::exception & e)
	{
		return false;
	}

	return true;
}

//Now, let's have a look at our variables
VARIABLE Catalog::variableValue(std::string variableName)
{
	Catalog & instance = Catalog::Instance();
	
	std::vector<std::string>::const_iterator iter = instance.variableNames.begin(), end = instance.variableNames.end();
	
	for(uint index = 0; iter != end; index++)
	{
		if(*iter == variableName)
		{
			try
			{
				return 	instance.variableContent.at(index);
			}
			catch (std::exception & e)
			{
				break;
			}
		}
	}
	
	std::stringstream error;
	error << variableName << " doesn't exist";
	throw std::invalid_argument(error.str());
}

void Catalog::setVariableValue(std::string variableName, VARIABLE content)
{
	Catalog & instance = Catalog::Instance();
	
	std::vector<std::string>::const_iterator iter = instance.variableNames.begin(), end = instance.variableNames.end();
	uint index = 0;
	
	for(; iter != end; index++)
	{
		if(*iter == variableName)
		{
			instance.variableContent[index] = content;
		}
	}
	
	instance.variableNames.push_back(variableName);
	instance.variableContent.push_back(content);
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
