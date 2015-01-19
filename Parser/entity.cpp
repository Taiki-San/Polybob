#include "parserPrivate.h"

Entity::Entity() : _monome(Complex::complexN(0, 0), 0)
{
	isMature = false;
	initialized = false;
	functionArg = false;
	polyfactOfDegree0 = isContainer = isFunction = false;
	previousOperator = OP_NONE;
	power = 1;
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
		
		if(end.power + _power == 0)
			end.resetToOne();
		else
			end.power = _power;
	}
	else
		power = _power;
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

void Entity::resetToOne()
{
	isMature = false;
	power = 1;
	_monome = monome(Complex::complexN(1, 0), 0);
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
			else if(polynome[0].power == 1)
				return FARG_TYPE_FACTORISED;
			else
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
	if(matureType & FARG_TYPE_DIV_RESULT)
	{
		std::cout << "Quotient: 	" << divisionResult.first.toString() << std::endl;
		std::cout << "Remaining:	" << divisionResult.second.toString();
	}
	else
	{
		std::cout << " => ";
		
		if(power  > 1)
			std::cout << '(';
		
		if(matureType & FARG_TYPE_NUMBER)
			printMonome();
		else if(matureType & FARG_TYPE_FACTORISED)
			std::cout << polynomeFact.toString();
		else
			std::cout << polynomePure.toString();
		
		if(power  > 1)
			std::cout << ")^" << power << ' ';
	}
	
	std::cout << std::endl;
}

void Entity::printMonome() const
{
	if(numberPure.real() == 0 && numberPure.imag() == 0)
	{
		std::cout << '0';
		return;
	}
	
	else if(numberPure.imag() == 0)
		std::cout << numberPure.real();
	
	else if(numberPure.real() == 0)
		std::cout << numberPure.imag() << 'i';
	
	else
		std::cout << '(' << numberPure.real() << '+' << numberPure.imag() << "i)";
	
	std::cout << ' ';
}

#pragma mark - Maturation

#define CHOOSEVAR(__type, __poly, __fact, __complex) ((__type & FARG_TYPE_FACTORISED) ? (__fact) : ((__type & FARG_TYPE_NUMBER) ? (__complex) : (__poly)))

//MT support
static unsigned char runningThreads;
unsigned char argument[3] = {1, 2, 3};
static size_t cut[4];
static Entity * _mainEntity;

static bool error;
static std::string errorMessage;

void maturationWrapper(unsigned char * currentThread)
{
	_mainEntity->maturation(*currentThread);
}

