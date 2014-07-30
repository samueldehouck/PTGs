#include "PTGSolver.hpp"

#include <iostream>

using namespace std;

PTGSolver::PTGSolver(){
  size = 0;
}

void PTGSolver::solvePTG(PTG* p){
  ptg = p;
  size = p->getSize();
  createEndPoints();
}

void PTGSolver::createEndPoints(){
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

  for (list<unsigned int>::iterator it = endPoints.begin(); it != endPoints.end(); ++it)
    cout << *it << " ";
  cout << endl;
}
