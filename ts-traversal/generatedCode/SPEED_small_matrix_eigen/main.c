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
		complex complex_one = 1;
		Matrix * V1 = NULL;
		Matrix * lambda1 = NULL;
		Matrix * evals1 = NULL;
		Matrix * evecs1 = NULL;
		eigM(a, &evals1, &evecs1);
		lambda1 = scaleM(evals1, &complex_one, COMPLEX);
		V1 = scaleM(evecs1, &complex_one, COMPLEX);
	
	}
	return 0;
}
