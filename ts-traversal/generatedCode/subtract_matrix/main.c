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
	double scalar1 = 0.5;
	Matrix * tmp2 = scaleM(tmp1, &scalar1, 1);
	Matrix * a = tmp2;
	complex* lhs_data1 = d_to_c(a);
	int ndim2 = getnDimM(a);
	int *dim2 = getDimsM(a);
	int idx1 = convertSubscript(ndim2, dim2, 1);
	complex tmp4;
	indexM(a, &tmp4, 1, idx1+1);
	complex tmp3 = tmp4 + 1*I;
	int idx2 = convertSubscript(ndim2, dim2, 1);
	lhs_data1[idx2] = tmp3;
	int idx3 = convertSubscript(ndim2, dim2, 5);
	double tmp6;
	indexM(a, &tmp6, 1, idx3+1);
	complex tmp5 = tmp6 + 1*I;
	int idx4 = convertSubscript(ndim2, dim2, 5);
	lhs_data1[idx4] = tmp5;
	int idx5 = convertSubscript(ndim2, dim2, 9);
	double tmp8;
	indexM(a, &tmp8, 1, idx5+1);
	complex tmp7 = tmp8 + 1*I;
	int idx6 = convertSubscript(ndim2, dim2, 9);
	lhs_data1[idx6] = tmp7;
	// Write matrix mat1
	int size1 = 1;
	for (int iter1 = 0 ; iter1 < ndim2; iter1++)
	{
		size1 *= dim2[iter1];
	}
	Matrix *mat1 = createM(ndim2, dim2, 2);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp9 = transposeM(mat1);
	a = tmp9;
	printM(a);
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp10 = onesM(ndim3, dim3);
	double scalar2 = 0.5;
	Matrix * tmp11 = scaleM(tmp10, &scalar2, 1);
	Matrix * b = tmp11;
	complex* lhs_data2 = d_to_c(b);
	int ndim4 = getnDimM(b);
	int *dim4 = getDimsM(b);
	complex tmp12 = -0.5 + 1*I;
	int idx7 = convertSubscript(ndim4, dim4, 1);
	lhs_data2[idx7] = tmp12;
	complex tmp13 = -0.5 + 1*I;
	int idx8 = convertSubscript(ndim4, dim4, 5);
	lhs_data2[idx8] = tmp13;
	complex tmp14 = -0.5 + 1*I;
	int idx9 = convertSubscript(ndim4, dim4, 9);
	lhs_data2[idx9] = tmp14;
	// Write matrix mat2
	int size2 = 1;
	for (int iter2 = 0 ; iter2 < ndim4; iter2++)
	{
		size2 *= dim4[iter2];
	}
	Matrix *mat2 = createM(ndim4, dim4, 2);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp15 = transposeM(mat2);
	b = tmp15;
	printM(b);
	Matrix * tmp16 = minusM(a, b);
	Matrix * c = tmp16;
	printM(c);
	Matrix * tmp17 = identityM(3);
	Matrix * tmp18 = minusM(tmp17, a);
	Matrix * d = tmp18;
	printM(d);
	//e = INT_MIN*eye(3)-eye(3);
	//disp(e);
	return 0;
}
