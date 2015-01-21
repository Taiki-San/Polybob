#ifndef FACTOR_H
#define FACTOR_H

#include <sstream>
#include <stdexcept>
//For unary_function
#include <functional>

#include "../Maths/complex.h"

class Factor
{
public:
    Factor(const Factor &factor);
    Factor(const Complex::complexN &value, const unsigned int &power);

    friend void swap(Factor &first, Factor &second);
    Factor& operator=(Factor rhs);

    Complex::complexN getValue() const;
    Factor& setValue(const Complex::complexN &value);

    unsigned int getPower() const;
    Factor& setPower(const unsigned int &power);

    std::string toString() const;

    friend bool operator==(const Factor &lhs, const Factor &rhs);

private:
    Complex::complexN value;
    unsigned int power;

    static const char nullPowerError[];

};

bool operator!=(const Factor &lhs, const Factor &rhs);

//TODO: Voir si les get sont performants…
//Utilisation de l’ordre lexicographique
struct lessThanValue : public std::binary_function<const Factor &, const Factor &, bool>
{
    inline bool operator() (const Factor &factor1, const Factor &factor2) const
    {
        return Complex::lexicographicLess()(factor1.getValue(), factor2.getValue());
    }
};

#endif // FACTOR_H
