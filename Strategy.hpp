#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_

#include <vector>
#include "Value.hpp"
#include "Fraction.hpp"

using namespace std;

class Strategy{
private:
	Value time;
	bool included;
	unsigned int dest;
	unsigned int type;
public:
	Strategy();
	Strategy(Value, unsigned int, unsigned int, bool);
	Strategy(const Strategy&);
	unsigned int getDest() const;
	void setDest(unsigned int);
	bool getInclusion() const;
	void setInclusion(bool);
	unsigned int getType() const;
	void setType(unsigned int);
	Value getTime() const;
	void setTime(Value);
	void show();
};

#endif
