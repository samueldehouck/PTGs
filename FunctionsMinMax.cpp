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
				result->push_back(Point(gEnd.getX(), gEnd.getY(), Strategy(state, 0, true)));
			}
		}
	}
	else if(fStart.getY() <= gStart.getY() + cost && fEnd.getY() <= gEnd.getY() + cost){
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
				result->push_back(Point(gEnd.getX(), gEnd.getY(), Strategy(state, 0, true)));
			}
			else{
				//Nothing to do
			}
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
		if(fEnd.getY() > gEnd.getY() + cost){
			if(max){

				result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));

				if(fEnd.getX() < gEnd.getX()){
					result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
				}
				else{
					//Nothing to do
				}
			}
			else{
				result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
				if(fEnd.getX() < gEnd.getX()){
					//Nothing to do
				}
				else{
					result->push_back(Point(gEnd.getX(), gEnd.getY(), Strategy(state, 0, true)));
				}
			}
		}
		else{
			if(max){
				result->push_back(Point(inter, zeroF + coefF * inter, Strategy(state, 0, true)));
				if(fEnd.getX() < gEnd.getX()){
					//Nothing to do
				}
				else{
					result->push_back(Point(gEnd.getX(), gEnd.getY(), Strategy(state, 0, true)));
				}
			}
			else{
				result->push_back(Point(inter, zeroF + coefF * inter, fStart.getStrategy()));
				if(fEnd.getX() < gEnd.getX()){
					result->push_back(Point(fEnd.getX(), fEnd.getY(), fEnd.getStrategy()));
				}
				else{
					//Nothing to do
				}
			}

		}
	}
}

void FunctionsMinMax::clean(list<Point> f){

	list<Point>::iterator itNext = f.begin();
	++itNext;
	if(itNext != f.end()){
		list<Point>::iterator itCurrent = f.begin();
		++itNext;
		++itCurrent;
		for (list<Point>::iterator itLast = f.begin(); itNext != f.end(); ++itNext){
			bool deleted = false;
			if(!itLast->getY().isInfinity() && !itCurrent->getY().isInfinity() && !itNext->getY().isInfinity() && (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				Value coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());
				if(itLast->getY() + (coef * (itCurrent->getX() - itLast->getX())) == itCurrent->getY()){
					f.erase(itCurrent);
					deleted = true;
				}
			}
			else if(itLast->getY().isInfinity() && itCurrent->getY().isInfinity() && itNext->getY().isInfinity()&& (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				f.erase(itCurrent);
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

void FunctionsMinMax::tryWaiting(list<Point> f, unsigned int state, Value val, bool max){
	list<Point>::iterator it = f.begin();


	while(it->getX() != 1 && it != f.end())
		++it;


	list<Point>::iterator itComp = it;
	list<Point>::iterator itLastComp = itComp;
	--itLastComp;


	while(itComp->getX() != 0 && itComp != f.begin() && it != f.begin()){
		cout << it->getX() << " compared to " << itLastComp->getX() << " "  << itComp->getX() << endl;
		Value coef;
		if(itComp->getY() != itLastComp->getY())
			coef = (itComp->getY() - itLastComp->getY())/(itComp->getX() - itLastComp->getX());
		else
			coef = 0;
		cout << "val: " << val << " " << "coef: " << coef << endl;
		if((!max && val > coef) || (max && val < coef)){

			//Waiting is improving the value
			list<Point>::iterator currentStrat = it;
			itComp = itLastComp;
			--itLastComp;
			bool intersect = false;
			Value zero = it->getY() - it->getX()*val;
			cout << "zero: " << zero << endl;
			cout << "itComp: " << itComp->getX() << " it:" << it->getX() << endl;
			while(!intersect && itComp->getX() != 0 && itComp != f.begin() && it != f.begin()){
				//cout << "looking for inter" << endl;
				//Waiting is optimal until we find an intersection
				coef = (itComp->getY() - itLastComp->getY())/(itComp->getX() - itLastComp->getX());
				Value zeroComp = itComp->getY() - itComp->getX()*coef;
				Value inter = (zero - zeroComp)/(coef- val);
				cout << "inter " << inter << endl;
				if(inter >= itLastComp->getX() && inter <= itComp->getX()){
					//We need to create a new breaking point
					if(itComp != it){
						itComp->setDest(it->getDest());
						itComp->setX(inter);
						itComp->setY(zero + inter*val);
						itComp->setType(1);
						itComp->setInclusion(true);
					}
					//And erase everything that's between itComp and it
					++itComp;
					while(itComp != it && itComp->getX() != 1 && itComp != f.end())
						itComp = f.erase(itComp);
					intersect = true;

				}
				else if(inter == itLastComp->getX()){
					//The intersection is the next point, we can delete the current (itComp) and update the strategy
					intersect = true;

					//We update the values
					itLastComp->setDest(it->getDest());
					itComp->setType(1);
					itComp->setInclusion(true);

					//We delete everything that is between itLastComp and it
					while(itComp != it && itComp->getX() != 1 && itComp != f.end())
						itComp = f.erase(itComp);
				}
				else{
					//There is no acceptable intersection
					itComp = itLastComp;
					--itLastComp;
				}

			}
			if(itComp->getX() == 0 && it->getX() != 0){
				//cout << "Correction" << endl;
				it = f.begin();
				it->setType(1);
				it->setDest(currentStrat->getDest());
				itComp->setY(zero);
			}
		}
		else{
			itComp = itLastComp;
			--itLastComp;
			--it;
		}
	}
}

list<Point>* FunctionsMinMax::getMinMax(SPTG* sptg, list<Point> f, unsigned int crtState, list<Point> g, unsigned int state, bool max){

	cout << "MinMax state " << crtState << " and " << state << endl;
	//We consider that f is the function of the current state (the one that will be replaced)
	list<Point>* result = new list<Point>();

	list<Point>::iterator itF = f.begin();
	list<Point>::iterator itG = g.begin();
	list<Point>::iterator itFNext = itF;
	++itFNext;
	list<Point>::iterator itGNext = itG;
	++itGNext;

	Value cost = sptg->getTransition(crtState, state);

	if(max){
		if(itF->getY() > itG->getX() + cost)
			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
		else
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

	}
	else{
		if(itF->getY() < itG->getX() + cost)
			result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
		else
			result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
	}


	while( itFNext != f.end() && itGNext != g.end()){
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
			if(itF->getY() > itG->getX() + cost)
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
			else
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));

		}
		else{
			if(itF->getY() < itG->getX() + cost)
				result->push_back(Point(itG->getX(), itG->getY() + cost, Strategy(state, 0, true)));
			else
				result->push_back(Point(itF->getX(), itF->getY(), itF->getStrategy()));
		}


	cout << "Result: " << endl;
	list<Point>::iterator it = result->begin();
	while (it != result->end()){
		cout << "(" << it->getX() << "," << it->getY() << ") ";
		++it;
	}
	cout << endl;
	//clean (*result);
	//tryWaiting(f, state, sptg->getState(state), max);
	return result;
}
