#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include "Value.hpp"

class Transition{
	//This class is used to save changements of transitions accessibles
public:  
	unsigned int origin, dest;
	Value cost;

	Transition();
	Transition(unsigned int, unsigned int, Value);
};

#endif
