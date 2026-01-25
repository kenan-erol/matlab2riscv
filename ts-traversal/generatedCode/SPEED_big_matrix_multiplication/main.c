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
	int size = 1000 * 1000;
	int ndim1 = 2;
	int dim1[2] = {1000,1000};
	Matrix * tmp1 = onesM(ndim1, dim1);
	Matrix * a = tmp1;
	double* lhs_data1 = i_to_d(a);
	int ndim2 = 2;
	int dim2[2] = {1000,1000};
	Matrix * tmp2 = onesM(ndim2, dim2);
	Matrix * b = tmp2;
	complex* lhs_data2 = i_to_c(b);
	for (int iter1 = 1; iter1 <= size; iter1++) {
		int tmp4 = pow(iter1, 2);
		double tmp3 = tmp4 + 0.5;
		int idx1 = convertSubscript(ndim1, dim1, iter1);
		lhs_data1[idx1] = tmp3;
		int tmp6 = pow(iter1, 2);
		complex tmp5 = (tmp6 + 0.5) * 1*I;
		int idx2 = convertSubscript(ndim2, dim2, iter1);
		lhs_data2[idx2] = tmp5;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim2; iter2++)
	{
		size1 *= dim2[iter2];
	}
	Matrix *mat1 = createM(ndim2, dim2, 1);
	writeM(mat1, size1, lhs_data1);
	// Write matrix mat2
	int size2 = 1;
	for (int iter3 = 0 ; iter3 < ndim2; iter3++)
	{
		size2 *= dim2[iter3];
	}
	Matrix *mat2 = createM(ndim2, dim2, 2);
	writeM(mat2, size2, lhs_data2);
	for (int iter4 = 1; iter4 <= iterations; iter4++) {
		Matrix * tmp7 = mtimesM(mat2, mat1);
		Matrix * c = tmp7;
		//disp(c);
	
	}
	return 0;
}
