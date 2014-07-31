#include "SPTG.hpp"
#include <iostream>

SPTG::SPTG(){
  
//  Example of the paper of the three danish

  states = new vector<unsigned int>;
/* 
 states->push_back(0);
 states->push_back(9);
 states->push_back(3);
 states->push_back(6);
 states->push_back(9);
 states->push_back(0);

 for (int i = 0; i <= 5; ++i){
   transitions->push_back(vector<int>());
   for (int j = 0; j <= 5; ++j)
    (*transitions)[i].push_back(-1);
 }
 
  (*transitions)[1][0] = 0;
  (*transitions)[2][1] = 0;
  (*transitions)[2][4] = 0;
  (*transitions)[3][1] = 3;
  (*transitions)[3][2] = 0;
  (*transitions)[4][3] = 0;
  (*transitions)[4][5] = 0;
  (*transitions)[5][0] = 5;
 
  //1 if owned by the controller
  owners->push_back(1);
  owners->push_back(0);
  owners->push_back(0);
  owners->push_back(1);
  owners->push_back(1);
  owners->push_back(1);
  
 cout << "SPTG initalized" << endl;*/
}

SPTG::~SPTG(){
  delete transitions;
  delete owners;
  delete states;  
}

void SPTG::show(){
    cout << "Rates: " << endl;
  for (unsigned int i =0; i < states->size(); ++i)
    cout << (*states)[i] << " ";
  cout << endl;
  
  cout << "Transitions: "<< endl;
  for (unsigned int i = 0; i < transitions->size(); ++i){
   for (unsigned int j = 0; j < (*transitions)[i].size(); ++j)
    cout << (*transitions)[i][j] << "	";
   cout << endl;
  }
  
    cout << "Owners: " << endl;
  for (unsigned int i = 0; i < owners->size(); ++i)
    cout << (*owners)[i] << " ";
  cout << endl;
}

int SPTG::getState(unsigned int index) const{
  return (*states)[index];
}
