#include "PTGSolver.hpp"

#include <iostream>

using namespace std;

PTGSolver::PTGSolver(){
  size = 0;
}

void PTGSolver::solvePTG(PTG* p){
  ptg = p;
  size = ptg->getSize();

  
  createEndPoints();
  time = endPoints.back();
  init();//Init is after createEndPoints because the time needs to be updated before creating the Strategy object

  keepTransAvailable(time, time);
  PGSolver pgSolver(ptg, &pathsLengths, &vals, &strategies);
  pgSolver.extendedDijkstra();
  show();
}

void PTGSolver::init(){
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

void PTGSolver::createEndPoints(){
//  TODO will need to improve
//   Create all endpoints where some calculation will be needed
  for (unsigned int i = 0; i < size; ++i){
    for (unsigned int j = 0; j < size; ++j){
      if(ptg->getTransition(i,j) != -1){
	endPoints.push_back(ptg->getStartCst(i,j));
	endPoints.push_back(ptg->getEndCst(i,j));
      }
    }
  }

  endPoints.sort();
  endPoints.unique();

}

void PTGSolver::keepTransAvailable(unsigned int start, unsigned int end){
  for (unsigned int i = 0; i < size; ++i){
   for (unsigned int j = 0; j < size; ++j){
    if(ptg->getTransition(i,j) != -1 && (ptg->getStartCst(i,j) > start || ptg->getEndCst(i,j) < end)){
      storage.push_back(Transition(i, j, ptg->getTransition(i,j)));
      ptg->setTransition(i,j,-1);
    }
   }
  }
}

void PTGSolver::show(){
  
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