#include "PGSolver.hpp"
#include <iostream>

using namespace std;

PGSolver::PGSolver(PG* p, vector<unsigned int>* pl, vector<vector<Fraction> >* v, list<Strategy>* s){
	pg = p;
	pathsLengths = pl;
	vals = v;
	strategies = s;
	size = vals->size();
	nbTransitions = 0;

	// We need to initialize all vectors
	ensStates.push_back(false);

	//The first row of the ensemble of transition needs to be initialized too (to false because we'll never leave the target)
	ensTransitions.push_back(vector<bool>());
	for (unsigned int j = 0; j < size; ++j){
		ensTransitions[0].push_back(false);
	}

	for (unsigned int i = 1; i < size; ++i){
		ensStates.push_back(true);
		ensTransitions.push_back(vector<bool>());
		for (unsigned int j = 0; j < size; ++j){
			if(pg->getTransition(i,j) != -1){
				ensTransitions[i].push_back(true);
				++nbTransitions;
			}
			else
				ensTransitions[i].push_back(false);
		}
	}
}

PGSolver::PGSolver(PG* p, vector<unsigned int>* pl, vector<vector<Fraction> >* v, list<Strategy>* s, vector<Fraction>* b){
	pg = p;
	pathsLengths = pl;
	vals = v;
	strategies = s;
	size = vals->size();
	nbTransitions = 0;
	bottoms = b;

	// We need to initialize all vectors
	ensStates.push_back(false);

	//The first row of the ensemble of transition needs to be initialized too (to false because we'll never leave the target)
	ensTransitions.push_back(vector<bool>());
	for (unsigned int j = 0; j < size; ++j){
		ensTransitions[0].push_back(false);
		ensBottoms.push_back(true);
	}

	for (unsigned int i = 1; i < size; ++i){
		ensStates.push_back(true);
		ensTransitions.push_back(vector<bool>());
		for (unsigned int j = 0; j < size; ++j){
			if(pg->getTransition(i,j) != -1){
				ensTransitions[i].push_back(true);
				++nbTransitions;
			}
			else
				ensTransitions[i].push_back(false);
		}
		if((*bottoms)[i] != -1)
			++nbTransitions;
	}
}

bool PGSolver::extendedDijkstra(bool withBottoms){
	//the parameter "withBottoms" is true when we have created a PG in solvePTG

	//TODO: maybe refactor it to improve the gestion of bottom transitions
	cout << "====Extended Dijkstra====" << endl;
	//Compute the values in a Priced Game
	unsigned int cnt = nbTransitions;


	while (remainsStates() && cnt > 0){
		Fraction min = ifnty + 1; //We need that an ifnty transition is still better than the min
		unsigned int finalState = 1;
		unsigned int finalTrans = 0;
		bool minIsBottom = false;
		//Look for the minimum
		for (unsigned int state = 1; state < size; ++state){
			if (ensStates[state]){
				//If the state is still in the ensemble
				//cout << "State " << state << endl;
				//Check the lambda transition if needed due to "withLambas"

				if(withBottoms && ensBottoms[state]){
					if( min >  (*bottoms)[state]){
						//cout << "minisBottom: " << (*bottoms)[state] << endl;
						finalState = state;
						finalTrans = 0;
						min = (*bottoms)[state];
						minIsBottom = true;
					}
				}


				for (unsigned int nextState = 0; nextState < size; ++nextState){
					if(ensTransitions[state][nextState]){
						if( min > ((*vals)[nextState][0] + pg->getTransition(state, nextState))){
							//cout << "New min to " << nextState << " with cost " << (*vals)[nextState][0] + pg->getTransition(state, nextState) << endl;
							finalState = state;
							finalTrans = nextState;
							min = (*vals)[nextState][0] + pg->getTransition(state, nextState);
							minIsBottom = false;
						}
					}
				}
			}
		}
		//Change the values

		if((pg->getOwner(finalState) || isLastTransition(finalState, finalTrans, minIsBottom, withBottoms))){
			//cout << "Change value of state " << finalState << " to ";
			if(minIsBottom){
				//cout << (*bottoms)[finalState] << endl;
				(*vals)[finalState][0] = (*bottoms)[finalState];
				(*pathsLengths)[finalState] = 1;
				strategies->front().insert(finalState, finalTrans, true);

			}
			else{
				(*vals)[finalState][0] = (*vals)[finalTrans][0] + pg->getTransition(finalState, finalTrans);
				//cout << (*vals)[finalState][0] << endl;

				(*vals)[finalState][0].upperSign();
				(*pathsLengths)[finalState] = (*pathsLengths)[finalTrans] + 1;
				strategies->front().insert(finalState, finalTrans, false);
			}
			ensStates[finalState] = false;
		}
		else{
			//cout << "Delete transition to " << finalTrans << " from state " << finalState << endl;
			if(minIsBottom)
				ensBottoms[finalState] = false;
			else
				ensTransitions[finalState][finalTrans] = false;
		}
		--cnt;
	}
	if(cnt == 0 && remainsStates()){
		for (unsigned int i = 1; i < size; ++i){
			if(ensStates[i]){
				(*vals)[i][0] = ifnty;
				if(withBottoms){
					strategies->front().insert(i, 0, true);
					(*pathsLengths)[i] = 1;

				}
				else{
					strategies->front().insert(i, i, false);

					(*pathsLengths)[i] = ifnty;
				}

			}

		}

		return false;
	}
	return true;
}

bool PGSolver::remainsStates(){
	//   Check if some states havn't been treated
	for (vector<bool>::iterator it = ensStates.begin(); it != ensStates.end(); ++it){
		if(*it)
			return true;
	}
	return false;
}

bool PGSolver::isLastTransition(unsigned int state, unsigned int nextState, bool isBottom, bool withBottoms){
	//Check if the transition going from state to nextState is the last one

	bool isLast = true;
	if(withBottoms && !isBottom && ensBottoms[state] != -1){
		return false;
	}
	else{
		for (unsigned int i = 0;isLast && i < ensTransitions[state].size(); ++i){
			if((i != nextState) && ensTransitions[state][i])
				isLast = false;
		}
	}
	return isLast;
}

