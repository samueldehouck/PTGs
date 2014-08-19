#include "SPTGSolver.hpp"
#include "constants.hpp"
#include <iostream>
#include <queue>

SPTGSolver::SPTGSolver(){
  sptg = NULL;
  time = Fraction(1);
}


SPTGSolver::~SPTGSolver(){
 //TODO Change when variables will be put as pointers 
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
  for (list<Strategy>::iterator it = strategies.begin(); it != strategies.end(); ++it){
    it->show();
  }
  cout << endl;
  
  cout << "Lengths: " << endl;
   for (unsigned int i = 0; i < pathsLengths.size(); ++i)
    cout << pathsLengths[i] << "	";
  cout << endl; 
  
  cout << "Vals:" << endl;
  for (unsigned int i = 0; i < vals.size(); ++i){
    for (unsigned int j = 0; j < vals[i].size(); ++j){
      cout << vals[i][j] << "	";
    }
    cout << endl;
  } 
    
  cout << "Value Functions" << endl;
  for (unsigned int i = 1; i < valueFcts.size(); ++i){
    cout << " State " << i <<": ";
    for(list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end(); ++it){
      cout << "(" << it->getX() << "," << it->getY() << ")	";
    }
    cout << endl;
  }
}

void SPTGSolver::init(){
    size = sptg->getSize();
//   Initialization;
    vals.push_back(vector<Fraction>());
    vals[0].push_back(Fraction(0));
    vals[0].push_back(Fraction(0));
    
    strategies.push_front(Strategy(size, time));
    strategies.front().insert(0,0,false);
    
    pathsLengths.push_back(0);
    
    lambdas.push_back(vector<Fraction>());
    lambdas[0].push_back(Fraction(0));
    lambdas[0].push_back(Fraction(0));  
    
    valueFcts.push_back(list<Point>());
    
  // Fill the initial valors, strategies and the ensemble of states
  for (unsigned int i = 1; i < size; ++i){
    vals.push_back(vector<Fraction>());
    vals[i].push_back(Fraction(ifnty));
    vals[i].push_back(Fraction(0));
    
    strategies.front().insert(i, -1, false);
    pathsLengths.push_back(0);
    
    lambdas.push_back(vector<Fraction>());
    lambdas[i].push_back(0);
    lambdas[i].push_back(0);
    
    valueFcts.push_back(list<Point>());
  } 
}

void SPTGSolver::solveSPTG(SPTG* s){
  sptg = s;
  init();
  show();
  PGSolver ps(sptg, &pathsLengths, &vals, &strategies);//PGSolver will consider sptg as a pg thanks to inheritance
  bool notCycling = ps.extendedDijkstra();
  show();
  while (notCycling && time > 0){
   strategies.push_front(Strategy(size));
   actualizeLambdas();
   if(time.num == 1 && time.den == 1)
        buildValueFcts(0);
   strategyIteration();
   Fraction epsilon = nextEventPoint();
   cout << "NextEventPoint: "; epsilon.show(); cout << endl;
   if((time - epsilon) < 0)
     epsilon = time;
   buildValueFcts(epsilon);
   actualizeVals(epsilon);
   strategies.front().setTime(time);
   show();
  }
  if(notCycling){
    cout << "====Result==== " << endl;
    show();
  }
  else
    cout << "There is a cycle!" << endl;
  sptg = NULL;
}

void SPTGSolver::strategyIteration(){
  
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
   lambdas[i][0] = vals[i][0];
   lambdas[i][1] = vals[i][1] + sptg->getState(i);
   
   
   //We need to force P1's states to take the lambda transition before changing.
   if(sptg->getOwner(i)){
   	  vals[i][0] = lambdas[i][0];
	  vals[i][1] = lambdas[i][1];
	  strategies.front().insert(i,0,true);
	  pathsLengths[i] = 1;   
      }
   }
}

void SPTGSolver::actualizeVals(Fraction epsilon){
  for (unsigned int i = 1; i < vals.size(); ++i){
    vals[i][0] = vals[i][0] + vals[i][1]*epsilon;
    vals[i][1] = Fraction(0);  
  }
}

bool SPTGSolver::makeImpSwitchesP1(){
//   cout << "==== SwitchP1====" << endl;
  bool allDone = false;
  bool changed = false;
  
  while (!allDone){
    //For all states
    allDone = true;
    for (unsigned int state = 1; state < size; ++state){
      //Owned by P1
      if(sptg->getOwner(state)){
// 	cout << "State: " << state << endl;
	
	for (unsigned int nextState = 0; nextState < size; ++nextState){
//   	  cout << "NextState: " << nextState << endl;
	  if(sptg->getTransition(state, nextState) != -1){
	    Fraction tempVal = vals[nextState][0] + sptg->getTransition(state, nextState);
	    unsigned int tempLength = pathsLengths[nextState] + 1;
	    if((tempVal < vals[state][0])
	      || ((tempVal == vals[state][0]) && (vals[nextState][1] < vals[state][1]))
	      ||((tempVal == vals[state][0]) && (vals[nextState][1] == vals[state][1]) && (tempLength < pathsLengths[state]))){
//  	      cout << "Changed state: " << state << " to: " << nextState << ": " << tempVal << " " << vals[nextState][1]<< endl;
	      vals[state][0] = tempVal;
	      vals[state][1] = vals[nextState][1];
	      pathsLengths[state] = tempLength;
	      strategies.front().insert(state, nextState, false);
	      allDone = false;
	      changed = true;
	    }
	  }
	}
      } 
      if(changed)
	propagate(state);
    }
    
  }
  
  return changed;
}

