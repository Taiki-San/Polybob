#ifndef Polybob_sharedWithCore_h

#define Polybob_sharedWithCore_h

#include <complex>
#include <sstream>
#include <list>
#include <cmath>

#define INVALID_FUNCTION_ID UINT_MAX

enum
{
	FCODE_EXPAND		= 0,
	FCODE_FACTOR		= 1,
	FCODE_EVALUATE		= 2,
	FCODE_INTERPOLATE	= 3,
	FCODE_COMPOSITION	= 4,
	FCODE_TEST			= 5,
	FCODE_DUMP			= 6
} FUNCTION_CODES;

enum
{
	FARG_TYPE_POLY_NOFACT 	= 1 << 0,
	FARG_TYPE_FACTORISED	= 1 << 1,
	FARG_TYPE_POLY			= FARG_TYPE_POLY_NOFACT | FARG_TYPE_FACTORISED,
	FARG_TYPE_REAL			= 1 << 2,
	FARG_TYPE_COMPLEX		= 1 << 3,
	FARG_TYPE_NUMBER		= FARG_TYPE_REAL | FARG_TYPE_COMPLEX
};

/*
 *	Placeholder until real classes are added
 */

namespace Complex
{
	typedef std::complex<double> complexN;
	
	template<typename T> inline std::string toString(const std::complex<T> &x)
	{
		std::ostringstream s;
		
		s << "(" << x.real();
		
		if(x.imag() >= 0)
			s << "+";
		
		s << x.imag() << "i)";
		return s.str();
	}
}

struct Monomial
{
	Complex::complexN coeff;
	unsigned int power;
	
	Monomial(const Complex::complexN &coeff, const unsigned int &power) : coeff(coeff), power(power) {}
};

typedef struct factorisedElem
{
	Complex::complexN coef;
	uint power;
} factorisedElem;

typedef std::vector<Monomial> vectorMonomials_t;
typedef std::vector<factorisedElem> vectorFact_t;

//Generic
class Polynomial
{
public:
	/*
	 * Par convention, le degré d’un polynôme nul est -∞. Ici, on réserve UINT_MAX
	 * pour cet usage.
	 */
	static const unsigned int ZERO_POLYNOMIAL_DEGREE = UINT_MAX;
	
	//Création d’un polynôme nul
	Polynomial();
	//Création d’un polynôme avec les monômes donnés en argument
	Polynomial(vectorMonomials_t monomials);
	//Création d’un polynôme à partir d’un autre polynôme donné en argument
	Polynomial(const Polynomial &poly);
};

//Factorised
class PolynomialFact
{
public:
	/*
	 * Par convention, le degré d’un polynôme nul est -∞. Ici, on réserve UINT_MAX
	 * pour cet usage.
	 */
	static const unsigned int ZERO_POLYNOMIAL_DEGREE = UINT_MAX;
	
	//Création d’un polynôme nul
	PolynomialFact();
	//Création d’un polynôme avec les monômes donnés en argument
	PolynomialFact(vectorFact_t monomials);
	//Création d’un polynôme à partir d’un autre polynôme donné en argument
	PolynomialFact(const PolynomialFact &poly);
};


#endif
