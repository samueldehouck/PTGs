#include "SPTGSolver.hpp"
#include <iostream>
#include <queue>

SPTGSolver::SPTGSolver(SPTG* s){
	sptg = s;
	solvePTG = false;
	time = Value(1);
	init();
}

SPTGSolver::SPTGSolver(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, list<Strategy>* st, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
	strategies = st;
	valueFcts = vF;
	resets = r;
	time = Value(1);

	size = sptg->getSize();
	if(size != 0){

		for (unsigned int i = 0; i < size; ++i)
			lambdas.push_back(CompositeValue());

	}
}


SPTGSolver::~SPTGSolver(){
	if(!solvePTG)
		delete valueFcts;
}

void SPTGSolver::show(){

	cout << "Lambdas:" << endl;
	for (unsigned int i = 0; i < lambdas.size(); ++i)
		cout <<  lambdas[i] << "	";

	cout << endl;


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

void SPTGSolver::init(){
	//   Initialization of all vectors and variables used by the solver
	size = sptg->getSize();
	if(size != 0){
		vals = new vector<CompositeValue>();
		vals->push_back(CompositeValue());

		strategies = new list<Strategy> ();
		(*strategies).push_front(Strategy(size, time, false));
		(*strategies).front().insert(0,0,0);

		pathsLengths = new vector<Value>();
		pathsLengths->push_back(0);

		lambdas.push_back(CompositeValue());

		valueFcts = new vector<list<Point> >();
		valueFcts->push_back(list<Point>());

		// Fill the initial valors, strategies and the ensemble of states
		for (unsigned int i = 1; i < size; ++i){
			vals->push_back(CompositeValue());
			vals->back().setInf(true);

			strategies->front().insert(i, -1, 0);
			pathsLengths->push_back(0);

			lambdas.push_back(CompositeValue());

			valueFcts->push_back(list<Point>());
		}
	}
}

void SPTGSolver::solveSPTG(){
	cout << "====SolveSPTG====" << endl;
	// This function is the core of the solver, it computes the strategies and values for the SPTG passed
	PGSolver* ps;

	if(solvePTG){
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, bottoms, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(true);
	}
	else{
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(false); //If extendedDijkstra returns false, some states can't be treated and there is a cycle
	}

	while (time > 0){

		strategies->push_front(Strategy(size));
		actualizeLambdas();
		if(time == 1)//If it's the first turn, we have to "initialize" the value functions
			buildValueFcts(0);
		strategyIteration();
		Value epsilon = nextEventPoint();
		if((time - epsilon).isInfinity() || (time - epsilon) < 0)
			epsilon = time;
		buildValueFcts(epsilon);
		actualizeVals(epsilon);
		strategies->front().setTime(time);
	}
	delete ps;
	sptg = NULL;
}

void SPTGSolver::strategyIteration(){
	// Computes the strategies in a priced game representing the sptg during an interval to be determined
	cout << "====StrategyIteration====" << endl;
	bool switched = true;

	while (switched){
		switched = false;
		switched = makeImpSwitchesP2();
		switched = makeImpSwitchesP1() || switched ;

	}
}

void SPTGSolver::actualizeLambdas(){
	for (unsigned int i = 1; i < lambdas.size(); ++i){
		if((*vals)[i].isInfinity())
			lambdas[i].setInf(true);
		else{
			lambdas[i] = (*vals)[i];
			lambdas[i].setEps(sptg->getState(i).getVal());
		}
		//The P1 will always take the lambda transitions at first
		if(sptg->getOwner(i)){
			(*vals)[i] = lambdas[i];
			strategies->front().insert(i,0,1);
			(*pathsLengths)[i] = 1;
		}
	}
}

void SPTGSolver::actualizeVals(Value epsilon){
	for (unsigned int i = 1; i < vals->size(); ++i){
		if(!(*vals)[i].isInfinity()){
			(*vals)[i] = (*vals)[i] + epsilon * (*vals)[i].getEps() ;
			(*vals)[i].setEps(0);
		}

	}
}

bool SPTGSolver::makeImpSwitchesP1(){
	cout << "==>Imp1" << endl;
	bool allDone = false;
	bool changed = false;
	bool oneChange = false;
	while (!allDone){
		//For all states
		allDone = true;
		for (unsigned int state = 1; state < size; ++state){
			//Owned by P1 because we are checking the improving switches for the P1
			changed = false;
			if(sptg->getOwner(state)){
				//cout << "State: " << state << endl;
				//cout << "Actual value: " << (*vals)[state] << endl;

				//We don't need to look at the bottom transitions for the player 1 because it goes to MAX and will never be taken

				//Check the lambda transition
				if(lambdas[state] < (*vals)[state]){
					//cout << "lambda is better" << endl;
					//If we have an improvement, we update the values found
					(*vals)[state] = lambdas[state];
					//cout << (*vals)[state]<< endl;
					strategies->front().insert(state, 0, 1);
					(*pathsLengths)[state] = 1;
					allDone = false;
					changed = true;
					oneChange = true;
				}

				for (unsigned int nextState = 0; nextState < size; ++nextState){
					//We need to check all transitions for every states except the lambda transition because it is taken by default and the ones that go to "MAX" (fictive state)
					if(sptg->getTransition(state, nextState) != -1){//If the transition exists
						CompositeValue tempVal = (*vals)[nextState] + sptg->getTransition(state, nextState);
						//cout << "tmp: " << tempVal << endl;
						Value tempLength = (*pathsLengths)[nextState] + 1;

						if((*resets)[state][nextState] != -1){
							//cout << (*resets)[state][nextState] << endl;
						}
						if(solvePTG && ((*resets)[state][nextState] != -1) &&  ((*resets)[state][nextState] < (*vals)[state])){//If we have an improvement, we update the values found

							//cout << "reset ("<< (*resets)[state][nextState] << ") to " << nextState << " is better" << endl;
							(*vals)[state] = (*resets)[state][nextState];
							(*vals)[state].setEps(0);
							cout << (*vals)[state] << endl;

							strategies->front().insert(state, nextState, 3);
							allDone = false;
							changed = true;
							oneChange = true;
						}

						else if(((*resets)[state][nextState] == -1) && ((tempVal < (*vals)[state])//If we have an improvement, we update the values found
								||((tempVal == (*vals)[state]) && (tempLength < (*pathsLengths)[state])))){
							if(!(*vals)[nextState].isInfinity()){
								(*vals)[state].setVal(tempVal.getVal());
								(*vals)[state].setEps((*vals)[nextState].getEps());
							}
							else
								(*vals)[state].setInf(true);
							//cout << "to " << nextState << " is better" << endl;

							//cout << "val: " << (*vals)[state]<< endl;

							(*pathsLengths)[state] = tempLength;
							strategies->front().insert(state, nextState, 0);
							allDone = false;
							changed = true;
							oneChange = true;

						}
					}
				}
			}
			if(changed)//If a change has been made, it can have an impact on the values of other states
				propagate(state);
		}

	}
	//cout << "changed: " << changed << endl;
	return oneChange;
}

bool SPTGSolver::makeImpSwitchesP2(){
	cout << "==>Imp2" << endl;
	bool allDone = false;
	bool changed = false;
	bool oneChange = false;
	while (!allDone){
		//For all states
		allDone = true;
		for (unsigned int state = 1; state < size; ++state){
			//Owned by P2 because we are checking the improving switches for the P2
			changed = false;
			if(!sptg->getOwner(state)){
				cout << "State: " << state << endl;
				cout << "Actual value: " << (*vals)[state]<< endl;
				//We don't need to take a look at the bottom transitions because the lambda transitions will always be better for P2

				cout << lambdas[state] << endl;
				//Check the lambda transition
				if(lambdas[state] > (*vals)[state]){
					cout << "lambda is better" << endl;
					//If we have an improvement, we update the values found
					(*vals)[state] = lambdas[state];
					cout << (*vals)[state]<< endl;
					strategies->front().insert(state, 0, 1);
					(*pathsLengths)[state] = 1;
					allDone = false;
					changed = true;
					oneChange = true;
				}

				for (unsigned int nextState = 0; nextState < size; ++nextState){
					if(sptg->getTransition(state, nextState) != -1){//If the transition exists
						CompositeValue tempVal = (*vals)[nextState] + sptg->getTransition(state, nextState);
						Value tempLength = (*pathsLengths)[nextState] + 1;
						cout << "temp: " << tempLength << endl;

						if(!tempLength.isInfinity()){
							//Check if the reset exists and is better
							if(solvePTG && ((*resets)[state][nextState] != -1) &&  ((*resets)[state][nextState] > (*vals)[state])){//If we have an improvement, we update the values found
								cout << "reset to " << nextState << " is better" << endl;
								(*vals)[state] = (*resets)[state][nextState];
								(*vals)[state].setEps(0);
								cout << (*vals)[state] << endl;

								strategies->front().insert(state, nextState, 3);
								allDone = false;
								changed = true;
								oneChange = true;
							}
							cout << "tempVal: " << tempVal << endl;
							cout << "val: " << (*vals)[state] << endl;
							if(!(*vals)[state].isInfinity() && (strategies->front().getDest(nextState) != state) && ((*resets)[state][nextState] == -1) &&
									((tempVal > (*vals)[state])
											||((tempVal == (*vals)[state]) && (tempLength > (*pathsLengths)[state])))){
								cout << "to " << nextState << " is better" << endl;
								//If we have an improvement, we update the values found
								if(!(*vals)[nextState].isInfinity()){
									(*vals)[state].setVal(tempVal.getVal());
									(*vals)[state].setEps((*vals)[nextState].getEps());
								}
								else
									(*vals)[state].setInf(true);
								cout << (*vals)[state] << endl;
								cout << tempLength << endl;

								(*pathsLengths)[state] = tempLength;
								strategies->front().insert(state, nextState, 0);
								allDone = false;
								changed = true;
								oneChange = true;
							}
						}
					}
				}
			}
			if(changed)//If a change has been made, it can have an impact on the values of other states
				propagate(state);
		}
	}

	return oneChange;
}

void SPTGSolver::propagate(unsigned int state){
	//Use a queue to update the value of all state that can reach the one given
	//cout << "====Propagate from state: " << state << " ===" << endl;
	queue<unsigned int> q;
	vector<bool> checked;
	for (unsigned int i = 0; i < size; ++i)
		checked.push_back(false);
	q.push(state);
	checked[state] = true;
	while (!q.empty() ){
		unsigned int tmpState = q.front();
		for(unsigned int i = 1; i < size; ++i){
			if(strategies->front().getDest(i) == tmpState && !checked[i] && strategies->front().getType(i) != 3){
				//cout  << "Not checked yet" << endl;
				//cout << "tmpstate: " << (*vals)[tmpState] << endl;
				if(!(*vals)[i].isInfinity()){
					(*vals)[i] = (*vals)[tmpState] + sptg->getTransition(i,tmpState);
					(*vals)[i].setEps((*vals)[tmpState].getEps());
				}
				(*pathsLengths)[i] = (*pathsLengths)[tmpState] + 1;
				q.push(i);
				checked[i] = true;
			}
			else if(strategies->front().getDest(i) == tmpState && checked[i] && !(*vals)[i].isInfinity()){
				//cout << "Cycle!!" << endl;
				(*vals)[i].setInf(true);
				(*pathsLengths)[i].setInf(true);
				q.push(i);
			}
			else if(strategies->front().getDest(i) == tmpState && checked[i] && (*vals)[i].isInfinity()){
				(*pathsLengths)[i].setInf(true);
			}
		}
		q.pop();
	}
}

void SPTGSolver::addPoint(unsigned int index, Value x, Value y){
	(*valueFcts)[index].push_front(Point(x,y));
}

void SPTGSolver::buildValueFcts(Value epsilon){
	//   Add a point for every state
	time = time - epsilon;
	if(time != 0 && time != 1){
		(strategies->front().setInclusion(true));
	}
	for (unsigned int i = 1; i < size; ++i){
		Value tmpVal = (*vals)[i] + (epsilon * (*vals)[i].getEps());
		list<Point>::iterator it = (*valueFcts)[i].begin();
		++it;
		if(it != (*valueFcts)[i].end() && !solvePTG){
			Value coef = (it->getY() - tmpVal)/(it->getX() - time);
			//Doesn't work when resolving PTGs because the rescaling hasn't been made yet
			if(tmpVal + (coef * ((*valueFcts)[i].front().getX() - time)) == (*valueFcts)[i].front().getY()){
				(*valueFcts)[i].pop_front();
			}
		}

		addPoint(i, time , tmpVal);
	}
}

Value SPTGSolver::nextEventPoint(){
	cout << "====NextEventPoint===" << endl;
	Value min;
	min.setInf(true);

	for (unsigned int state = 1; state < size; ++state){
		//cout << "State: " << state << endl;
		//First we have to check if an epsilon can be found using the lambda transition
		Value tempMin;
		tempMin.setInf(true);

		if(!(*vals)[state].isInfinity() && !lambdas[state].isInfinity() && (*vals)[state] != lambdas[state]){
			//cout << (*vals)[state] << " ?!= " << lambdas[state] << endl;
			tempMin = ((*vals)[state].getVal() - lambdas[state].getVal())/(lambdas[state].getEps() - (*vals)[state].getEps());
			//cout << "tempmin: " << tempMin << endl;
			if(tempMin <= 0)
				tempMin.setInf(true);
			if( time - tempMin < min)
				min = tempMin;

		}


		for (unsigned int nextState = 0; nextState < size; ++nextState){
			//Then we need to check if there is another epsilon that can be found using the other transitions
			//cout << "to " << nextState << endl;
			if(solvePTG && (*resets)[state][nextState] != -1 && sptg->getTransition(state, nextState) != -1 && (*vals)[state].getEps() != 0 && sptg->getOwner(state) == 0){
				//If we are solving a PTG (with resets), if there is a reset and the state belongs to P2 because it goes to the target

				//tempMin = ((*vals)[state].getVal() - (*resets)[state][nextState].getVal())/(*vals)[state].getEps();

				//cout << "reset" << tempMin << endl;
				if(tempMin <= 0)
					tempMin.setInf(true);

				if(tempMin < min)
					min = tempMin;
			}
			else if((*resets)[state][nextState] == -1 && sptg->getTransition(state, nextState) != -1 && ((*vals)[state].getVal() != (*vals)[nextState].getVal() + sptg->getTransition(state, nextState).getVal()) &&
					((*vals)[nextState].getEps() != (*vals)[state].getEps()))
			{
				//cout << "(" << (*vals)[state].getVal() << " - (" << (*vals)[nextState].getVal() << " + " << sptg->getTransition(state, nextState).getVal() << ")) / (" << (*vals)[nextState].getEps() << " - " << (*vals)[state].getEps() << ")" << endl;
				tempMin = ((*vals)[state].getVal() - ((*vals)[nextState].getVal() + sptg->getTransition(state, nextState).getVal()))/((*vals)[nextState].getEps() - (*vals)[state].getEps());
				//cout << tempMin << endl;
				if(tempMin <= 0)
					tempMin.setInf(true);

				if(tempMin < min)
					min = tempMin;
			}

		}

	}
	return min;
}

list<Strategy>* SPTGSolver::getStrategies(){
	return strategies;
}
