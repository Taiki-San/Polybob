#include "parserPrivate.h"

#define SEPARATOR '	'

Entity::Entity() : _monome(Complex::complexN(0, 0), 0)
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
	return !isContainer && _monome.power == SPIRIT_DEFAULT_POWER_VALUE && _monome.coeff.imag() == 0;
}

bool Entity::monomeCouldBePartFactorisedPoly(uint index) const
{
	return (polynome[index].power == 1 && polynome[index].coeff.real() == 1 && polynome[index].coeff.imag() == 0) || polynome[index].power == 0;
}

bool Entity::isFactorisedPoly() const
{
	if((previousOperator & (OP_MULT | OP_NONE)) && isMature && polynome.size() <= 2 && polynome[0].power < 2)
	{
		if(monomeCouldBePartFactorisedPoly(0))
		{
			if(polynome.size() == 1 || monomeCouldBePartFactorisedPoly(1))
				return true;
		}
	}
	
	return false;
}

#pragma mark - Getter

uint Entity::getType() const
{
	if(!isContainer)
	{
		if(polynome.size() == 1)
		{
			if(polynome[0].power == 0)
			{
				if(polynome[0].coeff.real() == 0)
					return FARG_TYPE_COMPLEX;

				else if(polynome[0].coeff.imag() == 0)
					return FARG_TYPE_REAL;
				
				return FARG_TYPE_NUMBER;
			}
			else	//x^4
				return FARG_TYPE_POLY_NOFACT;
		}
		
		//x + 3 isn't a factorised form, however, (x + 3)^2 is
		else if(isFactorisedPoly() && power > 1)
			return FARG_TYPE_FACTORISED;
	}
	
	else
	{
		for (std::vector<Entity>::const_iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
		{
			if(!iter->isFactorisedPoly())
				return FARG_TYPE_FACTORISED;
		}
	}
	
	return FARG_TYPE_POLY_NOFACT;
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
			
			if(isFunction)
				std::cout << std::string(depth, SEPARATOR) << ",\n";
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
	if(_monome.coeff.real() == 0 && _monome.coeff.imag() == 0)
	{
		std::cout << "0\n";
		return;
	}
	
	else if(_monome.coeff.imag() == 0)
		std::cout << _monome.coeff.real();
	
	else if(_monome.coeff.real() == 0)
		std::cout << _monome.coeff.imag() << 'i';
	
	else
		std::cout << '(' << _monome.coeff.real() << '+' << _monome.coeff.imag() << "i)";
	
	if(_monome.power > 1)
		std::cout << "x^" << _monome.power << '\n';
	else if(_monome.power == 1)
		std::cout << "x\n";
	else
		std::cout << '\n';
}

#pragma mark - Maturation

void Entity::maturation(bool & error)
{
	//We mature the sub-elems
	if(isContainer)
	{
		for(std::vector<Entity>::iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
		{
			iter->maturation(error);

			if(error)
				return;
		}
	}
	
	//Now, let's evaluate the content
	if(isFunction)
	{
		executeFunction(error);
		
		if(error)
			return;
	}
	else if(!isContainer)
	{
		polynome.push_back(_monome);
	}
	else
	{
		//Awesome, we're in the tricky part, here are out assumptions:
		//	Sublevel is consistent in priority, so no need to prioritize
		//	We don't really care about our power, it'll get evaluated later (by the parent)
		//	My earlier parser didn't messed with me :X
		
		bool firstRun = true;
		std::vector<monome> currentStage, final;
		
		for(std::vector<Entity>::const_iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
		{
			currentStage = iter->polynome;
			
#warning "Need linking to core the polynomial operations"

			if(iter->power > 1)
			{
				std::vector<monome> multiplier = currentStage;
				
				for(uint i = 1, power = iter->power; i < power; i++)
				{
					//Perform currentStage * multiplier
				}
			}
			
			if(firstRun)
			{
				firstRun = false;
				final = currentStage;
			}
			
			else
			{
				switch (iter->previousOperator)
				{
					case OP_MINUS:
					{
						//Perform minus
						break;
					}
						
					case OP_PLUS:
					{
						//Perform plus
						break;
					}
						
					case OP_MULT:
					{
						//PerformMult
						break;
					}
						
					case OP_DIV:
					{
						//PerformDiv
						break;
					}
						
					default:
					{
						std::cerr << "Unexpected operand! (" << iter->previousOperator << ") Can't really dump the context, sorry :/";
						error = true;
						return;
					}
				}
			}
		}

		polynome = final;
	}
	
	isMature = true;
	isFunction = isContainer = false;

}

bool Entity::checkArgumentConsistency(bool & error) const
{
	uint nbArg = Catalog::getNbArgsForID(functionCode), curType, pos = 0;
	std::vector<uint> typing = Catalog::getArgumentType(functionCode);
	std::vector<Entity>::const_iterator iter = subLevel.begin();
	
	//We set curType earlier so in the case where nbArg == 0 (wildcard), we already have curType
	for(curType = typing[0]; pos < nbArg && iter != subLevel.end(); ++iter)
	{
		if((iter->getType() & curType) == 0)
		{
			error = true;
			std::cerr << "Invalid argument for function " << Catalog::getFunctionName(functionCode) << ", " << iter->getType() << " instead of " << curType << '\n';
			return false;
		}
		
		curType = typing[++pos];
	}

	//This loop will only be considered if nbArg != actual length, that can only happen (because sanitization) when nbArg == 0
	while(iter != subLevel.end())
	{
		if((iter->getType() & curType) == 0)
		{
			error = true;
			std::cerr << "Invalid argument for function " << Catalog::getFunctionName(functionCode) << ", " << iter->getType() << " instead of " << curType << '\n';
			return false;
		}
		
		++iter;
	}

	return true;
}

void Entity::executeFunction(bool & error)
{
	if(!checkArgumentConsistency(error))
		return;
	
#warning "We need to put our arg in common variables and send them to the appropriate function"
}

