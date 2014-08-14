#include "SPTGSolver.hpp"
#include "constants.hpp"
#include <iostream>
#include <queue>

SPTGSolver::SPTGSolver(SPTG* s){
	sptg = s;
	solvePTG = false;
	time = Fraction(1);
	init();
	//show();
}

SPTGSolver::SPTGSolver(SPTG* s, vector<Fraction>* b,  vector<unsigned int>* pl, vector<vector<Fraction> >* v, list<Strategy>* st, vector<list<Point> >* vF, vector<vector<Fraction> >* r){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
	strategies = st;
	valueFcts = vF;
	resets = r;
	time = Fraction(1);
	//show();

	//Initialization of the other vectors and lists
	size = sptg->getSize();
	if(size != 0){
		lambdas.push_back(vector<Fraction>());
		lambdas[0].push_back(Fraction(0));
		lambdas[0].push_back(Fraction(0));

		for (unsigned int i = 1; i < size; ++i){
			lambdas.push_back(vector<Fraction>());
			lambdas[i].push_back(0);
			lambdas[i].push_back(0);

		}
	}
}


SPTGSolver::~SPTGSolver(){
	if(!solvePTG)
		delete valueFcts;
}

void SPTGSolver::show(){

	cout << "Lambdas:" << endl;
	for (unsigned int i = 0; i < lambdas.size(); ++i){
		for (unsigned int j = 0; j < lambdas[i].size(); ++j){
			;
			cout <<  lambdas[i][j] << "	";
		}
		cout << endl;
	}

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
	for (unsigned int i = 0; i < vals->size(); ++i){
		for (unsigned int j = 0; j < (*vals)[i].size(); ++j){
			cout << (*vals)[i][j] << "	";
		}
		cout << endl;
	}

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
		vals = new vector<vector<Fraction> >();
		vals->push_back(vector<Fraction>());
		(*vals)[0].push_back(Fraction(0));
		(*vals)[0].push_back(Fraction(0));

		strategies = new list<Strategy> ();
		(*strategies).push_front(Strategy(size, time, false));
		(*strategies).front().insert(0,0,0);

		pathsLengths = new vector<unsigned int>();
		pathsLengths->push_back(0);

		lambdas.push_back(vector<Fraction>());
		lambdas[0].push_back(Fraction(0));
		lambdas[0].push_back(Fraction(0));

		valueFcts = new vector<list<Point> >();
		valueFcts->push_back(list<Point>());

		// Fill the initial valors, strategies and the ensemble of states
		for (unsigned int i = 1; i < size; ++i){
			vals->push_back(vector<Fraction>());
			(*vals)[i].push_back(Fraction(ifnty));
			(*vals)[i].push_back(Fraction(0));

			strategies->front().insert(i, -1, 0);
			pathsLengths->push_back(0);

			lambdas.push_back(vector<Fraction>());
			lambdas[i].push_back(0);
			lambdas[i].push_back(0);

			valueFcts->push_back(list<Point>());
		}
	}
}

void SPTGSolver::solveSPTG(){
	cout << "====SolveSPTG====" << endl;
	// This function is the core of the solver, it computes the strategies and values for the SPTG passed
	PGSolver* ps;
	bool notCycling = false;

	if(solvePTG){
		cout << "ici" << endl;
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, bottoms, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		notCycling = ps->extendedDijkstra(true);
		cout << "ici" << endl;
	}
	else{
		ps = new PGSolver(sptg, pathsLengths, vals, strategies, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		notCycling = ps->extendedDijkstra(false); //If extendedDijkstra returns false, some states can't be treated and there is a cycle
	}
	//sptg->show();
	/*for (unsigned int i = 0; i < size; ++i){
		cout << (*bottoms)[i] << " ";
	}
	cout << endl;

	show();*/
	cout << "ici" << endl;
	while (notCycling && time > 0){
		strategies->push_front(Strategy(size));
		actualizeLambdas();
		if(time.num == 1 && time.den == 1)//If it's the first turn, we have to "initialize" the value functions
			buildValueFcts(0);
		strategyIteration();
		Fraction epsilon = nextEventPoint();
		//cout << "NextEventPoint: " << epsilon << endl;
		if((time - epsilon) < 0)
			epsilon = time;
		buildValueFcts(epsilon);
		actualizeVals(epsilon);
		strategies->front().setTime(time);
		//show();
	}
	/*if(notCycling){
		cout << "====Result==== " << endl;
		show();
	}
	else
		cout << "There is a cycle!" << endl;*/
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
		lambdas[i][0] = (*vals)[i][0];
		lambdas[i][1] = (*vals)[i][1] + sptg->getState(i);


		//We need to force P1's states to take the lambda transition before changing.
		if(sptg->getOwner(i)){
			(*vals)[i][0] = lambdas[i][0];
			(*vals)[i][1] = lambdas[i][1];
			strategies->front().insert(i,0,1);
			(*pathsLengths)[i] = 1;
		}
	}
}

