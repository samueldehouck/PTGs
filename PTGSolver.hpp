#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

#include "SPTGSolver.hpp"
#include "PTG.hpp"

class PTGSolver{
private:
  PTG* ptg;
  unsigned int size;
  vector<Fraction> vals;
  list<unsigned int> endPoints;
  
  void createEndPoints();
public:
  PTGSolver();
  void solvePTG(PTG*);
};

#endif