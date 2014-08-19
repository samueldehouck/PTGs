#ifndef PGSOLVER_HPP
#define PGSOLVER_HPP

#include <vector>
#include <list>

#include "PG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "constants.hpp"

class PGSolver{
private:
  PG* pg;
  unsigned int size;
  vector<bool> ensStates;
  vector<vector<bool> > ensTransitions;
  vector<unsigned int>* pathsLengths;
  vector<vector<Fraction> >* vals;
  list<Strategy>* strategies;

  bool remainsStates();
  bool isLastTransition(unsigned int, unsigned int);
public:
  PGSolver(PG* p, vector<unsigned int>*, vector<vector<Fraction> >*, list<Strategy>* );
  bool extendedDijkstra();

};

#endif