void SPTGSolver::actualizeVals(Fraction epsilon){
	for (unsigned int i = 1; i < vals->size(); ++i){
		(*vals)[i][0] = (*vals)[i][0] + (*vals)[i][1]*epsilon;
		if((*vals)[i][0] > ifnty)
			(*vals)[i][0] = ifnty;
		(*vals)[i][1] = Fraction(0);
	}
}

bool SPTGSolver::makeImpSwitchesP1(){
	cout << "Imp1" << endl;
	bool allDone = false;
	bool changed = false;
	while (!allDone){
		//For all states
		allDone = true;
		for (unsigned int state = 1; state < size; ++state){
			//Owned by P1 because we are checking the improving switches for the P1
			if(sptg->getOwner(state)){
				//We don't need to look at the bottom transitions for the player 1 because it goes to MAX and will never be taken
				/*if(solvePTG && ((*bottoms)[state] < (*vals)[state][0])){//If we have an improvement, we update the values found
					(*vals)[state][0] = (*bottoms)[state];
					(*vals)[state][1] = 0;
					strategies->front().insert(state, 0, 2);
					allDone = false;
					changed = true;
				}*/

				for (unsigned int nextState = 0; nextState < size; ++nextState){
					//We need to check all transitions for every states except the lambda transition because it is taken by default
					if(sptg->getTransition(state, nextState) != -1){//If the transition exists
						Fraction tempVal = (*vals)[nextState][0] + sptg->getTransition(state, nextState);
						if(tempVal > ifnty)
							tempVal = ifnty;
						unsigned int tempLength = (*pathsLengths)[nextState] + 1;
						if((tempVal < (*vals)[state][0])//If we have an improvement, we update the values found
								|| ((tempVal == (*vals)[state][0]) && ((*vals)[nextState][1] < (*vals)[state][1]))
								||((tempVal == (*vals)[state][0]) && ((*vals)[nextState][1] == (*vals)[state][1]) && (tempLength < (*pathsLengths)[state]))){
							(*vals)[state][0] = tempVal;
							(*vals)[state][1] = (*vals)[nextState][1];
							(*pathsLengths)[state] = tempLength;
							strategies->front().insert(state, nextState, 0);
							allDone = false;
							changed = true;
						}
					}
				}
			}
			if(changed)//If a changed has been made, it can have an impact on the values of other states
				propagate(state);
		}

	}

	return changed;
}

bool SPTGSolver::makeImpSwitchesP2(){
	bool allDone = false;
	bool changed = false;
	while (!allDone){
		//For all states
		allDone = true;
		for (unsigned int state = 1; state < size; ++state){
			//Owned by P2 because we are checking the improving switches for the P2
			if(!sptg->getOwner(state)){
				cout << "State: " << state << endl;
				//Check the lambda transition
				//We don't need to take a look at the bottom transitions because the lambda transitions will always be better for P2
				/*if(solvePTG && ((*bottoms)[state] > (*vals)[state][0])){//If we have an improvement, we update the values found
					(*vals)[state][0] = (*bottoms)[state];
					(*vals)[state][1] = 0;
					strategies->front().insert(state, 0, 2);
					allDone = false;
					changed = true;
				}*/
				if((lambdas[state][0] > (*vals)[state][0]) ||
						((lambdas[state][0] == (*vals)[state][0]) && (lambdas[state][1] > (*vals)[state][1]))){
					cout << "lambda is better" << endl;
					//If we have an improvement, we update the values found
					(*vals)[state][0] = lambdas[state][0];
					(*vals)[state][1] = lambdas[state][1];
					strategies->front().insert(state, 0, 1);
					(*pathsLengths)[state] = 1;
					allDone = false;
					changed = true;
				}
				for (unsigned int nextState = 0; nextState < size; ++nextState){
					if(sptg->getTransition(state, nextState) != -1){//If the transition exists
						Fraction tempVal = (*vals)[nextState][0] + sptg->getTransition(state, nextState);
						if(tempVal > ifnty)
							tempVal = ifnty;
						unsigned int tempLength = (*pathsLengths)[nextState] + 1;
						cout << state << ": " << (*vals)[state][0] << " " << (*vals)[state][1] << endl;
						cout << "to: " << nextState << ": " << tempVal <<  " " << (*vals)[nextState][1] << endl;
						if((tempVal > (*vals)[state][0]) || ((tempVal == (*vals)[state][0]) && ((*vals)[nextState][1] > (*vals)[state][1]))
								||((tempVal == (*vals)[state][0]) && ((*vals)[nextState][1] == (*vals)[state][1]) && (tempLength > (*pathsLengths)[state]))){
							cout << "to " << nextState << " is better" << endl;
							//If we have an improvement, we update the values found
							(*vals)[state][0] = tempVal;
							(*vals)[state][1] = (*vals)[nextState][1];
							(*pathsLengths)[state] = tempLength;
							strategies->front().insert(state, nextState, 0);
							allDone = false;
							changed = true;
						}
					}
				}
			}
			if(changed)//If a changed has been made, it can have an impact on the values of other states
				propagate(state);
		}
	}

	return changed;
}