void Entity::maturation(short threadID)
{
	if(isMature)
		return;
	
	//We mature the sub-elems
	if(isContainer)
	{
		//Yay, multi-threading
		if(threadID == 0)
		{
			runningThreads = 0;
			size_t length = subLevel.size(), count = 0;
			bool worthIt = false;
			
			for(std::vector<Entity>::iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
			{
				if(iter->isContainer)
				{
					count++;
					if(count > 2)
					{
						worthIt = true;
						break;
					}
				}
			}

			
			//If only one element, we check lower
			if(!worthIt)
			{
				return this->maturation(-1);
			}
			
			_mainEntity = this;
			error = false;
			
			//If less than 4 elems, we launch 1 thread per element
			if(length <= 4)
			{
				cut[0] = 1;

				runningThreads = length - 1;

				for(uint i = 1; i < length; i++)
				{
					cut[i] = i + 1;
					createNewThread((void*) maturationWrapper, &(argument[i-1]));
				}
			}
			//Otherwise, we dispatch the load, with most of it on the main thread
			else
			{
				size_t part = length / 4;
				cut[0] = part + length % 4;
				runningThreads = 3;
				
				for(uint i = 1; i < length; i++)
				{
					cut[i] = cut[i - 1] + part;
					createNewThread((void*) maturationWrapper, &(argument[i-1]));
				}
			}
			
			//We mature until our entry
			count = 0;
			for(std::vector<Entity>::iterator iter = subLevel.begin(); !error && iter != subLevel.end() && count < cut[0]; ++iter, ++count)
				iter->maturation(-1);
			
			//Wait for others to end their task
			while(1)
			{
				MUTEX_LOCK;
				if(runningThreads == 0)
					break;
				MUTEX_UNLOCK;
				
				usleep(100);
			}
			MUTEX_UNLOCK;

			//If any error, we abort
			if(error)
			{
				throw std::invalid_argument(errorMessage);
			}
			
		}
		//Code used by sub-threads
		else if(threadID > 0 && threadID < 4)
		{
			size_t count = cut[threadID - 1];
			std::vector<Entity>::iterator iter = subLevel.begin();
			
			for(size_t i = 0; iter != subLevel.end() && i < count; ++iter, ++i);
			
			try
			{
				for(;!error && iter != subLevel.end() && count < cut[threadID]; ++iter, ++count)
					iter->maturation(-1);
			}
			catch (const std::exception & e)
			{
				error = true;
				errorMessage = e.what();
			}
			
			MUTEX_LOCK;
			runningThreads--;
			MUTEX_UNLOCK;
			
			pthread_exit(0);
		}
		
		//Standard cycle
		else
		{
			for(std::vector<Entity>::iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
				iter->maturation(-1);
		}
	}
	
	//Now, let's evaluate the content
	if(isFunction)
		executeFunction();
	
	else if(functionArg)
		return;

	else if(!isContainer)
	{
		polynome.push_back(_monome);
		
		matureType = getType();
		if(matureType & FARG_TYPE_NUMBER)
			numberPure = _monome.coeff;

		else if(matureType & FARG_TYPE_FACTORISED)
		{
			Factor element = Factor(0, power);

			vectorFactors_t vector;
			vector.push_back(element);
			
			polynomeFact = PolyFact(vector, _monome.coeff);
			polyfactOfDegree0 = _monome.coeff.real() == 1 && _monome.coeff.imag() == 0 && power == 1;
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
		 *	We work on 3 types, Polynomial, PolyFact and Complex, and things get funny
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
		
		bool fullyFactorised = false, couldBePolyOfDeg1 = false;
		uint8_t currentType;
		uint currentPower;
		
		matureType = FARG_TYPE_NUMBER;
		
		Polynomial finalPoly = Polynomial(), currentPoly;
		PolyFact finalFact = PolyFact(), currentFact;
		Complex::complexN finalNumber = Complex::complexN(0, 0), currentNumber;

		currentPower = iter->power;
		matureType = currentType = iter->matureType;
		
		if(currentPower > 0)
		{
			if(currentType & FARG_TYPE_NUMBER)
			{
				finalNumber = std::pow(iter->numberPure, currentPower);
			}
			
			else if(currentType & FARG_TYPE_FACTORISED)
			{
				fullyFactorised = true;
				couldBePolyOfDeg1 = iter->polyfactOfDegree0;
				
				//Factorized form already consider the power
				finalFact = iter->polynomeFact;
			}
			
			else if(currentType & FARG_TYPE_DIV_RESULT)
			{
				std::stringstream error;
				error << "You can't reuse the output of an euclidian division";
				throw std::invalid_argument(error.str());
			}
			
			else
			{
				finalPoly = iter->polynomePure ^ currentPower;
			}
		}
		else
			finalNumber += 1;
		
		for(++iter; iter != subLevel.end() && currentType != FARG_TYPE_DIV_RESULT; ++iter)
		{
			currentType = iter->matureType;
			currentPower = iter->power;
			
			//Sanity check
			if(currentType & FARG_TYPE_DIV_RESULT)
			{
				std::stringstream error;
				error << "You can't reuse the output of an euclidian division";
				throw std::invalid_argument(error.str());
			}
			
			//We're not anymore a factorised form :(
			if(fullyFactorised && (currentType & (FARG_TYPE_FACTORISED | FARG_TYPE_NUMBER)) == 0)
			{
				currentPoly += finalFact.expand();
				finalFact = PolyFact::PolyFact();
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
					else if((matureType & FARG_TYPE_FACTORISED && couldBePolyOfDeg1 && currentType & FARG_TYPE_NUMBER) || (matureType & FARG_TYPE_NUMBER && currentType & FARG_TYPE_FACTORISED && iter->polyfactOfDegree0))
					{
						Complex::complexN number;

						if(matureType & FARG_TYPE_FACTORISED)
						{
							number = currentNumber;
							if(iter->previousOperator == OP_PLUS)
								number *= -1;
						}
						else
							number = finalNumber;
						

						vectorFactors_t vector;
						vector.push_back(Factor(number, 1));
						finalFact = PolyFact(vector, 1);
						matureType = FARG_TYPE_FACTORISED;
					}
					else
					{
						//Hum, we need to move our value in the appropriate variable, as type is about to change
						if((matureType & FARG_TYPE_NUMBER) || (matureType & FARG_TYPE_FACTORISED))
							migrateType(FARG_TYPE_POLY_NOFACT, finalPoly, finalFact, finalNumber);

						if(iter->previousOperator == OP_MINUS)
						{
							if(currentType & FARG_TYPE_NUMBER)
								finalPoly -= currentNumber;
							else if(currentType & FARG_TYPE_FACTORISED)
								finalPoly -= currentFact.expand();
							else
								finalPoly -= currentPoly;
						}
						else
						{
							if(currentType & FARG_TYPE_NUMBER)
								finalPoly += currentNumber;
							else if(currentType & FARG_TYPE_FACTORISED)
								finalPoly += currentFact.expand();
							else
								finalPoly += currentPoly;
						}
					}
					break;
				}
					
				case OP_DIV:
				{
					if(matureType & FARG_TYPE_NUMBER && currentType & FARG_TYPE_NUMBER)
						finalNumber /= currentNumber;

					else
					{
						std::stringstream error;
						error << "Invalid division, '/' operator can only be used with numbers! Use division[] to play with polynoms";
						throw std::invalid_argument(error.str());
					}
					break;
				}

				case OP_MULT:
				{
					if(CHOOSEVAR(matureType, 1, 2, 3) == CHOOSEVAR(currentType, 1, 2, 3))
					{
						if(matureType & FARG_TYPE_NUMBER)
							finalNumber *= currentNumber;
						else if(matureType & FARG_TYPE_FACTORISED)
							finalFact *= currentFact;
						else
							finalPoly *= currentPoly;
					}

					//If one of them is a number, we'll have the type of the other
					else if((matureType & FARG_TYPE_NUMBER) || (currentType & FARG_TYPE_NUMBER))
					{
						if(matureType & FARG_TYPE_NUMBER)
							migrateType(currentType, finalPoly, finalFact, finalNumber);
						
						if(matureType & FARG_TYPE_NUMBER)
							finalNumber *= currentNumber;
						else if(matureType & FARG_TYPE_FACTORISED)
							finalFact *= currentFact;
						else
							finalPoly *= currentPoly;
					}
					
					else
					{
						if(matureType & (FARG_TYPE_NUMBER | FARG_TYPE_FACTORISED))
							migrateType(FARG_TYPE_POLY, finalPoly, finalFact, finalNumber);
						
						if(currentType & FARG_TYPE_NUMBER)
							finalPoly *= currentNumber;
						else if(currentType & FARG_TYPE_POLY_NOFACT)
							finalPoly *= currentFact.expand();
						else
							finalPoly *= currentPoly;
					}

					break;
				}
					
				default:
				{
					std::stringstream error;
					error << "Unexpected operand! (" << int(iter->previousOperator) << ") Can't really dump the context, sorry :/";
					throw std::invalid_argument(error.str());
				}
			}
		}
		
		if(matureType & FARG_TYPE_NUMBER)
			numberPure = finalNumber;
		
		else if(matureType & FARG_TYPE_FACTORISED)
			polynomeFact = finalFact;
		
		else
			polynomePure = finalPoly;
		
		polyfactOfDegree0 = false;
	}
	
	isMature = true;
	isFunction = isContainer = false;
}

void Entity::migrateType(uint8_t newType, Polynomial & finalPoly, PolyFact & finalFact, Complex::complexN & finalNumber)
{
	if(CHOOSEVAR(newType, 1, 2, 3) == CHOOSEVAR(matureType, 1, 2, 3))
		return;

	if(newType & FARG_TYPE_POLY)
	{
		if(matureType & FARG_TYPE_NUMBER)
			finalPoly += finalNumber;
		else
			finalPoly += finalFact.expand();
	}
	
	if(matureType & FARG_TYPE_NUMBER)
		finalNumber = Complex::complexN(0, 0);

	else if(matureType & FARG_TYPE_FACTORISED)
		finalFact = PolyFact::PolyFact();
	
	else
		finalPoly = Polynomial::Polynomial();
	
	matureType = newType;
}

bool Entity::checkArgumentConsistency()
{
	uint nbArg = Catalog::getNbArgsForID(functionCode), curType, pos = 0;
	std::vector<uint> typing = Catalog::getArgumentType(functionCode);
	std::vector<Entity>::iterator iter = subLevel.begin();
	
	//We set curType earlier so in the case where nbArg == 0 (wildcard), we already have curType
	for(curType = typing[0]; pos < nbArg && iter != subLevel.end(); ++iter)
	{
		//Variables would create a sublevel
		if(iter->isContainer && iter->subLevel.size() == 1)
		{
			uint power = iter->power;
			*iter = iter->subLevel[0];
			iter->power += power - 1;
			iter->functionArg = true;
		}
		
		if((iter->matureType & curType) == 0)
		{
			std::stringstream error;
			error << "Invalid argument for function " << Catalog::getFunctionName(functionCode) << ", " << int(iter->matureType) << " instead of " << curType;
			throw std::invalid_argument(error.str());
			return false;
		}
		
		curType = typing[++pos];
	}

	//This loop will only be considered if nbArg != actual length, that can only happen (because sanitization) when nbArg == 0
	while(iter != subLevel.end())
	{
		if((iter->getType() & curType) == 0)
		{
			std::stringstream error;
			error << "Invalid argument for function " << Catalog::getFunctionName(functionCode) << ", " << iter->getType() << " instead of " << curType;
			throw std::invalid_argument(error.str());
			return false;
		}
		
		++iter;
	}

	return true;
}

void Entity::executeFunction()
{
	if(!checkArgumentConsistency())
		return;
	
	uint retType = Catalog::getFuncReturnType(functionCode), argPower = subLevel[0].power;
	
	if(argPower == 0)
		argPower = 1;

	//We apply power on the fly, so things get pretty dirty
	
	switch (functionCode)
	{
		case FCODE_EXPAND:
		{
			polynomePure = (subLevel[0].polynomeFact ^ argPower).expand();
			break;
		}
			
		case FCODE_FACTOR:
		{
#if 0
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
				polynomeFact = (subLevel[0].polynomeFact ^ argPower).factor();
			else
				polynomeFact = (subLevel[0].polynomePure ^ argPower).factor();
#else
			throw std::invalid_argument("Sorry, not implemented yet :/");
#endif
			break;
		}
			
		case FCODE_EVALUATE:
		{
			uint arg2Power = subLevel[1].power;
			if(arg2Power == 0)		arg2Power = 1;
			
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
			{
				numberPure = (subLevel[0].polynomeFact ^ argPower).evaluation(pow(subLevel[1].numberPure, arg2Power));
			}
			else
				numberPure = (subLevel[0].polynomePure ^ argPower).evaluation(pow(subLevel[1].numberPure, arg2Power));
			break;
		}
			
		case FCODE_INTERPOLATE:
		{
#if 0
			std::vector<Complex::complexN> argument;
			
			for(std::vector<Entity>::const_iterator iter = subLevel.begin(); iter != subLevel.end(); ++iter)
			{
				uint power = iter->power;
				Complex::complexN current = iter->numberPure;
				
				argument.push_back(pow(current, power));
			}
			
			polynomePure = polynomePure.composition(argument);
#else
			throw std::invalid_argument("Sorry, not implemented yet :/");
#endif
			break;
		}
		
		case FCODE_COMPOSITION:
		{
			uint arg2Power = subLevel[1].power;
			
			if(subLevel[0].matureType & FARG_TYPE_FACTORISED)
			{
				if(subLevel[1].matureType & FARG_TYPE_FACTORISED)
					polynomePure = (subLevel[0].polynomeFact ^ argPower).composition((subLevel[1].polynomeFact ^ arg2Power).expand());
				else
					polynomePure = (subLevel[0].polynomeFact ^ argPower).composition(subLevel[1].polynomePure ^ arg2Power);
			}
			else
			{
				if(subLevel[1].matureType & FARG_TYPE_FACTORISED)
					polynomePure = (subLevel[0].polynomePure ^ argPower).composition((subLevel[1].polynomeFact ^ arg2Power).expand());
				else
					polynomePure = (subLevel[0].polynomePure ^ argPower).composition(subLevel[1].polynomePure ^ arg2Power);
			}
			break;
		}
			
		case FCODE_DIVISION:
		{
			if(subLevel[0].matureType & FARG_TYPE_NUMBER && subLevel[1].matureType & FARG_TYPE_NUMBER)
			{
				Complex::complexN out = subLevel[0].numberPure / subLevel[1].numberPure;
				
				out.real(floor(out.real()));
				out.imag(floor(out.imag()));
				
				divisionResult.first += out;
				divisionResult.second += subLevel[0].numberPure - out * subLevel[1].numberPure;
			}
			else
			{
				uint currentType = subLevel[1].matureType;
				Polynomial poly = Polynomial::Polynomial(), poly2;

				if(subLevel[0].matureType & FARG_TYPE_NUMBER)
				{
					poly += subLevel[0].numberPure;
					
					if(currentType & FARG_TYPE_POLY_NOFACT)
						divisionResult = poly / subLevel[1].polynomePure;
					else
						divisionResult = poly / subLevel[1].polynomeFact.expand();
				}
				else if(matureType & FARG_TYPE_FACTORISED)
				{
					poly2 = subLevel[0].polynomeFact.expand();
					
					if(currentType & FARG_TYPE_NUMBER)
					{
						poly += subLevel[1].numberPure;
						divisionResult = poly2 / poly;
					}
					else if(currentType & FARG_TYPE_POLY_NOFACT)
						divisionResult = poly2 / subLevel[1].polynomePure;
					else
						divisionResult = poly2 / subLevel[1].polynomeFact.expand();
				}
				else
				{
					poly2 = subLevel[0].polynomePure;
					
					if(currentType & FARG_TYPE_NUMBER)
					{
						poly += subLevel[1].numberPure;
						divisionResult = poly2 / poly;
					}
					else if(currentType & FARG_TYPE_POLY_NOFACT)
						divisionResult = poly2 / subLevel[1].polynomePure;
					else
						divisionResult = poly2 / subLevel[1].polynomeFact.expand();
				}

			}
			
			break;
		}
	
		default:
		{
			return;
		}
	}
	
	matureType = retType;
}

