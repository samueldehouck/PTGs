#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <vector>
#include "Fraction.hpp"

class Strategy{
private:
	Fraction time;
	unsigned int size;
	vector<unsigned int> dests;
	vector<bool> lambdas;
public:
	Strategy();
	Strategy(unsigned int);
	Strategy(unsigned int, Fraction);
	Strategy(const Strategy&);
	void insert(unsigned int, unsigned int, bool);
	unsigned int getDest(unsigned int) const;
	bool getLambda(unsigned int) const;
	Fraction getTime() const;
	void setTime(Fraction);
	unsigned int getSize() const;
	void show();
};

#endif
