#include "PGSolver.hpp"
#include <iostream>

using namespace std;

PGSolver::PGSolver(PG* p, vector<Value>* pl, vector<CompositeValue>* v, list<Strategy>* s, vector<vector<CompositeValue> >* r){
	pg = p;
	pathsLengths = pl;
	vals = v;
	strategies = s;
	size = p->getSize();
	nbTransitions = 0;
	resets = r;
	bottoms = NULL;

	// We need to initialize all vectors
	ensStates.push_back(false);

	//The first row of the ensemble of transition needs to be initialized too (to false because we'll never leave the target)
	ensTransitions.push_back(vector<bool>());
	ensResets.push_back(vector<bool>());

	for (unsigned int j = 0; j < size; ++j){
		ensTransitions[0].push_back(false);
		ensResets[0].push_back(false);
	}

	for (unsigned int i = 1; i < size; ++i){
		ensStates.push_back(true);
		ensTransitions.push_back(vector<bool>());
		ensResets.push_back(vector<bool>());
		for (unsigned int j = 0; j < size; ++j){
			if(pg->getTransition(i,j) != -1){
				ensTransitions[i].push_back(true);
				++nbTransitions;

				if((*resets)[i][j] != -1){
					ensResets[i].push_back(true);
					//ensTransitions[i][j] = false;
					++nbTransitions;
				}
				else
					ensResets[i].push_back(false);
			}
			else{
				ensTransitions[i].push_back(false);
				ensResets[i].push_back(false);
			}
		}
	}

}

PGSolver::PGSolver(PG* p, vector<Value>* pl, vector<CompositeValue>* v, list<Strategy>* s, vector<Value>* b, vector<vector<CompositeValue> >* r){
	pg = p;
	pathsLengths = pl;
	vals = v;
	strategies = s;
	size = p->getSize();
	nbTransitions = 0;
	bottoms = b;
	resets = r;

	// We need to initialize all vectors
	ensStates.push_back(false);

	//The first row of the ensemble of transition needs to be initialized too (to false because we'll never leave the target)
	ensTransitions.push_back(vector<bool>());
	ensResets.push_back(vector<bool>());
	for (unsigned int j = 0; j < size; ++j){
		ensTransitions[0].push_back(false);
		ensBottoms.push_back(true);
		ensResets[0].push_back(false);
	}

	for (unsigned int i = 1; i < size; ++i){
		ensStates.push_back(true);
		ensTransitions.push_back(vector<bool>());
		ensResets.push_back(vector<bool>());
		for (unsigned int j = 0; j < size; ++j){
			if(pg->getTransition(i,j) != -1){
				ensTransitions[i].push_back(true);
				++nbTransitions;

				if((*resets)[i][j] != -1){
					ensResets[i].push_back(true);
					ensTransitions[i][j] = false;
					++nbTransitions;
				}
				else
					ensResets[i].push_back(false);

			}
			else{
				ensTransitions[i].push_back(false);
				ensResets[i].push_back(false);
			}
		}
		if((*bottoms)[i] != -1)
			++nbTransitions;
	}
}

