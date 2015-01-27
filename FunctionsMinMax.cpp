/*
 * Author: Samuel Dehouck
 */


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
	cout << "(" << fStart.getX() << "," << fStart.getY() << ") - (" << fEnd.getX() << "," << fEnd.getY() << ")" << endl;
	cout << "(" << gStart.getX() << "," << gStart.getY()+cost << ") - (" << gEnd.getX() << "," << gEnd.getY()+cost << ")" << endl;

	if(fStart.getY() > gStart.getY() + cost && fEnd.getY() > gEnd.getY() + cost){
		if(max){
			result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
		}
		else{
			result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
		}

	}
	else if(fStart.getY() < gStart.getY() + cost && fEnd.getY() < gEnd.getY() + cost){
		if(max){
			result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
		}
		else{
			result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
		}

	}
	else if(fStart.getY() == gStart.getY() + cost && fEnd.getY() == gEnd.getY() + cost){
		result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
	}
	else{
		//There is an intersection
		cout << "intersection" << endl;
		Value coefF = (fEnd.getY() - fStart.getY())/(fEnd.getX() - fStart.getX());
		Value coefG = (gEnd.getY() - gStart.getY())/(gEnd.getX() - gStart.getX());
		Value zeroF = fStart.getY() - fStart.getX() * coefF;
		Value zeroG = gStart.getY() - gStart.getX() * coefG + cost;


		Value inter = (zeroF - zeroG)/(coefG - coefF);

		if(fStart.getY() > gStart.getY() + cost){
			if(max){
				result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
				if(inter != 1)
					result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
			}
			else{
				result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
				if(inter != 1)
					result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));
			}
		}
		else if (fStart.getY() < gStart.getY() + cost){
			if(max){
				result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
				if(inter != 1)
					result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));
			}
			else{
				result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
				if(inter != 1)
					result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
			}
		}
		else if(fStart.getY() == gStart.getY() + cost){
			if(coefF > coefG){
				if(max){
					result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
				}
				else{
					result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
				}
			}
			else if(coefF < coefG){
				if(max){
					result->push_back(Point(gStart.getX(), gStart.getY() + cost, Strategy(state, 0, true)));
				}
				else{
					result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
				}
			}
			else{
				result->push_back(Point(fStart.getX(), fStart.getY(), fStart.getStrategy()));
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

list<Point>* FunctionsMinMax::tryWaiting(list<Point>* f, Value coefWait, bool max){
	//Checks if waiting in the state gives a better value fct

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

			cout << fEnd->getStrategy().getType() << endl;
			result->push_front(Point(fEnd->getX(), fEnd->getY(), fEnd->getStrategy()));


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
				if(inter != 0 && inter != 1)
					result->push_front(Point(inter, zeroF + inter*coefF, Strategy(destStrat,1,true)));
				else
					result->push_front(Point(inter, zeroF + inter*coefF, Strategy(destStrat,1,false)));

				waiting = false;
			}
		}

		fEnd = fBegin;
		++fBegin;
	}
	if(waiting){
		result->push_front(Point(0, zeroWait, Strategy(destStrat,1,false)));
	}
	else{
		result->push_front(Point(0, fEnd->getY(), Strategy(fEnd->getDest(),0,false)));

	}

	return result;
}

void FunctionsMinMax::sync(list<Point>* f, list<Point>* g){
	//Makes that both list have points at the same x everytimes


	list<Point>::iterator itF = f->begin();
	list<Point>::iterator itFNext = itF;
	++itFNext;
	list<Point>::iterator itG = g->begin();
	list<Point>::iterator itGNext = itG;
	++itGNext;
	if(!itF->getInf() && !itG->getInf()){
		while (itGNext != g->end() && itFNext != f->end()){
			Value coefF = (itFNext->getY() - itF->getY())/(itFNext->getX() - itF->getX());
			Value coefG = (itGNext->getY() - itG->getY())/(itGNext->getX() - itG->getX());
			Value zeroF = itF->getY() - itF->getX() * coefF;
			Value zeroG = itG->getY() - itG->getX() * coefG;
			cout << itFNext->getX() <<  " " << itGNext->getX() << endl;
			if(itGNext->getX() < itFNext->getX()){
				cout << "ici" << endl;
				f->insert(itFNext,Point(itGNext->getX(),zeroF + coefF* itGNext->getX(), itF->getStrategy()));
				itG = itGNext;
				++itGNext;
			}
			else if(itGNext->getX() > itFNext->getX()){
				cout << "la" << endl;
				g->insert(itGNext,Point(itFNext->getX(),zeroG + coefG* itFNext->getX(), itG->getStrategy()));
				itF = itFNext;
				++itFNext;
			}
			else{
				itF = itFNext;
				++itFNext;
				itG = itGNext;
				++itGNext;
			}
		}
	}
}

list<Point>* FunctionsMinMax::getMinMax(SPTG* sptg, list<Point>* f, unsigned int crtState, list<Point>* g, unsigned int state, bool max, Value cost){

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
		cout << "(" << it->getX() << "," << it->getY() + cost << ") ";
		++it;
	}
	cout << endl;

	list<Point>* result = new list<Point>();
	sync(f,g);


	cout << "after sync" << endl;
	cout << "f: " << endl;
	it = f->begin();
	while (it != f->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;
	cout << "g: " << endl;
	it = g->begin();
	while (it != g->end()){
		cout << "(" << it->getX() << "," << it->getY() + cost << ") ";
		++it;
	}
	cout << endl;
	//We consider that f is the function of the current state (the one that will be replaced)

	list<Point>::iterator itF = f->begin();
	list<Point>::iterator itG = g->begin();
	list<Point>::iterator itFNext = itF;
	++itFNext;
	list<Point>::iterator itGNext = itG;
	++itGNext;


	if(itF->getInf() || itG->getInf()){
		if(max){
			if(itG->getInf()){
				while(itG != g->end()){
					result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
					++itG;
				}
			}
			else{
				while(itF != f->end()){
					result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
					++itF;
				}
			}
		}
		else{
			if(itF->getInf()){
				while(itG != g->end()){
					result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
					++itG;
				}
			}
			else{
				while(itF != f->end()){
					result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
					++itF;
				}
			}
		}
	}
	else{

		//Build the result fct
		while( itFNext != f->end() && itGNext != g->end()){
			nextPoint(result, *itF, *itFNext, *itG, *itGNext, state, cost, max);
			itF = itFNext;
			++itFNext;
			itG = itGNext;
			++itGNext;

			cout << "Result: " << endl;
			it = result->begin();
			while (it != result->end()){
				cout << "(" << it->getX() << "," << it->getY() << ") ";
				++it;
			}
			cout << endl;
		}

		//Add the last point (in time 1)
		if(itF->getY() > itG->getY() + cost){
			if(max){
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
			}
			else{
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
			}
		}
		else if(itF->getY() < itG->getY() + cost){
			if(max){
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
			}
			else{
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
			}
		}
		else{
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

		}
		cout << "Result: " << endl;
		it = result->begin();
		while (it != result->end()){
			cout << "(" << it->getX() << "," << it->getY() << ") ";
			++it;
		}
		cout << endl;
		cout << "clean" << endl;
		clean (result);
	}
	cout << "Result: " << endl;
	it = result->begin();
	while (it != result->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;



	result = tryWaiting(result, sptg->getState(crtState), max);

	clean(f);
	clean(g);
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
