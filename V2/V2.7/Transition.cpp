#include "Transition.hpp"

Transition::Transition(){
	dest = 0;
	origin = 0;
	cost = 0;
}

Transition::Transition(unsigned int o, unsigned int d,  Fraction c):
		origin(o), dest(d), cost(c)
{}
