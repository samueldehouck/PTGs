#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <vector>
#include "Fraction.hpp"

using namespace std;

class Strategy{
private:
	Fraction time;
	bool included;
	unsigned int size;
	vector<unsigned int> dests;
	vector<unsigned int> lambdas;
public:
	Strategy();
	Strategy(unsigned int);
	Strategy(unsigned int, Fraction, bool);
	Strategy(const Strategy&);
	void insert(unsigned int, unsigned int, bool);
	unsigned int getDest(unsigned int) const;
	bool getInclusion() const;
	bool getLambda(unsigned int) const;
	Fraction getTime() const;
	void setTime(Fraction);
	unsigned int getSize() const;
	void show();
};

#endif
