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
	int exponent = 3;
	int ndim1 = 2;
	int dim1[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim1, dim1);
	Matrix * a = tmp1;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp3 = pow((-1), (iter1 + 1));
		int tmp2 = (tmp3) * (iter1 - 1);
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
	Matrix * tmp4 = transposeM(mat1);
	a = tmp4;
	printM(a);
	Matrix * tmp5 = sinM(a);
	printM(tmp5);
	Matrix * tmp6 = sindM(a);
	printM(tmp6);
	Matrix * tmp7 = cosM(a);
	printM(tmp7);
	Matrix * tmp8 = cosdM(a);
	printM(tmp8);
	Matrix * tmp9 = tanM(a);
	printM(tmp9);
	Matrix * tmp10 = tandM(a);
	printM(tmp10);
	//double_test
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp11 = zerosM(ndim2, dim2);
	a = tmp11;
	double* lhs_data2 = i_to_d(a);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		int tmp13 = pow((-1), (iter3 + 1));
		double tmp12 = (tmp13) * (iter3 + 0.4);
		int idx2 = convertSubscript(ndim2, dim2, iter3);
		lhs_data2[idx2] = tmp12;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim2; iter4++)
	{
		size2 *= dim2[iter4];
	}
	Matrix *mat2 = createM(ndim2, dim2, 1);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp14 = transposeM(mat2);
	a = tmp14;
	printM(a);
	Matrix * tmp15 = sinM(a);
	printM(tmp15);
	Matrix * tmp16 = sindM(a);
	printM(tmp16);
	Matrix * tmp17 = cosM(a);
	printM(tmp17);
	Matrix * tmp18 = cosdM(a);
	printM(tmp18);
	Matrix * tmp19 = tanM(a);
	printM(tmp19);
	Matrix * tmp20 = tandM(a);
	printM(tmp20);
	//complex_test
	double exponent1 = 1.2;
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp21 = zerosM(ndim3, dim3);
	a = tmp21;
	complex* lhs_data3 = i_to_c(a);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		complex tmp22 = iter5 + 0.5*I;
		int idx3 = convertSubscript(ndim3, dim3, iter5);
		lhs_data3[idx3] = tmp22;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim3; iter6++)
	{
		size3 *= dim3[iter6];
	}
	Matrix *mat3 = createM(ndim3, dim3, 2);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp23 = transposeM(mat3);
	a = tmp23;
	printM(a);
	Matrix * tmp24 = sinM(a);
	printM(tmp24);
	Matrix * tmp25 = sindM(a);
	printM(tmp25);
	Matrix * tmp26 = cosM(a);
	printM(tmp26);
	Matrix * tmp27 = cosdM(a);
	printM(tmp27);
	Matrix * tmp28 = tanM(a);
	printM(tmp28);
	Matrix * tmp29 = tandM(a);
	printM(tmp29);
	return 0;
}
