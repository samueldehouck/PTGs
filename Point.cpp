#include "Point.hpp"


Point::Point() : x() ,y()
{}


Point::Point(Value i, Value j, Strategy st) : x(i), y(j), s(st)
{}

Value Point::getX() const{
	return x;

}
Value Point::getY() const {
	return y;
}

Strategy Point::getStrategy() const{
	return s;
}

void Point::setY(Value i){
	y = i;
}

void Point::setX(Value i){
	x = i;
}

void Point::setStrategy(Strategy st){
	s = st;
}

unsigned int Point::getType() const{
	return s.getType();
}

void Point::setType(unsigned int i){
	s.setType(i);
}

unsigned int Point::getDest() const{
	return s.getDest();
}

void Point::setDest(unsigned int i){
	s.setDest(i);
}

bool Point::getInclusion() const{
	return s.getInclusion();
}

void Point::setInclusion(bool b){
	s.setInclusion(b);
}
