#ifndef POINT_HPP_
#define POINT_HPP_

#include "Value.hpp"
#include "Strategy.hpp"

class Point{
private:
	Value x;
	Value y;
	Strategy s;
public:
	Point();
	Point(Value, Value, Strategy);
	Value getX() const;
	void setX(Value);
	Value getY() const;
	void setY(Value);
	Strategy getStrategy() const;
	void setStrategy(Strategy);
	unsigned int getType() const;
	void setType(unsigned int);
	unsigned int getDest() const;
	void setDest(unsigned int);
	bool getInclusion() const;
	void setInclusion(bool);
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