bool SPTGSolver::makeImpSwitchesP2(){
//     cout << "==== SwitchP2====" << endl;

  bool allDone = false;
  bool changed = false;
  
  while (!allDone){
    //For all states
    allDone = true;
    for (unsigned int state = 1; state < size; ++state){
      //Owned by P2
      if(!sptg->getOwner(state)){
	//Check the lambda transition
// 	cout << "State: " << state << endl;
	
 	if((lambdas[state][0] > vals[state][0]) || ((lambdas[state][0] == vals[state][0]) && (lambdas[state][1] > vals[state][1])) ){
//  	  cout << "Changed state: " << state << " to lambda: " << lambdas[state][0] << " " << lambdas[state][1] << endl;
	  vals[state][0] = lambdas[state][0];
	  vals[state][1] = lambdas[state][1];
	  strategies.front().insert(state, 0, true);
	  pathsLengths[state] = 1;
	  allDone = false;
	  changed = true;
 	}
	for (unsigned int nextState = 0; nextState < size; ++nextState){
//  	  cout << "NextState: " << nextState << endl;
	  if(sptg->getTransition(state, nextState) != -1){
	    Fraction tempVal = vals[nextState][0] + sptg->getTransition(state, nextState);
	    unsigned int tempLength = pathsLengths[nextState] + 1;
	    if((tempVal > vals[state][0]) || ((tempVal == vals[state][0]) && (vals[nextState][1] > vals[state][1]))
	      ||((tempVal == vals[state][0]) && (vals[nextState][1] == vals[state][1]) && (tempLength > pathsLengths[state]))){
//  	      cout << "Changed state: " << state << " to: " << nextState << ": " << tempVal << " " << vals[nextState][1]<< endl;
	      vals[state][0] = tempVal;
	      vals[state][1] = vals[nextState][1];
	      pathsLengths[state] = tempLength;
	      strategies.front().insert(state, nextState, false);
	      allDone = false;
	      changed = true;
	    }
	  }
	}
      }
     if(changed)
      propagate(state);
    }
  }
  
  return changed;
}

void SPTGSolver::propagate(unsigned int state){
  queue<unsigned int> q;
  vector<bool> checked;
  for (unsigned int i = 0; i < size; ++i)
    checked.push_back(false);
  q.push(state);
  checked[state] = true;
  while (!q.empty() ){
      unsigned int tmpState = q.front();
      for(unsigned int i = 1; i < size; ++i){
	if(strategies.front().getDest(i) == tmpState){
	  vals[i][0] = vals[tmpState][0] + sptg->getTransition(i,tmpState);
	  vals[i][1] = vals[tmpState][1];
	  pathsLengths[i] = pathsLengths[tmpState] + 1;
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
   valueFcts[index].push_front(Point(x,y));
}

void SPTGSolver::buildValueFcts(Fraction epsilon){
//   Add a point for every state
    time = time - epsilon;
  for (unsigned int i = 1; i < size; ++i){
    addPoint(i, time , vals[i][0] + (vals[i][1]*epsilon));   
  }
}

Fraction SPTGSolver::nextEventPoint(){
   cout << "====NextEventPoint===" << endl;
  Fraction min(ifnty);
  
  for (unsigned int state = 1; state < size; ++state){
//     First we have to check if an epsilon can be found using the lambda transition
    Fraction tempMin(ifnty);
    if(vals[state][0] != lambdas[state][0] && lambdas[state][1] != vals[state][1]){
      tempMin = (vals[state][0] - lambdas[state][0])/(lambdas[state][1] - vals[state][1]);
      if(tempMin < 0)
	tempMin = ifnty;
        if( time - tempMin < min)
	  min = tempMin;  
    }

    for (unsigned int nextState = 0; nextState < size; ++nextState){
//       Then we need to check if there is another epsilon that can be found using the other transitions
      tempMin = Fraction(ifnty);
      if(sptg->getTransition(state, nextState) != -1 && (vals[state][0] != vals[nextState][0] + sptg->getTransition(state, nextState)) &&
		(vals[nextState][1] != vals[state][1]))
      {
	
	tempMin = (vals[state][0] - (vals[nextState][0]+sptg->getTransition(state, nextState)))/(vals[nextState][1] - vals[state][1]);
      
       if(tempMin < 0)
	tempMin = ifnty;
      
	if(tempMin < min)
	    min = tempMin;
      }
      
    }
    
  }
  return min;
}

list<Strategy> SPTGSolver::getStrategies(){
  return strategies;
}