#ifndef SPTGSOLVER_HPP
#define SPTGSOLVER_HPP

#include "SPTG.hpp"
#include "Point.hpp"
#include "Fraction.hpp"
#include <stddef.h>
#include <list>

class SPTGSolver{
private:
  SPTG* sptg;
  unsigned int size;
  vector<unsigned int> strats;
  vector<vector<Fraction> > vals;
  vector<unsigned int> pathsLengths;
  vector<vector<Fraction> > lambdas;
  vector<unsigned int> ensStates;
  vector<vector<bool> > ensTransitions;
  vector<list<Point> > valueFcts;
  Fraction time;
  
  void init();
  void extendedDijkstra();
  void strategyIteration();
  void actualizeLambdas();
  void actualizeVals(Fraction);
  bool makeImpSwitchesP1();
  bool makeImpSwitchesP2();
  void propagate(unsigned int);
  void addPoint(unsigned int, Fraction, Fraction);
  void buildValueFcts(Fraction);
  Fraction nextEventPoint();
  
public:
  SPTGSolver();
  void show();
  void solveSPTG(SPTG*);

};

#endif