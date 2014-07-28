#include "SPTGSolver.hpp"
#include "constants.hpp"
#include <iostream>

SPTGSolver::SPTGSolver(){
  sptg = NULL;
}

void SPTGSolver::showDijkstra(){
  int size = sptg->getSize() -1;
  cout << "Vals:" << endl;
  for (int i = 0; i < vals.size(); ++i)
    cout << vals[i] << "	";
  cout << endl;
  cout << "Strategies: " << endl;
   for (int i = 0; i < strats.size(); ++i)
    cout << strats[i] << "	";
  cout << endl; 
  cout << "Ensemble of states:" << endl;
  for (int i = 0; i < ensStates.size(); ++i)
    cout << ensStates[i] << "	";
  cout << endl;
  cout << "Ensemble of transitions:" << endl;
  for (int i = 0; i < ensTransitions.size(); ++i){
    for (int j =0; j < ensTransitions[i].size(); ++j)
      cout << ensTransitions[i][j] << "	";
    cout << endl;
  }
}

void SPTGSolver::solveSPTG(SPTG* s){
  sptg = s;
  extendedDijkstra();
  
  
  sptg = NULL;
}

void SPTGSolver::extendedDijkstra(){
  cout << endl << "====Extended Dijkstra====" << endl;
  int size = sptg->getSize();
//   Initialization
  vals.push_back(0);
  
  for (int i = 1; i < size; ++i){
    vals.push_back(ifnty);
    strats.push_back(-1);
    ensStates.push_back(i);
  }
   for (int i = 0; i < size; ++i){
     ensTransitions.push_back(vector<bool>());
    for (int j =0; j < size; ++j){
      if(sptg->getTransition(i,j) != -1)
	ensTransitions[i].push_back(true);
      else
	ensTransitions[i].push_back(false);
    }
   }
  cout << " " << ensTransitions.size() << endl; 
    
 while(!ensStates.empty()){
 int state = 0;
 int transition = 0;
 int min = ifnty;
 for (int i = 0; i < ensStates.size(); ++i){
   int tempState = ensStates[i];
  for (int j = 0; j < ensTransitions.size(); ++j){
    
   if (ensTransitions[tempState][j] && sptg->getTransition(tempState,j) + vals[j] < min){
    min = sptg->getTransition(tempState,j) + vals[j];
    state = tempState;
    transition = j;
      }    
    } 
  }
  int count = 0;
  for (int j = 0; j < ensTransitions.size(); ++j){
    if (ensTransitions[state][j])
      ++count;
  }
  if(sptg->getOwner(state) || count == 1){
    vals[state] = min;
    strats[state - 1] = transition; //Because we don't have any strategy in the final state
    vector<int>::iterator it = ensStates.begin(); 
    while(*it != state)
      ++it;
    ensStates.erase(it);     
  }
  else {
    ensTransitions[state][transition] = 0;
  }
  //  Results
  showDijkstra();
}


   
   
   

}