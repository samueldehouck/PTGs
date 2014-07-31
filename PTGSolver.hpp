#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

#include "SPTGSolver.hpp"
#include "PGSolver.hpp"
#include "PTG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "Point.hpp"

class PTGSolver{
private:
  PTG* ptg;
  Fraction time;
  unsigned int size;
  list<unsigned int> endPoints;
  vector<unsigned int> pathsLengths;
  vector<vector<Fraction> > vals;
  list<Strategy> strategies;
  vector<list<Point> > valueFcts;
  vector<vector<bool> > transAvailable;
  vector<vector<Fraction> > lambdas;
  
  void init();
  void createEndPoints();
public:
  PTGSolver();
  void solvePTG(PTG*);
  void show();
};

#endif