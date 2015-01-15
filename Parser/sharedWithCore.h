#ifndef Polybob_sharedWithCore_h

#define Polybob_sharedWithCore_h

#include <complex>
#include <sstream>
#include <list>
#include <cmath>

/*
 *	Placeholder until real classes are added
 */

#include "../Maths/complex.h"
#include "../Maths/monomial.h"
#include "../Maths/polynomial.h"

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

typedef std::vector<factorisedElem> vectorFact_t;

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
