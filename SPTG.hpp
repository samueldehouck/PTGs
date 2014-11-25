/*
 * Author: Samuel Dehouck
 */


#ifndef SPTG_HPP_
#define SPTG_HPP_

#include <vector>
#include <list>
#include "PG.hpp"
#include "Value.hpp"
#include "Point.hpp"

using namespace std;

class SPTG: public PG{
protected:
	vector<Value>* states;
	vector<list<Point>* > outputFcts;

public:
	SPTG();
	~SPTG();
	void show();
	Value getState(unsigned int) const;
	void setState(unsigned int, Value);
	list<Point>* getFct(unsigned int) const;
	void setFct(unsigned int, list<Point>*);
};

#endif
