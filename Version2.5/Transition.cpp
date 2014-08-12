#include "Transition.hpp"

Transition::Transition(){

}

Transition::Transition(unsigned int o, unsigned int d,  Fraction c):
		origin(o), dest(d), cost(c)
{}
