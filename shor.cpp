#include <cmath>
#include <algorithm>
#include "shor.h"
#include "rand.h"
#include "methods.h"
#include <omp.h>

#include <iostream>

// INVERSE QUANTUM FOURIER TRANSFORM
void IQFT(Register *reg, unsigned int start, unsigned int end)
{
	// Just run QFT backwords for qubits start through end - 1.
	// start and end are set by default be zero in the header file.

	// if (end == 0)
	// 	end = reg->num_qubits;
unsigned int i = 0;
// #pragma omp parallel shared(i, end, start) num_threads(2)
	for (; i < floor((end - start) / 2.0); i++)
		reg->Swap(start + i, end - i - 1);

	// note: can't use unsigned int's here because unsigned j=-1
	// is always greater than zero. NEED to convert end and start to int
	// in order to properly compare.


	for (int j = int(end) - 1; j >= int(start); j--)
	{
#pragma omp parallel for shared(j) num_threads(128)
		for (int k = int(end) - j - 1; k >= 1; k--)
		{
			// don't need to explicilty convert to unsigned int here, but might as well.
			reg->ControlledPhaseShift((unsigned int)(j + k), (unsigned int)j, -pi / double(1 << k)); // 1 << k is pow(2, k)
		}
		reg->Hadamard((unsigned int)j);
	}
	//*/
}

// FINDING THE PERIOD OF f(x) = a^x mod N USING THE QUANTUM ALGORITHM

unsigned int find_Shor_period(unsigned int a, unsigned int N, unsigned int depth_limit)
{
	/*
	Find the period r of the function
		f(x) = a^x mod N
	using the quantum algorithm
	*/

	if (depth_limit <= 0)
	{
		printf("Reached maximum depth limit in period find. Returning 1.\n");
		return 1;
	}

	// q is the number of numbers register 1 can hold.
	// q must be at least 2*N so that even if the period r = N-1, the first register
	// can still hold enough numbers to have at least two x such that f(x0) = f(x1)
	// because x0 + r = x1. Most literature I have found says that we should initialize
	// q such that N^2 <= q <= 2N^2 so that q / r > N even when r = N - 1, thus
	// there will be at least N x such that f(x0) = f(x1) = ... = f(xN) where
	// xi = x0 + i*r. But for the code, I've found that q = 2*N works fine. With a smaller
	// q, we have a smaller probability of measuring the correct r, so we may have to recurse
	// through this function more. But simulating a quanutum register on a classical computer is
	// exponential, so recursing through is still more faster than simulating more qubits.
	unsigned int q = 2 * N;

	unsigned int L1 = floor(log2(q)) + 1; // number of qubits in register 1
	unsigned int L2 = floor(log2(N)) + 1; // number of qubits in register 2
	// printf("Initialized register 1 with %d qubits and register 2 with %d\n", L1, L2);

	// This is very important! I messed this up for a while. q is 2^L1.
	// What we set earlier was just to help pick L1.
	q = (unsigned int)(1 << L1); // equiv to q = (unsigned int)pow(2, L1);

	Register reg(L1 + L2);

	// Make equal superposition in register 1.
	for (unsigned int i = 0; i < L1; i++)
		reg.Hadamard(i);
	// Could have also just QFTed the first L1 qubits. Has same effect.

	auto f = [a, N, L1, L2](string s)
	{
		/*
		Given a state of the two registers |x>|0>, this function
		will make it |x>|a^x mod N>. The first register has L1
		qubits, the second has L2 qubits.
		*/
		string t = s.substr(0, L1);
		unsigned int x = binary_to_base10(t);
		// use my defined mod_power function so that we don't overflow
		string res = base10_to_binary(mod_power(a, x, N));
		while (res.size() < L2)
			res = "0" + res;
		return t + res;
	};
	// This entangles the registers. Sends |x> to |f(x)>.
	// In our case, we define f so that this sends |x, y> to |x, f(x)>
	reg.apply_function(f);

	// Don't technically need to measure yet, I don't think.
	// Could just wait until the end, but this reduces the number
	// of states, and so the QFT will perform faster (on the computer,
	// in real life, I don't think this affects the speed).
	for (unsigned int i = L1; i < L1 + L2; i++)
		reg.measure(i); // measure register 2.

	// Quantum fourier transform the first register.
	IQFT(&reg, 0, L1);

	// m will be an integer multiple of q / r with high prbability
	// where r is the period.
	unsigned int m = binary_to_base10(reg.measure().substr(0, L1)); // Measurement of register 1.

	if (m == 0)
	{
		// printf("Quantum period find failed; trying again\n");
		return find_Shor_period(a, N, depth_limit - 1);
	}

	// with high probability, m = lambda * q / r for some
	// integer lambda where r is the period. Find the period.
	// Let c = m / q. We know c = lambda / r. Expand c with
	// continuous fractions (see the paper) to find the
	// denominator r. This is O(log N) time, I think, so not bad.

	unsigned int r = 0;				  // If r is still zero afte r the while loop, we've failed.
	double c = double(m) / double(q); // this equals lambda / r for some lambda. Find the denomitor = r.
	// printf("Beginning continuous fractions expansion to find denominator for %g\n", c);
	unsigned int a1, d0, d1, dtemp; // a1 will hold cf expansion coefficients. The d's are denominators
	a1 = floor(1 / c);
	d0 = 1;
	d1 = a1;

	// we know the denominator is not greater than q, because lam / r = m / q.
	// if c == 0 then we've completed the cf expansion.
	// if c > 1, then it means that at some point c was very small, basically zero
	// and somehow this caused c = 1.0 / c - floor(1 / c) to not be less than 1.
	// Basically, it means we finished the cf expansion and didn't find r.
	// This almost never happens when q >= N^2, but happens often when Q = 2*N.
	while (c != 0.0 && c <= 1.0 && d1 < q)
	{
		if (!(d1 & 1) && mod_power(a, d1, N) == 1)
		{
			r = d1;
			break;
		} // Make sure r is even. Found it.
		else if (2 * d1 < q && mod_power(a, 2 * d1, N) == 1)
		{
			r = 2 * d1;
			break;
		} // Try two times, which will be even.
		c = 1.0 / c - double(a1);
		a1 = floor(1.0 / c);
		dtemp = d1;
		d1 = d1 * a1 + d0;
		d0 = dtemp;
	}

	if (r == 0)
	{
		// printf("Quantum period find failed; trying again\n");
		return find_Shor_period(a, N, depth_limit - 1);
	}

	// We already made sure that r was even, so we're good.

	printf("Quantum period find found the period of %d^x mod %d to be %d\n", a, N, r);
	return r;
}

