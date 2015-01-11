#include "parserPrivate.h"

Catalog& Catalog::Instance()
{
	static Catalog instance;
	
	return instance;
}

Catalog::Catalog()
{
	haveCache = false;
}

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
