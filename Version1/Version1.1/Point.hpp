#ifndef POINT_HPP
#define POINT_HPP

class Point{
private:
  double x,y;
public:
 Point();
 Point(double, double);
 const double getX();
 const double getY();
};

#endif