// FACTORIZTION

unsigned int Shor(unsigned int N, unsigned int depth_limit)
{
	/*
	Find a single factor of N. N must not be an integer
	power of a prime. See the notes for an explaination of
	how Shor's algorithm works. The quantum computation only
	occurs in the find_period function.

	depth_limit is set by default in header. This limits the recursive depth.

	set_srand() must be called before using this function.
	*/
	if (depth_limit <= 0)
	{
		printf("Reached maximum depth limit in Shor. Try again, or increase depth limit\n");
		return 1;
	}

	if (N % 2 == 0)
		return 2;
	unsigned int a = (unsigned int)(floor(get_rand() * (N - 1) + 1));
	if (a == 1)
		a++;
	unsigned int g = gcd(a, N);
	if (g != 1 && g != N)
	{
		// printf("Completed Shor's algorithm classically. Found a factor of %d to be %d\n", N, g);
		// printf("But we want to solve it quantumly! So starting over...\n");
		// return g;
		return Shor(N, depth_limit);
	}

	// printf("Using quantum period finding algorithm to find the period of %d ^ x mod %d\n", a, N);
	unsigned int r = find_Shor_period(a, N);
	unsigned int n = mod_power(a, r / 2, N);
	// if (r % 2 == 1 || n % N == N-1) return Shor(N, depth_limit-1); // start over

	unsigned int res = gcd(n - 1, N);
	if (res != 1 && res != N)
	{
		// printf("Shor found a factor of %d to be %d\n", N, res);
		return res;
	}
	res = gcd(n + 1, N);
	if (res != 1 && res != N)
	{
		// printf("Shor found a factor of %d to be %d\n", N, res);
		return res;
	}

	// printf("Shor did not find a factor; trying again\n");
	return Shor(N, depth_limit - 1);
}