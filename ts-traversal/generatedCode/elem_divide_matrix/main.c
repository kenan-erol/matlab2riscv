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
	Matrix * tmp1 = onesM(ndim1, dim1);
	Matrix * a = tmp1;
	complex* lhs_data1 = i_to_c(a);
	complex tmp2 = -0.75 + 1*I;
	int idx1 = convertSubscript(ndim1, dim1, 1);
	lhs_data1[idx1] = tmp2;
	complex tmp3 = -0.75 + 1*I;
	int idx2 = convertSubscript(ndim1, dim1, 5);
	lhs_data1[idx2] = tmp3;
	complex tmp4 = -0.75 + 1*I;
	int idx3 = convertSubscript(ndim1, dim1, 9);
	lhs_data1[idx3] = tmp4;
	// Write matrix mat1
	int size1 = 1;
	for (int iter1 = 0 ; iter1 < ndim1; iter1++)
	{
		size1 *= dim1[iter1];
	}
	Matrix *mat1 = createM(ndim1, dim1, 2);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp5 = transposeM(mat1);
	a = tmp5;
	printM(a);
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp6 = onesM(ndim2, dim2);
	Matrix * b = tmp6;
	complex* lhs_data2 = i_to_c(b);
	complex tmp7 = 0.5 + 1*I;
	int idx4 = convertSubscript(ndim2, dim2, 1);
	lhs_data2[idx4] = tmp7;
	complex tmp8 = 0.5 + 1*I;
	int idx5 = convertSubscript(ndim2, dim2, 5);
	lhs_data2[idx5] = tmp8;
	complex tmp9 = 0.5 + 1*I;
	int idx6 = convertSubscript(ndim2, dim2, 9);
	lhs_data2[idx6] = tmp9;
	// Write matrix mat2
	int size2 = 1;
	for (int iter2 = 0 ; iter2 < ndim2; iter2++)
	{
		size2 *= dim2[iter2];
	}
	Matrix *mat2 = createM(ndim2, dim2, 2);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp10 = transposeM(mat2);
	b = tmp10;
	printM(b);
	Matrix * tmp11 = rdivideM(a, b);
	Matrix * c = tmp11;
	printM(c);
	Matrix * tmp12 = identityM(3);
	Matrix * tmp13 = rdivideM(tmp12, a);
	Matrix * d = tmp13;
	printM(d);
	Matrix * tmp14 = identityM(3);
	Matrix * tmp15 = rdivideM(b, tmp14);
	Matrix * e = tmp15;
	printM(e);
	Matrix * tmp16 = ldivideM(a, b);
	c = tmp16;
	printM(c);
	Matrix * tmp17 = identityM(3);
	Matrix * tmp18 = ldivideM(tmp17, a);
	d = tmp18;
	printM(d);
	Matrix * tmp19 = identityM(3);
	Matrix * tmp20 = ldivideM(b, tmp19);
	e = tmp20;
	printM(e);
	return 0;
}
