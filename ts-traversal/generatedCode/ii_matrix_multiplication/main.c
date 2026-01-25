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
	int dim1[2] = {3,2};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 6*sizeof(*input1));
	input1[0] = 1;
	input1[1] = 4;
	input1[2] = 9;
	input1[3] = 16;
	input1[4] = 25;
	input1[5] = 36;
	writeM( a, 6, input1);
	free(input1);
	
	
	int ndim2 = 2;
	int dim2[2] = {3,2};
	Matrix * mat1 = createM(ndim2, dim2, 0);
	int *input2 = NULL;
	input2 = malloc( 6*sizeof(*input2));
	input2[0] = 2;
	input2[1] = 0;
	input2[2] = 0;
	input2[3] = 2;
	input2[4] = 0;
	input2[5] = 0;
	writeM( mat1, 6, input2);
	free(input2);
	
	Matrix * tmp1 = transposeM(mat1);
	Matrix * b = tmp1;
	Matrix * tmp2 = mtimesM(a, b);
	Matrix * c = tmp2;
	printM(a);
	printM(b);
	printM(c);
	return 0;
}
