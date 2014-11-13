/*
 * Author: Samuel Dehouck
 */


#ifndef FRACTION_HPP_
#define FRACTION_HPP_

#include <iostream>

using namespace std;

class Fraction{
public:
	int num, den;

	Fraction();
	Fraction(int, int);
	Fraction(int);
	Fraction(const Fraction&);

	Fraction& operator=(const Fraction&);
	Fraction operator+(Fraction);
	Fraction operator+(int);
	Fraction operator-(Fraction);
	Fraction operator-(int);
	Fraction operator*(Fraction);
	Fraction operator/(Fraction);
	bool operator<(Fraction);
	bool operator<=(Fraction);
	bool operator>(Fraction);
	bool operator>=(Fraction);
	bool operator==(Fraction);
	bool operator!=(Fraction);
	void reduce();
	unsigned int gcd(int, int);
	void upperSign();
	double asDouble();
};

ostream& operator<<(ostream&, const Fraction&);

#endif
