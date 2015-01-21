#include "polyfact.h"

//Création d’un polynôme nul
PolyFact::PolyFact() :
    constantFactor(Complex::complexN(0, 0))
{
}

PolyFact::PolyFact(const vectorFactors_t &factors, const Complex::complexN &constantFactor) :
    constantFactor(constantFactor)
{
    if(constantFactor == Complex::complexN(0, 0))
        return;

    for(vectorFactors_t::const_iterator iter = factors.begin() ; iter != factors.end() ; ++iter)
    {
        listFactors.push_back(*iter);
    }
}

PolyFact::PolyFact(const PolyFact &poly) :
    listFactors(poly.listFactors),
    constantFactor(poly.constantFactor)
{

}

void swap(PolyFact &first, PolyFact &second)
{
    (first.listFactors).swap(second.listFactors);
    std::swap(first.constantFactor, second.constantFactor);
}

PolyFact& PolyFact::operator=(PolyFact rhs)
{
    swap(*this, rhs);

    return *this;
}

bool PolyFact::isNull() const
{
    return constantFactor == Complex::complexN(0, 0);
}

unsigned int PolyFact::getDegree() const
{
    return this->expand().getDegree();
}

//TODO: Voir si la simplification est souhaitable
//TODO: Voir si appliquer le facteur à la fin est souhaitable
Polynomial PolyFact::expand() const
{
    Polynomial poly(Monomial(Complex::complexN(1, 0), 0));

    for(listFactors_t::const_iterator iter = listFactors.begin() ; iter != listFactors.end() ; ++iter)
    {
        Polynomial factor(*iter);
		factor.fixFloatPrecision();
        poly *= factor;
    }

    poly *= constantFactor;
	
    return poly;
}

void PolyFact::simplify()
{
    listFactors.sort(lessThanValue());

    listFactors_t::iterator actualValue = listFactors.begin();
    listFactors_t::iterator iter = ++listFactors.begin();

    while(iter != listFactors.end())
    {
        if(actualValue->getValue() == iter->getValue())
        {
            actualValue->setPower(actualValue->getPower() + iter->getPower());
            iter = listFactors.erase(iter);
        }
        else
        {
            actualValue = iter;
            ++iter;
        }
    }
	
	fixFloatPrecision();
}

std::string PolyFact::toString() const
{
    std::string outString;

    if(constantFactor != Complex::complexN(1, 0))
        outString += Complex::toString(constantFactor, false);

    for(listFactors_t::const_iterator iter = listFactors.begin() ; iter != listFactors.end() ; ++iter)
    {
        outString += iter->toString();
    }

    return outString;
}

/*
 * Multiplication
 */

PolyFact& PolyFact::operator*=(const Complex::complexN &rhs)
{
    if(rhs == Complex::complexN(0, 0))
    {
        *this = PolyFact();
        return *this;
    }

    constantFactor *= rhs;
    return *this;
}

PolyFact operator*(PolyFact lhs, const Complex::complexN &rhs)
{
    return (lhs *= rhs);
}

PolyFact operator*(const Complex::complexN &lhs, PolyFact rhs)
{
    return (rhs *= lhs);
}

PolyFact& PolyFact::operator*=(const PolyFact &rhs)
{
    if(this->isNull())
        return *this;

    if(rhs.isNull())
    {
        *this = PolyFact();
        return *this;
    }

    constantFactor *= rhs.constantFactor;
    listFactors.insert(listFactors.end(), rhs.listFactors.begin(), rhs.listFactors.end());

    return *this;
}

PolyFact operator*(PolyFact lhs, const PolyFact &rhs)
{
    return (lhs *= rhs);
}

Polynomial operator*(Polynomial lhs, const PolyFact &rhs)
{
    return (lhs *= rhs.expand());
}

Polynomial operator*(const PolyFact &lhs, Polynomial rhs)
{
    return (rhs *= lhs.expand());
}

/*
 * Puissance
 */

