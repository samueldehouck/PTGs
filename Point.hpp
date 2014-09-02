#ifndef POINT_HPP_
#define POINT_HPP_

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
