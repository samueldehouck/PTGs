/*
 * Author: Samuel Dehouck
 */


#ifndef VALUE_HPP_
#define VALUE_HPP_

#include "Fraction.hpp"

class Value{
protected:
	Fraction val;
	bool inf;
public:
	Value();
	Value(const Value &);
	Value(Fraction);
	Value(int);
	Value& operator=(const Value &);
	Value operator+(Value);
	Value operator-(Value);
	Value operator/(Value);
	Value operator*(Value);
	bool operator==(Value);
	bool operator<(Value);
	bool operator<=(Value);
	bool operator>(Value);
	bool operator>=(Value);
	bool operator!=(Value);

	Fraction getValue() const;
	void setValue(Fraction);
	bool isInfinity() const;
	void setInf(bool);
};

ostream& operator<<(ostream&, const Value&);

#endif
