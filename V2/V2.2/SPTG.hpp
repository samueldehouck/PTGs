#ifndef SPTG_HPP
#define SPTG_HPP

#include <vector>

#include "PG.hpp"

using namespace std;

class SPTG: public PG{
protected:
  vector<unsigned int>* states;
public:
  SPTG();
  ~SPTG();
  void show();
  int getState(unsigned int) const;
};

#endif