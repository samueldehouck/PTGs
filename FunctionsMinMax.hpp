#ifndef FUNCTIONSMINMAX_HPP_
#define FUNCTIONSMINMAX_HPP_

#include "Point.hpp"
#include "SPTG.hpp"
#include "Value.hpp"
#include "Strategy.hpp"
#include <list>

using namespace std;


class FunctionsMinMax {
private:
	void nextPoint(list<Point>, Point, Point, Point, Point, unsigned int, Value, bool);
	void tryWaiting(list<Point>, unsigned int, Value, bool);
	void clean(list<Point>);
public:
	FunctionsMinMax();
	virtual ~FunctionsMinMax();

	list<Point> getMinMax(SPTG*, list<Point>, unsigned int, list<Point>, unsigned int,bool);

};

#endif /* FUNCTIONSMINMAX_HPP_ */