void PGSolver::extendedDijkstra(bool solvePTG){
	//the parameter "solvePTG" is true when we have created a PG in solvePTG


	cout << "====Extended Dijkstra====" << endl;
	//Compute the values in a Priced Game
	unsigned int cnt = nbTransitions;
	while (remainsStates() && cnt > 0){
		Value min;
		min.setInf(true);
		unsigned int finalState = 1;
		unsigned int finalTrans = 0;
		bool minIsBottom = false;
		bool minIsReset = false;
		//Look for the minimum
		for (unsigned int state = 1; state < size; ++state){
			if (ensStates[state]){
				//If the state is still in the ensemble

				//cout << "State " << state << endl;
				//Check the lambda transition if needed due to "withLambas"

				if(solvePTG && ensBottoms[state]){
				//	cout << min << " " << ((*bottoms)[state]) << " " << min.isInfinity() << endl;
					if( min >= (*bottoms)[state] || min.isInfinity()){
					//	cout << "minisBottom: " << (*bottoms)[state] << endl;
						finalState = state;
						finalTrans = 0;
						min = (*bottoms)[state];
						minIsBottom = true;
						minIsReset = false;
					}


				}
				for (unsigned int j = 0; resets != NULL && j < size; ++j){
					if(ensResets[state][j] && min >= (*resets)[state][j] ){
					//	cout << "minisReset: " << (*resets)[state][j] << endl;
						finalState = state;
						finalTrans = j;
						min = (*resets)[state][j];
						minIsBottom = false;
						minIsReset = true;
					}

				}


				for (unsigned int nextState = 0; nextState < size; ++nextState){
					if(ensTransitions[state][nextState]){
					//	cout << state << " to " << nextState << endl;
						//cout << "min: " << min << endl;
						//cout << "new val:" << (*vals)[nextState] + pg->getTransition(state, nextState) << endl;
						if( ((*resets)[state][nextState] == -1 && min > ((*vals)[nextState] + pg->getTransition(state, nextState))) || (!minIsBottom && ! minIsReset && min.isInfinity())){
						//	cout << "New min to " << nextState << " with cost " << (*vals)[nextState] + pg->getTransition(state, nextState) << endl;
							finalState = state;
							finalTrans = nextState;
							min = (*vals)[nextState] + pg->getTransition(state, nextState);
							minIsBottom = false;
							minIsReset = false;
						}
					}
				}
			}
		}
		//Change the values
		if((ensTransitions[finalState][finalTrans] || minIsBottom || minIsReset) && ensStates[finalState] && (pg->getOwner(finalState) || isLastTransition(finalState, finalTrans, minIsBottom, minIsReset, solvePTG))){
			//cout << "Change value of state " << finalState << " to ";
			if(minIsBottom){
				//cout << (*bottoms)[finalState] << endl;
				(*vals)[finalState] = (*bottoms)[finalState];
				(*pathsLengths)[finalState] = 1;
				list<Strategy>::iterator it = strategies->begin();
				++it;
				strategies->front().insert(finalState, it->getDest(finalState), 2);

			}
			else if(minIsReset){
				(*vals)[finalState] = (*resets)[finalState][finalTrans];
			//	cout << (*resets)[finalState][finalTrans] << endl;
				(*pathsLengths)[finalState] = 1;
				strategies->front().insert(finalState, finalTrans, 3);
			}
			else{
				if((*vals)[finalTrans].isInfinity() || pg->getTransition(finalState, finalTrans).isInfinity())
					(*vals)[finalState].setInf(true);
				else
					(*vals)[finalState] = (*vals)[finalTrans] + pg->getTransition(finalState, finalTrans);
				//cout << (*vals)[finalState] << endl;

				(*vals)[finalState].getVal().upperSign();
				(*pathsLengths)[finalState] = (*pathsLengths)[finalTrans] + 1;
				strategies->front().insert(finalState, finalTrans, 0);
			}
			ensStates[finalState] = false;
		}
		else if((ensTransitions[finalState][finalTrans] || minIsBottom || minIsReset) && ensStates[finalState]){
			//cout << "Delete transition to " << finalTrans << " from state " << finalState << " " << ensTransitions[finalState][finalTrans] << minIsBottom << minIsReset << endl;
			if(minIsBottom)
				ensBottoms[finalState] = false;
			else if(minIsReset)
				ensResets[finalState][finalTrans] = false;
			else
				ensTransitions[finalState][finalTrans] = false;
		}

		--cnt;
	}
	if(cnt == 0 && remainsStates()){
		for (unsigned int i = 1; i < size; ++i){
			if(ensStates[i]){
				(*vals)[i].setInf(true);
				if(solvePTG){
					strategies->front().insert(i, 0, 1);
					(*pathsLengths)[i] = 1;

				}
				else{
					strategies->front().insert(i, i, 0);

					(*pathsLengths)[i].setInf(true);
				}

			}

		}
	}
}

bool PGSolver::remainsStates(){
	//   Check if some states havn't been treated
	for (vector<bool>::iterator it = ensStates.begin(); it != ensStates.end(); ++it){
		if(*it)
			return true;
	}
	return false;
}

bool PGSolver::isLastTransition(unsigned int state, unsigned int nextState, bool isBottom, bool isReset, bool solvePTG){
	//Check if the transition going from state to nextState is the last one
	//cout << "is? " << endl;
	bool isLast = true;

	//cout << isBottom << " " << ensBottoms[state] << endl;
	if(solvePTG && !isBottom && ensBottoms[state] != false){
		return false;
	}

	//cout << " " << isReset << " " << ensResets[state][nextState] << endl;

	for(unsigned int i = 0; solvePTG && resets != NULL && isLast && !isReset && i < ensResets[state].size(); ++i)
		if(ensResets[state][i])
			isLast = false;
	//cout << ensResets[3][2] << endl;
	for (unsigned int i = 0; isLast && i < ensTransitions[state].size(); ++i){
		if((i != nextState || (i == 0 && (isBottom || isReset))) && ensTransitions[state][i])
			isLast = false;

	}
	//cout << "IsLast: " << isLast <<isBottom <<isReset << endl;
	return isLast;
}

