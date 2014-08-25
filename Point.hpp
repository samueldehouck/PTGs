#ifndef POINT_HPP
#define POINT_HPP

#include "Value.hpp"

class Point{
private:
	Value x;
	Value y;
public:
	Point();
	Point(Value, Value);
	Value getX() const;
	Value getY() const ;
	void setX(Value);
};

#endif
