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
  vector<bool> owner;
  list<Point> valueFunction;
public:
  SPTG();
  const void show();
  const int getSize();
  const int getState(int);
  const int getTransition(int, int);
  const bool getOwner(int);
};

#endif