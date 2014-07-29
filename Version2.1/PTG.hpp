#ifndef PTG_HPP
#define PTG_HPP

#include "SPTG.hpp"
#include <vector>
#include "Fraction.hpp"


class PTG: public SPTG{
private:
  vector<vector<bool> > resets;
  vector<vector<int> > startsCstraints;
  vector<vector<int> > endsCstraints;
public:
  PTG();
};

#endif