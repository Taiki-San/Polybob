#include <iostream>

#include "Maths/complex.h"
#include "Maths/monomial.h"
#include "Maths/polyfact.h"
#include "Maths/polynomial.h"
#include "Maths/factor.h"

#include <vector>
#include <cstdlib> //for random

unsigned int factorial(unsigned int n)
{
    unsigned int ret = 1;

    for(unsigned int i = 1; i <= n; ++i)
        ret *= i;

    return ret;
}

unsigned int binomialCoefficient(unsigned int nValue, unsigned int nValue2)
{
    unsigned int result;
    if(nValue2 == 1)
        return nValue;

    result = (factorial(nValue)) / (factorial(nValue2) * factorial((nValue - nValue2)));
    nValue2 = result;
    return nValue2;
}

int main()
{
//    std::srand(time(NULL));

    std::vector<Polynomial> faulhaber;

    Monomial a1(Complex::complexN(1, 0), 1),
             a2(Complex::complexN(1, 0), 0);

    vectorMonomials_t a;
    a.push_back(a1);
    a.push_back(a2);

    //faulhaber[0] = z + 1
    faulhaber.push_back(Polynomial(a));

    const unsigned int bound = 3;

    /*
     * Calcul et affichage des polynômes de Faulhabber
     */
    for(unsigned int p = 0 ; p <= bound ; ++p)
    {
        if(p != 0)
        {
            Polynomial Fp = (faulhaber[0] ^ (p + 1));

            Polynomial sum;
            for(unsigned int k = 0 ; k < p ; ++k)
            {
                sum += Complex::complexN(binomialCoefficient(p + 1, k), 0) * faulhaber[k];
            }
            Fp -= sum;

            Fp *= Complex::complexN(1, 0) / Complex::complexN(p + 1, 0);
            faulhaber.push_back(Fp.fixFloatPrecision());
        }

        std::cout << "F" << p << ": " << faulhaber[p].toString() << std::endl;
        std::cout << "F" << p << " (forme factorisée): " << faulhaber[p].factor().fixFloatPrecision().toString() << std::endl;
    }

    /*
     * On vérifie que 2F1 divise Fp
     */
    Polynomial dF1 = 2 * faulhaber[1];

    for(unsigned int p = 1 ; p <= bound ; ++p)
    {
        if((faulhaber[p] / dF1).second.fixFloatPrecision().isNull())
        {
            std::cout << "2F1 divise F" << p << std::endl;
        }
    }

    /*
     * Production des polynômes avec le changement de variables
     */
    Monomial b1(Complex::complexN(1, 0), 1),
             b2(Complex::complexN(-0.5, 0), 0);

    vectorMonomials_t b;
    b.push_back(b1);
    b.push_back(b2);

    //shift = z - 1/2
    Polynomial shift(b);

    for(unsigned int p = 1 ; p <= bound ; ++p)
    {
        Polynomial Shift_Fp = faulhaber[p].composition(shift);
        Shift_Fp.fixFloatPrecision();
        std::cout << "~F" << p << ": " << Shift_Fp.toString() << std::endl;

        Polynomial Shift_FpNeg = Shift_Fp.composition(Polynomial(Monomial(Complex::complexN(-1, 0), 1)));

        if(p % 2 == 0)
        {
            if(Shift_Fp == -Shift_FpNeg)
            {
                std::cout << "Shift_Fp = -Shift_FpNeg et p est pair, la propriété est vérifiée pour p=" << p << std::endl;
            }
        }
        else
        {
            if(Shift_Fp == Shift_FpNeg)
            {
                std::cout << "Shift_Fp = Shift_FpNeg et p est impair, la propriété est vérifiée pour p=" << p << std::endl;
            }
        }
    }

    return 0;
}
