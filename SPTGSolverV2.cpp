#include "SPTGSolverV2.hpp"
#include <iostream>
#include "PGSolver.hpp"

SPTGSolverV2::SPTGSolverV2(SPTG* s){
	sptg = s;
	solvePTG = false;
	size = sptg->getSize();
	bottoms = NULL;
	resets = NULL;
	if(size != 0){
		vals = new vector<CompositeValue>();
		vals->push_back(CompositeValue());

		strategies = new list<Strategy> ();

		pathsLengths = new vector<Value>();
		pathsLengths->push_back(0);


		valueFcts = new vector<list<Point> >();
		valueFcts->push_back(list<Point>());
		// Fill the initial valors, strategies and the ensemble of states
		for (unsigned int i = 1; i < size; ++i){
			vals->push_back(CompositeValue());
			vals->back().setInf(true);
			pathsLengths->push_back(0);
			treatedStates.push_back(false);
			valueFcts->push_back(list<Point>());
			for (unsigned int j = 0; j < size; ++j){
				if(sptg->getTransition(i,j) != -1)
					availableTrans[i].push_back(true);
				else
					availableTrans[i].push_back(false);

			}

		}
	}
}

SPTGSolverV2::SPTGSolverV2(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, list<Strategy>* st, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
	cout << "construction" << endl;
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
	strategies = st;
	valueFcts = vF;
	resets = r;

	size = sptg->getSize();
	for (unsigned int i = 0; i < size; ++i){
		treatedStates.push_back(false);
		availableTrans.push_back(vector<bool>());
		for (unsigned int j = 0; j < size; ++j){
			if(sptg->getTransition(i,j) != -1)
				availableTrans[i].push_back(true);
			else
				availableTrans[i].push_back(false);
		}
	}
}

SPTGSolverV2::~SPTGSolverV2(){
	if(!solvePTG)
		delete valueFcts;
}

void SPTGSolverV2::solveSPTG(){
	cout << "====SolveSPTGV2====" << endl;
	queue<unsigned int> q;

	PGSolver* ps;

	if(solvePTG){
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, bottoms, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(true);
	}
	else{
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(false); //If extendedDijkstra returns false, some states can't be treated and there is a cycle
	}
	//Starts by building the valueFcts such that all states are waiting then following their strategies
	buildValueFcts();

	//Push all states that are going directly to 0 but with value different from infinity
	for (unsigned int i = 0; i < size; ++i){
		if(!(*vals)[i].isInfinity() && sptg->getTransition(i,0) != -1){
			q.push(i);
			cout << "push: " << i << endl;

		}
		else{
			for (unsigned int j = 0; j < size; ++j)
				if(!(*vals)[i].isInfinity() && (*resets)[i][j] != -1){
					q.push(i);
					cout << "push: " << i << endl;

				}
		}
	}

	treatedStates[0] = true;
	//While all states aren't treated
	while(!q.empty()){
		unsigned int state = q.front();
		bool changed = updateValueFct(state);
		if(changed){
			propagate(q,state);
		}
		q.pop();
	}

	show();
}

void SPTGSolverV2::buildValueFcts(){
	//The base function is to wait in the state then go to the target

	for (unsigned int i = 0; i < size; ++i){
		(*valueFcts)[i].push_front(Point(1,(*vals)[i]));
		(*valueFcts)[i].push_front(Point(0,(*vals)[i] + sptg->getState(i)));
	}
}

bool SPTGSolverV2::updateValueFct(unsigned int state){
	cout << "update value: " << state << endl;
	bool changed = false;
	bool allDestsTreated = true;
	for (unsigned int j = 0; j < size; ++j){
		cout << "to: " << j << endl;
		if(availableTrans[state][j]){
			Value cost = sptg->getTransition(state,j);

			if(sptg->getOwner(state))
				changed = getMinFct(state,j,cost) || changed;
			else
				changed = getMaxFct(state,j, cost) || changed;
			cout << "results: ";
			for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
				cout << "(" << it->getX() << "," << it->getY() << ") ";
			cout << endl;

			cout << treatedStates[j] << endl;
			if(treatedStates[j]){
				cout << "removetrans: " << state << " to " << j << endl;
				//availableTrans[state][j] = false;
			}
			else
				allDestsTreated = false;
		}

	}
	if(allDestsTreated)
		treatedStates[state] = true;

	return changed;
}