void SPTGSolver::propagate(unsigned int state){
	//Use a queue to update the value of all state that can reach the one given
	queue<unsigned int> q;
	vector<bool> checked;
	for (unsigned int i = 0; i < size; ++i)
		checked.push_back(false);
	q.push(state);
	checked[state] = true;
	while (!q.empty() ){
		unsigned int tmpState = q.front();
		for(unsigned int i = 1; i < size; ++i){
			if(strategies->front().getDest(i) == tmpState){
				(*vals)[i][0] = (*vals)[tmpState][0] + sptg->getTransition(i,tmpState);
				if((*vals)[i][0] > ifnty)
					(*vals)[i][0] = ifnty;
				(*vals)[i][1] = (*vals)[tmpState][1];
				(*pathsLengths)[i] = (*pathsLengths)[tmpState] + 1;
				if(!checked[i]){
					q.push(i);
					checked[i] = true;
				}
			}
		}
		q.pop();
	}
}

void SPTGSolver::addPoint(unsigned int index, Fraction x, Fraction y){
	(*valueFcts)[index].push_front(Point(x,y));
}

void SPTGSolver::buildValueFcts(Fraction epsilon){
	//   Add a point for every state
	time = time - epsilon;
	for (unsigned int i = 1; i < size; ++i){
		Fraction tmpVal = (*vals)[i][0] + ((*vals)[i][1]*epsilon);
		if(tmpVal > ifnty)
			tmpVal = ifnty;
		list<Point>::iterator it = (*valueFcts)[i].begin();
		++it;
		if(it != (*valueFcts)[i].end() && !solvePTG){
			Fraction coef = (it->getY() - tmpVal)/(it->getX() - time);
			//Doesn't work when resolving PTGs because the rescaling hasn't been made yet
			if(tmpVal + (coef * ((*valueFcts)[i].front().getX() - time)) == (*valueFcts)[i].front().getY()){
				(*valueFcts)[i].pop_front();
			}
		}
		addPoint(i, time , tmpVal);
	}
}

Fraction SPTGSolver::nextEventPoint(){
	cout << "====NextEventPoint===" << endl;
	Fraction min(ifnty);

	for (unsigned int state = 1; state < size; ++state){
		//     First we have to check if an epsilon can be found using the lambda transition
		Fraction tempMin(ifnty);
		if((*vals)[state][0] != lambdas[state][0] && lambdas[state][1] != (*vals)[state][1]){
			tempMin = ((*vals)[state][0] - lambdas[state][0])/(lambdas[state][1] - (*vals)[state][1]);
			if(tempMin < 0)
				tempMin = ifnty;
			if( time - tempMin < min)
				min = tempMin;
		}

		for (unsigned int nextState = 0; nextState < size; ++nextState){
			//       Then we need to check if there is another epsilon that can be found using the other transitions
			tempMin = Fraction(ifnty);
			if(sptg->getTransition(state, nextState) != -1 && ((*vals)[state][0] != (*vals)[nextState][0] + sptg->getTransition(state, nextState)) &&
					((*vals)[nextState][1] != (*vals)[state][1]))
			{

				tempMin = ((*vals)[state][0] - ((*vals)[nextState][0]+sptg->getTransition(state, nextState)))/((*vals)[nextState][1] - (*vals)[state][1]);

				if(tempMin < 0)
					tempMin = ifnty;

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
