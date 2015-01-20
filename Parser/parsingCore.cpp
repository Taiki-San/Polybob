#include "parserPrivate.h"

Entity _parserCore(std::string input, int opType)
{
	Entity output, auxiliary;
	
	std::string receiver;
	
	//If we want to alloc the result of the operation, no need to evaluate what's at the right of the equality
	if(opType == TYPE_OP_ALLOC || opType == TYPE_OP_COMPARE)
	{
		size_t pos = input.find('=');
		
		//We cut the string in two parts
		std::string part1 = input.substr(0, pos);
		input = input.erase(0, pos+1);
		
		if(opType == TYPE_OP_ALLOC)
		{
			if(!Catalog::variableName(part1, receiver))
			{
				std::stringstream error;
				error << "Not a valid variable name: " << input;
				throw std::invalid_argument(error.str());
				return output;
			}
		}
		else
		{
			input.erase(0, 1);	//We remove the second =
			auxiliary = _parseEntity(part1);
			
			auxiliary.maturation(0);
			
			if(auxiliary.matureType & FARG_TYPE_FACTORISED)
				auxiliary.polynomeFact.simplify();
		}
	}
	
	output = _parseEntity(input);
	output.maturation(0);

	if(output.matureType & FARG_TYPE_FACTORISED)
		output.polynomeFact.simplify();
	
	if(opType == TYPE_OP_ALLOC)
	{
		addEntityToVar(receiver, output);
		output.print();
	}
	else if(opType == TYPE_OP_COMPARE)
	{
		bool equal = false;
		//We can compare auxiliary and output
		if(auxiliary.matureType == output.matureType)
		{
			if(output.matureType & FARG_TYPE_NUMBER)
				equal = output.numberPure == auxiliary.numberPure;
			
			else if(output.matureType & FARG_TYPE_FACTORISED)
				equal = output.polynomeFact == auxiliary.polynomeFact;
			
			else
			{
				equal = output.polynomePure == auxiliary.polynomePure;
			}
		}
		
		if(equal)
			std::cout << "Those expressions are equal\n";
		
		else
			std::cout << "Those expressions are not equal\n";
	}
	else if(opType == TYPE_OP_CALCUL)
	{
		output.print();
	}
	
	addEntityToVar("ans", output);
	
	return output;
}

void addEntityToVar(std::string variable, Entity entity)
{
	VARIABLE content;
	
	content.type = entity.matureType;
	if(content.type & FARG_TYPE_NUMBER)
		content.number = entity.numberPure;
	else if(content.type & FARG_TYPE_FACTORISED)
		content.PolyFact = entity.polynomeFact;
	else
		content.polynomial = entity.polynomePure;
	
	Catalog::setVariableValue(variable, content);
}

Entity _parseEntity(std::string level)
{
	Entity output;
	std::vector<uint> positions;
	uint functionCode;
	
	if(havePlusOnLevel(level, positions) || haveMultOnLevel(level, positions))
	{
		std::vector<Entity> parsedLevel = _parseLevel(level, positions);
		output.setSublevel(parsedLevel);
	}
	else if(isFunction(level, functionCode))
	{
		//We extract the function argument, then evaluate the expression
		
		size_t start = level.find('[') + 1, length = level.find_last_of(']') - start;
		if(length > 0)
		{
			size_t nbArg, expected = Catalog::getNbArgsForID(functionCode);
			std::vector<uint> positions;
			std::string argument = level.substr(start, length);
			std::vector<Entity> subLevel;
			
			separateFunctionArgs(argument, positions);
			nbArg = positions.size();
			
			//expected = 0 => nbArg > 0
			//expected > 0 => nbArg == expected
			//(expected != 0 && nbArg != expected) || (expected == 0 && nbArg == expected)
			if((nbArg != expected) ^ (expected == 0))
			{
				std::stringstream error;
				error << "Invalid number of argument for function " << Catalog::getFunctionName(functionCode) << " (" << nbArg << " instead of " << Catalog::getNbArgsForID(functionCode) << "), context, " << level;
				throw std::invalid_argument(error.str());
			}
			else if(nbArg == 1)
			{
				Entity subElem = _parseEntity(argument);
				
				subElem.functionArg = subElem.wasVar;
				subLevel.push_back(subElem);
			}
			else
			{
				subLevel = _parseLevel(argument, positions);
			}
			
			output.setSublevel(subLevel);
			output.setFunction(functionCode);
		}
		else
		{
			std::stringstream error;
			error << "No valid argument for function " << Catalog::getFunctionName(functionCode) <<  "\nContext: " << level;
			throw std::invalid_argument(error.str());
		}
	}
	else
		output = parseMonome(level);

	return output;
}

std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions)
{
	std::vector<Entity> output;
	
	Entity entity;
	uint basePos = 0;
	bool dropEntity = false, doesDiv = false, foundOne = false;
	
	//Positions contain the index of operators
	for(std::vector<uint>::const_iterator current = positions.begin(); current != positions.end(); ++current)
	{
		//We're trying to add something after divising, that is not allowed
		if(doesDiv)
		{
			std::stringstream error;
			error << "Invalid division, because of the specifications, it can only be used when directly outputed";
			throw std::invalid_argument(error.str());
			break;
		}
		
		entity = _parseEntity(level.substr(basePos, *current - basePos));

		if(!foundOne)
		{
			foundOne = true;
			entity.previousOperator = OP_NONE;
		}
		else
		{
			entity.previousOperator = getPreviousOP(level[basePos-1]);
			if(entity.previousOperator == OP_POWER)
			{
				Entity & previous = output.back();
				if(entity.isReal())
				{
					previous.updatePowerOfLast(int(entity._monome.coeff.real()));
					dropEntity = true;
				}
				else	//We don't support weird powers...
				{
					std::stringstream error;
					error << "Invalid power: " << level.substr(basePos, *current) << " ~ " << *current;
					throw std::invalid_argument(error.str());
					break;
				}
			}
			else if(entity.previousOperator == OP_MULT)
			{
				Entity & previous = output.back();
				if(previous._monome.coeff == Complex::complexN(1, 0) && previous._monome.power == 0)
				{
					entity.previousOperator = previous.previousOperator;
					previous = entity;
					dropEntity = true;
				}
				else if(entity._monome.coeff == Complex::complexN(1, 0) && entity._monome.power == 0)
					dropEntity = true;
			}
		}
		
		if(dropEntity)
			dropEntity = false;
		else
			output.push_back(entity);
		
		basePos = *current + 1;
	}
	
	return output;
}