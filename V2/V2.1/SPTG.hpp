#ifndef SPTG_HPP
#define SPTG_HPP

#include <vector>

using namespace std;

class SPTG{
protected:
  vector<int> states;
  vector<vector<int> > transitions;
  vector<bool> owners;
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