#ifndef TRANSITION_HPP
#define TRANSITION_HPP

class Transition{
  //This class is used to save changements of transitions accessibles
public:  
  unsigned int origin, dest;
  int cost;
  
  Transition();
  Transition(unsigned int, unsigned int, int);
};

#endif