all: main.o Fraction.o Point.o Strategy.o Transition.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o
	g++ -o solver main.o Fraction.o Point.o Transition.o Strategy.o PG.o SPTG.o SPTGSolver.o PTG.o PTGSolver.o PGSolver.o PTGFactory.o

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
	
main.o: main.cpp
	g++ -c -W -Wall -pedantic main.cpp
	
clean:
	rm *.o solver