bool SPTGSolverV2::getMaxFct(unsigned int state, unsigned int i, Value cost){
	cout << "==>getMax from: " << state << " to " << i << endl;
	list<bool> corrections;

	list<Point>::iterator itState = (*valueFcts)[state].begin();
	list<Point>::iterator itStateEnd = (*valueFcts)[state].begin();
	++itStateEnd;

	list<Point>::iterator itI = (*valueFcts)[i].begin();
	list<Point>::iterator itIEnd = (*valueFcts)[i].begin();
	++itIEnd;

	list<Point> result;

	for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << it->getY().getVal() << ") ";
	cout << endl;
	for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << (it->getY() + cost) << ") ";
	cout << endl;

	if((itI->getY() + cost) > itState->getY())
		result.push_back(Point(0,itI->getY() + cost));
	else
		result.push_back(Point(0, itState->getY()));

	while(itIEnd != (*valueFcts)[i].end() && itStateEnd != (*valueFcts)[state].end() && itIEnd->getX() != 0 && itStateEnd->getX() != 0 && itI->getX() != 1 && itState->getX() != 1){
		cout << itState->getX().getVal() << " " << itI->getX().getVal() << endl;
		if(itState->getY() > (itI->getY() + cost) && itStateEnd->getY() > (itIEnd->getY() + cost)){
			//If the valueFct is always better
			if(itStateEnd->getX() <= itIEnd->getX()){
				result.push_back(Point(itStateEnd->getX(),itStateEnd->getY()));
			}
			else{
				Value coef = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
				Value diff = itIEnd->getX() - itState->getX();
				result.push_back(Point(itIEnd->getX(), itState->getY() + coef*diff));
			}
			corrections.push_back(false);

		}
		else if(itState->getY() < (itI->getY() + cost) && itStateEnd->getY() < (itIEnd->getY() + cost)){
			//If the valueFct is always worse, it needs to be changed
			if(itStateEnd->getX() <= itIEnd->getX()){
				Value coef = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
				Value diff = itStateEnd->getX() - itI->getX();
				result.push_back(Point(itStateEnd->getX(), (itI->getY() + cost) + coef*diff));

			}
			else{
				result.push_back(Point(itIEnd->getX(),(itIEnd->getY() + cost)));
			}
			corrections.push_back(true);

		}
		else if(itState->getY() == (itI->getY() + cost) && itStateEnd->getY() == (itIEnd->getY() + cost)){
			if(itStateEnd->getX() <= itIEnd->getX()){
				result.push_back(Point(itStateEnd->getX(), itStateEnd->getY()));
			}

			else{
				result.push_back(Point(itIEnd->getX(),(itIEnd->getY() + cost)));

			}
			corrections.push_back(false);

		}
		else{
			Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
			Value zeroState = itState->getY() - itState->getX()*coefState;
			Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
			Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

			Value inter = (zeroI - zeroState)/(coefState - coefI);

			if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
				result.push_back(Point(inter, zeroState + coefState*inter));
				cout << "intersection" << endl;
				if(inter != itStateEnd->getX() && inter != itState->getX()){
					if(itState->getY() > itI.getY()){
						corrections.push_back(false);
						corrections.push_back(true);
					}
					else{
						corrections.push_back(true);
						corrections.push_back(false);
					}
				}
				else if(inter == itStateEnd->getX()){
					if(itState->getY() > itI->getY())
						corrections.push_back(false);
					else
						corrections.push_back(true);
				}
				else if (inter == itState->getX() && itStateEnd->getX() < itI->getX())
					corrections.push_back(true);
			}


		}


		if(itStateEnd->getX().getVal() < itIEnd->getX().getVal()){
			itState = itStateEnd;
			++itStateEnd;
		}
		else{
			itI = itIEnd;
			++itIEnd;
		}
	}

	//Put the value of end
	if((*valueFcts)[state].back().getY() > ((*valueFcts)[i].back().getY() + cost))
		result.push_back(Point(1,(*valueFcts)[state].back().getY()));
	else
		result.push_back(Point(1, (*valueFcts)[i].back().getY() + cost));


	itState = result.begin();
	list<bool>::iterator itC = corrections.begin();
	for (;itState != result.end() && itC != corrections.end(); ++itC, ++itState)
		cout << "(" << itState->getX() << "," << itState->getY() << ") " << *itC << " ";
	cout << endl;


	cleanValueFct(&result);
	bool changed = false;
	if(result.size() != (*valueFcts)[state].size() || (*valueFcts)[state].back().getX() != result.back().getX() || (*valueFcts)[state].back().getY() != result.back().getY())
		changed = true;
	list<Point>::iterator it = result.begin();
	list<Point>::iterator itVal = (*valueFcts)[state].begin();
	for(; !changed && it != result.end() && itVal != (*valueFcts)[state].end(); ++it, ++itVal){
		if(it->getX() != itVal->getX() || it->getY() != itVal->getY())
			changed = true;
	}

	(*valueFcts)[state].clear();
	for (list<Point>::iterator it = result.begin(); it != result.end(); ++it){
		(*valueFcts)[state].push_back(Point(it->getX(),it->getY()));
	}
	cout << "changed: " << changed << endl;
	return changed;

}

