#include "parserPrivate.h"

Entity _parserCore(std::string input, int opType, bool & error)
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
			if(!Catalog::variableName(input, receiver))
			{
				error = true;
#ifdef VERBOSE
				std::cerr << "No variable of name " << input << '\n';
#endif
				return output;
			}
		}
		else
		{
			input.erase(0, 1);	//We remove the second =
			auxiliary = _parseEntity(part1, true, error);
			
			if(error)
				return output;
			
			auxiliary.maturation(error);
			
			if(error)
				return output;
		}
	}
	
	output = _parseEntity(input, opType == TYPE_OP_CALCUL, error);
	
	if(!error)
	{
		output.maturation(error);
		
		if(!error)
		{
			if(opType == TYPE_OP_ALLOC)
			{
				VARIABLE content;
				
				content.type = output.matureType;
				if(content.type & FARG_TYPE_NUMBER)
					content.number = output.numberPure;
				
				else if(content.type & FARG_TYPE_FACTORISED)
					content.polynomialFact = output.polynomeFact;
				
				else
					content.polynomial = output.polynomePure;
				
				Catalog::setVariableValue(receiver, content);
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
						equal = output.polynomePure == auxiliary.polynomePure;
				}
				
				if(equal)
					std::cout << "Those equations are equal\n";
				
				else
					std::cout << "Those equations are not equal\n";
			}
			else if(opType == TYPE_OP_CALCUL)
			{
				output.print();
			}
		}
	}
	return output;
}

Entity _parseEntity(std::string level, bool canDiv, bool & error)
{
	Entity output;
	std::vector<uint> positions;
	uint functionCode;
	
	if(havePlusOnLevel(level, positions) || haveMultOnLevel(level, positions))
	{
		std::vector<Entity> parsedLevel = _parseLevel(level, positions, canDiv, error);
		if(!error)
			output.setSublevel(parsedLevel);
	}
	else if(isFunction(level, functionCode, error))
	{
		//We extract the function argument, then evaluate the expression
		
		size_t start = level.find('[') + 1, length = level.find(']') - start;
		if(length > 0)
		{
			size_t nbArg, expected = Catalog::getNbArgsForID(functionCode);
			std::vector<uint> positions;
			std::string argument = level.substr(start, length);
			
			separateFunctionArgs(argument, positions);
			nbArg = positions.size();
			
			//expected = 0 => nbArg > 0
			//expected > 0 => nbArg == expected
			//(expected != 0 && nbArg != expected) || (expected == 0 && nbArg == expected)
			if((nbArg != expected) ^ (expected == 0))
			{
				std::cerr << "Invalid number of argument for function " << Catalog::getFunctionName(functionCode) << " (" << nbArg << " instead of " << Catalog::getNbArgsForID(functionCode) << "), context, " << level << '\n';
				error = true;
			}
			else if(nbArg == 1)
				output = _parseEntity(argument, false, error);
			else
			{
				std::vector<Entity> subLevel = _parseLevel(argument, positions, false, error);
				if(!error)
					output.setSublevel(subLevel);
			}

			if(!error)
				output.setFunction(functionCode);
		}
		else
		{
			error = true;
#ifdef VERBOSE
			std::cerr << "No valid argument for function " << Catalog::getFunctionName(functionCode) <<  "\nContext: " << level << '\n';
#endif
		}
	}
	else if(!error)
	{
		output = parseMonome(level, error);
	}
	
	return output;
}

std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions, bool canDiv, bool & error)
{
	std::vector<Entity> output;
	
	//Shouldn't happen (as error would have been triggered)
	if(positions.size() == 0)
	{
		error = true;
#ifdef VERBOSE
		std::cerr << "No positions in array\n";
#endif
		return output;
	}
	
	Entity entity;
	uint basePos = 0;
	bool dropEntity = false, doesDiv = false;
	
	//Positions contain the index of operators
	for(std::vector<uint>::const_iterator current = positions.begin(); current != positions.end(); ++current)
	{
#ifdef VERBOSE
		std::cout << "[LOG]: Current portion: " << level.substr(basePos, *current-basePos) << '\n';
#endif
		//We're trying to add something after divising, that is not allowed
		if(doesDiv)
		{
			error = true;
#ifdef VERBOSE
			std::cerr << "Invalid division, because of the specifications, it can only be used when directly outputed"<< '\n';
#endif
			break;
		}
		
		entity = _parseEntity(level.substr(basePos, *current - basePos), false, error);
		if(error)
			break;

		if(basePos <= 1)
			entity.previousOperator = OP_NONE;
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
					error = true;
#ifdef VERBOSE
					std::cerr << "Invalid power: " << level.substr(basePos, *current) << " ~ " << *current << '\n';
#endif
					break;
				}
			}
			else if(entity.previousOperator == OP_DIV)
			{
				//Division is a pretty special cas, and can't be used except on the highest level
				if(canDiv)
				{
					canDiv = false;
					doesDiv = true;
				}
				else
				{
					error = true;
#ifdef VERBOSE
					std::cerr << "Invalid division, because of the specifications, it can only be used when directly outputed"<< '\n';
#endif
					break;
				}
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