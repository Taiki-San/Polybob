#include "polynomial.h"
#include "polyfact.h"

//TODO: Voir si il vaut mieux créer une méthode pour rendre le polynôme nul
//ou si il vaut mieux garder la méthode actuel : *this = Polynomial()
//(idem avec PolyFact)
Polynomial::Polynomial()
{
}

Polynomial::Polynomial(const Monomial &monomial)
{
    if(!isZeroMonomial()(monomial))
    {
        listMonomials.push_back(monomial);
    }
}

Polynomial::Polynomial(const Factor &factor)
{
    if(factor.getValue() != Complex::complexN(0, 0))
    {
        listMonomials.push_back(Monomial(-factor.getValue(), 0));
    }

    listMonomials.push_back(Monomial(Complex::complexN(1, 0), 1));
    *this ^= factor.getPower();
}

Polynomial::Polynomial(vectorMonomials_t monomials)
{
    /*
     * On cherche à minimiser le nombre de monômes, on veut donc fusionner les
     * monômes de même exposant. On commence donc par classer les monômes pour
     * ensuite additionner ceux qui sont équivalents.
     *
     * Exemple : 9x² + 4x + 9 - 2x²
     *         = 9x² - 2x² + 4x + 9
     *         = 7x² + 4x + 9
     */
    std::sort(monomials.begin(), monomials.end(), lessThanPower());

    /*
     * On fusionne les monômes de même puissance et on les ajoute à la liste
     * chaînée.
     */
    for(size_t i = 1, size = monomials.size() ; i != (size + 1) ; ++i)
    {
        if((i < size) && (monomials[i].power == monomials[i - 1].power))
        {
            monomials[i].coeff += monomials[i - 1].coeff;
        }
        else
        {
            listMonomials.push_back(monomials[i - 1]);
        }
    }

    //On supprime les monômes nuls de la liste chaînée
    removeZeroMonomials();
}

Polynomial::Polynomial(const Polynomial &poly) :
    listMonomials(poly.listMonomials)
{
}

void swap(Polynomial &first, Polynomial &second)
{
    (first.listMonomials).swap(second.listMonomials);
}

Polynomial& Polynomial::operator=(Polynomial rhs)
{
    swap(*this, rhs);

    return *this;
}

Polynomial& Polynomial::operator=(const Monomial &rhs)
{
    listMonomials.clear();
    listMonomials.push_back(rhs);

    return *this;
}

bool Polynomial::isNull() const
{
    return listMonomials.empty();
}

unsigned int Polynomial::getDegree() const
{
    if(this->isNull())
    {
        return Polynomial::ZERO_POLYNOMIAL_DEGREE;
    }

    return this->getHighestMonomial().power;
}

unsigned int Polynomial::getNbMonomials() const
{
    return this->listMonomials.size();
}

Monomial Polynomial::getHighestMonomial() const
{
    return listMonomials.back();
}

std::string Polynomial::toString() const
{
    if(this->isNull())
    {
        return "0";
    }

    std::ostringstream outString;

    for(listMonomials_t::const_reverse_iterator iter = listMonomials.rbegin() ; iter != listMonomials.rend() ; ++iter)
    {
        outString << iter->toString(iter != listMonomials.rbegin());
    }

    return outString.str();
}

void Polynomial::debugPrint() const
{
    std::cout << "==DEBUG==\n";
    for(listMonomials_t::const_iterator iter = listMonomials.begin() ; iter != listMonomials.end() ; ++iter)
    {
        std::cout << (*iter).coeff << "z^" << (*iter).power << std::endl;
    }
    std::cout << "==END DEBUG==" << std::endl;
}

void Polynomial::removeZeroMonomials()
{
    listMonomials.erase(std::remove_if(listMonomials.begin(), listMonomials.end(), isZeroMonomial()), listMonomials.end());
}

/*
 * Multiplication par un scalaire
 */

