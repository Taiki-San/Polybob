#include "parserPrivate.h"

Variables* Variables::instance = NULL;

Variables* Variables::Instance()
{
	if (!instance)
		instance = new Variables();
	
	return instance;
}

Variables::Variables()
{
	//constructeur normal
}