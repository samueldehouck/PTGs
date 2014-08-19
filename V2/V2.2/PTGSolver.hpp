#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

#include "SPTGSolver.hpp"
#include "PTG.hpp"

class PTGSolver{
private:
  PTG* ptg;
  list<unsigned int> intervals;
  vector<Fraction> vals;
public:
  PTGSolver();
  void solvePTG(PTG*);
};

#endif