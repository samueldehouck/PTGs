#include "Fraction.hpp"

#include <iostream>

using namespace std;

Fraction::Fraction() : num(0), den(1)
{}

Fraction::Fraction(int i, int j)
{
	num = i;
	den = j;
	reduce();

}

Fraction::Fraction(int i){
	num = i;
	den = 1;
}

Fraction::Fraction(const Fraction &i): num(i.num), den(i.den){
	upperSign();
	reduce();
}

Fraction& Fraction::operator=(const Fraction &i){
	num = i.num;
	den = i.den;

	upperSign();
	reduce();
	return *this;
}

Fraction Fraction::operator+(Fraction i){
	Fraction tmp;

	upperSign();
	reduce();

	tmp.num = i.num * den + i.den * num;
	tmp.den = i.den * den;

	tmp.upperSign();
	tmp.reduce();

	return tmp;

}

Fraction Fraction::operator+(int i){
	Fraction tmp;
	upperSign();
	reduce();
	tmp = (*this + Fraction(i));
	tmp.reduce();
	return tmp;

}
Fraction Fraction::operator-(Fraction i){
	Fraction tmp;
	upperSign();
	reduce();

	tmp.num = num * i.den - den * i.num;
	tmp.den = i.den * den;
	tmp.upperSign();
	tmp.reduce();

	return tmp;

}

Fraction Fraction::operator-(int i){
	Fraction tmp;
	upperSign();
	reduce();
	tmp = (*this + Fraction(i));
	tmp.reduce();
	return tmp;
}

Fraction Fraction::operator*(Fraction i){

	upperSign();
	reduce();

	Fraction tmp(num*i.num, den*i.den);

	tmp.reduce();
	tmp.upperSign();
	return tmp;

}

Fraction Fraction::operator/(Fraction i){
	Fraction tmp(num*i.den,den*i.num);

	tmp.reduce();
	tmp.upperSign();
	return tmp;
}

bool Fraction::operator<(Fraction i){
	if((num/(double)den) < (i.num/(double)i.den))
		return true;
	else
		return false;

}

bool Fraction::operator <=(Fraction i){
	if((num/(double)den) <= (i.num/(double)i.den))
		return true;
	else
		return false;
}

bool Fraction::operator>(Fraction i){
	if((num/(double)den) > (i.num/(double)i.den))
		return true;
	else
		return false;

}

bool Fraction::operator >=(Fraction i){
	if((num/(double)den) >= (i.num/(double)i.den))
		return true;
	else
		return false;
}

bool Fraction::operator==(Fraction i){
	reduce();
	upperSign();

	if((num == i.num) && (den == i.den))
		return true;
	else
		return false;

}

bool Fraction::operator!=(Fraction i){
	reduce();
	upperSign();

	if((num != i.num) || (den != i.den))
		return true;
	else
		return false;

}

void Fraction::reduce(){
	int g = gcd(num,den);
	if(g != 0){
		num /= g;
		den /= g;
	}
	upperSign();
}

unsigned int Fraction::gcd(int i, int j){
	int tmp = 0;
	if(i < j)
	{
		tmp = i;
		i = j;
		j = tmp;
	}
	while(j != 0)
	{
		tmp = i % j;
		i = j;
		j = tmp;
	}
	return i;
}

void Fraction::upperSign(){
	if(den < 0){
		num = 0 - num;
		den = 0 - den;
	}
}

ostream& operator<<(ostream& out, const Fraction& f){
	if(f.den == 1)
		out << f.num;
	else if(f.den == 0)
		out << "DIVBYZERO ";
	else
		out << f.num << "/" << f.den;
	return out;
}
