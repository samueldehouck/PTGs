#include "Point.hpp"


Point::Point() : x() ,y()
{}


Point::Point(Value i, Value j) : x(i), y(j)
{}

Value Point::getX() const{
	return x;

}
Value Point::getY() const {
	return y;
}

void Point::setY(Value i){
	y = i;
}

void Point::setX(Value i){
	x = i;
}


