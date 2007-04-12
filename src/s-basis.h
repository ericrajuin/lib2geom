/*
 *  s-basis.h - S-power basis function class + supporting classes
 *
 *  Authors:
 *   Nathan Hurst <njh@mail.csse.monash.edu.au>
 *   Michael Sloan <mgsloan@gmail.com>
 *
 * Copyright (C) 2006-2007 authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 */

#ifndef SEEN_SBASIS_H
#define SEEN_SBASIS_H
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <math.h>

namespace Geom{

class SBasis;

class Hat{
public:
    Hat () {}
    Hat(double d) :d(d) {}
    operator double() const { return d; }
    double d;
};

class Tri{
public:
    Tri () {}
    Tri(double d) :d(d) {}
    operator double() const { return d; }
    double d;
};

class Linear{
public:
    double a[2];
    Linear() {}
    Linear(double aa, double b) {a[0] = aa; a[1] = b;}
    Linear(Hat h, Tri t) {
        a[0] = double(h) - double(t)/2; 
        a[1] = double(h) + double(t)/2;
    }

    Linear(Hat h) {
        a[0] = double(h); 
        a[1] = double(h);
    }

    double operator[](const int i) const {
        assert(i >= 0);
        assert(i < 2);
        return a[i];
    }
    double& operator[](const int i) {
        assert(i >= 0);
        assert(i < 2);
        return a[i];
    }

    //Fragment implementation
    inline bool isZero() const { return a[0] == 0 && a[1] == 0; }
    bool isFinite() const;
    inline double at0() const { return a[0]; }
    inline double at1() const { return a[1]; }

    inline double pointAt(double t) const { 
        return a[0]*(1-t) + a[1]*t;
    }
    inline double operator()(double t) const { return pointAt(t); }

    inline SBasis toSBasis() const;

    inline Linear reverse() const { return Linear(a[1], a[0]); }

    operator Tri() const {
        return a[1] - a[0];
    }
    operator Hat() const {
        return (a[1] + a[0])/2;
    }
};

inline Linear operator-(Linear const &a) {
    return Linear(-a.a[0], -a.a[1]);
}
inline Linear operator+(Linear const & a, Linear const & b) {
    return Linear(a[0] + b[0], a[1] + b[1]);
}
inline Linear operator-(Linear const & a, Linear const & b) {
    return Linear(a[0] - b[0], a[1] - b[1]);
}
inline Linear& operator+=(Linear & a, Linear const & b) {
    a[0] += b[0]; a[1] += b[1];
    return a;
}
inline Linear& operator-=(Linear & a, Linear const & b) {
    a[0] -= b[0]; a[1] -= b[1];
    return a;
}
inline Linear operator+(Linear const & a, double b) {
    return Linear(a[0] + b, a[1] + b);
}
inline Linear operator-(Linear const & a, double b) {
    return Linear(a[0] - b, a[1] - b);
}
inline Linear& operator+=(Linear & a, double b) {
    a[0] += b; a[1] += b;
    return a;
}
inline Linear& operator-=(Linear & a, double b) {
    a[0] -= b; a[1] -= b;
    return a;
}
inline bool operator==(Linear const & a, Linear const & b) {
    return a[0] == b[0] && a[1] == b[1];
}
inline bool operator!=(Linear const & a, Linear const & b) {
    return a[0] != b[0] || a[1] != b[1];
}
inline Linear operator*(double const a, Linear const & b) {
    return Linear(a*b[0], a*b[1]);
}
inline Linear operator*=(Linear & a, double b) {
    a[0] *= b; a[1] *= b;
    return a;
}

/*** An empty SBasis is identically 0. */
class SBasis : public std::vector<Linear>{
public:
    SBasis() {}
    SBasis(SBasis const & a) :
        std::vector<Linear>(a)
    {}
    SBasis(Linear const & bo) {
        push_back(bo);
    }

    //Fragment implementation
    inline bool isZero() const {
        if(empty()) return true;
        for(unsigned i = 0; i < size(); i++) {
            if(!(*this)[i].isZero()) return false;
        }
        return true;
    }
    bool isFinite() const;
    inline double at0() const { 
        if(empty()) return 0; else return (*this)[0][0];
    }
    inline double at1() const{
        if(empty()) return 0; else return (*this)[0][1];
    }

    double pointAt(double t) const {
        double s = t*(1-t);
        double p0 = 0, p1 = 0;
        double sk = 1;
        int k = 0;
//TODO: rewrite as horner
        for(int k = 0; k < size(); k++) {
            p0 += sk*(*this)[k][0];
            p1 += sk*(*this)[k][1];
            sk *= s;
        }
        return (1-t)*p0 + t*p1;
    }
    double operator()(double t) const {
        return pointAt(t);
    }
    SBasis toSBasis() const { return SBasis(*this); }
    SBasis reverse() const {
        SBasis a;
        a.reserve(size());
        for(unsigned k = 0; k < size(); k++)
            a.push_back((*this)[k].reverse());
        return a;
    }

    double tailError(unsigned tail) const;

// compute f(g)
    SBasis operator()(SBasis const & g) const;


//MUTATOR PRISON
    //remove extra zeros
    void normalize() {
        while(!empty() && 0 == back()[0] && 0 == back()[1])
            pop_back();
    }
    void truncate(unsigned k) { if(k < size()) resize(k); }

