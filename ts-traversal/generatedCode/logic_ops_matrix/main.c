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
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 9*sizeof(*input1));
	input1[0] = 0;
	input1[1] = 1;
	input1[2] = 0;
	input1[3] = 1;
	input1[4] = 0;
	input1[5] = 1;
	input1[6] = 0;
	input1[7] = 0;
	input1[8] = 0;
	writeM( a, 9, input1);
	free(input1);
	
	printM(a);
	Matrix * tmp1 = notM(a);
	printM(tmp1);
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp2 = zerosM(ndim2, dim2);
	Matrix * tmp3 = notM(tmp2);
	printM(tmp3);
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp4 = onesM(ndim3, dim3);
	Matrix * tmp5 = notM(tmp4);
	printM(tmp5);
	Matrix * tmp6 = identityM(3);
	Matrix * tmp7 = notM(tmp6);
	printM(tmp7);
	int ndim4 = 2;
	int dim4[2] = {3,3};
	Matrix * tmp8 = zerosM(ndim4, dim4);
	Matrix * tmp9 = andM(a, tmp8);
	printM(tmp9);
	Matrix * tmp10 = identityM(3);
	Matrix * tmp11 = notM(tmp10);
	Matrix * tmp12 = andM(a, tmp11);
	printM(tmp12);
	int ndim5 = 2;
	int dim5[2] = {3,3};
	Matrix * tmp13 = zerosM(ndim5, dim5);
	int ndim6 = 2;
	int dim6[2] = {3,3};
	Matrix * tmp14 = onesM(ndim6, dim6);
	Matrix * tmp15 = notM(tmp14);
	Matrix * tmp16 = orM(tmp13, tmp15);
	printM(tmp16);
	int ndim7 = 2;
	int dim7[2] = {3,3};
	Matrix * tmp17 = onesM(ndim7, dim7);
	Matrix * tmp18 = notM(tmp17);
	Matrix * tmp19 = identityM(3);
	Matrix * tmp20 = orM(tmp18, tmp19);
	printM(tmp20);
	return 0;
}
