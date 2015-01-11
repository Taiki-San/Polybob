#include "parserPrivate.h"

Entity::Entity()
{
	isContainer = isFunction = false;
	previousOperator = PREV_OP_NONE;
}

bool Entity::insertMonomeEntry(monome entry)
{
	if(isContainer)
		return false;

	for(std::vector<monome>::iterator begin = levelMember.begin(); begin != levelMember.end(); ++begin)
	{
		if(begin->exponent == entry.exponent)
		{
			begin->coef.coefReal += entry.coef.coefReal;
			begin->coef.coefComplex += entry.coef.coefComplex;
			return true;
		}
	}

	levelMember.push_back(entry);
	return true;
}