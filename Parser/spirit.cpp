/*=============================================================================
	Copyright (c) 2001-2011 Joel de Guzman

	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A Calculator example demonstrating generation of AST. The AST,
//  once created, is traversed, 1) To print its contents and
//  2) To evaluate the result.
//
//  [ JDG April 28, 2008 ]      For BoostCon 2008
//  [ JDG February 18, 2011 ]   Pure attributes. No semantic actions.
//
///////////////////////////////////////////////////////////////////////////////

#include "parserPrivate.h"

#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

typedef struct localMonome
{
	std::string coefficientBegin;
	std::string coefficientEnd;
	uint x_exponent;
} internalMonome;

namespace client
{
	///////////////////////////////////////////////////////////////////////////////
	//  The grammar
	///////////////////////////////////////////////////////////////////////////////

	template <typename Iterator>
	struct calculator : qi::grammar<Iterator, internalMonome(), ascii::space_type>
	{
		calculator() : calculator::base_type(polynomial)
		{
			using qi::_val;
			using qi::eps;
			qi::char_type char_;

			//This part is going to parse polynoms
			x_term = char_("x") >> ((char_("^") >> qi::uint_[_val = qi::_1]) | eps[_val = 1]);

			//Will parse a single component
			variable = -char_("i")
						>> ((char_("{") >> (+char_("a-zA-Z_0-9") - char_("}")) >> char_("}"))
						| (+char_("0-9") >> -(char_(".") >> +char_("0-9"))))
						>> -char_("i");

			//Monome core
			polynomial = eps[_val = internalMonome()]
				>>	(variable[phx::bind(&internalMonome::coefficientBegin, _val) = qi::_1]
					| eps[phx::bind(&internalMonome::coefficientBegin, _val) = "1"])

				>> (-char_("*") >> (x_term[phx::bind(&internalMonome::x_exponent, _val) = qi::_1]
								| eps[phx::bind(&internalMonome::x_exponent, _val) = SPIRIT_DEFAULT_POWER_VALUE]))

				>>  (variable[phx::bind(&internalMonome::coefficientEnd, _val) = qi::_1]
					| eps[phx::bind(&internalMonome::coefficientEnd, _val) = "1"]);
		}

		qi::rule<Iterator, internalMonome(), ascii::space_type> polynomial;
		qi::rule<Iterator, uint(), ascii::space_type> x_term;
		qi::rule<Iterator, std::string(), ascii::space_type> variable;
	};
}

monome parseMonome(std::string str, bool & error)
{
	client::calculator<std::string::const_iterator> calc;
	boost::spirit::ascii::space_type space;

	std::string::const_iterator begin = str.begin();
	std::string::const_iterator end = str.end();

	internalMonome internMonome;
	monome output;

	bool r = phrase_parse(begin, end, calc, space, internMonome);

	if (r && begin == end)
	{
		error = false;
		output.coef = combineComplexParser(getNumber(internMonome.coefficientBegin), getNumber(internMonome.coefficientEnd));
		output.exponent = internMonome.x_exponent;
	}
	else
	{
		error = true;

		std::string rest(begin, end);
#ifdef VERBOSE
		std::cout << "Parsing failed, stopped at: \" " << rest << "\"" << " ~ full string: " << str << '\n';
#endif
	}

	return output;
}
