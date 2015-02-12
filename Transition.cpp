/*
 * Author: Samuel Dehouck
 */


#include "Transition.hpp"

Transition::Transition(){
	dest = 0;
	origin = 0;
	val = 0;
}

Transition::Transition(unsigned int o, unsigned int d,  Value v):
		origin(o), dest(d), val(v)
{}
