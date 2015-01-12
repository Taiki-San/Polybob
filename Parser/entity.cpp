#include "parserPrivate.h"

#define SEPARATOR '	'

Entity::Entity()
{
	initialized = false;
	isContainer = isFunction = false;
	previousOperator = PREV_OP_NONE;
	power = 0;
}

bool Entity::setMonome(monome entry)
{
	if(isContainer)
		return false;
	
	_monome = entry;
	return true;
}

bool Entity::setSublevel(std::vector<Entity> entry)
{
	if(initialized && !isContainer)
		return false;
	
	isContainer = true;
	subLevel = entry;
	
	return true;
}

bool Entity::setFunction(uint function)
{
	if(initialized)
		return false;
	
	isFunction = true;
	functionCode = function;
	
	return true;
}

void Entity::updatePowerOfLast(int _power)
{
	if(isContainer && !isFunction)
	{
		Entity & end = subLevel.back();
		end.power += _power;
	}
	else
		power += _power;
}

bool Entity::isReal() const
{
	return !isContainer && _monome.exponent == SPIRIT_DEFAULT_POWER_VALUE && _monome.coef.coefComplex == 0;
}

#pragma mark - IO

void Entity::print() const
{
	print(0);
}

void Entity::print(uint depth) const
{
	if(isFunction)
		std::cout << std::string(depth++, SEPARATOR) << Catalog::getFunctionName(functionCode) << "[\n";
	
	if(isContainer)
	{
		std::cout << std::string(depth, SEPARATOR) << "{\n";

		for (std::vector<Entity>::const_iterator i = subLevel.begin(); i != subLevel.end(); ++i)
		{
			i->print(depth + 1);
		}

		if(power != SPIRIT_DEFAULT_POWER_VALUE)
			std::cout << std::string(depth, SEPARATOR) << "}^" << power << '\n';
		else
			std::cout << std::string(depth, SEPARATOR) << "}\n";
	}
	else
	{
		if(power != SPIRIT_DEFAULT_POWER_VALUE)
			std::cout << std::string(depth, SEPARATOR) << '(';
		else
			std::cout << std::string(depth, SEPARATOR);
		
		printMonome();
		
		if(power != SPIRIT_DEFAULT_POWER_VALUE)
			std::cout << ")^" << power << '\n';
	}
	
	if(isFunction)
	{
		if(power != SPIRIT_DEFAULT_POWER_VALUE)
			std::cout << std::string(--depth, SEPARATOR) << "]^" << power << '\n';
		else
			std::cout << std::string(--depth, SEPARATOR) << "]\n";
	}
}

void Entity::printMonome() const
{
	std::cout << "coefficient: ";
	if(_monome.coef.coefReal == 0 && _monome.coef.coefComplex == 0)
	{
		std::cout << "0\n";
		return;
	}
	
	else if(_monome.coef.coefComplex == 0)
		std::cout << _monome.coef.coefReal;
	
	else if(_monome.coef.coefReal == 0)
		std::cout << _monome.coef.coefComplex << 'i';
	
	else
		std::cout << '(' << _monome.coef.coefReal << '+' << _monome.coef.coefComplex << "i)";
	
	if(_monome.exponent > 1)
		std::cout << "x^" << _monome.exponent << '\n';
	else if(_monome.exponent == 1)
		std::cout << "x\n";
	else
		std::cout << '\n';
}
