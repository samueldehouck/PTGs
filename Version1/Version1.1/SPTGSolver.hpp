#ifndef SPTGSOLVER_HPP
#define SPTGSOLVER_HPP

#include "SPTG.hpp"
#include <stddef.h>

class SPTGSolver{
private:
  SPTG* sptg;
  vector<int> strats;
  vector<int> vals;
  vector<int> ensStates;
  vector<vector<bool> > ensTransitions;
public:
  SPTGSolver();
  void showDijkstra();
  void solveSPTG(SPTG*);
  void extendedDijkstra();
};

#endif