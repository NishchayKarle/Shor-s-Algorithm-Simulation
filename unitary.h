#ifndef UNITARY_INCLUDE
#define UNITARY_INCLUDE

#include "types.h"

class Unitary
{

private:
	amp **matrix;

public:
	unsigned int dimension;
	Unitary(unsigned int dimension);
	amp *operator[](unsigned int i);
	Unitary operator*(amp x);
	Unitary operator*(Unitary &U);
	void print_matrix();
	friend std::ostream &operator<<(std::ostream &os, Unitary &U);

	static Unitary Hadamard();
	static Unitary ControlledNot();
	static Unitary ControlledPhaseShift(double theta);
	static Unitary Swap();
	static Unitary QFT(unsigned int num_qubits);
	static Unitary IQFT(unsigned int num_qubits);

	// Matrices to be created upon initialization.
	// const static Unitary MHadamard;
};

// For left multiplication.
Unitary operator*(amp x, Unitary &U);

#endif