#ifndef SPTGSOLVER_HPP
#define SPTGSOLVER_HPP

#include "SPTG.hpp"
#include <stddef.h>

class SPTGSolver{
private:
  SPTG* sptg;
  vector<int> strats;
  vector<vector<int> > vals;
  vector<vector<int> > lambdas;
  vector<int> ensStates;
  vector<vector<bool> > ensTransitions;
  float clock;
  
  void extendedDijkstra();
  void strategyIteration();
  void createLambdas();
  void actualizeLambdas();
  unsigned int findImpSwitchP1();
  unsigned int findImpSwitchP2();
  
public:
  SPTGSolver();
  void show();
  void solveSPTG(SPTG*);

};

#endif