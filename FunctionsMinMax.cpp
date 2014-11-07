#include "FunctionsMinMax.hpp"

using namespace std;
FunctionsMinMax::FunctionsMinMax() {
	// TODO Auto-generated constructor stub

}

FunctionsMinMax::~FunctionsMinMax() {
	// TODO Auto-generated destructor stub
}

void FunctionsMinMax::nextPoint(list<Point>* result, Point fStart, Point fEnd, Point gStart, Point gEnd, unsigned int state, Value cost, bool max){

	cout << "NextPoint" << endl;
	if(fStart.getY() > gStart.getY() + cost && fEnd.getY() > gEnd.getY() + cost){
		if(fEnd.getX() < gEnd.getX()){
			if(max){
				result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
			}
			else{
				//Nothing to do
			}
		}
		else{
			if(max){
				//Nothing to do
			}
			else{
				result->push_back(Point(gEnd.getX(), gEnd.getY() + cost, Strategy(state, 0, true)));
			}
		}
	}
	else if(fStart.getY() < gStart.getY() + cost && fEnd.getY() < gEnd.getY() + cost){
		if(fEnd.getX() < gEnd.getX()){
			if(max){
				//Nothing to do
			}
			else{
				result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
			}
		}
		else{
			if(max){
				result->push_back(Point(gEnd.getX(), gEnd.getY() + cost, Strategy(state, 0, true)));
			}
			else{
				//Nothing to do
			}
		}
	}
	else if(fStart.getY() == gStart.getY() + cost && fEnd.getY() == gEnd.getY() + cost){
		if(fEnd.getX() < gEnd.getX()){
			result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
		}
		else{
			result->push_back(Point(gEnd.getX(), gEnd.getY() + cost, Strategy(state, 0, true)));
		}
	}
	else{
		//There is an intersection
		cout << "intersection" << endl;
		Value coefF = (fEnd.getY() - fStart.getY())/(fEnd.getX() - fStart.getX());
		Value coefG = (gEnd.getY() - gStart.getY())/(gEnd.getX() - gStart.getX());
		Value zeroF = fStart.getY() - fStart.getX() * coefF;
		Value zeroG = gStart.getY() - gStart.getX() * coefG + cost;


		Value inter = (zeroF - zeroG)/(coefG - coefF);

		if(inter >= fStart.getX() && inter <= fEnd.getX() && inter >= gStart.getX() && inter <= gEnd.getX()){
			if(coefF < coefG){
				if(max){
					result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));

					if(fEnd.getX() < gEnd.getX()){
						//Nothing to do
					}
					else{
						result->push_back(Point(gEnd.getX(), gEnd.getY() + cost, Strategy(state, 0, true)));
					}
				}
				else{
					result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
					if(fEnd.getX() < gEnd.getX()){
						result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));

					}
					else{
						//Nothing to do

					}
				}
			}
			else{
				cout << "ici" << endl;
				if(max){
					result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
					if(fEnd.getX() < gEnd.getX()){
						result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
					}
					else{
						//Nothing to do
					}
				}
				else{
					result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));
					if(fEnd.getX() < gEnd.getX()){
						//Nothing to do
					}
					else{
						result->push_back(Point(gEnd.getX(), gEnd.getY() + cost, Strategy(state, 0, true)));
					}
				}
			}
		}
	}
}

