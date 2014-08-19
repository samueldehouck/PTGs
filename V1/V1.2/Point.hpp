#ifndef POINT_HPP
#define POINT_HPP

class Point{
private:
  double x,y;
public:
 Point();
 Point(double, double);
 double getX() const ;
 double getY() const ;
};

#endif