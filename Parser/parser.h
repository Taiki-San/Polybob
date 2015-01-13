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
	FARG_TYPE_REAL			= 1 << 2,
	FARG_TYPE_COMPLEX		= 1 << 3,
	FARG_TYPE_NUMBER		= FARG_TYPE_REAL | FARG_TYPE_COMPLEX
};

//Private types

struct _parserComplex
{
	double coefReal;
	double coefComplex;
};

struct _parserMonome
{
	struct _parserComplex coef;
	int exponent;
};

	class Entity;

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
		
		//Variable tools
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
		
		//Variable manipulation
		static bool variableExist(std::string variableName);
		static std::vector<struct _parserMonome> variableValue(std::string variableName, bool & error);
		static void setVariableValue(std::string variableName, std::vector<struct _parserMonome>);
		
		static void registerCache(std::string rawInput, std::string variableName);
		static bool getCache(std::string & raw, std::string & name);
	};

	Entity parserCore(std::string input, bool & error);

#endif