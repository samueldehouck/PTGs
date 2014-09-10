#include "Strategy.hpp"

using namespace std;

Strategy::Strategy(){
	time.setInf(true);
	included = false;
	dest = 0;
	type = 0;
}

Strategy::Strategy(Value t, unsigned int d, unsigned int ty, bool i){
	time = t;
	included = i;
	dest = d;
	type = ty;
}

Strategy::Strategy(const Strategy& s){
	time = s.getTime();
	dest = s.getDest();
	type = s.getType();
	included = s.getInclusion();

}

unsigned int Strategy::getDest() const{
	return dest;
}

void Strategy::setDest(unsigned int val){
	dest = val;
}

unsigned int Strategy::getType() const{
	return type;
}

void Strategy::setType(unsigned int t){
	type = t;
}

Value Strategy::getTime() const{
	return time;
}

void Strategy::setTime(Value t){
	time = t;
}

bool Strategy::getInclusion() const{
	return included;
}

void Strategy::setInclusion(bool in){
	included = in;
}

void Strategy::show(){
	if(included)
		cout << "Time: " << time << " (in) -> ";
	else
		cout << "Time: " << time << " -> ";
	cout << dest << " (" << type  << ") ";
}
