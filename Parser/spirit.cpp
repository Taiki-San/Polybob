#include "parserPrivate.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

typedef struct localMonome
{
	std::string coefficientBegin;
	std::string coefficientEnd;
	uint exponent;

} internalMonome;

/*
 *	Parser grammar
 */
template <typename Iterator>
struct grammar : qi::grammar<Iterator, internalMonome(), ascii::space_type>
{
	grammar() : grammar::base_type(polynomial)
	{
		using qi::_val;
		using qi::eps;
		qi::char_type char_;
		
		//This part is going to parse polynoms
		exponent = char_("x") >> ((char_("^") >> qi::uint_[_val = qi::_1]) | eps[_val = 1]);
		
		//Will parse a single component
		variable = -char_("i")
		>> ((char_("{") >> (+(ascii::char_ - char_("}"))) >> char_("}"))
			| (+char_("0-9") >> -(char_(".") >> +char_("0-9"))))
		>> -char_("i");
		
		//Monome core
		polynomial = eps[_val = internalMonome()]
		>>	(variable[phx::bind(&internalMonome::coefficientBegin, _val) = qi::_1]
			 | eps[phx::bind(&internalMonome::coefficientBegin, _val) = "1"])
		
		>> (-char_("*") >> (exponent[phx::bind(&internalMonome::exponent, _val) = qi::_1]
							| eps[phx::bind(&internalMonome::exponent, _val) = SPIRIT_DEFAULT_POWER_VALUE]))
		
		>>  (variable[phx::bind(&internalMonome::coefficientEnd, _val) = qi::_1]
			 | eps[phx::bind(&internalMonome::coefficientEnd, _val) = "1"]);
	}
	
	qi::rule<Iterator, internalMonome(), ascii::space_type> polynomial;
	qi::rule<Iterator, std::string(), ascii::space_type> variable;
	qi::rule<Iterator, uint(), ascii::space_type> exponent;
};

/*
 *	Parser entrypoint
 */

Entity _parseSubElement(VARIABLE element)
{
	Entity stage = Entity();
	stage.isMature = true;
	stage.matureType = element.type;
	
	if(element.type & FARG_TYPE_NUMBER)
		stage.numberPure = element.number;
	
	else if(element.type & FARG_TYPE_FACTORISED)
		stage.polynomeFact = element.polynomialFact;
	
	else
		stage.polynomePure = element.polynomial;
	
	return stage;
}

Entity parseMonome(std::string str, bool & error)
{
	grammar<std::string::const_iterator> grammar;
	boost::spirit::ascii::space_type space;

	std::string::const_iterator begin = str.begin(), end = str.end();

	internalMonome internMonome;

	//Actual parsing
	bool success = phrase_parse(begin, end, grammar, space, internMonome);

	if (success && begin == end)
	{
		VARIABLE first = getNumber(internMonome.coefficientBegin, error), second = getNumber(internMonome.coefficientEnd, error);
		Entity output;
		
		if((first.type & FARG_TYPE_NUMBER) && (second.type & FARG_TYPE_NUMBER))
		{
			output.setMonome(monome(first.number * second.number, internMonome.exponent));
		}
		else
		{
			Entity stage;
			std::vector<Entity> sublevel;
			
			if(!(first.type & FARG_TYPE_NUMBER) || (first.number != Complex::complexN(1, 0)))
				sublevel.push_back(_parseSubElement(first));
			
			if(internMonome.exponent > 0)
			{
				Entity stage;
				stage.setMonome(monome(1, internMonome.exponent));
				sublevel.push_back(stage);
			}
			
			if(!(second.type & FARG_TYPE_NUMBER) || (second.number != Complex::complexN(1, 0)))
				sublevel.push_back(_parseSubElement(second));

			output.setSublevel(sublevel);
		}
		return output;
	}

	error = true;
#ifdef VERBOSE
	std::string rest(begin, end);
	std::cout << "Parsing failed, stopped at: \" " << rest << "\"" << " ~ full string: " << str << '\n';
#endif
	
	return Entity();
}
