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
		strategies = new vector<list<Strategy> >();
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

SPTGSolverV2::SPTGSolverV2(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, vector<list<Strategy> >* st, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
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
	list<unsigned int> q;

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


	//Because we need to push successors of the first states even if they don't have changed
	int directStates = 0;

	//Push all states that are going directly to 0 but with value different from infinity
	for (unsigned int i = 0; i < size; ++i){
		if(!(*vals)[i].isInfinity() && sptg->getTransition(i,0) != -1){
			q.push_back(i);
			++directStates;
			cout << "push: " << i << endl;

		}
		else{
			for (unsigned int j = 0; j < size; ++j)
				if(!(*vals)[i].isInfinity() && (*resets)[i][j] != -1){
					q.push_back(i);
					cout << "push: " << i << endl;

				}
		}
	}

	treatedStates[0] = true;
	//While all states aren't treated
	while(!q.empty()){
		unsigned int state = q.front();
		q.pop_front();
		bool changed = updateValueFct(state);
		if(changed || directStates > 0){
			propagate(q,state);
		}
		--directStates;
	}

	show();
}

void SPTGSolverV2::buildValueFcts(){
	//The base function is to wait in the state then go to the target
	for (unsigned int i = 0; i < size; ++i){
		(*valueFcts)[i].push_front(Point(1,(*vals)[i]));
		(*valueFcts)[i].push_front(Point(0,(*vals)[i] + sptg->getState(i)));

		list<Strategy>::iterator itS = (*strategies)[i].begin();
		(*strategies)[i].push_front(Strategy(0,itS->getDest(),1,false));

	}
}