Polynomial& Polynomial::operator*=(const Complex::complexN &rhs)
{
    //Si le polynôme est nul, le résultat le sera aussi
    if(this->isNull())
    {
        return *this;
    }

    //Si on multiplie un polynôme par zéro, le résultat est nul
    if(rhs == Complex::complexN(0, 0))
    {
        //On transforme donc le polynôme courant en polynôme nul
        *this = Polynomial();
        return *this;
    }

    for(listMonomials_t::iterator iter = listMonomials.begin() ; iter != listMonomials.end() ; ++iter)
    {
        iter->coeff *= rhs;
    }

    return *this;
}

Polynomial operator*(Polynomial lhs, const Complex::complexN &rhs)
{
    return (lhs *= rhs);
}

Polynomial operator*(const Complex::complexN &lhs, Polynomial rhs)
{
    return (rhs *= lhs);
}

/*
 * Multiplication de polynômes
 */

Polynomial& Polynomial::operator*=(const Monomial &rhs)
{
    for(listMonomials_t::iterator iter = listMonomials.begin() ; iter != listMonomials.end() ; ++iter)
    {
        iter->coeff *= rhs.coeff;
        iter->power += rhs.power;
    }

    return *this;
}

Polynomial operator*(Polynomial lhs, const Monomial &rhs)
{
    return (lhs *= rhs);
}

Polynomial operator*(const Monomial &lhs, Polynomial rhs)
{
    return (rhs *= lhs);
}

Polynomial& Polynomial::operator*=(const Polynomial &rhs)
{
    //Dans le cas où a *= a est appelé, on effectue l’opération sur une copie de a
    if(this == &rhs)
    {
        Polynomial copy(rhs);
        return *this *= copy;
    }

    //Si le polynôme est nul, le résultat le sera aussi
    if(this->isNull())
    {
        return *this;
    }

    //Si on multiplie un polynôme par un polynôme nul, le résultat est nul
    if(rhs.isNull())
    {
        //On transforme donc le polynôme courant en polynôme nul
        *this = Polynomial();
        return *this;
    }

    Polynomial initialPoly;
    swap(*this, initialPoly);

    for(listMonomials_t::const_iterator iter = rhs.listMonomials.begin() ; iter != rhs.listMonomials.end() ; ++iter)
    {
        *this += (*iter * initialPoly);
    }

    return *this;
}

Polynomial operator*(Polynomial lhs, const Polynomial &rhs)
{
    return (lhs *= rhs);
}

Polynomial& Polynomial::operator^=(unsigned int rhs)
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
            *this = Polynomial(Monomial(Complex::complexN(1, 0), 0));
        }
    }
    else if(rhs != 1) //Dans le cas où la puissance = 1, il n’y a rien à faire (a ^ 1 = a)
    {
        if(this->getDegree() == 0)
        {
            Monomial & monom = listMonomials.front();
            monom.coeff = pow(monom.coeff, rhs);
        }
        else
        {
            Polynomial r(Monomial(Complex::complexN(1, 0), 0));

            while(rhs != 0)
            {
                if(rhs % 2 == 1)
                {
                    r *= *this;
                    --rhs;
                }

                if(rhs != 0)
                {
                    *this *= *this;
                    rhs /= 2;
                }
            }

            *this = r;
        }
    }

    return *this;
}

Polynomial operator^(Polynomial lhs, const unsigned int &rhs)
{
    return (lhs ^= rhs);
}

/*
 * Division
 */

std::pair<Polynomial, Polynomial> operator/(Polynomial lhs, const Polynomial &rhs)
{
    if(rhs.isNull())
    {
        throw std::domain_error("Division by zero is forbidden.");
    }

    Polynomial q;
    Polynomial r(lhs);
    Polynomial t;
    unsigned int previousDegree;

    while(!r.isNull() && (previousDegree = r.getDegree()) >= rhs.getDegree())
    {
        t = (r.getHighestMonomial() / rhs.getHighestMonomial());
        q += t;
        r -= (t * rhs);

        //Double precision is messing with us, yay
        if(previousDegree == r.getDegree())
            r -= r.getHighestMonomial();
    }

    return std::pair<Polynomial, Polynomial>(q, r);
}

/*
 * Addition
 */

