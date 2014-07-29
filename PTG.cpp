#include "PTG.hpp"
#include <iostream>

using namespace std;

PTG::PTG(){
  //TODO put everything together to go faster
  states.push_back(0);
  states.push_back(6);
  states.push_back(2);
  states.push_back(5);
  states.push_back(4);
  
  for (unsigned int i = 0; i < states.size(); ++i){
    transitions.push_back(vector<int>());
    for (unsigned int j = 0; j < states.size(); ++j)
      transitions[i].push_back(-1);
  }
  transitions[1][3] = 1;
  transitions[1][4] = 5;
  transitions[2][1] = 1;
  transitions[3][0] = 3;
  transitions[3][4] = 1;
  transitions[4][0] = 1;
  transitions[4][2] = 2;

  
   owners.push_back(1);
   owners.push_back(1);
   owners.push_back(0);
   owners.push_back(0);
   owners.push_back(1);
  

  for (unsigned int i = 0; i < transitions.size(); ++i){
   resets.push_back(vector<bool>());
   for (unsigned int j = 0; j < transitions[i].size(); ++j)
    resets[i].push_back(false);
 }

   resets[1][4] = true;
  
  for (unsigned int i = 0; i < transitions.size(); ++i){
   startsCstraints.push_back(vector<int>());
   endsCstraints.push_back(vector<int>());
   for (unsigned int j = 0; j < transitions[i].size(); ++j){
      startsCstraints[i].push_back(-1);
      endsCstraints[i].push_back(-1);
   }
    
 }  
  startsCstraints[1][3] = 1;
  endsCstraints[1][3] = 5;
  startsCstraints[1][4] = 0;
  endsCstraints[1][4] = 2;
  startsCstraints[2][1] = 1;
  endsCstraints[2][1] = 3;
  startsCstraints[3][0] = 0;
  endsCstraints[3][0] = 5;
  startsCstraints[3][4] = 4;
  endsCstraints[3][4] = 4;
  startsCstraints[4][2] = 2;
  endsCstraints[4][2] = 2;
  startsCstraints[4][0] = 1;
  endsCstraints[4][0] = 3;
  
   cout << "PTG initalized" << endl;

}

void PTG::show(){
  SPTG::show();
  
  for (unsigned int i = 0; i < startsCstraints.size(); ++i){
   for (unsigned int j = 0; j < startsCstraints[i].size(); ++j){
      cout << "[" << startsCstraints[i][j] << "," << endsCstraints[i][j] << "]	";
   }
   cout << endl;
  }
}
