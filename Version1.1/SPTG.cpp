#include "SPTG.hpp"
#include <iostream>

SPTG::SPTG(){
 states.push_back(-1);
 states.push_back(0);
 states.push_back(3);
 states.push_back(3);
 states.push_back(3);
 states.push_back(5);

 for (int i = 0; i <= 5; ++i){
   transitions.push_back(vector<int>());
   for (int j = 0; j <= 5; ++j)
    transitions[i].push_back(-1);
 }
 
  transitions[1][0] = 0;
  transitions[2][1] = 0;
  transitions[2][4] = 0;
  transitions[3][1] = 3;
  transitions[3][2] = 0;
  transitions[4][3] = 0;
  transitions[4][5] = 0;
  transitions[5][0] = 5;
 
  //1 if owned by the controller
  owner.push_back(1);
  owner.push_back(0);
  owner.push_back(0);
  owner.push_back(1);
  owner.push_back(1);
  owner.push_back(1);
    
 cout << "SPTG initalized" << endl;
}

const void SPTG::show(){
  cout << "Rates: " << endl;
  for (int i =0; i <= 5; ++i)
    cout << states[i] << " ";
  cout << endl;
  
  cout << "Transitions: "<< endl;
  for (int i = 0; i <= 5; ++i){
   for (int j = 0; j <= 5; ++j)
    cout << transitions[i][j] << "	";
   cout << endl;
  }
  
    cout << "Owners: " << endl;
  for (int i =0; i <= 5; ++i)
    cout << owner[i] << " ";
  cout << endl;
}

const int SPTG::getSize(){
return states.size();  
}

const int SPTG::getState(int index){
  return states[index];
}

const int SPTG::getTransition(int origin, int end){
  return transitions[origin][end];
}

const bool SPTG::getOwner(int index){
  return owner[index];
}