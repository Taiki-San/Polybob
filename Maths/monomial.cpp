#include "monomial.h"

Monomial::Monomial(const Monomial &monom) :
    coeff(monom.coeff),
    power(monom.power)
{
}

Monomial::Monomial(const Complex::complexN &coeff, const unsigned int &power) :
    coeff(coeff),
    power(power)
{
}

void swap(Monomial &first, Monomial &second)
{
    std::swap(first.coeff, second.coeff);
    std::swap(first.power, second.power);
}

Monomial& Monomial::operator=(Monomial rhs)
{
    swap(*this, rhs);
    return *this;
}

Monomial& Monomial::operator/=(const Monomial &rhs)
{
    if(rhs.power > this->power)
        return *this;

    this->coeff /= rhs.coeff;
    this->power -= rhs.power;

    return *this;
}

Monomial operator/(Monomial lhs, const Monomial &rhs)
{
    return (lhs /= rhs);
}

bool operator==(const Monomial &lhs, const Monomial &rhs)
{
    return (lhs.coeff == rhs.coeff && lhs.power == rhs.power);
}

bool operator!=(const Monomial &lhs, const Monomial &rhs)
{
    return !operator==(lhs, rhs);
}

std::string Monomial::toString(const bool &prefix) const
{
    std::ostringstream s;
	Complex::complexN cpy = coeff;
	
	if(cpy.real() > -1e-15 && cpy.real() < 1e-15)
		cpy.real(0);
	
	if(cpy.imag() < 1e-15 && cpy.imag() > -1e-15)
		cpy.imag(0);

    /*
     * On cherche à représenter un nombre complex (de la forme a + bi) sous forme de chaîne.
     * Si “prefix” est vrai, on veut forcer l’affichage de l’opérateur devant le nombre.
     *
     * 1) Lorsque a ≠ 0 et b ≠ 0 :
     *
     *    Non préfixé : (5 + 4i)z ; (-2 - 6i)z^2 ; (1 + i)z ; (4 - i)
     *    Préfixé :     + (5 + 4i)z ; + (-2 - 6i)z ; + (1 + i)z ; + (4 - i)
     *
     * 2) Lorsque a ≠ 0 et b = 0 :
     *
     *    Non préfixé : 42z ; - 5z ; z ; - z ; 1
     *    Préfixé :     + 42z ; - 5z ; + z ; - z ; + 1
     *
     * 3) Lorsque a = 0 et b ≠ 0 :
     *
     *    Non préfixé : 42iz ; - 5iz ; iz ; - iz ; i
     *    Préfixé :     + 42iz ; - 5iz ; + iz ; - iz ; + i
     *
     * 4) Lorsque a = 0 et b = 0 :
     *
     *    Non préfixé : 0
     *    Préfixé :     + 0
     */
    //TODO: Problème d’espace avec le signe.
    //Par exemple, avec prefix = false, le monôme (0,-4) devrait devenir "- 4i", mais devient actuellement
    // " - 4i" (une espace en trop)
    //N.B. : le problème n’est pas cantonné au imaginaires pures
    if(cpy.real() != 0 && cpy.imag() != 0) //Le nombre a une partie réel et une partie imaginaire
    {
        if(prefix)
            s << " + ";

        s << "(" << cpy.real();

        //On ajoute le signe de la partie imaginaire
        if(cpy.imag() > 0)
            s << " + ";
        else
            s << " - ";

        //On ajoute la partie imaginaire (sans son signe qui est déjà présent)
        if(std::abs(cpy.imag()) != 1)
            s << std::abs(cpy.imag());

        s << "i)";
    }
    else if(cpy.real() != 0 && cpy.imag() == 0) //Le nombre a juste une partie réel
    {
        if(prefix && cpy.real() > 0)
            s << " + ";

        if(cpy.real() < 0)
            s << " - ";

        /*
         * Si la puissance égale 0 : dans ce cas, la variable n’est pas affichée (z^0 = 1),
         * donc il faut bien afficher quelque chose.
         *
         * Ou si la partie réelle n’est pas une unité : il faut afficher la partie réelle car
         * elle ne vaut pas 1 (ou -1).
         */
        if(power == 0 || std::abs(cpy.real()) != 1)
            s << std::abs(cpy.real());
    }
    else if(cpy.real() == 0 && cpy.imag() != 0) //Le nombre a juste une partie imaginaire
    {
        if(prefix && cpy.imag() > 0)
            s << " + ";

        if(cpy.imag() < 0)
            s << " - ";

        /*
         * Même remarque (voire ci-dessus) avec la partie imaginaire
         */
        if(power == 0 || std::abs(cpy.imag()) != 1)
            s << std::abs(cpy.imag());

        s << "i";
    }
    else //Le nombre est nul
    {
        if(prefix)
            s << " + ";

        s << "0";
    }

    if(power != 0) //Si la puissance égale 0, on n’affiche rien
    {
        if(power == 1)
            s << "z";
        else
        {
            s << "z^";
            s << power;
        }
    }

    return s.str();
}
