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
	int dim1[2] = {3,3};
	Matrix * a = createM(ndim1, dim1, 2);
	complex *input1 = NULL;
	input1 = malloc( 9*sizeof(*input1));
	input1[0] = 0;
	input1[1] = 10;
	input1[2] = 10*I;
	input1[3] = 10.102;
	input1[4] = 10.102 + 0.5*I;
	input1[5] = -12*I;
	input1[6] = -0.0002 - 0.1*I;
	input1[7] = -100.01*I;
	input1[8] = 81;
	writeM( a, 9, input1);
	free(input1);
	
	printM(a);
	
	int ndim2 = 2;
	int dim2[2] = {1,2};
	Matrix * mat1 = createM(ndim2, dim2, 0);
	int *input2 = NULL;
	input2 = malloc( 2*sizeof(*input2));
	input2[0] = 9;
	input2[1] = 1;
	writeM( mat1, 2, input2);
	free(input2);
	
	int * dim3 = i_to_i(mat1);
	int ndim3 = 2;
	Matrix * tmp1 = reshapeM(a, ndim3, dim3);
	Matrix * b = tmp1;
	printM(b);
	return 0;
}
