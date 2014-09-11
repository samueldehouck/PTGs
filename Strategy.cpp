#include "Strategy.hpp"

using namespace std;

Strategy::Strategy(){
	included = false;
	dest = 0;
	type = 0;
}

Strategy::Strategy(unsigned int d, unsigned int ty, bool i){
	included = i;
	dest = d;
	type = ty;
}

Strategy::Strategy(const Strategy& s){
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

bool Strategy::getInclusion() const{
	return included;
}

void Strategy::setInclusion(bool in){
	included = in;
}

void Strategy::show(){
	cout << dest << " (" << type << ")";
	if(included)
		cout << "(in)";
	cout << endl;
}
