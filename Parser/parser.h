#ifndef PARSER_HEADER

	#define PARSER_HEADER

	#define INVALID_FUNCTION_ID UINT_MAX

enum
{
	FCODE_EXPAND		= 0,
	FCODE_FACTOR		= 1,
	FCODE_EVALUATE		= 2,
	FCODE_INTERPOLATE	= 3,
	FCODE_COMPOSITION	= 4,
	FCODE_TEST			= 5,
	FCODE_DUMP			= 6
} FUNCTION_CODES;

enum
{
	FARG_TYPE_POLY_NOFACT 	= 1 << 0,
	FARG_TYPE_FACTORISED	= 1 << 1,
	FARG_TYPE_POLY			= FARG_TYPE_POLY_NOFACT | FARG_TYPE_FACTORISED,
	FARG_TYPE_NUMBER		= 1 << 2
};

	class Catalog
	{
		std::string cacheRaw, cacheName;
		bool haveCache;
		
		std::vector<uint> functionCodes;
		std::vector<std::string> functionNames;
		std::vector<uint> functionArgumentNumber;
		std::vector<std::vector<uint>> functionArgumentType;

		Catalog();
		Catalog(Catalog const&);
		
	public:
		static Catalog& Instance();
		
		static uint getIDOfFunction(std::string name);
		static uint getNbArgsForID(uint ID);
		static std::string getFunctionName(uint ID);
		static std::vector<uint> getArgumentType(uint ID);
		
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
		
		static void registerCache(std::string rawInput, std::string variableName);
		static bool getCache(std::string & raw, std::string & name);
	};

	int parserEntrypoint();

#endif