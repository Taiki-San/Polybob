#ifndef POLYFACT_H
#define POLYFACT_H

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "../Maths/complex.h"
#include "../Maths/factor.h"
#include "../Maths/polynomial.h"

typedef std::vector<Factor> vectorFactors_t;
typedef std::list<Factor> listFactors_t;

class PolyFact
{
public:
    //Création d’un polynôme nul
    PolyFact();
    //Création d’un polynôme avec les monômes donnés en argument
    PolyFact(const vectorFactors_t &factors, const Complex::complexN &constantFactor);
    //Création d’un polynôme à partir d’un autre polynôme donné en argument
    PolyFact(const PolyFact &poly);

    friend void swap(PolyFact &first, PolyFact &second);
    PolyFact& operator=(PolyFact rhs);

    bool isNull() const;
    unsigned int getDegree() const;
    Polynomial expand() const;
    void simplify();

    std::string toString() const;

    //Opérateurs
    PolyFact& operator*=(const Complex::complexN &rhs);
    PolyFact& operator*=(const PolyFact &rhs);

    PolyFact& operator^=(unsigned int rhs);

    //TODO: Pour l’instant, par sécurité, == prend des copies pour ne pas les modifier
    //avec le simplify. Ce comportement doit peut-être être changé
    friend bool operator==(PolyFact lhs, PolyFact rhs);

    //Fonctionnalités mathématiques
    Polynomial composition(const Polynomial &poly) const;
    Polynomial decomposition(const Polynomial &g) const;

    Polynomial derivative() const;
    Polynomial integral() const;

    Polynomial gcd(const Polynomial &b) const;

    Complex::complexN evaluation(const Complex::complexN &value) const;

    std::pair<Polynomial, Polynomial> complexExtraction() const;
    PolyFact conjugate() const;

    PolyFact& fixFloatPrecision();

private:
    listFactors_t listFactors;
    Complex::complexN constantFactor;

};

PolyFact operator*(PolyFact lhs, const Complex::complexN &rhs);
PolyFact operator*(const Complex::complexN &lhs, PolyFact rhs);

PolyFact operator*(PolyFact lhs, const PolyFact &rhs);
Polynomial operator*(Polynomial lhs, const PolyFact &rhs);
Polynomial operator*(const PolyFact &lhs, Polynomial rhs);

PolyFact operator^(PolyFact lhs, const unsigned int &rhs);

std::pair<Polynomial, Polynomial> operator/(const PolyFact &lhs, const PolyFact &rhs);
std::pair<Polynomial, Polynomial> operator/(const Polynomial &lhs, const PolyFact &rhs);
std::pair<Polynomial, Polynomial> operator/(const PolyFact &lhs, const Polynomial &rhs);

Polynomial operator+(const PolyFact &lhs, const PolyFact &rhs);
Polynomial operator+(Polynomial lhs, const PolyFact &rhs);
Polynomial operator+(const PolyFact &lhs, Polynomial rhs);

PolyFact operator-(PolyFact rhs);

Polynomial operator-(const PolyFact &lhs, const PolyFact &rhs);
Polynomial operator-(Polynomial lhs, const PolyFact &rhs);
Polynomial operator-(const PolyFact &lhs, Polynomial rhs);

//operator== is friend of Polynomial
bool operator!=(const PolyFact &lhs, const PolyFact &rhs);

#endif // POLYFACT_H
