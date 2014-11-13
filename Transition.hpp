/*
 * Author: Samuel Dehouck
 */


#ifndef TRANSITION_HPP_
#define TRANSITION_HPP_

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
