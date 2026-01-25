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
	//int_test
	
	int ndim1 = 2;
	int dim1[2] = {2,2};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 4*sizeof(*input1));
	input1[0] = 1;
	input1[1] = 2;
	input1[2] = 3;
	input1[3] = 4;
	writeM( a, 4, input1);
	free(input1);
	
	printM(a);
	Matrix * tmp1 = invertM(a);
	printM(tmp1);
	Matrix * tmp2 = invertM(a);
	Matrix * tmp3 = invertM(tmp2);
	printM(tmp3);
	//double_test
	
	int ndim2 = 2;
	int dim2[2] = {2,2};
	a = createM(ndim2, dim2, 1);
	double *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 1.5;
	input2[1] = 2.5;
	input2[2] = 3.5;
	input2[3] = 4.5;
	writeM( a, 4, input2);
	free(input2);
	
	printM(a);
	Matrix * tmp4 = invertM(a);
	printM(tmp4);
	Matrix * tmp5 = invertM(a);
	Matrix * tmp6 = invertM(tmp5);
	printM(tmp6);
	//complex_test
	
	int ndim3 = 2;
	int dim3[2] = {2,2};
	a = createM(ndim3, dim3, 2);
	complex *input3 = NULL;
	input3 = malloc( 4*sizeof(*input3));
	input3[0] = 1.5 + 1*I;
	input3[1] = 2.5 + 2*I;
	input3[2] = 3.5 + 3*I;
	input3[3] = 4.5 + 4*I;
	writeM( a, 4, input3);
	free(input3);
	
	printM(a);
	Matrix * tmp7 = invertM(a);
	printM(tmp7);
	Matrix * tmp8 = invertM(a);
	Matrix * tmp9 = invertM(tmp8);
	printM(tmp9);
	//singular_test
	int ndim4 = 2;
	int dim4[2] = {2,2};
	Matrix * tmp10 = onesM(ndim4, dim4);
	printM(tmp10);
	//non_square_test
	int ndim5 = 2;
	int dim5[2] = {2, 3};
	Matrix * tmp11 = onesM(ndim5, dim5);
	printM(tmp11);
	return 0;
}
