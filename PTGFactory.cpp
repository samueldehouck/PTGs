#include "PTGFactory.hpp"
#include "pugixml/pugixml.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace pugi;


PTGFactory::PTGFactory(){

}

PTG* PTGFactory::build(int nbStates, int nbTrans, int nbResets, int maxRate, int maxCost, int maxCst){
	//The PTG is randomly built
	PTG* ptg = new PTG(nbStates);
	srand (time(NULL));
	ptg->setNbResets(nbResets);
	//Generates everything
	ptg->setOwner(0,1);
	ptg->setState(0,0);
	for (unsigned int i = 1; i < ptg->getSize(); ++i){
		ptg->setState(i, rand() % (maxRate + 1));
		ptg->setOwner(i, rand()%2);
	}
	int tmp = nbTrans;
	while (tmp != 0){
		int i = rand() % (nbStates - 1) + 1;
		int j = rand() % nbStates;
		if(ptg->getTransition(i,j) == -1 && i != j){
			ptg->setTransition(i,j,rand() % (maxCost + 1));
			int endCst = rand() % (maxCst + 1);
			ptg->setEndCst(i, j, endCst);
			ptg->setStartCst(i, j, rand() % (endCst + 1));
			--tmp;
		}
	}

	tmp = nbResets;
	while(tmp != 0){
		int i = rand() % nbStates;
		int j = rand() % nbStates;
		if(ptg->getTransition(i,j) != -1){
			ptg->setReset(i,j,true);
			--tmp;
		}
	}
	ptg->show();
	return ptg;
}


PTG* PTGFactory::buildFromFile(char* f){
	string fstr(f);
	if(fstr.find(".xml") != string::npos)
		return buildFromXmlFile(f);
	else
		return buildFromFlatFile(f);
}


PTG* PTGFactory::buildFromFlatFile(char* f){
	ifstream file(f);
	string line = "";
	getline(file,line);//====PTG====
	getline(file,line);//Rates:
	getline(file,line);
	istringstream* stream = new istringstream(line);
	int n;
	vector<int> v;

	while (*stream >> n)
	{
		v.push_back(n);
	}
	PTG* ptg = new PTG(v.size());
	for (unsigned int i = 0; i < v.size(); ++i)
		ptg->setState(i,v[i]);

	delete stream;
	getline(file,line);//Transitions:
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		stream = new istringstream(line);
		for (unsigned int j = 0; *stream >> n && j < ptg->getSize(); ++j){
			ptg->setTransition(i,j,n);
		}
		delete stream;
	}
	getline(file,line);//Owners
	getline(file, line);
	stream = new istringstream(line);
	for (unsigned i = 0; *stream >> n && i < ptg->getSize(); ++i){
		ptg->setOwner(i,n);
	}
	getline(file,line);//Constraints
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		int index = 0;
		for (unsigned int j = 0; j < ptg->getSize(); ++j){
			string next = "";
			while (line[index] != '[')
				++index;
			++index;
			while(line[index] != ','){
				next.push_back(line[index]);
				++index;
			}
			n = atoi(next.c_str());
			ptg->setStartCst(i,j,n);
			++index;
			next = "";
			while(line[index] != ']'){
				next.push_back(line[index]);
				++index;
			}
			n = atoi(next.c_str());
			ptg->setEndCst(i,j,n);
		}
	}
	getline(file,line);//Resets:
	unsigned int cnt = 0;
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		stream = new istringstream(line);
		for (unsigned int j = 0; *stream >> n && j < ptg->getSize(); ++j){
			ptg->setReset(i,j,(bool)n);
			if(n == 1)
				++cnt;
		}
		delete stream;
	}
	ptg->setNbResets(cnt);
	return ptg;
}

