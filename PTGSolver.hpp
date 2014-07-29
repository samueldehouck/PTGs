#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

class PTGSolver{
private:
  PTG* ptg;
public:
  PTGSolver();
  solvePTG(PTG*);
};

#endif