bool SPTGSolverV2::updateValueFct(unsigned int state){
	cout << "====Update value: " << state << "====" << endl;
	bool changed = false;
	bool allDestsTreated = true;

	//We first need to check if waiting at certain point is better
	changed = compareWaiting(state);

	for (unsigned int j = 0; j < size; ++j){
		cout << "to: " << j << endl;
		if(availableTrans[state][j]){
			Value cost = sptg->getTransition(state,j);

			if(sptg->getOwner(state)){
				changed = getMinFct(state,j,cost) || changed;
			}
			else
				changed = getMaxFct(state,j, cost) || changed;


			cout << "results: ";
			for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
				cout << "(" << it->getX() << "," << it->getY() << ") ";
			cout << endl;
			cout << "State " << state << ": ";
			for (list<Strategy>::iterator it = (*strategies)[state].begin(); it != (*strategies)[state].end(); ++it){
				it->show();
			}
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

void SPTGSolverV2::propagate(list<unsigned int> &q, unsigned int state){
	cout << "====Propagate====" << endl;
	//Pushes all states that have "state" as a successor and propagates the changes

	for (unsigned int i = 0; i < size; ++i){
		cout << "update " << i << endl;
		if(sptg->getTransition(i, state) != -1){
			//Need to update all states that have state as a destination
			list<Strategy>::iterator it = (*strategies)[i].begin();
			list<Point>::iterator itI = (*valueFcts)[i].begin();
			list<Point>::iterator itState = (*valueFcts)[state].begin();
			list<Point>::iterator itStateLast = itState;

			//Every part of the value fct with strategy is going to state needs to be changed
			while (it != (*strategies)[i].end() && itI != (*valueFcts)[i].end() && itState != (*valueFcts)[state].end() && it->getTime() != 1 && itI->getX() != 1 && itState->getX() != 1){
				if(it->getDest() == state){
					if(itState->getX() == itI->getX() && itI->getX() == it->getTime()){
						//Easy case, we juste need to update the value fct
						cout << "A" << endl;
						itI->setY(itState->getY() + sptg->getTransition(i, state));
						++it;
						++itI;
						itStateLast = itState;
						++itState;
					}
					else if(itI->getX() == it->getTime() && itState->getX() > itI->getX() && itStateLast->getX() < itI->getX()){
						//There is a breakpoint that is in value fct of i but isn't in state's.
						//We have to modify it (in case where it was the first one otherwise it will be cleaned).
						cout << "B" << endl;
						Value coef = (itState->getY() - itStateLast->getY())/(itState->getX() - itStateLast->getX());
						Value diff = itI->getX() - itStateLast->getX();
						itI->setY(itStateLast->getY() + sptg->getTransition(i,state) - diff*coef);
					}
					else if(itI->getX() == it->getTime() && itState->getX() < itI->getX()){
						cout << "C" << endl;
						//There is a missing break point (that is in the state value fct but not in i's)
						(*valueFcts)[i].insert(itI,Point(itState->getX(), itState->getY() + sptg->getTransition(i,state)));
						++itState;

					}
					else if(itI->getX() != it->getTime()){
						cerr << "Error, valueFct and strat not synchronized!" << endl;
					}
				}
				else{
					++it;
					++itI;
					while (itState->getX() < itI->getX()){
						itStateLast = itState;
						++itState;
					}
				}
			}
			q.push_back(i);
			for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
				cout << "(" << it->getX() << "," << it->getY() << ") ";
			cout << endl;
		}

	}
}

void SPTGSolverV2::updateStrategies(list<Point> &res, list<bool> &correct, unsigned int state, unsigned int i){
	cout << "====UpdateStrategies====" << endl;

	list<Point>::iterator itR = res.begin();
	list<bool>::iterator itC = correct.begin();
	list<Strategy>::iterator itS = (*strategies)[state].begin();
	list<Strategy>::iterator itSLast = itS;


	while(itR != res.end() && itC != correct.end() && itS != (*strategies)[i].end()){
		cout << itR->getX() << " " << itS->getTime() << endl;
		if(itR->getX() == itS->getTime()){
			if(*itC){
				itS->setDest(i);
				itS->setType(0);
			}
			++itR;
			++itC;
			itSLast = itS;
			++itS;
		}
		else if(itR->getX() < itS->getTime()){
			(*strategies)[state].insert(itS, Strategy(itR->getX(),0,0,true));
			list<Strategy>::iterator it = itSLast;
			++itSLast;
			if(*itC){
				itS->setDest(i);
				itS->setType(0);
			}
			else{
				itSLast->setDest(itS->getDest());
				itSLast->setType(0);
			}

			++itR;
			++itC;
		}
		else{

			itS = (*strategies)[i].erase(itS);
		}
	}

	for (list<Strategy>::iterator it = (*strategies)[i].begin(); it != (*strategies)[i].end(); ++it)
		cout << it->getTime() << ": " << it->getDest() << " " << it->getType() << endl;

}

bool SPTGSolverV2::compareWaiting(unsigned int state){
	cout << "====CompareWaiting====" << endl;
	bool changed = false;
	bool owner = sptg->getOwner(state);
	Value coefWait = Value(0) - sptg->getState(state);
	list<Point>::iterator it = (*valueFcts)[state].begin();
	list<Strategy>::iterator itS = (*strategies)[state].begin();


	while(it->getX() != 1 && it != (*valueFcts)[state].end())
		++it;
	while(itS->getTime() != 1 && itS != (*strategies)[state].end()){
		++itS;
	}

	list<Point>::iterator itComp = it;
	list<Point>::iterator itLastComp = itComp;
	--itLastComp;

	while(itComp->getX() != 0 && itComp != (*valueFcts)[state].begin() && it != (*valueFcts)[state].begin()){
		cout << it->getX() << " compared to " << itLastComp->getX() << " "  << itComp->getX() << endl;
		Value coef;
		if(itComp->getY() != itLastComp->getY())
			coef = (itComp->getY() - itLastComp->getY())/(itComp->getX() - itLastComp->getX());
		else
			coef = 0;
		cout << "coefWait: " << coefWait << " " << "coef: " << coef << endl;
		if((owner && coefWait < coef) || (!owner && coefWait > coef)){
			changed = true;
			//Waiting is improving the value
			list<Strategy>::iterator currentStrat = itS;
			itComp = itLastComp;
			--itLastComp;
			bool intersect = false;
			Value zero = it->getY() - it->getX()*coefWait;
			while(!intersect && itComp->getX() != 0 && itComp != (*valueFcts)[state].begin() && it != (*valueFcts)[state].begin()){
				cout << "looking for inter" << endl;
				//Waiting is optimal until we find an intersection
				coef = (itComp->getX() - itLastComp->getX())/(itComp->getY() - itLastComp->getY());
				Value zeroComp = itComp->getY() - itComp->getX()*coef;
				Value inter = (zero - zeroComp)/(coef- coefWait);
				if(inter > itLastComp->getX() && inter < itComp->getX()){
					//We need to create a new breaking point
					intersect = true;
					itS->setTime(inter);
					itS->setDest(currentStrat->getDest());
					itS->setType(1);
					//We can take the last breakpoint and change its position
					itComp->setX(inter);
					itComp->setY(zeroComp + inter*coef);
					itComp = itLastComp;
					--itLastComp;
					--itS;
					it = itComp;
				}
				else if(inter == itLastComp->getX()){
					//The intersection is the next point, we can delete the current (itComp) and update the strategy
					intersect = true;
					list<Strategy>::iterator tmp = itS;
					--itS;
					(*strategies)[state].erase(tmp);
					itS->setDest(currentStrat->getDest());
					itS->setType(1);
					itComp = (*valueFcts)[state].erase(itComp);
					itComp = itLastComp;
					--itLastComp;
					it = itComp;
				}
				else{
					//There is no acceptable intersection
					list<Strategy>::iterator tmp = itS;
					--itS;
					(*strategies)[state].erase(tmp);

					itComp = (*valueFcts)[state].erase(itComp);
					itComp = itLastComp;
					--itLastComp;
				}

			}
		}
		else{
			itComp = itLastComp;
			--itLastComp;
			--itS;
			--it;
		}
	}
	cout << "changed " << changed << endl;
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

	//Shows the two lists that are compared
	for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << it->getY().getVal() << ") ";
	cout << endl;
	for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
		cout << "(" << it->getX().getVal() << "," << (it->getY() + cost) << ") ";
	cout << endl;


	//We have to first push the value at time 0
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
			//There is an intersection
			Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
			Value zeroState = itState->getY() - itState->getX()*coefState;
			Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
			Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

			Value inter = (zeroI - zeroState)/(coefState - coefI);

			if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
				cout << "intersection" << endl;
				//The update of the strategy depends on where the intersection is

				if(inter != itStateEnd->getX() && inter != itState->getX()){
					result.push_back(Point(inter, zeroState + coefState*inter));

					if(itState->getY() > itI->getY()){
						corrections.push_back(false);
						if(itIEnd->getX() != 1)
							result.push_back(Point(itIEnd->getX(), itIEnd->getY()));

						corrections.push_back(true);
					}
					else{
						corrections.push_back(true);
						if(itStateEnd->getX() != 1)
							result.push_back(Point(itStateEnd->getX(), itStateEnd->getY()));

						corrections.push_back(false);
					}
				}
				else if(inter == itStateEnd->getX()){
					if(coefState < coefI)
						corrections.push_back(false);
					else{
						corrections.push_back(true);
						if(itI->getX() != 0)
							result.push_back(Point(itI->getX(), itI->getY()));
					}
				}
				else if (inter == itState->getX()){
					if(coefState > coefI)
						corrections.push_back(false);
					else{
						if(itIEnd->getX() != 1)
							result.push_back(Point(itIEnd->getX(), itIEnd->getY()));
						corrections.push_back(true);
					}
				}
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

	//Still need to push the value at time 1
	itState = (*valueFcts)[state].begin();
	while(itState->getX() != 1)
		++itState;
	itI = (*valueFcts)[state].begin();
	while(itI->getX() != 1)
		++itI;
	if(itState->getY() > (itI->getY() + cost))
		result.push_back(Point(1,itState->getY()));
	else
		result.push_back(Point(1, (itI->getY() + cost)));


	//Show the result vector
	itState = result.begin();
	list<bool>::iterator itC = corrections.begin();
	for (;itState != result.end() && itC != corrections.end(); ++itC, ++itState)
		cout << "(" << itState->getX() << "," << itState->getY() << ") " << *itC << " ";
	cout << endl;


	bool changed = false;
	cleanValueFct(&result);

	//Compare the results and the valueFct (knowing that there are points from earlier computation in valueFct)
	list<Point>::iterator it = result.begin();
	list<Point>::iterator itVal = (*valueFcts)[state].begin();
	for(; !changed && it != result.end() && itVal != (*valueFcts)[state].end(); ++it, ++itVal){
		if(it->getX() != itVal->getX() || it->getY() != itVal->getY())
			changed = true;
	}

	if(changed){
		updateStrategies(result, corrections,state, i);

		//Copy the result into the valueFct
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		while(!(*valueFcts)[state].empty() && ((*valueFcts)[state].front()).getX() != 1 ){
			(*valueFcts)[state].pop_front();
		}
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		for (list<Point>::reverse_iterator it = result.rbegin(); it != result.rend(); ++it){
			(*valueFcts)[state].push_front(Point(it->getX(),it->getY()));
		}
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
			//There is an intersection
			Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
			Value zeroState = itState->getY() - itState->getX()*coefState;
			Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
			Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

			Value inter = (zeroI - zeroState)/(coefState - coefI);

			if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
				cout << "intersection" << endl;

				//The update of the strategy depends on where the intersection is
				if(inter != itStateEnd->getX() && inter != itState->getX()){
					cout << "A" << endl;
					result.push_back(Point(inter, zeroState + coefState*inter));

					if(itState->getY() < itI->getY()){
						corrections.push_back(false);
						if(itIEnd->getX() != 1)
							result.push_back(Point(itIEnd->getX(), itIEnd->getY()));
						corrections.push_back(true);
					}
					else{
						corrections.push_back(true);
						if(itStateEnd->getX() != 1)
							result.push_back(Point(itStateEnd->getX(), itStateEnd->getY()));

						corrections.push_back(false);
					}
				}
				else if(inter == itStateEnd->getX()){
					//We need to check at the coefficients
					cout << "B" << endl;
					cout << "coefS: " <<  coefState << " " << "coefI: " << coefI << endl;
					if(coefState > coefI){
						corrections.push_back(false);
					}
					else{
						corrections.push_back(true);
						if(itI->getX() != 0)
							result.push_back(Point(itI->getX(), itI->getY()));

					}
				}
				else if (inter == itState->getX()){
					cout << "C" << endl;
					cout << "coefS: " <<  coefState << " " << "coefI: " << coefI << endl;

					if(coefState < coefI){
						corrections.push_back(false);
					}
					else{
						corrections.push_back(true);
						if(itIEnd->getX() != 1)
							result.push_back(Point(itIEnd->getX(), itIEnd->getY()));
					}
				}
			}
		}

		//Update the iterators
		if(itStateEnd->getX().getVal() < itIEnd->getX().getVal()){
			itState = itStateEnd;
			++itStateEnd;
		}
		else{
			itI = itIEnd;
			++itIEnd;
		}
	}

	//Still need to push the last value (of time 1)
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

	//Compare the results and the valueFct (knowing that there are points from earlier computation in valueFct)
	list<Point>::iterator it = result.begin();
	list<Point>::iterator itVal = (*valueFcts)[state].begin();
	for(; !changed && it != result.end() && itVal != (*valueFcts)[state].end(); ++it, ++itVal){
		if(it->getX() != itVal->getX() || it->getY() != itVal->getY())
			changed = true;
	}
	if(changed){
		updateStrategies(result, corrections,state, i);

		//Copy the into the valueFct
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		while(!(*valueFcts)[state].empty() && (*valueFcts)[state].front().getX() != 1){
			(*valueFcts)[state].pop_front();
		}
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		for (list<Point>::reverse_iterator it = result.rbegin(); it != result.rend(); ++it){
			(*valueFcts)[state].push_front(Point(it->getX(),it->getY()));
		}
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


void SPTGSolverV2::show(){

	cout << "Strategies: " << endl;
	for (unsigned int i = 0; i < size; ++i){
		for (list<Strategy>::iterator it = (*strategies)[i].begin(); it != (*strategies)[i].end(); ++it){
			it->show();
		}
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
