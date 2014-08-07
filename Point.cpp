#include "Point.hpp"

Point::Point() : x() ,y()
{}

Point::Point(int i, int j, int k, int l) :  x(i,j), y(k,l)
{}

Point::Point(Fraction i, Fraction j) : x(i), y(j)
{}

Fraction Point::getX() const{
	return x;

}
Fraction Point::getY() const {
	return y;
}

void Point::setX(Fraction i){
	x = i;
}
