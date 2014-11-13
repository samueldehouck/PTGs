/*
 * Author: Samuel Dehouck
 */

#ifndef COMPOSITEVALUE_HPP_
#define COMPOSITEVALUE_HPP_

#include "Value.hpp"
#include "Fraction.hpp"

class CompositeValue: public Value {
private:
	Fraction epsilon;

public:
	CompositeValue();
	CompositeValue(const CompositeValue &);
	CompositeValue(Fraction, Fraction);
	CompositeValue(int);
	CompositeValue(Value);
	CompositeValue& operator=(const CompositeValue &);
	CompositeValue operator+(CompositeValue);
	CompositeValue operator-(CompositeValue);
	CompositeValue operator/(int);
	CompositeValue operator*(int);
	bool operator==(CompositeValue);
	bool operator<(CompositeValue);
	bool operator<=(CompositeValue);
	bool operator>(CompositeValue);
	bool operator>=(CompositeValue);
	bool operator!=(CompositeValue);
	Fraction getEps() const;

	void setEps(Fraction);
};

ostream& operator<<(ostream&, const CompositeValue&);
#endif