Polynomial& Polynomial::operator+=(const Polynomial &rhs)
{
    if(rhs.isNull())
        return *this;

    if(this->isNull())
    {
        listMonomials = rhs.listMonomials;
        return *this;
    }

    listMonomials_t::const_iterator lEnd = listMonomials.end();
    listMonomials_t::const_iterator rEnd = rhs.listMonomials.end();

    listMonomials_t::iterator iter1 = listMonomials.begin();
    listMonomials_t::const_iterator iter2 = rhs.listMonomials.begin();

    while(iter1 != lEnd && iter2 != rEnd)
    {
        if(iter1->power < iter2->power)
        {
            ++iter1;
        }
        else if(iter1->power > iter2->power)
        {
            listMonomials.insert(iter1, *iter2);
            ++iter2;
        }
        else
        {
            iter1->coeff += iter2->coeff;

            //Si après l’addition le monôme est nul, on le supprime de la liste
            if(isZeroMonomial()(*iter1))
            {
                //Le standart garantie que seul la référence vers le monôme nul sera affecté par erase
                iter1 = listMonomials.erase(iter1);
            }
            else
            {
                ++iter1;
            }

            ++iter2;
        }
    }

    while(iter2 != rEnd)
    {
        listMonomials.insert(iter1, *iter2);
        ++iter2;
    }

    return *this;
}

Polynomial operator+(Polynomial lhs, const Polynomial &rhs)
{
    return (lhs += rhs);
}

Polynomial& Polynomial::operator+=(const Complex::complexN &rhs)
{
    if(rhs == Complex::complexN(0, 0))
        return *this;

    if(this->isNull())
    {
        listMonomials.push_back(Monomial(rhs, 0));
        return *this;
    }

    listMonomials_t::iterator first = listMonomials.begin();

    if(first->power == 0)
    {
        first->coeff += rhs;

        if(isZeroMonomial()(*first))
            listMonomials.erase(first);
    }
    else
    {
        listMonomials.push_front(Monomial(rhs, 0));
    }

    return *this;
}

Polynomial operator+(Polynomial lhs, const Complex::complexN &rhs)
{
    return (lhs += rhs);
}

Polynomial operator+(const Complex::complexN &lhs, Polynomial rhs)
{
    return (rhs += lhs);
}

/*
 * Soustraction
 */
//TODO: Optimiser la soustraction (a - a = 0)

Polynomial operator-(Polynomial rhs)
{
    return (rhs *= Complex::complexN(-1, 0));
}

Polynomial& Polynomial::operator-=(const Polynomial &rhs)
{
    *this += (-rhs);
    return *this;
}

Polynomial operator-(Polynomial lhs, const Polynomial &rhs)
{
    return (lhs -= rhs);
}

Polynomial& Polynomial::operator-=(const Complex::complexN &rhs)
{
    *this += (-rhs);
    return *this;
}

Polynomial operator-(Polynomial lhs, const Complex::complexN &rhs)
{
    return (lhs -= rhs);
}

Polynomial operator-(const Complex::complexN &lhs, Polynomial rhs)
{
    return (rhs -= lhs);
}

/*
 * Comparaison
 */

bool operator==(const Polynomial &lhs, const Polynomial &rhs)
{
    if(lhs.getNbMonomials() != rhs.getNbMonomials())
        return false;

    listMonomials_t::const_iterator lhsIter = lhs.listMonomials.begin();
    listMonomials_t::const_iterator rhsIter = rhs.listMonomials.begin();

    while(lhsIter != lhs.listMonomials.end())
    {
        if(*lhsIter != *rhsIter)
            return false;

        ++lhsIter;
        ++rhsIter;
    }

    return true;
}

bool operator!=(const Polynomial &lhs, const Polynomial &rhs)
{
    return !operator==(lhs, rhs);
}

/*
 * Composition
 */

Polynomial Polynomial::composition(const Polynomial &poly) const
{
    Polynomial result;

    for(listMonomials_t::const_iterator iter = listMonomials.begin() ; iter != listMonomials.end() ; ++iter)
    {
        if(iter->power == 0)
            result += Polynomial(Monomial(iter->coeff, 0));
        else
            result += iter->coeff * (poly ^ iter->power);
    }

    return result;
}

/*
 * Décomposition
 */

