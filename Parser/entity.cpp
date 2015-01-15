#include "parserPrivate.h"

#define SEPARATOR '	'

Entity::Entity() : _monome(Complex::complexN(0, 0), 0)
{
	initialized = false;
	isContainer = isFunction = false;
	previousOperator = OP_NONE;
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

#define CHOOSEVAR(__type, __poly, __fact, __complex) ((__type & FARG_TYPE_FACTORISED) ? __fact : ((__type & FARG_TYPE_NUMBER) ? __complex : __poly))

#if 0
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
		
		matureType = getType();
		if(matureType & FARG_TYPE_NUMBER)
			numberPure = _monome.coeff;

		else if(matureType & FARG_TYPE_FACTORISED)
		{
			factorisedElem element;
			element.coef = _monome.coeff;
			element.power = power;

			vectorFact_t vector;
			vector.push_back(element);
			
			polynomeFact = PolynomialFact(vector);
			power = 1;
		}
		else
		{
			vectorMonomials_t vector;
			vector.push_back(_monome);
			polynomePure = Polynomial(vector);
		}
	}
	else
	{
		//Awesome, we're in the tricky part, here are our assumptions:
		//	Sublevel is consistent in priority, so no need to prioritize
		//	We don't really care about our power, it'll get evaluated later (by the parent)
		//	power == 1 if this is a factorised polynome
		//	My earlier parser didn't messed with me :X
		
		/*
		 *	We work on 3 types, Polynomial, PolynomialFact and Complex, and things get funny
		 *
		 *	+|-		Complex + Complex 	= Complex
		 *			Sinon, 				Poly
		 *
		 *	*		Type * Type 		= Type
		 *			Complex * Type 		= Type
		 *			PolyFact * Complex	= PolyFact
		 *			PolyFact * Poly		= Poly
		 *			Poly * Type			= Poly
		 *
		 *	/		Type * OtherType	= divResult
		 *
		 */
		
		std::vector<Entity>::const_iterator iter = subLevel.begin();
		
		bool firstRun = true, fullyFactorised = false;
		uint8_t currentType;
		uint currentPower;
		
		matureType = FARG_TYPE_NUMBER;
		
		Polynomial finalPoly = Polynomial(), currentPoly;
		PolynomialFact finalFact = PolynomialFact(), currentFact;
		Complex::complexN finalNumber = Complex::complexN(0, 0), currentNumber;
		
		if((currentPower = iter->power) == 0)
			currentPower = 1;
		
		currentType = iter->matureType;
		
		if(currentType & FARG_TYPE_NUMBER)
		{
			finalNumber = std::pow(iter->numberPure, currentPower);
		}
		
		else if(currentType & FARG_TYPE_FACTORISED)
		{
			fullyFactorised = true;

			//Factorized form already consider the power
			finalFact = iter->polynomeFact;
		}
		
		else
		{
			finalPoly = iter->polynomePure ^ currentPower;
		}
		
		for(++iter; iter != subLevel.end() && currentType != FARG_TYPE_DIV_RESULT; ++iter)
		{
			currentType = iter->matureType;
			currentPower = iter->power;
			
			//We're not anymore a factorised form :(
			if(fullyFactorised && (currentType & (FARG_TYPE_FACTORISED | FARG_TYPE_NUMBER)) == 0)
			{
				currentPoly += finalFact;
				finalFact = PolynomialFact::PolynomialFact();
				fullyFactorised = false;
			}

			//We transfer in the appropriate receiver
			if(currentType & FARG_TYPE_NUMBER)
			{
				currentNumber = std::pow(iter->numberPure, currentPower);
			}
			
			else if(currentType & FARG_TYPE_FACTORISED)
			{
				//Factorized form already consider the power
				currentFact = iter->polynomeFact;
			}
			
			else
			{
				currentPoly = iter->polynomePure ^ currentPower;
			}
			
			//Now, witchcraft, power are applied, we now have to combine them
			//I rely a lot on a macro in order to get the proper argument depending of the type
			
			switch (iter->previousOperator)
			{
				case OP_MINUS:
				case OP_PLUS:
				{
					if(matureType & FARG_TYPE_NUMBER && currentType & FARG_TYPE_NUMBER)
					{
						if(iter->previousOperator == OP_MINUS)
							finalNumber -= currentNumber;
						else
							finalNumber += currentNumber;
					}
					else
					{
						//Hum, we need to move our value in the appropriate variable, as type is about to change
						if((matureType & FARG_TYPE_NUMBER) || (matureType & FARG_TYPE_FACTORISED))
							migrateType(currentType, finalPoly, finalFact, finalNumber);

						if(iter->previousOperator == OP_MINUS)
							CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) -= CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);
						else
							CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) += CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);
					}
					break;
				}
					
				case OP_DIV:
				{
					try
					{
						divisionResult = CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) / CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);
						currentType = FARG_TYPE_DIV_RESULT;
					}
					catch (const std::exception & e)
					{
						std::cerr << e.what();
						error = true;
						return;
					}
					break;
				}

				case OP_MULT:
				{
					if(CHOOSEVAR(matureType, 1, 2, 3) == CHOOSEVAR(currentType, 1, 2, 3))
						CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) *= CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);

					//If one of them is a number, we'll have the type of the other
					else if((matureType & FARG_TYPE_NUMBER) || (currentType & FARG_TYPE_NUMBER))
					{
						if(matureType & FARG_TYPE_NUMBER)
							migrateType(currentType, finalPoly, finalFact, finalNumber);
						
						CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) *= CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);
					}
					
					else
					{
						if(matureType & (FARG_TYPE_NUMBER | FARG_TYPE_FACTORISED))
							migrateType(FARG_TYPE_POLY, finalPoly, finalFact, finalNumber);
						
						CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber) *= CHOOSEVAR(currentType, currentPoly, currentFact, currentNumber);
					}

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
		
		if(matureType == FARG_TYPE_NUMBER)
			numberPure = finalNumber;
		
		else if(matureType == FARG_TYPE_FACTORISED)
			polynomeFact = finalFact;
		
		else
			polynomePure = finalPoly;
	}
	
	isMature = true;
	isFunction = isContainer = false;

}

