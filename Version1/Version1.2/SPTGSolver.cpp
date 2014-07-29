#include "SPTGSolver.hpp"
#include "constants.hpp"
#include <iostream>

SPTGSolver::SPTGSolver(){
  sptg = NULL;
  clock = 1.0;
}

void SPTGSolver::show(){
  cout << "Vals:" << endl;
  for (unsigned int i = 0; i < vals.size(); ++i){
    for (unsigned int j = 0; j < vals[i].size(); ++j)
      cout << vals[i][j] << "	";
    cout << endl;
  }
  cout << "Strategies: " << endl;
   for (unsigned int i = 0; i < strats.size(); ++i)
    cout << strats[i] << "	";
  cout << endl; 
  
  cout << "Lambdas:" << endl;
  for (unsigned int i = 0; i < lambdas.size(); ++i){
    for (unsigned int j = 0; j < lambdas[i].size(); ++j)
      cout << lambdas[i][j] << "	";
    cout << endl;
  }
 /* cout << "Ensemble of states:" << endl;
  for (unsigned int i = 0; i < ensStates.size(); ++i)
    cout << ensStates[i] << "	";
  cout << endl;
  cout << "Ensemble of transitions:" << endl;
  for (unsigned int i = 0; i < ensTransitions.size(); ++i){
    for (unsigned int j =0; j < ensTransitions[i].size(); ++j)
      cout << ensTransitions[i][j] << "	";
    cout << endl;
  }*/
}

void SPTGSolver::solveSPTG(SPTG* s){
  sptg = s;
  extendedDijkstra();
  createLambdas();
  actualizeLambdas();
  strategyIteration();
  
  show();
  sptg = NULL;
}

void SPTGSolver::extendedDijkstra(){
  cout << endl << "====Extended Dijkstra====" << endl;
  int size = sptg->getSize();
//   Initialization;
    vals.push_back(vector<int>());
    vals[0].push_back(0);
    vals[0].push_back(0);
  // Fill the initial valors, strategies and the ensemble of states
  for (int i = 1; i < size; ++i){
    vals.push_back(vector<int>());
    vals[i].push_back(ifnty);
    vals[i].push_back(0);
    strats.push_back(-1);
    ensStates.push_back(i);
  }
  //Fill the matrix representing the ensemble of transitions
   for (int i = 0; i < size; ++i){
     ensTransitions.push_back(vector<bool>());
    for (int j =0; j < size; ++j){
      if(sptg->getTransition(i,j) != -1)
	ensTransitions[i].push_back(true);
      else
	ensTransitions[i].push_back(false);
    }
   }
  //Loop computing the strategies and valors  
  while(!ensStates.empty()){
    int state = 0;
    int transition = 0;
    int min = ifnty;
    for (unsigned int i = 0; i < ensStates.size(); ++i){
      int tempState = ensStates[i];
      for (unsigned int j = 0; j < ensTransitions.size(); ++j){ 
	if (ensTransitions[tempState][j] && sptg->getTransition(tempState,j) + vals[j][0] < min){
	  min = sptg->getTransition(tempState,j) + vals[j][0];
	  state = tempState;
	  transition = j;
	}    
      } 
    }
    int count = 0;
    for (unsigned int j = 0; j < ensTransitions.size(); ++j){
      if (ensTransitions[state][j])//If only one transition remains, we have to take it
	++count;
    }
    if(sptg->getOwner(state) || count == 1){
      vals[state][0] = min;
      strats[state - 1] = transition; //Because we don't have any strategy in the final state
      vector<int>::iterator it = ensStates.begin(); 
      //Finding the right state to erase
      while(*it != state)
	 ++it;
      ensStates.erase(it);     
    }
    else 
      //Erase the transition
      ensTransitions[state][transition] = 0;
  }
  //  Results
  show();
}

void SPTGSolver::strategyIteration(){
  
  cout << "====StrategyIteration====" << endl;
  unsigned int impSwtch = 1;
  //while (impSwtch != 0){
    impSwtch = 0;
    //update all costs
    impSwtch = findImpSwitchP1();
    cout << "switch1: " << impSwtch << endl;
    //if != 0 -> update
    //impSwtch = findImpSwitchP2();
    //cout << "switch2: " << impSwtch << endl;
  //}
}

void SPTGSolver::createLambdas(){
 //A lambda transition always goes to the final node. Moreover, it is a sum of two integers written on the two first rows
 //ex: 3 + 3
  
  for (unsigned int i = 0; i < sptg->getSize(); ++i){
    lambdas.push_back(vector<int>());
    lambdas[i].push_back(0);
    lambdas[i].push_back(0);
  }
}


void SPTGSolver::actualizeLambdas(){
  for (unsigned int i = 0; i < lambdas.size(); ++i){
   lambdas[i][0] = vals[i][0];
   lambdas[i][1] = vals[i][1] + sptg->getState(i);
  }
}

unsigned int SPTGSolver::findImpSwitchP1(){
     unsigned int impSwtch = 0;
     for (unsigned int i = 1; i < strats.size(); ++i){
      if(sptg->getOwner(i)){//Owned by P1
	cout << "state: " << i << endl;
	//We have to look at all costs that can be achieved using every transitions
	if(lambdas[i][0] < vals[i][0] || (lambdas[i][0] == vals[i][0] && lambdas[i][1] < vals[i][1])){
	  //We can take the lambda transition
	  cout << "lambda: " << lambdas[i][0] << " " << lambdas[i][1] << endl;
	  strats[i] = 0;
	  vals[i][0]= lambdas[i][0];
	  vals[i][1] = lambdas[i][1];
	  impSwtch = i + 1;
	}
	else{
	  //We have to take a look at the other transitions
	   for (unsigned int j = 0; j < sptg->getSize(); ++j){
	     if(sptg->getTransition(i,j) != -1 && (sptg->getTransition(i,j) + vals[j][0] < vals[i][0] ||
	       (sptg->getTransition(i,j) + vals[j][0] == vals[i][0] && vals[j][1] < vals[i][1]))){
	       	cout << "transition: " << sptg->getTransition(i,j) + vals[j][0] << " " << vals[j][1] << endl;
		strats[i] = j;
		vals[i][0]= sptg->getTransition(i,j) + vals[j][0];
		vals[i][1] = vals[j][1];
		impSwtch = i + 1;	     	       
	    }
	    }	     
	  }
	}
	
      }
    return impSwtch;
}

unsigned int SPTGSolver::findImpSwitchP2(){
     unsigned int impSwtch = 0;
     for (unsigned int i = 1; i < strats.size(); ++i){
      if(!sptg->getOwner(i)){//Owner by P2
	if(lambdas[i][0] > vals[i][0] || (lambdas[i][0] == vals[i][0] && lambdas[i][1] > vals[i][1])){//We can take the lambda transition
	  strats[i] = 0;
	  vals[i][0]= lambdas[i][0];
	  vals[i][1] = lambdas[i][1];
	  impSwtch = i + 1;	  
	}
	else{
	  //We have to take a look at the other transitions
	   for (unsigned int j = 0; j < sptg->getSize(); ++j){
	     if(sptg->getTransition(i,j) != -1 && (sptg->getTransition(i,j) + vals[j][0] > vals[i][0] ||
	       (sptg->getTransition(i,j) + vals[j][0] == vals[i][0] && vals[j][1] > vals[i][1]))){
		strats[i] = j;
		vals[i][0]= sptg->getTransition(i,j) + vals[j][0];
		vals[i][1] = vals[j][1];
		impSwtch = i + 1;
	    }
	    }		
	    
	}      
	
      }
    }
    return impSwtch;
}