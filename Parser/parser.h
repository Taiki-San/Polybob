#ifndef PARSER_HEADER

	#define PARSER_HEADER

	class Catalog
	{
		std::string cacheRaw, cacheName;
		bool haveCache;
		
		Catalog();
		Catalog(Catalog const&);
		
	public:
		static Catalog& Instance();
		
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
		
		static void registerCache(std::string rawInput, std::string variableName);
		static bool getCache(std::string & raw, std::string & name);
	};

	int parserEntrypoint();

#endif