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
	int dim1[2] = {3,1};
	Matrix * a = createM(ndim1, dim1, 1);
	double *input1 = NULL;
	input1 = malloc( 3*sizeof(*input1));
	input1[0] = 1;
	input1[1] = 4;
	input1[2] = 9.5;
	writeM( a, 3, input1);
	free(input1);
	
	printM(a);
	return 0;
}
