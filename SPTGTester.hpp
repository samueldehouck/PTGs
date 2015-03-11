/*
 * Author: Samuel Dehouck
 */


#ifndef SPTGTESTER_HPP_
#define SPTGTESTER_HPP_

#include "Point.hpp"
#include <list>
#include <vector>

using namespace std;

class SPTGTester {
public:
	SPTGTester();
	void test();
	void debug(unsigned int);
	virtual ~SPTGTester();
private:
	bool compareVF(unsigned int, vector<list<Point> >*,vector<list<Point> >*);
};

#endif /* SPTGTESTER_HPP_ */