PolyFact& PolyFact::operator^=(unsigned int rhs)
{
    //a ^ 0 = 1 si a ≠ 0
    if(rhs == 0)
    {
        if(this->isNull())
        {
            throw std::domain_error("The expression 0^0 is undefined.");
        }
        else
        {
            throw std::domain_error("It is not possible to represent polynomials of degree 0 in factored form.");
        }
    }
    else if(rhs != 1) //Dans le cas où la puissance = 1, il n’y a rien à faire (a ^ 1 = a)
    {
        for(listFactors_t::iterator iter = listFactors.begin() ; iter != listFactors.end() ; ++iter)
        {
            iter->setPower(iter->getPower() * rhs);
        }

        constantFactor = std::pow(constantFactor, rhs);
    }

    return *this;
}

PolyFact operator^(PolyFact lhs, const unsigned int &rhs)
{
    return (lhs ^= rhs);
}

/*
 * Division
 */
//TODO: Optimisation

std::pair<Polynomial, Polynomial> operator/(const PolyFact &lhs, const PolyFact &rhs)
{
    return (lhs.expand() / rhs.expand());
}

std::pair<Polynomial, Polynomial> operator/(const Polynomial &lhs, const PolyFact &rhs)
{
    return (lhs / rhs.expand());
}

std::pair<Polynomial, Polynomial> operator/(const PolyFact &lhs, const Polynomial &rhs)
{
    return (lhs.expand() / rhs);
}

/*
 * Addition
 */

Polynomial operator+(const PolyFact &lhs, const PolyFact &rhs)
{
    return (lhs.expand() + rhs.expand());
}

Polynomial operator+(Polynomial lhs, const PolyFact &rhs)
{
    return (lhs += rhs.expand());
}

Polynomial operator+(const PolyFact &lhs, Polynomial rhs)
{
    return (rhs += lhs.expand());
}

/*
 * Soustraction
 */

PolyFact operator-(PolyFact rhs)
{
    return (rhs *= Complex::complexN(-1, 0));
}

Polynomial operator-(const PolyFact &lhs, const PolyFact &rhs)
{
    return (lhs.expand() + (-rhs).expand());
}

Polynomial operator-(Polynomial lhs, const PolyFact &rhs)
{
    return (lhs += (-rhs).expand());
}

Polynomial operator-(const PolyFact &lhs, Polynomial rhs)
{
    return (rhs += (-lhs).expand());
}

/*
 * Comparaison
 */

bool operator==(PolyFact lhs, PolyFact rhs)
{
    if(lhs.constantFactor != rhs.constantFactor)
        return false;

	return lhs.toString() == rhs.toString();
}

bool operator!=(const PolyFact &lhs, const PolyFact &rhs)
{
    return !operator==(lhs, rhs);
}

/*
 * Fonctionnalités mathématiques
 */

Polynomial PolyFact::composition(const Polynomial &poly) const
{
    return (this->expand()).composition(poly);
}

Polynomial PolyFact::decomposition(const Polynomial &g) const
{
    return (this->expand()).decomposition(g);
}

Polynomial PolyFact::derivative() const
{
    return (this->expand()).derivative();
}

Polynomial PolyFact::integral() const
{
    return (this->expand()).integral();
}

Polynomial PolyFact::gcd(const Polynomial &b) const
{
    return (this->expand()).gcd(b);
}

Complex::complexN PolyFact::evaluation(const Complex::complexN &value) const
{
    return (this->expand()).evaluation(value);
}

std::pair<Polynomial, Polynomial> PolyFact::complexExtraction() const
{
    return (this->expand()).complexExtraction();
}

PolyFact PolyFact::conjugate() const
{
    PolyFact a(*this);

    for(listFactors_t::iterator iter = a.listFactors.begin() ; iter != a.listFactors.end() ; ++iter)
    {
        if(iter->getValue().imag() != 0)
        {
            iter->setValue(std::conj(iter->getValue()));
        }
    }

    a.constantFactor = std::conj(a.constantFactor);

    return a;
}

PolyFact& PolyFact::fixFloatPrecision()
{
    for(listFactors_t::iterator iter = listFactors.begin() ; iter != listFactors.end() ; ++iter)
    {
        if(std::abs(iter->getValue().real()) < 10e-7)
            iter->setValue(Complex::complexN(0, iter->getValue().imag()));

        if(std::abs(iter->getValue().imag()) < 10e-7)
            iter->setValue(Complex::complexN(iter->getValue().real(), 0));
    }

    return *this;
}
