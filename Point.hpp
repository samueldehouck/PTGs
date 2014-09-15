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
	Point& operator=(const Point&);
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


#endif
