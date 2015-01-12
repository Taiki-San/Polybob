#ifndef PARSER_HEADER

	#define PARSER_HEADER

	#define INVALID_FUNCTION_ID UINT_MAX

	class Catalog
	{
		std::string cacheRaw, cacheName;
		bool haveCache;
		
		Catalog();
		Catalog(Catalog const&);
		
	public:
		std::vector<std::string> functionNames;
		std::vector<uint> functionArgumentNumber;
		
		static Catalog& Instance();
		
		static uint getIDOfFunction(std::string name);
		static uint getNbArgsForID(uint ID);
		static std::string getFunctionName(uint ID);
		
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
		
		static void registerCache(std::string rawInput, std::string variableName);
		static bool getCache(std::string & raw, std::string & name);
	};

	int parserEntrypoint();

#endif