#ifndef SPTGSOLVER_HPP
#define SPTGSOLVER_HPP

#include "SPTG.hpp"
#include "Point.hpp"
#include <stddef.h>
#include <list>

class SPTGSolver{
private:
  SPTG* sptg;
  unsigned int size;
  vector<unsigned int> strats;
  vector<vector<double> > vals;
  vector<unsigned int> pathsLengths;
  vector<vector<double> > lambdas;
  vector<unsigned int> ensStates;
  vector<vector<bool> > ensTransitions;
  vector<list<Point> > valueFcts;
  double time;
  
  void init();
  void extendedDijkstra();
  void strategyIteration();
  void actualizeLambdas();
  void actualizeVals(double);
  bool makeImpSwitchesP1();
  bool makeImpSwitchesP2();
  void propagate(unsigned int);
  void addPoint(unsigned int, double, double);
  void buildValueFcts(double);
  double nextEventPoint();
  bool isEpsClose(double, double);
  bool greater(double, double);
  bool lesser(double, double);
  
public:
  SPTGSolver();
  void show();
  void solveSPTG(SPTG*);

};

#endif