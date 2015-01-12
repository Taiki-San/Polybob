#include "parserPrivate.h"

Catalog& Catalog::Instance()
{
	static Catalog instance;
	
	return instance;
}

Catalog::Catalog()
{
	haveCache = false;
	
	functionNames = {"f", "g"};
	functionArgumentNumber = {1, 2};
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
	Catalog & instance = Catalog::Instance();
	if (ID >= instance.functionArgumentNumber.size())
	{
		return INVALID_FUNCTION_ID;
	}
	
	return instance.functionArgumentNumber[ID];
}

std::string Catalog::getFunctionName(uint ID)
{
	Catalog & instance = Catalog::Instance();
	if (ID >= instance.functionNames.size())
	{
		return "";
	}
	
	return instance.functionNames[ID];
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
	
	if(length <= 2 || input[0] != '[' || input[length - 1] != ']')
		return false;
	
	variableName = input.substr(1, length - 2);

	//We check if there is no illegal character in the variable name
	for(std::string::const_iterator iterator = variableName.begin(); iterator != input.end(); ++iterator)
	{
		if(*iterator == '[' || *iterator == ']')
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