bool SPTGSolverV2::getMinFct(unsigned int state, unsigned int i, Value cost){
	cout << "==>getMin from: " << state << " to " << i << endl;
	list<bool> corrections;
	list<Point>::iterator itState = (*valueFcts)[state].begin();
	list<Point>::iterator itStateEnd = (*valueFcts)[state].begin();
	++itStateEnd;

	list<Point>::iterator itI = (*valueFcts)[i].begin();
	list<Point>::iterator itIEnd = (*valueFcts)[i].begin();
	++itIEnd;

	list<Point> result;

	for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << it->getY().getVal() << ") ";
	cout << endl;
	for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << (it->getY() + cost) << ") ";
	cout << endl;

	//We first need to update the start point
	if((itI->getY() + cost) < itState->getY())
		result.push_back(Point(0,itI->getY() + cost));
	else
		result.push_back(Point(0, itState->getY()));

	while(itIEnd != (*valueFcts)[i].end() && itStateEnd != (*valueFcts)[state].end()  && itIEnd->getX() != 0 && itStateEnd->getX() != 0  && itI->getX() != 1 && itState->getX() != 1){
		cout << itState->getX().getVal() << " " << itI->getX().getVal() << endl;
		if(itState->getY() < (itI->getY() + cost) && itStateEnd->getY() < (itIEnd->getY() + cost)){
			//Nothing needs to be changed
			if(itStateEnd->getX() <= itIEnd->getX()){
				result.push_back(Point(itStateEnd->getX(),itStateEnd->getY()));
			}
			else{

				Value coef = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
				Value diff = itIEnd->getX() - itState->getX();
				result.push_back(Point(itIEnd->getX(), itState->getY() + coef*diff));
			}
			corrections.push_back(false);

		}
		else if(itState->getY() > (itI->getY() + cost) && itStateEnd->getY() > (itIEnd->getY() + cost)){
			//We need to change the value Fct
			if(itStateEnd->getX() <= itIEnd->getX()){
				//Build the value fct up to the the lesser point of the two lines
				Value coef = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
				Value diff = itStateEnd->getX() - itI->getX();
				result.push_back(Point(itStateEnd->getX(), (itI->getY() + cost) + coef*diff));
			}

			else{
				//The line can be drawn up to the itIEnd
				result.push_back(Point(itIEnd->getX(),(itIEnd->getY() + cost)));

			}
			corrections.push_back(true);

		}
		else if(itState->getY() == (itI->getY() + cost) && itStateEnd->getY() == (itIEnd->getY() + cost)){
			if(itStateEnd->getX() <= itIEnd->getX()){
				result.push_back(Point(itStateEnd->getX(), itStateEnd->getY()));
			}

			else{
				result.push_back(Point(itIEnd->getX(),(itIEnd->getY() + cost)));

			}
			corrections.push_back(false);

		}
		else{
			Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
			Value zeroState = itState->getY() - itState->getX()*coefState;
			Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
			Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

			Value inter = (zeroI - zeroState)/(coefState - coefI);

			if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
				result.push_back(Point(inter, zeroState + coefState*inter));
				cout << "intersection" << endl;
				if(inter != itStateEnd->getX() || (inter == itStateEnd->getX() && itState->getX() > itI->getX()) || (inter == itState->getX() && itStateEnd->getX() > itI->getX()))
					corrections.push_back(true);

			}

		}



		if(itStateEnd->getX().getVal() < itIEnd->getX().getVal()){
			itState = itStateEnd;
			++itStateEnd;
		}
		else{
			itI = itIEnd;
			++itIEnd;
		}
	}

	if((*valueFcts)[state].back().getY() < ((*valueFcts)[i].back().getY() + cost))
		result.push_back(Point(1,(*valueFcts)[state].back().getY()));
	else
		result.push_back(Point(1, (*valueFcts)[i].back().getY() + cost));

	itState = result.begin();
	list<bool>::iterator itC = corrections.begin();
	for (;itState != result.end() && itC != corrections.end(); ++itC, ++itState)
		cout << "(" << itState->getX() << "," << itState->getY() << ") " << *itC << " ";
	cout << endl;

	cleanValueFct(&result);
	bool changed = false;
	if(result.size() != (*valueFcts)[state].size() || (*valueFcts)[state].back().getX() != result.back().getX() || (*valueFcts)[state].back().getY() != result.back().getY())
		changed = true;
	list<Point>::iterator it = result.begin();
	list<Point>::iterator itVal = (*valueFcts)[state].begin();
	for(; !changed && it != result.end() && itVal != (*valueFcts)[state].end(); ++it, ++itVal){
		if(it->getX() != itVal->getX() || it->getY() != itVal->getY())
			changed = true;
	}

	(*valueFcts)[state].clear();
	for (list<Point>::iterator it = result.begin(); it != result.end(); ++it){
		(*valueFcts)[state].push_back(Point(it->getX(),it->getY()));
	}
	cout << "changed: " << changed << endl;
	return changed;
}

