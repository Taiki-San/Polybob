#ifndef PARSER_HEADER

	#define PARSER_HEADER

	class Variables
	{
		std::string cacheRaw, cacheName;
		bool haveCache;
		
		Variables();
		Variables(Variables const&);
		
	public:
		static Variables& Instance();
		
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
		
		static void registerCache(std::string rawInput, std::string variableName);
		static bool getCache(std::string & raw, std::string & name);
	};

	int parserEntrypoint();

#endif