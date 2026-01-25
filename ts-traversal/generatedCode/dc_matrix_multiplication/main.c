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
	
	
	int ndim2 = 2;
	int dim2[2] = {2,2};
	Matrix * b = createM(ndim2, dim2, 2);
	complex *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 2.1 + 0.5*I;
	input2[1] = 0;
	input2[2] = 0;
	input2[3] = 2.1 + 0.5*I;
	writeM( b, 4, input2);
	free(input2);
	
	Matrix * tmp1 = mtimesM(a, b);
	Matrix * c = tmp1;
	printM(a);
	printM(b);
	printM(c);
	return 0;
}
