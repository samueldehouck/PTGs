#include <iostream>
#include "SPTG.hpp"
#include "SPTGSolver.hpp"

using namespace std;

int main(int argc, char *argv[]){
   SPTG sptg;
   sptg.show();
   SPTGSolver solver;
  solver.solveSPTG(&sptg);
}