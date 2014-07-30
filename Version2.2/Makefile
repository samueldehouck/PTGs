all: main.o Point.o PG.o SPTG.o SPTGSolver.o Fraction.o Strategy.o PTG.o PTGSolver.o PGSolver.o
	g++ -o solver main.o Point.o PG.o SPTG.o SPTGSolver.o Fraction.o Strategy.o PTG.o PTGSolver.o PGSolver.o

Point.o: Point.cpp Point.hpp
	g++ -c -W -Wall -pedantic Point.cpp
	
SPTG.o: SPTG.cpp SPTG.hpp
	g++ -c -W -Wall -pedantic SPTG.cpp

SPTGSolver.o: SPTGSolver.cpp SPTGSolver.hpp
	g++ -c -W -Wall -pedantic SPTGSolver.cpp

Fraction.o: Fraction.cpp Fraction.hpp
	g++ -c -W -Wall -pedantic Fraction.cpp
	
Strategy.o: Strategy.cpp Strategy.hpp
	g++ -c -W -Wall -pedantic Strategy.cpp
	
PTG.o: PTG.cpp PTG.hpp
	g++ -c -W -Wall -pedantic PTG.cpp
	
PTGSolver.o: PTGSolver.cpp PTGSolver.hpp
	g++ -c -W -Wall -pedantic PTGSolver.cpp
	
PG.o: PG.cpp PG.hpp
	g++ -c -W -Wall -pedantic PG.cpp
	
PGSolver.o: PGSolver.cpp PGSolver.cpp
	g++ -c -W -Wall -pedantic PGSolver.cpp
	
main.o: main.cpp
	g++ -c -W -Wall -pedantic main.cpp
	
clean:
	rm *.o solver