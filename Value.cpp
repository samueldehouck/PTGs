/*
 * Author: Samuel Dehouck
 */


#include "Value.hpp"

Value::Value(){
	val = 0;
	inf = false;
}

Value::Value(const Value &v){
	val = v.getVal();
	inf = v.isInfinity();
}

Value::Value(Fraction v){
	val = v;
	inf = false;
}

Value::Value(int i){
	val = i;
	inf = false;
}

Value& Value::operator=(const Value &v){
	val = v.getVal();
	inf = v.isInfinity();
	return *this;
}

Value Value::operator+(Value v){
	Value tmp;
	if(inf || v.isInfinity())
		tmp.setInf(true);
	else
		tmp.setVal(val + v.getVal());
	return tmp;
}

Value Value::operator-(Value v){
	Value tmp;
	if(inf || v.isInfinity())
		tmp.setInf(true);
	else
		tmp.setVal(val - v.getVal());
	return tmp;
}

Value Value::operator/(Value v){
	Value tmp;
	if(inf || v.isInfinity())
		tmp.setInf(true);
	else
		tmp.setVal(val / v.getVal());
	return tmp;
}

Value Value::operator*(Value v){
	Value tmp;
	if(inf || v.isInfinity())
		tmp.setInf(true);
	else
		tmp.setVal(val * v.getVal());
	return tmp;
}

bool Value::operator==(Value v){
	if((inf && v.isInfinity()) || (!inf && !v.isInfinity() && val == v.getVal()))
		return true;

	return false;
}
bool Value::operator<(Value v){
	if((!inf && !v.isInfinity() && val < v.getVal()) ||
			(!inf && v.isInfinity()))
		return true;

	return false;
}

bool Value::operator<=(Value v){
	if((inf && v.isInfinity()) ||
			(!inf && !v.isInfinity() && val <= v.getVal()) ||
			(!inf && v.isInfinity()))
		return true;

	return false;
}

bool Value::operator>(Value v){
	if((!inf && !v.isInfinity() && val > v.getVal()) ||
			(inf && !v.isInfinity()))
		return true;

	return false;
}
bool Value::operator>=(Value v){
	if((inf && v.isInfinity()) ||
			(!inf && !v.isInfinity() && val >= v.getVal()) ||
			(inf && !v.isInfinity()))
		return true;

	return false;
}
bool Value::operator!=(Value v){
	if((inf && !v.isInfinity()) ||
			(!inf && v.isInfinity()) ||
			(!inf && !v.isInfinity() && val != v.getVal()))
		return true;
	return false;

}

Fraction Value::getVal() const{
	if (!inf)
		return val;

	return 0;
}

void Value::setVal(Fraction v){
	val = v;
	inf = false;
}

bool Value::isInfinity() const{
	return inf;
}

void Value::setInf(bool i){
	inf = i;
}

ostream& operator<<(ostream& out, const Value& v){
	if(v.isInfinity())
		out << "inf";
	else
		out << v.getVal();
	return out;
}
