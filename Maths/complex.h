#ifndef COMPLEX_CPP
#define COMPLEX_CPP

#include <complex>
#include <string>
#include <sstream>
#include <cmath>
//For unary_function
#include <functional>

#include <cstdlib> //for random

namespace Complex
{
    typedef std::complex<double> complexN;

    inline complexN pickRandomBoundedComplex(const double &modulusBound)
    {
        const double sqrtHiBound = std::sqrt(modulusBound);
        const double sqrtLowBound = -sqrtHiBound;

        const double real = sqrtLowBound + static_cast<double>( rand() / (static_cast <double>(RAND_MAX / (sqrtHiBound - sqrtLowBound) )) );
        const double complex = sqrtLowBound + static_cast<double>( rand() / (static_cast <double>(RAND_MAX / (sqrtHiBound - sqrtLowBound) )) );

        return complexN(real, complex);
    }

    template<typename T> inline std::string toString(const std::complex<T> &x, const bool &prefix = false)
    {
        std::ostringstream s;
		std::complex<T> cpy = x;
		
		if(cpy.real() > -1e-15 && cpy.real() < 1e-15)
			cpy.real(0);

		if(cpy.imag() < 1e-15 && cpy.imag() > -1e-15)
			cpy.imag(0);

        /*
         * On cherche à représenter un nombre complex (de la forme a + bi) sous forme de chaîne.
         *
         * 1) Lorsque a ≠ 0 et b ≠ 0 :
         *
        *    Non préfixé : (5 + 4i) ; (-2 - 6i) ; (1 + i) ; (4 - i)
        *    Préfixé :     + (5 + 4i) ; + (-2 - 6i) ; + (1 + i) ; + (4 - i)
        *
        * 2) Lorsque a ≠ 0 et b = 0 :
        *
        *    Non préfixé : 42 ; - 5 ; 1 ; - 1
        *    Préfixé :     + 42 ; - 5 ; + 1 ; - 1
        *
        * 3) Lorsque a = 0 et b ≠ 0 :
        *
        *    Non préfixé : 42i ; - 5i ; i ; - i
        *    Préfixé :     + 42i ; - 5i ; + i ; - i
        *
        * 4) Lorsque a = 0 et b = 0 :
        *
        *    Non préfixé : 0
        *    Préfixé :     + 0
         */
        if(cpy.real() != 0 && cpy.imag() != 0) //Le nombre a une partie réel et une partie imaginaire
        {
            if(prefix)
                s << "+ ";

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
                s << "+ ";

            if(cpy.real() < 0)
                s << "- ";

            s << std::abs(cpy.real());
        }
        else if(cpy.real() == 0 && cpy.imag() != 0) //Le nombre a juste une partie imaginaire
        {
            if(prefix && cpy.imag() > 0)
                s << "+ ";

            if(cpy.imag() < 0)
                s << "- ";

            s << std::abs(cpy.imag()) << "i";
        }
        else //Le nombre est nul
        {
            if(prefix)
                s << "+ ";

            s << "0";
        }

        return s.str();
    }

    //TODO: Voir si les get sont performants…
    //Utilisation de l’ordre lexicographique
    struct lexicographicLess : public std::binary_function<const complexN &, const complexN &, bool>
    {
        inline bool operator() (const complexN &complex1, const complexN &complex2) const
        {
            return (complex1.real() < complex2.real()) ||
                   (complex1.real() == complex2.real() && complex2.imag() < complex2.imag());
        }
    };
}

#endif // COMPLEX_CPP
