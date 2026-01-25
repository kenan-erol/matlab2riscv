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
	// trueTest
	int ndim1 = 2;
	int dim1[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim1, dim1);
	Matrix * a = tmp1;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp2 = iter1 * iter1;
		int idx1 = convertSubscript(ndim1, dim1, iter1);
		lhs_data1[idx1] = tmp2;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim1; iter2++)
	{
		size1 *= dim1[iter2];
	}
	Matrix *mat1 = createM(ndim1, dim1, 0);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp3 = transposeM(mat1);
	a = tmp3;
	printM(a);
	Matrix * b = a;
	printM(b);
	Matrix * c = a;
	printM(c);
	Matrix * d = a;
	printM(d);
	Matrix * tmp4 = equalM(a, b);
	Matrix * tmp5 = equalM(a, c);
	Matrix * tmp6 = andM((tmp4), (tmp5));
	Matrix * tmp7 = equalM(a, d);
	Matrix * tmp8 = andM(tmp6, (tmp7));
	printM(tmp8);
	// falseTest
	printM(a);
	printM(b);
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp9 = zerosM(ndim2, dim2);
	c = tmp9;
	int* lhs_data2 = i_to_i(c);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		int tmp10 = iter3 * iter3;
		int idx2 = convertSubscript(ndim2, dim2, iter3);
		lhs_data2[idx2] = tmp10;
	
	}
	int tmp11 = 10;
	int idx3 = convertSubscript(ndim2, dim2, 2);
	lhs_data2[idx3] = tmp11;
	int tmp12 = 11;
	int idx4 = convertSubscript(ndim2, dim2, 3);
	lhs_data2[idx4] = tmp12;
	int tmp13 = 12;
	int idx5 = convertSubscript(ndim2, dim2, 6);
	lhs_data2[idx5] = tmp13;
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim2; iter4++)
	{
		size2 *= dim2[iter4];
	}
	Matrix *mat2 = createM(ndim2, dim2, 0);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp14 = transposeM(mat2);
	c = tmp14;
	printM(c);
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp15 = zerosM(ndim3, dim3);
	d = tmp15;
	int* lhs_data3 = i_to_i(d);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		int tmp16 = iter5 * iter5;
		int idx6 = convertSubscript(ndim3, dim3, iter5);
		lhs_data3[idx6] = tmp16;
	
	}
	int tmp17 = 13;
	int idx7 = convertSubscript(ndim3, dim3, 4);
	lhs_data3[idx7] = tmp17;
	int tmp18 = 14;
	int idx8 = convertSubscript(ndim3, dim3, 7);
	lhs_data3[idx8] = tmp18;
	int tmp19 = 15;
	int idx9 = convertSubscript(ndim3, dim3, 8);
	lhs_data3[idx9] = tmp19;
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim3; iter6++)
	{
		size3 *= dim3[iter6];
	}
	Matrix *mat3 = createM(ndim3, dim3, 0);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp20 = transposeM(mat3);
	d = tmp20;
	printM(d);
	Matrix * tmp21 = equalM(a, b);
	Matrix * tmp22 = equalM(a, c);
	Matrix * tmp23 = andM((tmp21), (tmp22));
	Matrix * tmp24 = equalM(a, d);
	Matrix * tmp25 = andM(tmp23, (tmp24));
	printM(tmp25);
	return 0;
}
