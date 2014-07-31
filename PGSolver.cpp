#include "PGSolver.hpp"
#include <iostream>

using namespace std;

PGSolver::PGSolver(PG* p, vector<unsigned int>* pl, vector<vector<Fraction> >* v, list<Strategy>* s){
 pg = p;
 pathsLengths = pl; 
 vals = v;
 strategies = s;
 size = vals->size();
 
 // We need to initialize all vectors
  ensStates.push_back(false);
  ensTransitions.push_back(vector<bool>());
  for (unsigned int j = 0; j < size; ++j){
    if(pg->getTransition(0,j) != -1)
      ensTransitions[0].push_back(true);
    else
      ensTransitions[0].push_back(false);
    }
    
   for (unsigned int i = 1; i < size; ++i){
   
    ensStates.push_back(true);  
    ensTransitions.push_back(vector<bool>());
    for (unsigned int j = 0; j < size; ++j){
      if(pg->getTransition(i,j) != -1)
	ensTransitions[i].push_back(true);
      else
	ensTransitions[i].push_back(false);
    }
  }
}

bool PGSolver::extendedDijkstra(){
   cout << endl << "====Extended Dijkstra====" << endl;
   //Compute the values in a Priced Game
   unsigned int cnt = size * size;
     
 
   while (remainsStates() && cnt > 0){
     Fraction min = ifnty;
     unsigned int finalState = 0;
     unsigned int finalTrans = 0;
     //Look for the minimum
     for (unsigned int state = 1; state < size; ++state){   
	if (ensStates[state]){
	  //If the state is still in the ensemble
	  for (unsigned int nextState = 0; nextState < size; ++nextState){
	      if(ensTransitions[state][nextState]){
		if( min > ((*vals)[nextState][0] + pg->getTransition(state, nextState))){
		  finalState = state;
		  finalTrans = nextState;
		  min = (*vals)[nextState][0] + pg->getTransition(state, nextState);
		}
	      }
	  }
	}
     }
     //Change the values
     if(pg->getOwner(finalState) || isLastTransition(finalState, finalTrans)){
	(*vals)[finalState][0] = (*vals)[finalTrans][0] + pg->getTransition(finalState, finalTrans);
	(*pathsLengths)[finalState] = (*pathsLengths)[finalTrans] + 1;
	strategies->front().insert(finalState, finalTrans, false);
	ensStates[finalState] = false;
    }
    else{
       ensTransitions[finalState][finalTrans] = false;
    }
    --cnt;
   }
   if(cnt == 0 && remainsStates())
     return false;
   return true;
}

bool PGSolver::remainsStates(){
//   Check if some state hasn't been treated
  for (vector<bool>::iterator it = ensStates.begin(); it != ensStates.end(); ++it){
    if(*it)
      return true;
  }
  return false;
}

bool PGSolver::isLastTransition(unsigned int state, unsigned int transition){
  bool isLast = true;
  for (unsigned int i;isLast && i < ensTransitions[state].size(); ++i){
      if(i != transition && ensTransitions[state][i])
	isLast = false;
  }
  return isLast;
}