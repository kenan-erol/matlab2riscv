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
	int exponent = 30;
	int iterations = 100000;
	
	int ndim1 = 2;
	int dim1[2] = {2,2};
	Matrix * b = createM(ndim1, dim1, 2);
	complex *input1 = NULL;
	input1 = malloc( 4*sizeof(*input1));
	input1[0] = 2.1 + 0.5*I;
	input1[1] = 1;
	input1[2] = 0;
	input1[3] = 2.1 + 0.5*I;
	writeM( b, 4, input1);
	free(input1);
	
	printM(b);
	Matrix * tmp1 = mpowerM(b, &exponent, 0);
	Matrix * c = tmp1;
	printM(c);
	for (int iter1 = 1; iter1 <= iterations; iter1++) {
		Matrix * tmp2 = mpowerM(b, &exponent, 0);
		c = tmp2;
		//disp(c);
	
	}
	return 0;
}
