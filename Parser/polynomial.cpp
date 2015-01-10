#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <iostream>
#include <string>
#include <complex>
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

namespace client
{
    namespace ast
    {
        ///////////////////////////////////////////////////////////////////////////
        //  The AST
        ///////////////////////////////////////////////////////////////////////////
        struct nil {};
        struct signed_;
        struct program;
        struct monome;

        typedef boost::variant<
                nil
              , boost::recursive_wrapper<monome>
              , boost::recursive_wrapper<signed_>
              , boost::recursive_wrapper<program>
            >
        operand;

        struct signed_
        {
            char sign;
            operand operand_;
        };

        struct operation
        {
            char operator_;
            operand operand_;
        };

        struct program
        {
            operand first;
            std::list<operation> rest;
        };

        struct monome
        {
            boost::optional<std::string> coefficientBegin;
            boost::optional<std::string> coefficientEnd;
            boost::optional<int> x_exponent;
        };

        typedef struct complex
        {
            double coefficient;
            bool isComplex;
        }complexType;
    }
}

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::signed_,
    (char, sign)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::operation,
    (char, operator_)
    (client::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::program,
    (client::ast::operand, first)
    (std::list<client::ast::operation>, rest)
)

namespace client
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phx = boost::phoenix;

    namespace ast
    {
        ///////////////////////////////////////////////////////////////////////////
        //  The AST Printer
        ///////////////////////////////////////////////////////////////////////////
        struct printer
        {
            typedef void result_type;

            void operator()(nil) const {}
            void operator()(ast::monome tt) const
            {
                std::cout << "  coefficientBegin: " << (tt.coefficientBegin->empty() ? "" : *tt.coefficientBegin) << "\n";
                std::cout << "  coefficientEnd: " << (tt.coefficientEnd->empty() ? "" : *tt.coefficientEnd) << "\n"
                << "  x_exponent: " << tt.x_exponent << "\n";
            }

            void operator()(operation const& x) const
            {
                boost::apply_visitor(*this, x.operand_);
                switch (x.operator_)
                {
                    case '+': std::cout << " add"; break;
                    case '-': std::cout << " subt"; break;
                    case '*':
                    case '(': std::cout << " mult"; break;
                    case '^': std::cout << " pow"; break;
                    case '/': std::cout << " div"; break;
                    default:  std::cout << " (unsuported: " << x.operator_ << ")";
                }
            }

            void operator()(signed_ const& x) const
            {
                boost::apply_visitor(*this, x.operand_);
                switch (x.sign)
                {
                    case '-': std::cout << " neg"; break;
                    case '+': std::cout << " pos"; break;
                    // default:  std::cout << " (unsuported2: " << x.operator_ << ")";
                }
            }

            void operator()(program const& x) const
            {
                boost::apply_visitor(*this, x.first);
                BOOST_FOREACH(operation const& oper, x.rest)
                {
                    std::cout << ' ';
                    (*this)(oper);
                }
            }
        };

        ///////////////////////////////////////////////////////////////////////////
        //  The AST evaluator
        ///////////////////////////////////////////////////////////////////////////
        struct eval
        {
            typedef int result_type;

            int operator()(nil) const { BOOST_ASSERT(0); return 0; }
            int operator()(unsigned int n) const { return n; }

            int operator()(operation const& x, int lhs) const
            {
                int rhs = boost::apply_visitor(*this, x.operand_);
                switch (x.operator_)
                {
                    case '+': return lhs + rhs;
                    case '-': return lhs - rhs;
                    case '*':
                    case '(': return lhs * rhs;
                    case '^': printf("[%d ~ %d]\n", lhs, rhs); return pow(lhs, rhs);
                    case '/': return lhs / rhs;
                }
                // std::cout << "Nop :" << rhx << " ~ " << lhs << " ~ " << x.operand_;
                BOOST_ASSERT(0);
                return 0;
            }

            int operator()(signed_ const& x) const
            {
                int rhs = boost::apply_visitor(*this, x.operand_);
                switch (x.sign)
                {
                    case '-': return -rhs;
                    case '+': return +rhs;
                }
                // std::cout << "Nop :" << rhx << " ~ " << x.operand_;
                BOOST_ASSERT(0);
                return 0;
            }

            int operator()(program const& x) const
            {
                int state = boost::apply_visitor(*this, x.first);
                BOOST_FOREACH(operation const& oper, x.rest)
                {
                    state = (*this)(oper, state);
                }
                return state;
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  The calculator grammar
    ///////////////////////////////////////////////////////////////////////////////

    ast::complexType getNumber(std::string string, bool negative, bool &a)
    {
        ast::complexType complex;
        char haveI = 0;

        size_t size = string.size();
        double value;

        if(string.at(0) == 'i')
        {
            haveI++;
            string.erase(0, 1);
            size--;
        }

        if(string.at(size-1) == 'i')
        {
            haveI++;
            string.erase(size-1, 1);
            size--;
        }

        if(string.at(0) == '[')
        {
            value = 0;
        }
        else
            value = atof(string.c_str());

        if(haveI == 1)
            complex.isComplex = true;
        else
        {
            complex.isComplex = false;
            if(haveI == 2)
                value *= -1;
        }

        if(negative)
            value *= -1;

        complex.coefficient = value;
        return complex;
    }

    void print( const ast::monome& tt )
    {
        int x_exponent = (tt.x_exponent)? *tt.x_exponent : 0;

        std::cout << "term\n";
        std::cout << "  coefficientBegin: " << (tt.coefficientBegin->empty() ? "" : *tt.coefficientBegin) << "\n";
        std::cout << "  coefficientEnd: " << (tt.coefficientEnd->empty() ? "" : *tt.coefficientEnd) << "\n"
            << "  x_exponent: " << x_exponent << "\n";
    }

    template <typename Iterator>

    struct polynomial_parser : qi::grammar<Iterator, std::vector<ast::monome>(), ascii::space_type>
    {
        //Toolbox

        polynomial_parser(bool &a) : polynomial_parser::base_type(start)
        {
            namespace phx = boost::phoenix;
            using qi::_val;
            using qi::eps;

            qi::double_type uint_;
            qi::char_type char_;

            exponent %= '^' >> qi::int_;
            x_term %= 'x' >> ( exponent | eps[_val = 1] );

            //Will parse a single component
          	variable = -char_("i") >> (char_("[") >> (+char_("a-zA-Z_0-9") - char_("]")) >> char_("]") >> -char_("i")) | (+char_("0-9") >> -char_("i"));

            polynomial = eps[_val = ast::monome()]
            	>>	(variable[phx::bind(&ast::monome::coefficientBegin, _val) = qi::_1]
					| eps[phx::bind(&ast::monome::coefficientBegin, _val) = "1"])

            	>> ((char_("*") >> x_term[phx::bind(&ast::monome::x_exponent, _val) = qi::_1])
            		| (x_term[phx::bind(&ast::monome::x_exponent, _val) = qi::_1]
            			| eps[phx::bind(&ast::monome::x_exponent, _val) = 0]))

                >>  (variable[phx::bind(&ast::monome::coefficientEnd, _val) = qi::_1, a]
                    | eps[phx::bind(&ast::monome::coefficientEnd, _val) = "1"]);


            start = eps[_val = std::vector<ast::monome>()] >> polynomial[phx::push_back(_val, qi::_1)];
        }

        qi::rule<Iterator, int(), ascii::space_type> exponent;
        qi::rule<Iterator, int(), ascii::space_type> x_term;
        qi::rule<Iterator, std::string(), ascii::space_type> variable;
        qi::rule<Iterator, ast::monome(), ascii::space_type> polynomial;
        qi::rule<Iterator, std::vector<ast::monome>(), ascii::space_type> start;
    };
}

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tA polynomial parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout
        << "Give me a polynomial with terms of the form :"
        << "\"A*x^N\", where A, and N are integers\n";
    std::cout << "Type [q or Q] to quit\n\n";

    bool wentWell = true;
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef client::polynomial_parser<iterator_type> polynomial_parser;

    polynomial_parser pparser(wentWell);
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::vector<client::ast::monome> poly;
        client::ast::term term;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, pparser, space, poly);

        if (r && iter == end)
        {
            std::cout << boost::fusion::tuple_open('[');
            std::cout << boost::fusion::tuple_close(']');
            std::cout << boost::fusion::tuple_delimiter(", ");

            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            for ( std::vector< client::ast::term >::iterator it = poly.begin( );
                it != poly.end( ); ++it )
            {
                client::print( *it );
            }
            std::cout << "\n-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
