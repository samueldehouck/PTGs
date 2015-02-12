/*
 * Author: Samuel Dehouck
 */


#ifndef FUNCTIONSMINMAX_HPP_
#define FUNCTIONSMINMAX_HPP_

#include "Point.hpp"
#include "SPTG.hpp"
#include "Value.hpp"
#include "Strategy.hpp"
#include "Constants.hpp"
#include <list>

using namespace std;


class FunctionsMinMax {
private:
	void nextPoint(list<Point>*, Point, Point, Point, Point, unsigned int, Value, bool);
	void clean(list<Point>*);
public:
	FunctionsMinMax();
	virtual ~FunctionsMinMax();

	void sync(list<Point>*, list<Point>*);
	list<Point>* getMinMax(SPTG*, list<Point>*, unsigned int, list<Point>*, unsigned int,bool, Value);
	list<Point>* tryWaiting(list<Point>*, Value, bool);


};

#endif /* FUNCTIONSMINMAX_HPP_ */