Polynomial Polynomial::decomposition(const Polynomial &g) const
{
    if(this->isNull())
        return Polynomial();

    if(this->getDegree() == 0)
    {
        return Polynomial(this->getHighestMonomial());
    }

    static const char notDecomposable[] =
        "It is not possible to decompose these polynomials: there is no polynomial f such that h(x) = f(g(x)).";

    /*
     * Si g(x) = 0 ou g(x) = a, f(x) existe si et seulement si h(x) = 0 ou h(x) = b.
     * Or ces deux cas ont été traités ci-dessus.
     */
    if(g.getDegree() == 0 || g.isNull())
    {
        throw std::logic_error(notDecomposable);
    }

    //Si deg h n’est pas divisible par deg g, alors f(x) n’existe pas
    if(this->getDegree() % g.getDegree() != 0)
    {
        throw std::logic_error(notDecomposable);
    }

    Polynomial remainder(*this);

    Polynomial f;
    unsigned int degF = this->getDegree() / g.getDegree();

    for(unsigned int power = (degF + 1) ; power-- != 0 && !remainder.isNull() ;)
    {
        Polynomial node(g);
        node ^= power;

        if(remainder.getDegree() > node.getDegree())
        {
            throw std::logic_error(notDecomposable);
        }
        else if(remainder.getDegree() < node.getDegree())
            continue;

        Complex::complexN factor = remainder.getHighestMonomial().coeff / node.getHighestMonomial().coeff;

        f.listMonomials.insert(f.listMonomials.begin(), Monomial(factor, power));

        node *= factor;
        remainder -= node;
    }

    if(!remainder.isNull())
    {
        throw std::logic_error(notDecomposable);
    }

    return f;
}

Polynomial Polynomial::derivative() const
{
    if(this->getDegree() == 0 || this->isNull())
    {
        return Polynomial();
    }

    Polynomial derivative(*this);

    listMonomials_t::iterator iter = derivative.listMonomials.begin();
    while(iter != derivative.listMonomials.end())
    {
        if(iter->power == 0)
        {
            iter = derivative.listMonomials.erase(iter++);
        }
        else
        {
            iter->coeff *= iter->power;
            --iter->power;
            ++iter;
        }
    }

    return derivative;
}

Polynomial Polynomial::integral() const
{
    if(this->isNull())
    {
        return Polynomial();
    }

    Polynomial integral(*this);

    for(listMonomials_t::iterator iter = integral.listMonomials.begin() ; iter != integral.listMonomials.end() ; ++iter)
    {
        iter->coeff /= ++iter->power;
    }

    return integral;
}

Polynomial Polynomial::gcd(Polynomial b) const
{
    if(*this == b)
    {
        return *this;
    }

    Polynomial a(*this);
    Polynomial c;

    while(!b.isNull())
    {
        c = (a / b).second;
        a = b;
        b = c;
    }

    //Si il ne l’est pas, on rend le polynôme unitaire
    if(a.getHighestMonomial().coeff != Complex::complexN(1, 0))
    {
        a *= (Complex::complexN(1, 0) / a.getHighestMonomial().coeff);
    }

    return a;
}

Complex::complexN Polynomial::evaluation(const Complex::complexN &value) const
{
    if(this->isNull())
        return Complex::complexN(0, 0);

    if(this->getDegree() == 0)
        return this->getHighestMonomial().coeff;

    listMonomials_t::const_reverse_iterator iter = listMonomials.rbegin();

    //Légère optimisation, on initialise pour gagner une itération (et donc une multiplication)
    Complex::complexN result = iter->coeff;
    ++iter;

    for(unsigned int i = this->getDegree() ; i-- != 0 ;)
    {
        result *= value;

        if(i == iter->power)
        {
            result += iter->coeff;
            ++iter;
        }
    }

    return result;
}

