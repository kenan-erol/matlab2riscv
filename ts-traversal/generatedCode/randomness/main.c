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
	// rand_test
	randomSeed();
	int ndim1 = 2;
	int dim1[2] = {3, 4};
	Matrix * tmp1 = randM(ndim1, dim1);
	Matrix * a = tmp1;
	randomSeed();
	randomSeed();
	int ndim2 = 2;
	int dim2[2] = {3, 4};
	Matrix * tmp2 = randM(ndim2, dim2);
	Matrix * b = tmp2;
	randomSeed();
	//randi_test
	randomSeed();
	int ndim3 = 2;
	int dim3[2] = {3, 4};
	Matrix * tmp3 = randiM(ndim3, dim3, 0, 10);
	a = tmp3;
	int ndim4 = 2;
	int dim4[2] = {3,4};
	int scalar1 = 10;
	Matrix * tmp4 = scaleM(onesM(ndim4, dim4), &scalar1, 0);
	Matrix * tmp5 = matlab_leM(a, tmp4);
	printM(tmp5);
	//randn_test
	randomSeed();
	int ndim5 = 2;
	int dim5[2] = {1000,1000};
	Matrix * tmp6 = randnM(ndim5, dim5);
	a = tmp6;
	Matrix * tmp7 = meanM(a);
	Matrix * mu = tmp7;
	Matrix * tmp8 = varM(a);
	Matrix * sigma = tmp8;
	return 0;
}
