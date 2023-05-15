test_omp: *.cpp *.h
	g++ -std=c++11 -o test_omp *.cpp -fopenmp -DOPENMP
test_serial: *.cpp *.h
	g++ -std=c++11 -o test_serial *.cpp -fopenmp
all: *.cpp *.h
	g++ -std=c++11 -o test_omp *.cpp -fopenmp -DOPENMP
	g++ -std=c++11 -o test_serial *.cpp -fopenmp
