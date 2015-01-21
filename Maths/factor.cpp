#include "factor.h"

const char Factor::nullPowerError[] = "The power of a factor can not be null.";

Factor::Factor(const Factor &factor) :
    value(factor.value),
    power(factor.power)
{
}

Factor::Factor(const Complex::complexN &value, const unsigned int &power) :
    value(value),
    power(power)
{
    if(power == 0)
        throw std::invalid_argument(nullPowerError);
}

void swap(Factor &first, Factor &second)
{
    std::swap(first.value, second.value);
    std::swap(first.power, second.power);
}

Factor& Factor::operator=(Factor rhs)
{
    swap(*this, rhs);
    return *this;
}

Complex::complexN Factor::getValue() const
{
    return this->value;
}

Factor& Factor::setValue(const Complex::complexN &value)
{
    this->value = value;
    return *this;
}

unsigned int Factor::getPower() const
{
    return this->power;
}

Factor& Factor::setPower(const unsigned int &power)
{
    if(power == 0)
        throw std::invalid_argument(nullPowerError);

    this->power = power;
    return *this;
}

std::string Factor::toString() const
{
    std::ostringstream s;

    s << "(z";

    if(value.real() != 0 && value.imag() != 0)
    {
        s << " - " << Complex::toString(value, false);
    }
    else if(value.real() != 0 || value.imag() != 0)
    {
        s << " " << (Complex::toString(-value, true));
    }

    s << ")";

    if(power != 1)
        s << "^" << power;

    return s.str();
}

bool operator==(const Factor &lhs, const Factor &rhs)
{
    return (lhs.value == rhs.value && lhs.power == rhs.power);
}

bool operator!=(const Factor &lhs, const Factor &rhs)
{
    return !operator==(lhs, rhs);
}
