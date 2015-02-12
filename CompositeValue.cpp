/*
 * Author: Samuel Dehouck
 */

#include "CompositeValue.hpp"

CompositeValue::CompositeValue(){
	Value();
	epsilon = 0;
}

CompositeValue::CompositeValue(const CompositeValue &cv): Value(cv){
	epsilon = cv.getEps();
}

CompositeValue::CompositeValue(Fraction v, Fraction e): Value(v){
	epsilon = e;
}

CompositeValue::CompositeValue(int i){
	val = i;
	inf = false;
	epsilon = 0;
}

CompositeValue::CompositeValue(Value v){
	val = v.getValue();
	inf = v.isInfinity();
	epsilon = 0;
}

CompositeValue& CompositeValue::operator=(const CompositeValue &cv){
	val = cv.getValue();
	inf = cv.isInfinity();
	epsilon = cv.getEps();
	return *this;
}

CompositeValue CompositeValue::operator+(CompositeValue cv){
	//cout << "sum: " << *this << " + " << cv;
	CompositeValue tmp;
	if(inf || cv.isInfinity())
		tmp.setInf(true);
	else{
		tmp.setValue(val + cv.getValue());
		tmp.setEps(epsilon + cv.getEps());
	}
	//cout << " = " << tmp << endl;
	return tmp;
}

CompositeValue CompositeValue::operator-(CompositeValue cv){
	CompositeValue tmp = (Value)(*this) - (Value)cv;
	if(!inf && !cv.isInfinity())
		tmp.setEps(epsilon - cv.getEps());
	return tmp;
}

CompositeValue CompositeValue::operator*(int i){
	CompositeValue tmp;
	if(inf)
		tmp.setInf(true);
	else
		tmp.setValue(val * i);
	tmp.setEps(epsilon*i);
	return tmp;
}

CompositeValue CompositeValue::operator/(int i){
	CompositeValue tmp;
	if(inf)
		tmp.setInf(true);
	else
		tmp.setValue(val / i);
	tmp.setEps(epsilon / i);

	return tmp;
}

bool CompositeValue::operator==(CompositeValue cv){
	if((inf && cv.isInfinity())
			|| (!inf && !cv.isInfinity() && val == cv.getValue() && epsilon == cv.getEps()))
		return true;

	return false;
}

bool CompositeValue::operator<(CompositeValue cv){

	if((!inf && !cv.isInfinity() && val < cv.getValue()) ||
			(!inf && !cv.isInfinity() && val == cv.getValue() && epsilon < cv.getEps()) ||
			(!inf && cv.isInfinity()))
		return true;
	return false;
}

bool CompositeValue::operator<=(CompositeValue cv){
	if((inf && cv.isInfinity()) ||
			(!inf && !cv.isInfinity() && val < cv.getValue()) ||
			(!inf && !cv.isInfinity() && val == cv.getValue() && epsilon <= cv.getEps()) ||
			(!inf && cv.isInfinity()))
		return true;

	return false;
}

bool CompositeValue::operator>(CompositeValue cv){
	if((!inf && !cv.isInfinity() && val > cv.getValue()) ||
			(!inf && !cv.isInfinity() && val == cv.getValue() && epsilon > cv.getEps()) ||
			(inf && !cv.isInfinity()))
		return true;

	return false;
}

bool CompositeValue::operator>=(CompositeValue cv){
	if((inf && cv.isInfinity()) ||
			(!inf && !cv.isInfinity() && val > cv.getValue()) ||
			(!inf && !cv.isInfinity() && val == cv.getValue() && epsilon >= cv.getEps()) ||
			(inf && !cv.isInfinity()))
		return true;

	return false;
}

bool CompositeValue::operator!=(CompositeValue cv){
	if((inf && !cv.isInfinity()) ||
			(!inf && cv.isInfinity()) ||
			(!inf && !cv.isInfinity() && val != cv.getValue())||
			(!inf && !cv.isInfinity() && val == cv.getValue() && epsilon != cv.getEps()))
		return true;
	return false;
}



Fraction CompositeValue::getEps() const{
	return epsilon;
}

void CompositeValue::setEps(Fraction e){
	epsilon = e;
	inf = false;
}

ostream& operator<<(ostream& out, const CompositeValue& cv){
	if(cv.isInfinity())
		out << "inf";
	else if(cv.getEps() == 0)
		out << cv.getValue();
	else
		out << cv.getValue() << "+" << cv.getEps() << "e";

	return out;
}
