/*
 * Author: Samuel Dehouck
 */


#ifndef PTGVISU_HPP_
#define PTGVISU_HPP_

#include "Point.hpp"
#include "PTG.hpp"
#include <vector>
#include <list>

using namespace std;

class PTGVisu{
private:

public:
	PTGVisu();
	~PTGVisu();
	void visualizeStrats(PTG*, vector<list<Point> >* );
	void visualizeVals(PTG*, vector<list<Point> > *);
};


#endif /* PTGVISU_HPP_ */
