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
	int iterations = 1;
	double exponent = 20.48;
	int size = 1000 * 1000;
	int ndim1 = 2;
	int dim1[2] = {1000,1000};
	Matrix * tmp1 = onesM(ndim1, dim1);
	Matrix * a = tmp1;
	double* lhs_data1 = i_to_d(a);
	for (int iter1 = 1; iter1 <= size; iter1++) {
		int tmp3 = pow(iter1, 2);
		double tmp2 = tmp3 + 0.5;
		int idx1 = convertSubscript(ndim1, dim1, iter1);
		lhs_data1[idx1] = tmp2;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim1; iter2++)
	{
		size1 *= dim1[iter2];
	}
	Matrix *mat1 = createM(ndim1, dim1, 1);
	writeM(mat1, size1, lhs_data1);
	for (int iter3 = 1; iter3 <= iterations; iter3++) {
		Matrix * tmp4 = mpowerM(mat1, &exponent, 1);
		Matrix * c = tmp4;
		//disp(c);
	
	}
	return 0;
}
