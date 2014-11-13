/*
 * Author: Samuel Dehouck
 */


#include "Point.hpp"

Point::Point() : x() ,y()
{}


Point::Point(Value i, Value j, Strategy st) : x(i), y(j), s(st)
{}

Point& Point::operator=(const Point &p){
	if(&p != this){
		x = p.getX();
		y = p.getY();
		s = p.getStrategy();
	}
	return *this;
}

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

void Point::setStrategy(Strategy st){
	s = st;
}

Strategy Point::getStrategy() const{
	return s;
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

void Point::setInf(bool b){
	y.setInf(b);
}

bool Point::getInf() const{
	return y.isInfinity();
}
