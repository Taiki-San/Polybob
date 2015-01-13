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
	uint x_exponent;

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
		
		>> (-char_("*") >> (exponent[phx::bind(&internalMonome::x_exponent, _val) = qi::_1]
							| eps[phx::bind(&internalMonome::x_exponent, _val) = SPIRIT_DEFAULT_POWER_VALUE]))
		
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

monome parseMonome(std::string str, bool & error)
{
	grammar<std::string::const_iterator> grammar;
	boost::spirit::ascii::space_type space;

	std::string::const_iterator begin = str.begin(), end = str.end();

	internalMonome internMonome;
	monome output;

	//Actual parsing
	bool success = phrase_parse(begin, end, grammar, space, internMonome);

	if (success && begin == end)
	{
		output.coef = combineComplexParser(getNumber(internMonome.coefficientBegin, error), getNumber(internMonome.coefficientEnd, error));
		output.exponent = internMonome.x_exponent;
	}
	else
	{
		error = true;
#ifdef VERBOSE
		std::string rest(begin, end);
		std::cout << "Parsing failed, stopped at: \" " << rest << "\"" << " ~ full string: " << str << '\n';
#endif
	}

	return output;
}
