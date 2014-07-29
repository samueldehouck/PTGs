#ifndef POINT_HPP
#define POINT_HPP

#include "Fraction.hpp"

class Point{
private:
  Fraction x;
  Fraction y;
public:
 Point();
 Point(int, int, int, int);
 Point(Fraction, Fraction);
 Fraction getX() const;
 Fraction getY() const ;
};

#endif