void Entity::migrateType(uint8_t newType, Polynomial & finalPoly, PolynomialFact & finalFact, Complex::complexN & finalNumber)
{
	if((matureType & FARG_TYPE_DIV_RESULT) || (newType & FARG_TYPE_DIV_RESULT) || CHOOSEVAR(newType, 1, 2, 3) != CHOOSEVAR(matureType, 1, 2, 3))
		return;

	CHOOSEVAR(newType, finalPoly, finalFact, finalNumber) += CHOOSEVAR(matureType, finalPoly, finalFact, finalNumber);
	
	if(matureType & FARG_TYPE_NUMBER)
		finalNumber = Complex::complexN(0, 0);

	else if(matureType & FARG_TYPE_FACTORISED)
		finalFact = PolynomialFact::PolynomialFact();
	
	else
		finalPoly = Polynomial::Polynomial();
	
	newType = matureType;
}

#endif

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
	
	uint retType = Catalog::getFuncReturnType(functionCode), argPower = subLevel[0].power;
	
	if(argPower == 0)	argPower = 1;

	//We apply power on the fly, so things get pretty dirty
	
	switch (functionCode)
	{
		case FCODE_EXPAND:
		{
			CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).expand();
			break;
		}
			
		case FCODE_FACTOR:
		{
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).factor();
			else
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).factor();
			break;
		}
			
		case FCODE_EVALUATE:
		{
			uint arg2Power = subLevel[1].power;
			if(arg2Power == 0)		arg2Power = 1;
			
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).eval(pow(subLevel[1].numberPure, arg2Power));
			else
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).eval(pow(subLevel[1].numberPure, arg2Power));
			break;
		}
			
		case FCODE_INTERPOLATE:
		{
			std::vector<Complex::complexN> argument;
			
			for(std::vector<Entity>::const_iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
			{
				uint power = iter->power;
				Complex::complexN current = iter->numberPure;
				
				argument.push_back(pow(current, power == 0 ? 1 : power));
			}
			
			CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = polynomePure.composition(argument);
			
			break;
		}
		
		case FCODE_COMPOSITION:
		{
			uint arg2Power = subLevel[1].power;
			if(arg2Power == 0)		arg2Power = 1;
			
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomeFact ^ argPower).compose(((subLevel[1].matureType & FARG_TYPE_FACTORISED) ? subLevel[1].polynomeFact : subLevel[1].polynomePure) ^ arg2Power);
			else
				CHOOSEVAR(retType, polynomePure, polynomeFact, numberPure) = (subLevel[0].polynomePure ^ argPower).compose(((subLevel[1].matureType & FARG_TYPE_FACTORISED) ? subLevel[1].polynomeFact : subLevel[1].polynomePure) ^ arg2Power);
			break;
		}
			
		default:
		{
			return;
		}
	}
	
	matureType = retType;
}

