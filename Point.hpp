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

/*ostream& operator<<(ostream& out, const Point& p){
	out << "(";
	if(p.getX().isInfinity())
		out << "inf";
	else
		out << p.getX().getVal();
	out << ",";
	if(p.getY().isInfinity())
		out << "inf";
	else
		out << p.getY().getVal();
	out << ")";
	return out;
}*/

#endif
