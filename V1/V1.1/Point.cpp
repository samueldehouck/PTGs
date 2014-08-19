#include "Point.hpp"

Point::Point() : x(0),y(0)
{}

Point::Point(double i, double j) :  x(i), y(j)
{}

const double Point::getX()
{
  return x;
}

const double Point::getY()
{
  return y;
}