#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include "Fraction.hpp"

class Transition{
	//This class is used to save changements of transitions accessibles
public:  
	unsigned int origin, dest;
	Fraction cost;

	Transition();
	Transition(unsigned int, unsigned int, Fraction);
};

#endif