std::pair<Polynomial, Polynomial> Polynomial::complexExtraction() const
{
    Polynomial real(*this);
    Polynomial imag(*this);

    listMonomials_t::iterator realIter = real.listMonomials.begin();
    listMonomials_t::iterator imagIter = imag.listMonomials.begin();

    while(realIter != real.listMonomials.end())
    {
        realIter->coeff = realIter->coeff.real();
        imagIter->coeff = Complex::complexN(0, imagIter->coeff.imag());

        if(isZeroMonomial()(*realIter))
            realIter = real.listMonomials.erase(realIter);
        else
            ++realIter;

        if(isZeroMonomial()(*imagIter))
            imagIter = imag.listMonomials.erase(imagIter);
        else
            ++imagIter;
    }

    return std::pair<Polynomial, Polynomial>(real, imag);
}

Polynomial Polynomial::conjugate() const
{
    if(this->isNull())
        return *this;

    Polynomial a(*this);

    for(listMonomials_t::iterator iter = a.listMonomials.begin() ; iter != a.listMonomials.end() ; ++iter)
    {
        if(iter->coeff.imag() != 0)
        {
            iter->coeff = std::conj(iter->coeff);
        }
    }

    return a;
}

vectorRoots_t Polynomial::getRoots() const
{
    const unsigned int degree = this->getDegree();

    //Si le polynôme est nul ou de degré 0, il n’y a pas de solutions
    if(this->isNull() || degree == 0)
        return vectorRoots_t();

    if(this->getNbMonomials() == 1)
    {
        return vectorRoots_t(degree, Complex::complexN(0, 0));
    }

    //On trouve une borne des solutions à l’aide du théorème de Rouché
    double bound = 0;

    listMonomials_t::const_iterator iter = listMonomials.begin();
    listMonomials_t::const_iterator penultimate = --listMonomials.end();

    for(; iter != penultimate ; ++iter)
    {
        bound += std::abs(iter->coeff);

        //std::cout << "B: " << bound << std::endl;
    }

    bound *= (1.0 / std::abs(iter->coeff));
    bound = std::max(1.0, bound);

    //std::cout << "Bound: " << bound << std::endl;

    //On utilise la borne pour générer un jeu de racines aléatoires
    vectorRoots_t roots;
    roots.reserve(degree);

    while(roots.size() != degree)
    {
        for(size_t i = 0 ; i < degree ; ++i)
        {
            roots.push_back(Complex::pickRandomBoundedComplex(bound));
            //std::cout << "Rand roots: " << roots[i] << std::endl;
        }

        //On supprime les doubles (il faut que le jeu de racines initial soit
        //composé de complexes uniques)
        std::sort(roots.begin(), roots.end(), Complex::lexicographicLess());
        roots.erase(std::unique(roots.begin(), roots.end()), roots.end());
    }

    //On dispose du jeu initial, on peut commencer à itérer
    const Polynomial derivate(this->derivative());
    vectorRoots_t offset(degree);

    double convergence = 10;

    for(size_t iter = 0 ; iter < 50 && convergence != 0.0 ; ++iter)
    {
        convergence = 0;

        for(size_t k = 0 ; k < degree ; ++k)
        {
            Complex::complexN inverseSum(0, 0);
            for(size_t j = 0 ; j < degree ; ++j)
            {
                if(j == k)
                    continue;

                inverseSum += (Complex::complexN(1, 0) / (roots[k] - roots[j]));
            }

            Complex::complexN diff = (this->evaluation(roots[k]) / derivate.evaluation(roots[k]));
            //std::cout << "Diff:" << diff << std::endl;

            offset[k] = -(diff / (Complex::complexN(1, 0) - (diff * inverseSum)));
            convergence += std::abs(offset[k]);
            //std::cout << "Offset " << k << ":" << offset[k] << std::endl;
            roots[k] += offset[k];
            //std::cout << "Root " << k << ":" << roots[k] << std::endl;
        }

        //std::cout << "Convergence: " << convergence << std::endl;
    }

    return roots;
}

PolyFact Polynomial::factor() const
{
    vectorRoots_t roots = this->getRoots();

    vectorFactors_t factors;
    factors.reserve(roots.size());

    for(vectorRoots_t::const_iterator iter = roots.begin() ; iter != roots.end() ; ++iter)
    {
        factors.push_back(Factor(*iter, 1));
    }

    return PolyFact(factors, this->getHighestMonomial().coeff);
}
