/*
 * Author: Samuel Dehouck
 */


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

SPTGSolverV2::SPTGSolverV2(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
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

	for(unsigned int i = 0; i < size; ++i){
		//cout << "results: ";
		cout << "State " << i << ": " << endl;

		for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
			cout << "(" << it->getX() << "," << it->getY() << ") ";
		cout << endl;
	}




	PGSolver* ps;
	if(solvePTG){
		ps = new PGSolver(sptg, pathsLengths, vals, valueFcts, bottoms, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(true);
		delete ps;
	}
	else{
		ps = new PGSolver(sptg, pathsLengths, vals, valueFcts, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(false); //If extendedDijkstra returns false, some states can't be treated and there is a cycle
		delete ps;
	}

	//Starts by building the valueFcts such that all states are waiting then following their strategies
	buildValueFcts();


	//Because we need to push successors of the first states even if they don't have changed
	int directStates = 0;

	//Push all states that are going directly to 0 but with value different from infinity
	for (unsigned int i = 0; i < size; ++i){
		if(sptg->getTransition(i,0) != -1 && !(*vals)[i].isInfinity()){
			q.push(i);
			++directStates;
			cout << "push: " << i << endl;

		}
		else if(!(*vals)[i].isInfinity()){
			for (unsigned int j = 0; j < size; ++j)
				if((*resets)[i][j] != -1){
					q.push(i);
					cout << "push: " << i << endl;

				}
		}
	}

	treatedStates[0] = true;
	//While all states aren't treated
	while(!q.empty()){
		unsigned int state = q.front();
		treatedStates[state] = true;
		q.pop();
		bool changed = updateValueFct(state);
		if(changed){
			propagate(state);
			for (unsigned int i = 0; i < size; ++i){
				if(sptg->getTransition(i,state) != -1 && !(*vals)[i].isInfinity()){
					q.push(i);

				}
			}
		}
		unsigned int i = 0;
		while(treatedStates[i] && i < size)
			++i;
		if(i != size)
			q.push(i);
	}

	//show();

	for(unsigned int i = 0; i < size; ++i)
		(*vals)[i] = (*valueFcts)[i].front().getY();
}

void SPTGSolverV2::buildValueFcts(){
	//The base function is to wait in the state then go to the target
	for (unsigned int i = 0; i < size; ++i){
		(*valueFcts)[i].push_front(Point(0,((*vals)[i] + sptg->getState(i)),Strategy((*valueFcts)[i].front().getDest(),1,false)));
	}
}

bool SPTGSolverV2::updateValueFct(unsigned int state){
	cout << "====Update value: " << state << "====" << endl;

	for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
		cout << "(" << it->getX() << "," << it->getY() << ") ";
	cout << endl;

	bool changed = false;
	bool allDestsTreated = true;

	//We first need to check if waiting at certain point is better
	changed = compareWaiting(state);

	for (unsigned int j = 0; j < size; ++j){
		//	cout << "to: " << j << endl;
		if(sptg->getTransition(state,j) != -1 && !(*vals)[j].isInfinity() &&(!solvePTG || (*resets)[state][j] == -1)){
			Value cost = sptg->getTransition(state,j);

			if(sptg->getOwner(state)){
				changed = getMinFct(state,j,cost) || changed;
			}
			else
				changed = getMaxFct(state,j, cost) || changed;


			//cout << "results: ";
			/*for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it)
				cout << "(" << it->getX() << "," << it->getY() << ") ";
			cout << endl;
			cout << "State " << state << ": " << endl;

			for (list<Point>::iterator it = (*valueFcts)[state].begin(); it != (*valueFcts)[state].end(); ++it){
				cout << "t: " << it->getX() << " -> ";
				it->getStrategy().show();
			}*/

			//cout << treatedStates[j] << endl;

		}
		if(solvePTG && sptg->getTransition(state,j) != -1 && (*vals)[j].isInfinity() && (*resets)[state][j] != -1){
			//cout << "====Compare reset to " << j << " ====" << endl;
			if(sptg->getOwner(state)){
				changed = getMinFct(state,0,(*resets)[state][j]) || changed;
			}
			else
				changed = getMaxFct(state,0, (*resets)[state][j]) || changed;
		}

	}
	if(allDestsTreated)
		treatedStates[state] = true;

	return changed;
}

void SPTGSolverV2::propagate(unsigned int state){
	cout << "====Propagate====" << endl;
	//Pushes all states that have "state" as a successor and propagates the changes

	queue<unsigned int> p;
	p.push(state);
	vector<bool> marked;

	for (unsigned int i = 0; i < size; ++i)
		marked.push_back(false);
	marked[state] = true;

	while (!p.empty()){
		unsigned int tmpState = p.front();
		p.pop();

		for(unsigned int i = 0; i < size; ++i){
			if(sptg->getTransition(i, tmpState) != -1 && !(*vals)[i].isInfinity() && !marked[i]){
				//Need to update all states that have state as a destination
				list<Point>::iterator itI = (*valueFcts)[i].begin();
				list<Point>::iterator itState = (*valueFcts)[tmpState].begin();
				list<Point>::iterator itLastState = itState;

				cout << "State " << i << ": " << endl;
				for (list<Point>::iterator itI = (*valueFcts)[i].begin(); itI != (*valueFcts)[i].end(); ++itI){
					cout << "t: " << itI->getX() << " -> ";
					itI->getStrategy().show();
				}
				for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
					cout << "(" << it->getX() << "," << it->getY() << ") ";
				cout << endl;

				bool changed = false;
				//Every part of the value fct with strategy going to state needs to be changed
				while (itI != (*valueFcts)[i].end() && itState != (*valueFcts)[tmpState].end() && itI->getX() != 1 && itLastState->getX() != 1){
					cout << itLastState->getX() << " <? " << itI->getX()<< " <? " << itState->getX() << endl;

					if(itI->getDest() == tmpState && itI->getX() == itState->getX() && itI->getType() == 0){
						cout << "1" << endl;
						changed = true;
						//If the transition to state is taken
						if(itState->getX() == itI->getX()){
							//Easy case, we juste need to update the value fct
							Value old = itI->getY();
							itI->setY(itState->getY() + sptg->getTransition(i, tmpState));
							if(old != itI->getY())
								changed = true;
							++itI;
							if(itI->getX() > itState->getX()){
								itLastState = itState;
								++itState;
							}
							cout << " " <<  itI->getX() << " " << itI->getDest() << " " << itI->getType() << endl;

						}
					}
					else if(itI->getDest() == tmpState && itI->getX() == itState->getX() && itI->getType() == 1){
						cout << "2" << endl;
						list<Point>::iterator itINext = itI;
						++itINext;
						list<Point>::iterator itStateNext = itState;
						while(itStateNext->getX() < itINext->getX() && itStateNext->getX() != 1)
							++itStateNext;
						Value old = itI->getY();

						Value diff = itINext->getX() - itI->getX();

						itI->setY(itStateNext->getY() + sptg->getTransition(i, tmpState) + (diff* sptg->getState(i)));
						if(old != itI->getY())
							changed = true;
						++itI;
						if(itI->getX() > itState->getX()){
							itLastState = itState;
							++itState;
						}

					}
					else if(itI->getDest() == tmpState && itState->getX() > itI->getX() && itLastState->getX() < itI->getX()){
						//There is a breakpoint that is in value fct of i but isn't in state's.
						//We have to modify it (in case where it was the first one otherwise it will be cleaned).
						//cout << "B" << endl;
						Value coef = (itState->getY() - itLastState->getY())/(itState->getX() - itLastState->getX());
						Value diff = itI->getX() - itLastState->getX();
						Value old = itI->getY();
						if(itI->getType() == 0){
							cout << "ici " << coef << " " << diff << endl;
							list<Point>::iterator itLastI = itI;
							list<Point>::iterator itNextI = itI;
							--itLastI;
							++itNextI;
							Value coefLast = (itI->getY() - itLastI->getY())/(itI->getX() - itLastI->getX());
							Value zeroLast = itI->getY() - coefLast * itI->getX();
							Value zeroI = itLastState->getY() + sptg->getTransition(i,tmpState) - coef*itLastState->getX();
							Value inter = (zeroLast - zeroI)/(coefLast - coef);
							//If there is the intersection with the strategy that was before that hasn't the same X, we change the X of our point

							if(inter > itLastI->getX() && inter < itState->getX() ){
								itI->setX(inter);
								itI->setY(zeroLast + coef*inter);
							}
							else{
								//Will maybe need to implement it
							}

						}
						else if(itI->getType() == 1){

							list<Point>::iterator itINext = itI;
							++itINext;
							Value diff = itINext->getX() - itI->getX();

							itI->setY(itState->getY() + sptg->getTransition(i,tmpState) +  diff * sptg->getState(i));
						}
						if(old != itI->getY())
							changed = true;
						++itI;
					}
					else if(itI->getDest() == tmpState && itState->getX() < itI->getX()){
						cout << "C" << endl;
						//There is a missing break point (that is in the tempstate value fct but not in i's)
						list<Point>::iterator itILast = itI;
						--itILast;

						if(itILast->getType() == 0)
							(*valueFcts)[i].insert(itI,Point(itState->getX(), itState->getY() + sptg->getTransition(i,tmpState), Strategy(tmpState,0,true)));
						else if(itILast->getType() == 1){
							list<Point>::iterator itINext = itI;
							++itINext;
							Value diff = itINext->getX() - itI->getX();
							(*valueFcts)[i].insert(itI,Point(itState->getX(), itState->getY() + sptg->getTransition(i,tmpState) + (diff * sptg->getState(i)), Strategy(tmpState,1,true)));

						}
						changed = true;
						++itState;
					}

					else{
						//otherwise, the iterators are incremented
						++itI;
						while (itState->getX() < itI->getX()){
							itLastState = itState;
							++itState;
						}
					}
				}
				list<Point>::iterator tmpIt = itI;
				--tmpIt;
				while(itState->getX() != 1 && tmpIt->getDest() == tmpState && tmpIt->getType() == 0){
					//In the case where the state has some breakpoints that i hasn't
					(*valueFcts)[i].insert(itI, Point(itState->getX(), itState->getY() + sptg->getTransition(i,tmpState), Strategy(tmpState, 0, true)));
					++itState;
					changed = true;
				}
				if(changed){
					cout << i << " pushed" << endl;
					p.push(i);
					marked[i] = true;
				}



				for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it)
					cout << "(" << it->getX() << "," << it->getY() << ") ";
				cout << endl;

			cout << "State " << state << ": " << endl;
			for (list<Point>::iterator itI = (*valueFcts)[state].begin(); itI != (*valueFcts)[state].end(); ++itI){
				cout << "t: " << itI->getX() << " -> ";
				itI->getStrategy().show();
			}

			}
		}

	}
}

bool SPTGSolverV2::compareWaiting(unsigned int state){
	cout << "====CompareWaiting====" << endl;
	bool changed = false;
	bool owner = sptg->getOwner(state);
	Value coefWait = Value(0) - sptg->getState(state);
	list<Point>::iterator it = (*valueFcts)[state].begin();


	while(it->getX() != 1 && it != (*valueFcts)[state].end())
		++it;


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
		if((owner && coefWait > coef) || (!owner && coefWait < coef)){

			changed = true;
			//Waiting is improving the value
			list<Point>::iterator currentStrat = it;
			itComp = itLastComp;
			--itLastComp;
			bool intersect = false;
			Value zero = it->getY() - it->getX()*coefWait;
			cout << "zero: " << zero << endl;
			cout << "itComp: " << itComp->getX() << " it:" << it->getX() << endl;
			while(!intersect && itComp->getX() != 0 && itComp != (*valueFcts)[state].begin() && it != (*valueFcts)[state].begin()){
				//cout << "looking for inter" << endl;
				//Waiting is optimal until we find an intersection
				coef = (itComp->getY() - itLastComp->getY())/(itComp->getX() - itLastComp->getX());
				Value zeroComp = itComp->getY() - itComp->getX()*coef;
				Value inter = (zero - zeroComp)/(coef- coefWait);
				cout << "inter " << inter << endl;
				if(inter >= itLastComp->getX() && inter <= itComp->getX()){
					//We need to create a new breaking point
					if(itComp != it){
						itComp->setDest(it->getDest());
						itComp->setX(inter);
						itComp->setY(zero + inter*coefWait);
						itComp->setType(1);
						itComp->setInclusion(true);
					}
					//And erase everything that's between itComp and it
					++itComp;
					while(itComp != it && itComp->getX() != 1 && itComp != (*valueFcts)[state].end())
						itComp = (*valueFcts)[state].erase(itComp);
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
					while(itComp != it && itComp->getX() != 1 && itComp != (*valueFcts)[state].end())
						itComp = (*valueFcts)[state].erase(itComp);
				}
				else{
					//There is no acceptable intersection
					itComp = itLastComp;
					--itLastComp;
				}

			}
			if(itComp->getX() == 0 && it->getX() != 0){
				//cout << "Correction" << endl;
				it = (*valueFcts)[state].begin();
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

	cout << "changed " << changed << endl;
	return changed;
}


bool SPTGSolverV2::getMaxFct(unsigned int state, unsigned int i, Value cost){
	cout << "==>getMax from: " << state << " to " << i << endl;

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


	while( itI->getX() != 1 && itState->getX() != 1 && itI != (*valueFcts)[i].end() && itState != (*valueFcts)[state].end()){
		cout << itState->getX().getVal() << " " << itI->getX().getVal() << endl;

		Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
		Value zeroState = itState->getY() - itState->getX()*coefState;
		Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
		Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

		if((zeroState + coefState*itI->getX()) > (itI->getY() + cost) && (zeroState + coefState*itIEnd->getX()) > (itIEnd->getY() + cost)){
			//If one is on always higher than the other
			if(itState->getX() == 0 && itI->getX() == 0){
				if (itState->getY() >= itI->getY() + cost)
					result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
				else
					result.push_back(Point(itI->getX(),itI->getY() + cost,Strategy(i,0,false)));

			}
			else if(result.back().getX() < itState->getX())
				result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
		}
		else if((zeroState + coefState*itI->getX()) < (itI->getY() + cost) && (zeroState + coefState*itIEnd->getX()) < (itIEnd->getY() + cost)){
			//If one is always lower than the other
			if(itState->getX() == 0 && itI->getX() == 0){
				if (itState->getY() >= itI->getY() + cost)
					result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
				else
					result.push_back(Point(itI->getX(),itI->getY() + cost,Strategy(i,0,false)));

			}
			else if (result.back().getX() < itI->getX())
				result.push_back(Point(itI->getX(),itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
		}
		else{
			//There is an intersection
			if(coefState == coefI && zeroState == zeroI){
				//Both lines are equal but not of the same lengths
				result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));

				if(itStateEnd->getX() < itIEnd->getX()){
					itState = itStateEnd;
					++itStateEnd;
					Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
					if(coefState < coefI){
						result.push_back(Point(itState->getX(), itState->getY(),Strategy(i,0,itI->getInclusion())));
					}
					else{
						result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));

					}
				}
				else{
					itI = itIEnd;
					++itIEnd;
					coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
					if(coefState < coefI){
						result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
					}
					else{
						//If coefState is bigger, it means that we can still follow this strategy
					}
				}
			}
			else{
				Value inter = (zeroI - zeroState)/(coefState - coefI);
				if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
					//The update of the strategy depends on where the intersection is
					if(inter != itState->getX() && inter != itI->getX() && inter != itStateEnd->getX() && inter != itIEnd->getX()){

						if(coefState <= coefI){
							//Before the intersection we can follow state then change
							if((result.empty() || (result.back().getX() < itState->getX())))
								result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
							//We need to change

							result.push_back(Point(inter, zeroI + coefI*inter ,Strategy(i,0,true)));
						}
						else{
							//Before the intersection we can follow i then change

							if((result.empty() || (result.back().getX() < itI->getX())))
								result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
							result.push_back(Point(inter, zeroState + coefState*inter,Strategy(itState->getDest(),itState->getType(),true)));
						}
					}
					else if(inter == itState->getX() && inter == itI->getX()){
						//If the intersection is at the start of both, we need to take a look at the coeffs
						if(coefState < coefI){
							//If the coef is lesser, the next point will be lower (and we don't want to go there)
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
						else{
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));

						}

					}
					else if(inter == itStateEnd->getX() && inter == itIEnd->getX()){
						//If the intersection is at the end of both, we need to take a look at the coeffs
						if(coefState <= coefI && (result.empty() || (result.back().getX() < itState->getX()))){
							//If the coef is lesser, it means the last point was higher (and we want to come from there)
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));

						}
						else if(coefState > coefI && (result.empty() || (result.back().getX() < itI->getX()))){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}

					}
					else if(inter == itStateEnd->getX() && inter != itIEnd->getX()){

						if(coefState <= coefI && (result.back().getX() < itState->getX() || result.empty()))
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						else if(coefState > coefI && (result.back().getX() < itI->getX() || result.empty()))
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));


						itState = itStateEnd;
						++itStateEnd;
						coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
						Value zeroState = itState->getY() - itState->getX()*coefState;

						if(coefState < coefI){
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(i,0,true)));
						}
						else{
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(itState->getDest(),itState->getType(),true)));

						}

					}
					else if(inter == itIEnd->getX() && inter != itStateEnd->getX()){
						if(coefState <= coefI && (result.back().getX() < itState->getX() || result.empty()))
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						else if (coefState > coefI && (result.back().getX() < itI->getX() || result.empty()))
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));


						itI = itIEnd;
						++itIEnd;
						coefI = ((itIEnd->getY() +cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());

						if(coefState < coefI){
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(i,0,true)));
						}
						else{
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(itState->getDest(),itState->getType(),true)));

						}
					}

				}

				else{
					cout << "la" << endl;
					if(inter < itState->getX() || inter < itI->getX()){
						//The intersection is before itState->getX() ou itI->getX()
						cout << " " << result.back().getY() << endl;
						if(coefState < coefI && (result.back().getX() < itI->getX() || result.empty())){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
						else if (coefState >= coefI && (result.back().getX() < itState->getX() || result.empty())){
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}
					}
					else if(inter > itStateEnd->getX() || inter > itIEnd->getX()){
						if(coefState < coefI  && (result.back().getX() < itState->getX() || result.empty())){
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}
						else if(coefState >= coefI && (result.back().getX() < itI->getX()|| result.empty())){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
					}
				}
			}
		}

		cout << "ends " << itStateEnd->getX() << " " << itIEnd->getX() << endl;
		if(itStateEnd->getX() < itIEnd->getX()){
			itState = itStateEnd;
			++itStateEnd;
		}
		else if(itStateEnd->getX() > itIEnd->getX()){
			itI = itIEnd;
			++itIEnd;
		}
		else{
			itState = itStateEnd;
			++itStateEnd;
			itI = itIEnd;
			++itIEnd;
		}
	}


	if(itState->getY() > (itI->getY() + cost))
		result.push_back(Point(1,itState->getY(),Strategy(itState->getDest(),itState->getType(),false)));
	else
		result.push_back(Point(1, (itI->getY() + cost),Strategy(itState->getDest(),itState->getType(),false)));


	//Show the result vector
	itState = result.begin();



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
		//Copy the result into the valueFct
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		while(!(*valueFcts)[state].empty() && ((*valueFcts)[state].front()).getX() != 1 ){
			(*valueFcts)[state].pop_front();
		}
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		for (list<Point>::reverse_iterator it = result.rbegin(); it != result.rend(); ++it){
			(*valueFcts)[state].push_front(Point(it->getX(),it->getY(),Strategy(it->getDest(),it->getType(),it->getInclusion())));
		}
	}
	cout << "changed: " << changed << endl;
	//Show the result vector
	for(itState = result.begin(); itState != result.end(); ++ itState){
		cout << "(" << itState->getX().getVal() << "," << itState->getY() << ") ";
	}
	cout << endl;


	return changed;

}

