#include "SPTGSolverValIt.hpp"

SPTGSolverValIt::SPTGSolverValIt() {
	// TODO Auto-generated constructor stub

}

SPTGSolverValIt::SPTGSolverValIt(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
	valueFcts = vF;
	resets = r;

	size = sptg->getSize();
	if(size != 0){

		for (unsigned int i = 0; i < size; ++i){

			//Used to force the player 2 to, at least, set his strategy
			updated.push_back(false);
		}

	}
}

SPTGSolverValIt::~SPTGSolverValIt() {
	if(!solvePTG)
		delete valueFcts;
}

void SPTGSolverValIt::show(){

	cout << "Lambdas:" << endl;
	for (unsigned int i = 0; i < lambdas.size(); ++i)
		cout <<  lambdas[i] << "	";

	cout << endl;

	cout << "Strategies: " << endl;

	for (unsigned int i = 0; i < size; ++i){
		cout << "State " << i << ": ";
		for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it){
			cout << "t: " << it->getX() << " ";
			it->getStrategy().show();
		}
		cout << endl;
	}

	cout << "Lengths: " << endl;
	for (unsigned int i = 0; i < pathsLengths->size(); ++i)
		cout << (*pathsLengths)[i] << "	";
	cout << endl;

	cout << "Vals:" << endl;
	for (unsigned int i = 0; i < vals->size(); ++i)
		cout << (*vals)[i] << "	";
	cout << endl;


	cout << "Value Functions" << endl;
	for (unsigned int i = 1; i < valueFcts->size(); ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ")	";
		}
		cout << endl;
	}
}

