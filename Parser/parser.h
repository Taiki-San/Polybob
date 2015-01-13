#ifndef PARSER_HEADER

	#define PARSER_HEADER

	class Entity;

	Entity parserCore(std::string input, bool & error);

	//IO
	void print(Entity entity);

	//Auto-completion
	bool haveVariableSuggestion(std::string begining, std::string & suggestion);
	bool haveFunctionSuggestion(std::string begining, std::string & suggestion);

#endif