bool SPTGSolverV2::getMinFct(unsigned int state, unsigned int i, Value cost){
	cout << "==>getMin from: " << state << " to " << i << endl;
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


	while( itI->getX() != 1 && itState->getX() != 1 && itI != (*valueFcts)[i].end() && itState != (*valueFcts)[state].end()){
		cout << itState->getX().getVal() << " " << itI->getX().getVal() <<  endl;

		Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
		Value zeroState = itState->getY() - itState->getX()*coefState;
		Value coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
		Value zeroI = (itI->getY() + cost) - itI->getX()*coefI;

		if((zeroState + coefState*itI->getX()) < (itI->getY() + cost) && (zeroState + coefState*itIEnd->getX()) < (itIEnd->getY() + cost)){
			//We have nothing to do because we already have the good valueFct
			if(itState->getX() == 0 && itI->getX() == 0){
				if (itState->getY() <= itI->getY() + cost)
					result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
				else
					result.push_back(Point(itI->getX(),itI->getY() + cost,Strategy(i,0,false)));

			}
			else if (result.back().getX() < itState->getX())
				result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
		}
		else if((zeroState + coefState*itI->getX()) > (itI->getY() + cost) && (zeroState + coefState*itIEnd->getX()) > (itIEnd->getY() + cost)){
			if(itState->getX() == 0 && itI->getX() == 0){
				if (itState->getY() <= itI->getY() + cost)
					result.push_back(Point(itState->getX(),itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
				else
					result.push_back(Point(itI->getX(),itI->getY() + cost,Strategy(i,0,false)));

			}
			else if (result.back().getX() < itI->getX())
				result.push_back(Point(itI->getX(),itI->getY(),Strategy(i,0,itI->getInclusion())));
		}
		else{
			//There is an intersection


			if(coefState == coefI && zeroState == zeroI){
				//Both lines are equal but not of the same lengths

				//We have to push it because it can happen when they are both always equal but if we have
				result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));

				if(itStateEnd->getX() < itIEnd->getX()){

					itState = itStateEnd;
					++itStateEnd;
					Value coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
					if(coefState < coefI){
						//We need to add the new point
						result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
					}
					else{
						result.push_back(Point(itState->getX(), itState->getY(),Strategy(i,0,itI->getInclusion())));
					}
				}
				else if(itStateEnd->getX() > itIEnd->getX()){
					itI = itIEnd;
					++itIEnd;
					coefI = ((itIEnd->getY() + cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());
					if(coefState < coefI){
						//We can still follow the same strategy and value
					}
					else{
						//We need to add the new break point and change the strategy
						result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));

					}
				}
				else{
				}
			}
			else{
				Value inter = (zeroI - zeroState)/(coefState - coefI);

				if(inter >= itState->getX() && inter >= itI->getX() && inter <= itStateEnd->getX() && inter <= itIEnd->getX()){
					//The update of the strategy depends on where the intersection is
					if(inter != itState->getX() && inter != itI->getX() && inter != itStateEnd->getX() && inter != itIEnd->getX()){
						if(coefState >= coefI){
							//Before the intersection we can follow state then change
							if((result.empty() || (result.back().getX() < itState->getX())))//If it had, the breakpoint is already pushed
								result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
							result.push_back(Point(inter, zeroI+ coefI*inter ,Strategy(i,0,true)));

						}
						else{
							//Before the intersection we can follow i then change
							if((result.empty() || (result.back().getX() < itI->getX())))
								result.push_back(Point(itI->getX(), itI->getY() + cost ,Strategy(i,0,itI->getInclusion())));

							result.push_back(Point(inter, zeroState + coefState*inter,Strategy(itState->getDest(),itState->getType(),true)));
						}

					}
					else if(inter == itState->getX() && inter == itI->getX()){
						//If the intersection is at the start of both, we need to take a look at the coeffs
						if(coefState > coefI){
							//if the coef is bigger, it means the next point will be lower (and we don't want it)
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
						else{
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}

					}
					else if(inter == itStateEnd->getX() && inter == itIEnd->getX()){
						//If the intersection is at the end of both, we need to take a look at the coeffs

						if(coefState >= coefI &&(result.empty() || (result.back().getX() < itState->getX()))){
							//If the coef is bigger, it means the last point was lower (and that's what we want)
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}
						else if(coefState < coefI && (result.empty() || (result.back().getX() < itI->getX()))){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}


					}
					else if(inter == itStateEnd->getX() && inter != itIEnd->getX()){

						if(coefState >= coefI && (result.back().getX() < itState->getX() || result.empty()))
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						else if(coefState < coefI && (result.back().getX() < itI->getX() || result.empty()))
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));


						itState = itStateEnd;
						++itStateEnd;
						coefState = (itStateEnd->getY() - itState->getY())/(itStateEnd->getX() - itState->getX());
						Value zeroState = itState->getY() - itState->getX()*coefState;

						if(coefState > coefI){
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(i,0,true)));
						}
						else{
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(itState->getDest(),itState->getType(),true)));

						}

					}
					else if(inter == itIEnd->getX() && inter != itStateEnd->getX()){


						if(coefState >= coefI && (result.back().getX() < itState->getX() || result.empty()))
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						else if(coefState < coefI && (result.back().getX() < itState->getX() || result.empty()))
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));


						itI = itIEnd;
						++itIEnd;
						coefI = ((itIEnd->getY() +cost) - (itI->getY() + cost))/(itIEnd->getX() - itI->getX());

						if(coefState > coefI){
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(i,0,true)));
						}
						else{
							result.push_back(Point(inter, zeroState + coefState*inter, Strategy(itState->getDest(),itState->getType(),true)));
						}

					}
				}
				else{
					if(inter < itState->getX() || inter < itI->getX()){
						//The intersection is before itState->getX()
						if(coefState > coefI && (result.back().getX() < itI->getX() || result.empty())){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
						else if (coefState <= coefI && (result.back().getX() < itState->getX() || result.empty())){
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}
					}
					else if(inter > itStateEnd->getX() || inter > itIEnd->getX()){
						//The intersection is after
						cout << "i" << endl;
						if(coefState > coefI && (result.back().getX() < itState->getX()|| result.empty())){
							result.push_back(Point(itState->getX(), itState->getY(),Strategy(itState->getDest(),itState->getType(),itState->getInclusion())));
						}
						else if (coefState <= coefI && (result.back().getX() < itI->getX()|| result.empty())){
							result.push_back(Point(itI->getX(), itI->getY() + cost,Strategy(i,0,itI->getInclusion())));
						}
					}
				}
			}
		}


		if(itStateEnd->getX() < itIEnd->getX()){
			itState = itStateEnd;
			++itStateEnd;
		}
		else if(itStateEnd->getX() > itIEnd->getX()){
			itI = itIEnd;
			++itIEnd;
		}
		else{
			itState = itStateEnd;
			++itStateEnd;
			itI = itIEnd;
			++itIEnd;
		}
	}


	if(itState->getY() < (itI->getY() + cost))
		result.push_back(Point(1,itState->getY(),Strategy(itState->getDest(),itState->getType(),false)));
	else
		result.push_back(Point(1, (itI->getY() + cost),Strategy(itState->getDest(),itState->getType(),false)));



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

		//Copy the result into the valueFct
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		while(!(*valueFcts)[state].empty() && ((*valueFcts)[state].front()).getX() != 1 ){
			(*valueFcts)[state].pop_front();
		}
		if(!(*valueFcts)[state].empty())
			(*valueFcts)[state].pop_front();
		for (list<Point>::reverse_iterator it = result.rbegin(); it != result.rend(); ++it){
			(*valueFcts)[state].push_front(Point(it->getX(),it->getY(),Strategy(it->getDest(),it->getType(),it->getInclusion())));
		}
	}
	cout << "changed: " << changed << endl;
	//Show the result vector
	for(itState = result.begin(); itState != result.end(); ++ itState){
		cout << "(" << itState->getX().getVal() << "," << itState->getY() << ") ";
	}
	cout << endl;

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
	cout << "Strategies:" << endl;
	for(unsigned int i = 0; i < size; ++i){
		cout << "State " << i << ": " << endl;
		for (list<Point>::iterator itI = (*valueFcts)[i].begin(); itI != (*valueFcts)[i].end(); ++itI){
			cout << "t: " << itI->getX() << " -> ";
			itI->getStrategy().show();
		}
		cout << endl;
	}

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
