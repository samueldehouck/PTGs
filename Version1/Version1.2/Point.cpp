#include "Point.hpp"

Point::Point() : x(0),y(0)
{}

Point::Point(double i, double j) :  x(i), y(j)
{}

double Point::getX() const {
  return x;
}

double Point::getY() const {
  return y;
}