// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"

// Entry-point function
int main(void) {

	//more off
	//format short
	//source octaveIncludes.m;
	int iterations = 1000000;
	
	int ndim1 = 2;
	int dim1[2] = {2,2};
	Matrix * a = createM(ndim1, dim1, 1);
	double *input1 = NULL;
	input1 = malloc( 4*sizeof(*input1));
	input1[0] = 1.5;
	input1[1] = 4.5;
	input1[2] = 9.5;
	input1[3] = 16.5;
	writeM( a, 4, input1);
	free(input1);
	
	printM(a);
	for (int iter1 = 1; iter1 <= iterations; iter1++) {
		Matrix * tmp1 = invertM(a);
		Matrix * b = tmp1;
	
	}
	return 0;
}