    Linear& operator[](unsigned i) { return this->at(i); }
//END OF MUTATOR PRISON

    Linear operator[](unsigned i) const {
        assert(i < size());
        return std::vector<Linear>::operator[](i);
    }
};

inline SBasis Linear::toSBasis() const { return SBasis(*this); }

inline SBasis operator-(const SBasis& p) {
    if(p.isZero()) return SBasis();
    SBasis result;
    result.reserve(p.size());
        
    for(unsigned i = 0; i < p.size(); i++) {
        result.push_back(-p[i]);
    }
    return result;
}

SBasis operator+(const SBasis& a, const SBasis& b);
SBasis operator-(const SBasis& a, const SBasis& b);
SBasis& operator+=(SBasis& a, const SBasis& b);
SBasis& operator-=(SBasis& a, const SBasis& b);

inline SBasis operator+(Linear const & b, const SBasis & a) {
    if(b.isZero()) return a;
    if(a.isZero()) return b;
    SBasis result(a);
    result[0] += b;
    return result;
}

inline SBasis operator-(Linear const & b, const SBasis & a) {
    if(a.isZero()) return b;
    SBasis result = -a;
    result[0] += b;
    return result;
}

inline SBasis& operator+=(SBasis& a, const Linear& b) {
    if(a.isZero())
        a.push_back(b);
    else
        a[0] += b;
    return a;
}

inline SBasis& operator-=(SBasis& a, const Linear& b) {
    if(a.isZero())
        a.push_back(-b);
    else
        a[0] -= b;
    return a;
}

inline SBasis operator+(double b, const SBasis & a) {
    if(a.isZero()) return Linear(b, b);
    SBasis result(a);
    result[0] += b;
    return a;
}

//TODO: no double - sbasis?

inline SBasis& operator+=(SBasis& a, double b) {
    if(a.isZero())
        a.push_back(Linear(b,b));
    else
        a[0] += b;
    return a;
}

inline SBasis& operator-=(SBasis& a, double b) {
    if(a.isZero())
        a.push_back(Linear(b,b));
    else
        a[0] -= b;
    return a;
}

SBasis& operator*=(SBasis& a, double b);
SBasis& operator/=(SBasis& a, double b);

SBasis operator*(double k, SBasis const &a);
SBasis operator*(SBasis const &a, SBasis const &b);
//TODO: division equivalent?

SBasis shift(SBasis const &a, int sh);
SBasis shift(Linear const &a, int sh);

inline SBasis truncate(SBasis const &a, unsigned terms) {
    SBasis c;
    c.insert(c.begin(), a.begin(), a.begin() + std::min(terms, a.size()));
    return c;
}

SBasis multiply(SBasis const &a, SBasis const &b);

SBasis integral(SBasis const &c);
SBasis derivative(SBasis const &a);

SBasis sqrt(SBasis const &a, int k);

// return a kth order approx to 1/a)
SBasis reciprocal(Linear const &a, int k);
SBasis divide(SBasis const &a, SBasis const &b, int k);

//TODO: remove above decleration of same function
inline SBasis
operator*(SBasis const & a, SBasis const & b) {
    return multiply(a, b);
}

inline SBasis& operator*=(SBasis& a, SBasis const & b) {
    a = multiply(a, b);
    return a;
}

//valuation: degree of the first non zero coefficient.
//TODO: perhaps -1 would be better for const 0?
inline unsigned 
valuation(SBasis const &a, double tol=0){
    if(a.empty()) return 0;
    for(unsigned val = a.size() - 1; val != 0; val--)  //if no longer unsigned, val < 0 would be a better cond
        for(int c = 0; c < 2; c++)
            if(fabs(a[val][c]) < tol) return val;
    return 0;
}

// a(b(t))
SBasis compose(SBasis const &a, SBasis const &b);
SBasis compose(SBasis const &a, SBasis const &b, unsigned k);
SBasis inverse(SBasis a, int k);
//compose_inverse(f,g)=compose(f,inverse(g)), but is numerically more stable in some good cases...
SBasis compose_inverse(SBasis const &f, SBasis const &g, unsigned order, double tol=1e-7);

inline SBasis portion(const SBasis &t, double from, double to) { return compose(t, Linear(from, to)); }

// compute f(g)
inline SBasis
SBasis::operator()(SBasis const & g) const {
    return compose(*this, g);
}
 
inline std::ostream &operator<< (std::ostream &out_file, const Linear &bo) {
    out_file << "{" << bo[0] << ", " << bo[1] << "}";
    return out_file;
}

inline std::ostream &operator<< (std::ostream &out_file, const SBasis & p) {
    for(int i = 0; i < p.size(); i++) {
        out_file << p[i] << "s^" << i << " + ";
    }
    return out_file;
}

SBasis sin(Linear bo, int k);
SBasis cos(Linear bo, int k);

//void bounds(SBasis const & s, double &lo, double &hi);
void bounds(SBasis const & s, double &lo, double &hi,int order=0);
void local_bounds(SBasis const & s, double t0, double t1, double &lo, double &hi,int order=0);
//void slow_bounds(SBasis const & s, double &lo, double &hi,int order=0,double tol=0.01);

std::vector<double> roots(SBasis const & s);
std::vector<std::vector<double> > multi_roots(SBasis const &f,
                                 std::vector<double> const &levels,
                                 double tol=1e-7,
                                 double a=0,
                                 double b=1);
    
};

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=99 :
#endif
