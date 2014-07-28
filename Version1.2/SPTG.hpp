#ifndef SPTG_HPP
#define SPTG_HPP

#include "Point.hpp"
#include <list>
#include <vector>

using namespace std;

class SPTG{
private:
  vector<int> states;
  vector<vector<int> > transitions;
  vector<bool> owners;
  list<Point> valueFunction;
public:
  SPTG();
  void show();
  unsigned int getSize() const;
  int getState(unsigned int) const;
  int getTransition(unsigned int, unsigned int) const;
  void setTransition(unsigned int,unsigned int, int);
  bool getOwner(unsigned int) const;
};

#endif