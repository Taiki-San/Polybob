#ifndef PARSER_HEADER

	#define PARSER_HEADER

	class Variables
	{
		Variables();
		Variables(Variables const&);
		
	public:
		static Variables& Instance();
		
		static bool variableName(std::string input, std::string & variableName);
		static bool isVariable(std::string input);
	};

#endif