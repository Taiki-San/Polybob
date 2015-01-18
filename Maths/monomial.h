#ifndef MONOMIAL_CPP
#define MONOMIAL_CPP

//For sstream
#include <sstream>
//For unary_function
#include <functional>

#include "../Maths/complex.h"

struct Monomial
{
    Complex::complexN coeff;
    unsigned int power;

    Monomial(const Monomial &monom);
    Monomial(const Complex::complexN &coeff, const unsigned int &power);

    friend void swap(Monomial &first, Monomial &second);
    Monomial& operator=(Monomial rhs);

    Monomial& operator/=(const Monomial &rhs);

    std::string toString(const bool &prefix) const;
};

Monomial operator/(Monomial lhs, const Monomial &rhs);

bool operator==(const Monomial &lhs, const Monomial &rhs);
bool operator!=(const Monomial &lhs, const Monomial &rhs);

struct lessThanPower : public std::binary_function<const Monomial &, const Monomial &, bool>
{
    inline bool operator() (const Monomial &monomial1, const Monomial &monomial2) const
    {
        return (monomial1.power < monomial2.power);
    }
};

struct isZeroMonomial : public std::unary_function<const Monomial &, bool>
{
    bool operator()(const Monomial &monomial) const
    {
        return (monomial.coeff.real() == 0 && monomial.coeff.imag() == 0);
    }
};

#endif // MONOMIAL_CPP
