
cs463project3: dpll.o InputParser.o Constants.o CommonTypes.o
	mpiCC dpll.o InputParser.o Constants.o CommonTypes.o -o cs463project3


dpll.o: dpll.cpp
	mpiCC -c dpll.cpp

InputParser.o: InputParser.cpp
	mpiCC -c InputParser.cpp

Constants.o: Constants.h
	mpiCC -c Constants.h

CommonTypes.o: CommonTypes.h
	mpiCC -c CommonTypes.h

clean:
	rm -f *.o cs463project3