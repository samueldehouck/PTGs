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
}

Fraction& Fraction::operator=(const Fraction &i){
  num = i.num;
  den = i.den;
  return *this;
}

Fraction Fraction::operator+(Fraction i){
    Fraction tmp;

   tmp.num = i.num * den + i.den * num;
   tmp.den = i.den * den;
   tmp.reduce();

   return tmp;
  
}

Fraction Fraction::operator+(int i){
  return (*this + Fraction(i));
  
}
Fraction Fraction::operator-(Fraction i){
    Fraction tmp;

   tmp.num = num * i.den - den * i.num;
   tmp.den = i.den * den;
   tmp.reduce();

   return tmp;
  
}

Fraction Fraction::operator*(Fraction i){
  Fraction tmp(num*i.num, den*i.den);
  return tmp;
  
}

Fraction Fraction::operator/(Fraction i){
  Fraction tmp(num*i.den,den*i.num);
  return tmp;
}

bool Fraction::operator<(Fraction i){
  if((num/(double)den) < (i.num/(double)i.den))
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

bool Fraction::operator==(Fraction i){
  reduce();
  i.reduce();
  if((num == i.num) && (den == i.den))
    return true;
  else
    return false;
  
}

bool Fraction::operator!=(Fraction i){
  reduce();
  i.reduce();
  if((num != i.num) || (den != i.den))
    return true;
  else
    return false;
  
}

void Fraction::show(){
  if(den == 1)
   cout << num;
  else if(den == 0)
    cout << "DIVBYZERO ";
  else
    cout << num << "/" << den;
  
}

void Fraction::reduce(){
 int g = gcd(num,den);
 num /= g;
 den /= g;
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