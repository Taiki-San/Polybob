#ifndef PARSER_HEADER

	#define PARSER_HEADER

	void simpleParserAPI(char * cString);
	void finalProcessing();

	//Auto-completion

#ifdef __cplusplus
	bool haveVariableSuggestion(std::string begining, std::string & suggestion);
#endif

#endif