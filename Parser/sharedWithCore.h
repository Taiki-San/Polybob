#ifndef Polybob_sharedWithCore_h

#define Polybob_sharedWithCore_h

#include <complex>
#include <sstream>
#include <list>
#include <cmath>

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
	uint power;
	
	Monomial(const Complex::complexN &coeff, const unsigned int &power) : coeff(coeff), power(power) {}
};

typedef struct factorisedElem
{
	Complex::complexN coef;
	uint power;
} factorisedElem;

typedef struct divResult
{
	uint quotient;
	uint remain;
	
} divResult;

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