void FunctionsMinMax::clean(list<Point>* f){

	list<Point>::iterator itNext = f->begin();
	++itNext;
	if(itNext != f->end()){
		list<Point>::iterator itCurrent = f->begin();
		++itNext;
		++itCurrent;
		for (list<Point>::iterator itLast = f->begin(); itNext != f->end(); ++itNext){
			bool deleted = false;
			if(!itLast->getY().isInfinity() && !itCurrent->getY().isInfinity() && !itNext->getY().isInfinity() && (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				Value coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());
				if(itLast->getY() + (coef * (itCurrent->getX() - itLast->getX())) == itCurrent->getY()){
					f->erase(itCurrent);
					deleted = true;
				}
			}
			else if(itLast->getY().isInfinity() && itCurrent->getY().isInfinity() && itNext->getY().isInfinity()&& (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				f->erase(itCurrent);
				deleted = true;
			}
			if(deleted){
				//itCurrent is on a space that has been deleted so we can't do ++itCurrent and itLast needs to stay where it was
				itCurrent = itNext;
			}
			else{
				++itCurrent;
				++itLast;
			}
		}

	}
}

list<Point>* FunctionsMinMax::tryWaiting(list<Point>* f, unsigned int state, Value coefWait, bool max){

	cout << "Waiting" << endl;
	list<Point>* result = new list<Point> ();

	list<Point>::reverse_iterator fEnd = f->rbegin();
	list<Point>::reverse_iterator fBegin = fEnd;
	++fBegin;

	unsigned int destStrat = 0;
	coefWait = Value(0) - coefWait; //coefWait was positive
	Value zeroWait = 0;
	bool waiting = false;
	while (fBegin != f->rend()){
		Value coefF = (fEnd->getY() - fBegin->getY())/(fEnd->getX() - fBegin->getX());
		Value zeroF = fEnd->getY() - fEnd->getX()*coefF;
		if(!waiting){
			zeroWait = fEnd->getY() - fEnd->getX()*coefWait;

			if(fEnd->getX() != 1){
				result->push_front(Point(fEnd->getX(), fEnd->getY(), Strategy(fEnd->getDest(), 0, true)));
			}
			else{
				result->push_front(Point(fEnd->getX(), fEnd->getY(), Strategy(fEnd->getDest(), 0, true)));
			}

			if(max){
				if(zeroWait > zeroF){//Is above
					destStrat = fEnd->getDest();
					waiting = true;
				}
			}
			else{

				if(zeroWait < zeroF){//Is below
					destStrat = fEnd->getDest();
					waiting = true;
				}
			}
		}
		else{
			Value inter = (zeroF - zeroWait)/(coefWait - coefF);
			if(inter >= fBegin->getX() && inter <= fEnd->getX()){
				result->push_front(Point(inter, zeroF + inter*coefF, Strategy(destStrat,1,true)));
				waiting = false;
			}
		}

		fEnd = fBegin;
		++fBegin;
	}
	if(waiting){
		result->push_front(Point(0, zeroWait, Strategy(destStrat,1,true)));
	}
	else{
		result->push_front(Point(0, fEnd->getY(), Strategy(fEnd->getDest(),0,false)));

	}

	return result;
}

list<Point>* FunctionsMinMax::getMinMax(SPTG* sptg, list<Point>* f, unsigned int crtState, list<Point>* g, unsigned int state, bool max){

	cout << "MinMax state " << crtState << " and " << state << endl;
	cout << "f: " << endl;
	list<Point>::iterator it = f->begin();
	while (it != f->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;
	cout << "g: " << endl;
	it = g->begin();
	while (it != g->end()){
		cout << "(" << it->getX() << "," << it->getY() + sptg->getTransition(crtState,state) << ") ";
		++it;
	}
	cout << endl;


	//We consider that f is the function of the current state (the one that will be replaced)
	list<Point>* result = new list<Point>();

	list<Point>::iterator itF = f->begin();
	list<Point>::iterator itG = g->begin();
	list<Point>::iterator itFNext = itF;
	++itFNext;
	list<Point>::iterator itGNext = itG;
	++itGNext;

	Value cost = sptg->getTransition(crtState, state);

	if(max){
		if(itF->getY() < itG->getY() + cost){

			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
		}
		else if (itF->getY() > itG->getY() + cost){
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

		}
		else{
			Value coefG = (itGNext->getY() - itG->getY())/(itGNext->getX() - itG->getX());
			Value coefF = (itFNext->getY() - itF->getY())/(itFNext->getX() - itF->getX());
			if(coefG > coefF){
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));

			}
			else{
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

			}
		}
	}
	else{
		if(itF->getY() > itG->getY() + cost){
			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));

		}
		else if(itF->getY() < itG->getY() + cost){
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

		}
		else{
			Value coefG = (itGNext->getY() - itG->getY())/(itGNext->getX() - itG->getX());
			Value coefF = (itFNext->getY() - itF->getY())/(itFNext->getX() - itF->getX());
			if(coefG < coefF){
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));

			}
			else{
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

			}
		}
	}


	while( itFNext != f->end() && itGNext != g->end()){
		nextPoint(result, *itF, *itFNext, *itG, *itGNext, state, sptg->getTransition(crtState, state), max);
		if(itFNext->getX() < itGNext->getX()){
			itF = itFNext;
			++itFNext;
		}
		else if(itFNext->getX() > itGNext->getX()){
			itG = itGNext;
			++itGNext;
		}
		else{
			itF = itFNext;
			++itFNext;
			itG = itGNext;
			++itGNext;
		}

	}

	if(max){
		if(itF->getY() <= itG->getY() + cost)
			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
		else
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

	}
	else{
		if(itF->getY() > itG->getY() + cost)
			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
		else
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
	}

	clean (result);
	cout << "Result: " << endl;
	it = result->begin();
	while (it != result->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;

	result = tryWaiting(result, crtState, sptg->getState(crtState), max);
	clean(result);

	cout << "Result after wait: " << endl;
	it = result->begin();
	while (it != result->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;


	return result;
}
