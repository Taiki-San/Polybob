#include "parserPrivate.h"

Entity parserCore(std::string input, int opType, bool & error)
{
	Entity output;
	std::string receiver;
	
	//If we want to alloc the result of the operation, no need to evaluate what's at the right of the equality
	if(opType == TYPE_OP_ALLOC)
	{
		size_t pos = input.find('=');
		
		if(!Variables::variableName(input.substr(0, pos), receiver))
		{
			error = true;
			return output;
		}
		else
			input = input.erase(0, pos+1);
	}
	
	output = _parseEntity(input, error);
	
	return output;
}

Entity _parseEntity(std::string level, bool & error)
{
	Entity output;
	std::vector<uint> positions;
	std::string functionName;
	
	if(havePlusOnLevel(level, positions) || haveMultOnLevel(level, positions))
	{
		std::vector<Entity> parsedLevel = _parseLevel(level, positions, error);
		if(!error)
			output.setSublevel(parsedLevel);
	}
	else if(isFunction(level, functionName, error))
	{
		//We extract the function argument, then evaluate the expression
		
		size_t start = level.find('[') + 1, length = level.find(']') - start;
		if(length > 0)
		{
			output = _parseEntity(level.substr(start, length), error);
			if(!error)
				output.setFunction(functionName);
		}
		else
			error = true;
	}
	else if(!error)
	{
		monome tmp = parseMonome(level, error);
		if(!error)
			output.setMonome(tmp);
	}
	
	return output;
}

std::vector<Entity> _parseLevel(std::string level, std::vector<uint> positions, bool & error)
{
	std::vector<Entity> output;
	
	//Shouldn't happen (as error would have been triggered)
	if(positions.size() == 0)
	{
		error = true;
		return output;
	}
	
	Entity entity;
	uint basePos = 0;
	bool dropEntity = false;
	
	//Positions contain the index of operators
	for(std::vector<uint>::const_iterator current = positions.begin(); current != positions.end(); ++current)
	{
		entity = _parseEntity(level.substr(basePos, *current), error);
		if(error)
			break;

		if(basePos <= 1)
			entity.previousOperator = PREV_OP_NONE;
		else
		{
			entity.previousOperator = getPreviousOP(level[basePos-1]);
			if(entity.previousOperator == OP_POWER)
			{
				Entity previous = output.back();
				if(entity.isReal())
				{
					previous.power += int(entity._monome.coef.coefReal);
					dropEntity = true;
				}
				else	//We don't support weird powers...
				{
					error = true;
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