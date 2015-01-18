#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
#include <climits>
#include <utility> //Pairs
#include <stdexcept> //Exceptions

#include "../Maths/complex.h"
#include "../Maths/monomial.h"
#include "../Maths/factor.h"

typedef std::vector<Monomial> vectorMonomials_t;
typedef std::list<Monomial> listMonomials_t;
typedef std::vector<Complex::complexN> vectorRoots_t;

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
    //Création d’un polynôme avec un monôme donné en argument
    Polynomial(const Monomial &monomial);
    //Création d’un polynôme avec un facteur donné en argument
    Polynomial(const Factor &factor);
    //Création d’un polynôme avec les monômes donnés en argument
    Polynomial(vectorMonomials_t monomials);
    //Création d’un polynôme à partir d’un autre polynôme donné en argument
    Polynomial(const Polynomial &poly);

    friend void swap(Polynomial &first, Polynomial &second);
    Polynomial& operator=(Polynomial rhs);
    Polynomial& operator=(const Monomial &rhs);

    bool isNull() const;
    unsigned int getDegree() const;
    unsigned int getNbMonomials() const;
    Monomial getHighestMonomial() const;

    std::string toString() const;
    void debugPrint() const;

    //Opérateurs
    Polynomial& operator*=(const Complex::complexN &rhs);
    Polynomial& operator*=(const Monomial &rhs);
    Polynomial& operator*=(const Polynomial &rhs);

    Polynomial& operator^=(unsigned int rhs);

    Polynomial& operator+=(const Polynomial &rhs);
    Polynomial& operator+=(const Complex::complexN &rhs);
    Polynomial& operator-=(const Polynomial &rhs);
    Polynomial& operator-=(const Complex::complexN &rhs);

    friend bool operator==(const Polynomial &lhs, const Polynomial &rhs);

    //Fonctionnalités mathématiques
    Polynomial composition(const Polynomial &poly) const;
    Polynomial decomposition(const Polynomial &g) const;

    Polynomial derivative() const;
    Polynomial integral() const;

    Polynomial gcd(Polynomial b) const;

    Complex::complexN evaluation(const Complex::complexN &value) const;

    std::pair<Polynomial, Polynomial> complexExtraction() const;

    vectorRoots_t getRoots() const;

private:
    listMonomials_t listMonomials;

    void removeZeroMonomials();

};

Polynomial operator*(Polynomial lhs, const Complex::complexN &rhs);
Polynomial operator*(const Complex::complexN &lhs, Polynomial rhs);

Polynomial operator*(Polynomial lhs, const Monomial &rhs);
Polynomial operator*(const Monomial &lhs, Polynomial rhs);

Polynomial operator*(Polynomial lhs, const Polynomial &rhs);

Polynomial operator^(Polynomial lhs, const unsigned int &rhs);

std::pair<Polynomial, Polynomial> operator/(Polynomial lhs, const Polynomial &rhs);

Polynomial operator+(Polynomial lhs, const Polynomial &rhs);
Polynomial operator+(Polynomial lhs, const Complex::complexN &rhs);
Polynomial operator+(const Complex::complexN &lhs, Polynomial rhs);

Polynomial operator-(Polynomial rhs);
Polynomial operator-(Polynomial lhs, const Polynomial &rhs);
Polynomial operator-(Polynomial lhs, const Complex::complexN &rhs);
Polynomial operator-(const Complex::complexN &lhs, Polynomial rhs);

//operator== is friend of Polynomial
bool operator!=(const Polynomial &lhs, const Polynomial &rhs);

#endif // POLYNOMIAL_H
