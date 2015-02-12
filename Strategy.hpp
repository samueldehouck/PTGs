/*
 * Author: Samuel Dehouck
 */


#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_

#include <vector>
#include "Fraction.hpp"
#include "Constants.hpp"

using namespace std;

class Strategy{
private:
	bool included;
	unsigned int dest;
	unsigned int type;
public:
	Strategy();
	Strategy(unsigned int, unsigned int, bool);
	Strategy(const Strategy&);
	Strategy& operator=(const Strategy&);
	unsigned int getDest() const;
	void setDest(unsigned int);
	bool getInclusion() const;
	void setInclusion(bool);
	unsigned int getType() const;
	void setType(unsigned int);

	void show();
};

#endif