PTG* PTGFactory::buildFromXmlFile(char* f){
	//Build a PTG from an xml file of uppaal tiga
	PTG* ptg;
	vector<Fraction> states;
	vector<string> ids;
	vector<string> names;
	unsigned int nbResets = 0;
	xml_document doc;
	xml_parse_result result = doc.load_file(f);
	if(result){
		xml_node nta = doc.first_child();
		xml_node node = nta.child("template");

		for(xml_node_iterator it = node.begin(); it != node.end(); ++it){
			//If the following data is a location
			if(string(it->name()).compare("location") == 0){
				//Store it because in the PTG intern representation, the target is at index 0
				states.push_back(it->child("label").text().as_int());
				ids.push_back(it->attribute("id").as_string());
				names.push_back(string(it->child("name").text().as_string()));
			}
			else if(string(it->name()).compare("init") == 0){
				//All locations have been declared, the target can be swapped
				ptg = new PTG(states.size(), true);
				for( unsigned int i = 0; i < states.size(); ++i){
					if(ids[i].compare(it->attribute("ref").as_string()) == 0){
						ptg->setState(0,states[i]);
						Fraction tmp = states[0];
						states[0] = states[i];
						states[i] = tmp;

						string id = ids[0];
						ids[0] = ids[i];
						ids[i] = id;

						string name = names[0];
						names[0] = names[i];
						names[i] = name;
					}

				}
				for (unsigned int i = 0; i < ids.size(); ++i){
					ptg->setLabel(i,names[i]);
					ptg->setState(i, states[i]);
				}

			}
			else if(string(it->name()).compare("transition") == 0){
				unsigned int src = 0;
				unsigned int dest = 0;
				unsigned int cost = 0;
				unsigned int startCst = 0;
				unsigned int endCst = 0;
				bool reset = false;
				bool owner = false;

				if(string(it->attribute("controllable").as_string()).compare("false") == 0)
					owner = false;
				else
					owner = true;

				for(xml_node_iterator itT = it->begin(); itT != it->end(); ++itT){
					if(string(itT->name()).compare("source") == 0){
						unsigned int i = 0;
						cout << itT->attribute("ref").as_string() << endl;
						for (; i < ids.size() && ids[i].compare(itT->attribute("ref").as_string()); ++i);
						if(i != ids.size()){
							src = i;
						}
					}
					else if(string(itT->name()).compare("target") == 0){
						unsigned int i = 0;
						for (; i < ids.size() && ids[i].compare(itT->attribute("ref").as_string()); ++i);
						if(i != ids.size()){
							dest = i;
						}
					}
					else if(string(itT->name()).compare("label") == 0){
						if(string(itT->attribute("kind").as_string()).compare("guard") == 0){
							string guard = string(itT->text().as_string());
							unsigned int i = 0;
							string nb = "";
							while (guard[i] != '[' && i < guard.length())
								++i;
							++i;
							while (guard[i] != ',' && i < guard.length()){
								nb += guard[i];
								++i;
							}
							startCst = atoi(nb.c_str());
							nb = "";
							++i;
							while(guard[i] != ']' && i < guard.length()){
								nb += guard[i];
								++i;
							}
							endCst = atoi(nb.c_str());
						}
						else if(string(itT->attribute("kind").as_string()).compare("synchronisation") == 0){
							cost = itT->text().as_int();
						}
						else if(string(itT->attribute("kind").as_string()).compare("assignment") == 0){
							if(string(itT->text().as_string()).compare("r") == 0){
								reset = true;
								++nbResets;
							}
						}
					}
				}

				ptg->setOwner(src,owner);
				ptg->setTransition(src,dest,cost);
				ptg->setStartCst(src,dest,startCst);
				ptg->setEndCst(src,dest,endCst);
				ptg->setReset(src,dest,reset);
			}
		}
		ptg->setNbResets(nbResets);
		ptg->setOwner(0,1);
	}
	else
		cerr << "Error while loading the xml file" << endl;
	return ptg;
}