void SPTGSolverV2::cleanValueFct(list<Point>* l){
	list<Point>::iterator itNext = l->begin();
	++itNext;
	if(itNext != l->end()){
		list<Point>::iterator itCurrent = l->begin();
		++itNext;
		++itCurrent;
		for (list<Point>::iterator itLast = l->begin(); itNext != l->end(); ++itNext){
			bool deleted = false;
			if(!itLast->getY().isInfinity() && !itCurrent->getY().isInfinity() && !itNext->getY().isInfinity() && (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				Value coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());
				if(itLast->getY() + (coef * (itCurrent->getX() - itLast->getX())) == itCurrent->getY()){
					l->erase(itCurrent);
					deleted = true;
				}
			}
			else if(itLast->getY().isInfinity() && itCurrent->getY().isInfinity() && itNext->getY().isInfinity()&& (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
				l->erase(itCurrent);
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

void SPTGSolverV2::propagate(queue<unsigned int> &q, unsigned int state){
	//Push all states that have "state" as a successor and propagate the changes
	for (unsigned int i = 0; i < size; ++i){
		if(sptg->getTransition(i,state) != -1 && !(*vals)[i].isInfinity()){

			q.push(i);
			cout << "push: " << i << endl;
		}
	}
}


void SPTGSolverV2::show(){

	cout << "Strategies: " << endl;
	for (list<Strategy>::iterator it = strategies->begin(); it != strategies->end(); ++it){
		it->show();
	}
	cout << endl;

	cout << "Lengths: " << endl;
	for (unsigned int i = 0; i < pathsLengths->size(); ++i)
		cout << (*pathsLengths)[i] << "	";
	cout << endl;

	cout << "Vals:" << endl;
	for (unsigned int i = 0; i < vals->size(); ++i)
		cout << (*vals)[i] << "	";
	cout << endl;


	cout << "Value Functions" << endl;
	for (unsigned int i = 1; i < valueFcts->size(); ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ")	";
		}
		cout << endl;
	}
}
