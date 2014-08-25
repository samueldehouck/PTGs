#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <vector>
#include "Value.hpp"
#include "Fraction.hpp"

using namespace std;

class Strategy{
private:
	Value time;
	bool included;
	unsigned int size;
	vector<unsigned int> dests;
	vector<unsigned int> types;
public:
	Strategy();
	Strategy(unsigned int);
	Strategy(unsigned int, Value, bool);
	Strategy(const Strategy&);
	void insert(unsigned int, unsigned int, unsigned int);
	unsigned int getDest(unsigned int) const;
	bool getInclusion() const;
	unsigned int getType(unsigned int) const;
	Value getTime() const;
	void setTime(Value);
	unsigned int getSize() const;
	void show();
};

#endif