PTG* PTGFactory::hardBuild(unsigned int build){
	//Can build some examples of PTGs
	PTG* ptg;
	if(build == 1)
	{
		ptg = new PTG(6);

		ptg->setState(0,0);
		ptg->setState(1,3);
		ptg->setState(2,2);
		ptg->setState(3,0);
		ptg->setState(4,1);
		ptg->setState(5,5);

		ptg->setTransition(1,3,1);
		ptg->setTransition(2,1,2);
		ptg->setTransition(2,3,1);
		ptg->setTransition(2,4,2);
		ptg->setTransition(2,5,1);
		ptg->setTransition(3,0,0);
		ptg->setTransition(3,4,2);
		ptg->setTransition(4,5,3);
		ptg->setTransition(5,0,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(4,0);
		ptg->setOwner(5,1);

		ptg->setStartCst(1,3,2);
		ptg->setStartCst(2,1,1);
		ptg->setStartCst(2,3,0);
		ptg->setStartCst(2,4,0);
		ptg->setStartCst(2,5,0);
		ptg->setStartCst(3,4,0);
		ptg->setStartCst(3,0,1);
		ptg->setStartCst(4,5,1);
		ptg->setStartCst(5,0,0);
		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,1,2);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(2,4,1);
		ptg->setEndCst(2,5,2);
		ptg->setEndCst(3,4,1);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(4,5,2);
		ptg->setEndCst(5,0,2);

		ptg->setReset(3,4,true);
		ptg->setReset(2,5,true);

		ptg->setNbResets(2);
	}
	else if (build == 2){
		ptg = new PTG(5);

		ptg->setState(0,0);
		ptg->setState(1,1);
		ptg->setState(2,3);
		ptg->setState(3,1);
		ptg->setState(4,3);

		ptg->setTransition(1,2,2);
		ptg->setTransition(1,3,2);
		ptg->setTransition(2,0,5);
		ptg->setTransition(2,3,0);
		ptg->setTransition(3,0,1);
		ptg->setTransition(3,4,3);
		ptg->setTransition(4,1,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,0);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(4,1);

		ptg->setStartCst(1,2,1);
		ptg->setStartCst(1,3,0);
		ptg->setStartCst(2,0,0);
		ptg->setStartCst(2,3,1);
		ptg->setStartCst(3,0,0);
		ptg->setStartCst(3,4,0);
		ptg->setStartCst(4,1,0);
		ptg->setEndCst(1,2,2);
		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,0,1);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(3,4,2);
		ptg->setEndCst(4,1,2);

		ptg->setReset(4,1,true);
		ptg->setReset(2,3,true);
		ptg->setNbResets(2);
	}
	else if(build == 3){
		ptg = new PTG(6);


		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,5);
		ptg->setState(3,1);
		ptg->setState(4,2);
		ptg->setState(5,3);

		ptg->setTransition(1,4,2);
		ptg->setTransition(2,1,3);
		ptg->setTransition(2,5,1);
		ptg->setTransition(3,0,1);
		ptg->setTransition(4,3,2);
		ptg->setTransition(4,0,0);
		ptg->setTransition(5,4,1);
		ptg->setTransition(5,0,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,1);
		ptg->setOwner(4,0);
		ptg->setOwner(5,0);


		ptg->setStartCst(1,4,0);
		ptg->setEndCst(1,4,3);
		ptg->setStartCst(2,1,0);
		ptg->setEndCst(2,1,1);
		ptg->setStartCst(2,5,2);
		ptg->setEndCst(2,5,3);
		ptg->setStartCst(3,0,0);
		ptg->setEndCst(3,0,3);
		ptg->setStartCst(4,3,1);
		ptg->setEndCst(4,3,3);
		ptg->setStartCst(4,0,2);
		ptg->setEndCst(4,0,2);
		ptg->setStartCst(5,4,1);
		ptg->setEndCst(5,4,2);
		ptg->setStartCst(5,0,0);
		ptg->setEndCst(5,0,3);

		ptg->setReset(4,3,true);
		ptg->setNbResets(1);
	}
	else if(build == 4){
		ptg = new PTG(5);



		ptg->setState(0,0);
		ptg->setState(1,6);
		ptg->setState(2,2);
		ptg->setState(3,5);
		ptg->setState(4,3);

		ptg->setTransition(1,3,1);
		ptg->setTransition(1,4,5);
		ptg->setTransition(2,1,1);
		ptg->setTransition(3,0,3);
		ptg->setTransition(3,4,1);
		ptg->setTransition(4,0,1);
		ptg->setTransition(4,2,2);


		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,0);
		ptg->setOwner(3,0);
		ptg->setOwner(4,1);


		ptg->setReset(1,4,true);
		ptg->setNbResets(1);

		//ptg->setNbResets(0);

		ptg->setStartCst(1,3,1);
		ptg->setEndCst(1,3,3);
		ptg->setStartCst(1,4,0);
		ptg->setEndCst(1,4,2);
		ptg->setStartCst(2,1,1);
		ptg->setEndCst(2,1,3);
		ptg->setStartCst(3,0,0);
		ptg->setEndCst(3,0,3);
		ptg->setStartCst(3,4,2);
		ptg->setEndCst(3,4,2);
		ptg->setStartCst(4,2,2);
		ptg->setEndCst(4,2,2);
		ptg->setStartCst(4,0,1);
		ptg->setEndCst(4,0,3);
	}
	else if(build == 5){
		ptg = new PTG(4);
		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,1);
		ptg->setState(3,1);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,0);
		ptg->setOwner(3,1);

		ptg->setTransition(1,3,3);
		ptg->setTransition(2,0,1);
		ptg->setTransition(2,1,3);
		ptg->setTransition(2,3,0);
		ptg->setTransition(3,0,3);
		ptg->setTransition(3,2,1);


		ptg->setNbResets(0);
		ptg->setStartCst(1,3,1);
		ptg->setStartCst(2,0,0);
		ptg->setStartCst(2,1,0);
		ptg->setStartCst(2,3,1);
		ptg->setStartCst(3,0,1);
		ptg->setStartCst(3,2,0);

		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,0,2);
		ptg->setEndCst(2,1,1);
		ptg->setEndCst(2,3,1);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(3,2,2);

	}
	else if(build == 6){

		ptg = new PTG(5);
		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,1);
		ptg->setState(3,2);
		ptg->setState(4,1);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(3,0);

		ptg->setTransition(1,3,1);
		ptg->setTransition(1,2,1);
		ptg->setTransition(2,3,0);
		ptg->setTransition(2,4,2);
		ptg->setTransition(3,2,2);
		ptg->setTransition(3,0,1);
		ptg->setTransition(4,0,2);

		ptg->setStartCst(1,3,0);
		ptg->setStartCst(1,2,0);
		ptg->setStartCst(2,3,0);
		ptg->setStartCst(2,4,0);
		ptg->setStartCst(3,2,1);
		ptg->setStartCst(3,0,0);
		ptg->setStartCst(4,0,0);

		ptg->setEndCst(1,3,2);
		ptg->setEndCst(1,2,2);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(2,4,1);
		ptg->setEndCst(3,2,2);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(4,0,2);

		ptg->setNbResets(2);
		ptg->setReset(1,2,true);
		ptg->setReset(3,2,true);
	}


	return ptg;
}
