#include <iostream>
#include "shor.h"
#include <omp.h>

using namespace std;

void _test_Shor_factorization()
{
	unsigned int a = 97 * 101;
	cout << "\tLooking for factor of " << a << endl;
	int s = omp_get_wtime();
	unsigned int b = Shor(a);
	int e = omp_get_wtime();
	cout << "\t"
		 << "a Shor factor of " << a << " is " << b << endl;

	cout << "time: " << e - s << endl;
	cout << endl;
}

int main()
{
	_test_Shor_factorization();
	return 0;
}
