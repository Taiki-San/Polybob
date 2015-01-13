#include "parserPrivate.h"

Entity parserCore(std::string input, bool & error)
{
	Entity output;
	int opType = TYPE_OP_INVALID;
	
	input.erase(remove_if(input.begin(), input.end(), isspace), input.end());

	if(!checkString(input) || (opType = syntaxAnalysis(input)) == TYPE_OP_INVALID)
	{
		error = true;
		return output;
	}
	
	std::string receiver;
	
	//If we want to alloc the result of the operation, no need to evaluate what's at the right of the equality
	if(opType == TYPE_OP_ALLOC)
	{
		size_t pos = input.find('=');
		
		if(!Catalog::variableName(input.substr(0, pos), receiver))
		{
			error = true;
#ifdef VERBOSE
			std::cerr << "No variable of name " << input.substr(0, pos) << '\n';
#endif
			return output;
		}
		else
			input = input.erase(0, pos+1);
	}
	
	output = _parseEntity(input, opType == TYPE_OP_CALCUL, error);
	
#ifdef MATURE
	if(!error)
	{
		output.maturation(error);
	}
#endif
	
	if(opType == TYPE_OP_ALLOC)
	{
		Catalog::setVariableValue(receiver, output.polynome);
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
		monome tmp = parseMonome(level, error);
		if(!error)
			output.setMonome(tmp);
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
			entity.previousOperator = PREV_OP_NONE;
		else
		{
			entity.previousOperator = getPreviousOP(level[basePos-1]);
			if(entity.previousOperator == OP_POWER)
			{
				Entity & previous = output.back();
				if(entity.isReal())
				{
					
					previous.updatePowerOfLast(int(entity._monome.coef.coefReal));
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