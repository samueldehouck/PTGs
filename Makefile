all: pugixml.o Constants.o Fraction.o Value.o CompositeValue.o Point.o Strategy.o Transition.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o PerfEvaluator.o FunctionsMinMax.o SPTGSolverValIt.o PTGVisu.o SPTGTester.o main.o
	g++ -W -Wextra -Wall -pedantic -o solver pugixml.o Constants.o Fraction.o Value.o CompositeValue.o Point.o Transition.o Strategy.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o PerfEvaluator.o FunctionsMinMax.o SPTGSolverValIt.o PTGVisu.o SPTGTester.o main.o 

Constants.o: Constants.cpp Constants.hpp
	g++ -c -W -Wextra -Wall -pedantic Constants.cpp

Point.o: Point.cpp Point.hpp
	g++ -c -W -Wextra -Wall -pedantic Point.cpp
	
SPTG.o: SPTG.cpp SPTG.hpp
	g++ -c -W -Wextra -Wall -pedantic SPTG.cpp

SPTGSolver.o: SPTGSolver.cpp SPTGSolver.hpp
	g++ -c -W -Wextra -Wall -pedantic SPTGSolver.cpp

Fraction.o: Fraction.cpp Fraction.hpp
	g++ -c -W -Wextra -Wall -pedantic Fraction.cpp
	
Strategy.o: Strategy.cpp Strategy.hpp
	g++ -c -W -Wextra -Wall -pedantic Strategy.cpp
	
Transition.o: Transition.cpp Transition.hpp
	g++ -c -W -Wextra -Wall -pedantic Transition.cpp
	
PTG.o: PTG.cpp PTG.hpp
	g++ -c -W -Wextra -Wall -pedantic PTG.cpp
	
PTGSolver.o: PTGSolver.cpp PTGSolver.hpp
	g++ -c -W -Wextra -Wall -pedantic PTGSolver.cpp
	
PG.o: PG.cpp PG.hpp
	g++ -c -W -Wextra -Wall -pedantic PG.cpp
	
PGSolver.o: PGSolver.cpp PGSolver.hpp
	g++ -c -W -Wextra -Wall -pedantic PGSolver.cpp
	
PTGFactory.o: PTGFactory.cpp PTGFactory.hpp
	g++ -c -W -Wextra -Wall -pedantic PTGFactory.cpp
	
Value.o: Value.cpp Value.hpp
	g++ -c -W -Wextra -Wall -pedantic Value.cpp

CompositeValue.o: CompositeValue.cpp CompositeValue.hpp
	g++ -c -W -Wextra -Wall -pedantic CompositeValue.cpp
	
PerfEvaluator.o: PerfEvaluator.cpp PerfEvaluator.hpp
	g++ -c -W -Wextra -Wall -pedantic PerfEvaluator.cpp
	
pugixml.o: pugixml/pugixml.cpp pugixml/pugixml.hpp
	g++ -c -W -Wextra -Wall -pedantic pugixml/pugixml.cpp	
	
SPTGSolverValIt.o: SPTGSolverValIt.cpp SPTGSolverValIt.hpp
	g++ -c -W -Wextra -Wall -pedantic SPTGSolverValIt.cpp
	
FunctionsMinMax.o: FunctionsMinMax.cpp FunctionsMinMax.hpp
	g++ -c -W -Wextra -Wall -pedantic FunctionsMinMax.cpp	

PTGVisu.o: PTGVisu.cpp PTGVisu.hpp
	g++ -c -W -Wextra -Wall -pedantic PTGVisu.cpp	
	
SPTGTester.o: SPTGTester.cpp SPTGTester.hpp
	g++ -c -W -Wextra -Wall -pedantic SPTGTester.cpp	
	
main.o: main.cpp
	g++ -c -W -Wextra -Wall -pedantic main.cpp

debug: pugixml.o Constants.o Fraction.o Value.o CompositeValue.o Point.o Strategy.o Transition.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o PerfEvaluator.o FunctionsMinMax.o SPTGSolverValIt.o PTGVisu.o SPTGTester.o main.o
	 g++ -W -Wextra -Wall -pedantic -g pugixml.o Constants.o Fraction.o Value.o CompositeValue.o Point.o Strategy.o Transition.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o PerfEvaluator.o FunctionsMinMax.o SPTGSolverValIt.o PTGVisu.o SPTGTester.o main.o

clean:
	rm *.o solver output.txt *.tex *.log *.aux *.pdf