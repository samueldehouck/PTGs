#include "PG.hpp"

#include <iostream>
#include <cstdlib> 

using namespace std;

PG::PG(){
 states = NULL;
 transitions = NULL;
 owners = NULL; 
}
PG::PG(vector<unsigned int>* s, vector<vector<int> >* t, vector<bool>* o){
 states = s;
 transitions = t;
 owners = o; 
}