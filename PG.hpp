#ifndef PG_HPP
#define PG_HPP

#include <vector>


using namespace std;

class PG{
private:
  vector<unsigned int>* states;
  vector<vector<int> >* transitions;
  vector<bool>* owners;
public:
  PG();
  PG(vector<unsigned int>*, vector<vector<int> >*, vector<bool>*);
};

#endif