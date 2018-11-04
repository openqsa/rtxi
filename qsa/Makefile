OBJ = intermodulation.o frequencies.o stimulation.o stimulationbuilder.o stimulationconverter.o recorder.o

SRC = intermodulation.cpp frequencies.cpp stimulation.cpp stimulationbuilder.cpp stimulationconverter.cpp recorder.cpp

all:
	g++ -c -std=c++17 -Wall -Wextra -pedantic-errors -fPIC -I./ $(SRC)
	ar rvs qsa.a $(OBJ)

clean: 
	rm -f $(OBJ)
	rm -f qsa.a
