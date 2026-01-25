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
	//test1
	
	int ndim1 = 2;
	int dim1[2] = {1,1};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 1*sizeof(*input1));
	input1[0] = 1;
	writeM( a, 1, input1);
	free(input1);
	
	printM(a);
	int d1;
	detM(a, &d1);
	printf("\n%d\n", d1);
	//test2
	
	int ndim2 = 2;
	int dim2[2] = {2,2};
	a = createM(ndim2, dim2, 2);
	complex *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 26 + 1*I;
	input2[1] = 3 - 8*I;
	input2[2] = 20*I;
	input2[3] = 1 + 25*I;
	writeM( a, 4, input2);
	free(input2);
	
	printM(a);
	complex d2;
	detM(a, &d2);
	printf("\n%.4f + %.4fi\n", creal(d2), cimag(d2));
	//test3
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim3, dim3);
	a = tmp1;
	int* lhs_data1 = i_to_i(a);
	int counter = 1;
	for (int iter1 = 1; iter1 <= 3; iter1++) {
		for (int iter2 = 1; iter2 <= 3; iter2++) {
			int tmp2 = counter * counter;
			lhs_data1[(iter2-1) + (iter1-1)*3 + (1-1)*3*3 + (1-1)*3*3*1] = tmp2;
			counter = counter + 1;
		
		}
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter3 = 0 ; iter3 < ndim3; iter3++)
	{
		size1 *= dim3[iter3];
	}
	Matrix *mat1 = createM(ndim3, dim3, 0);
	writeM(mat1, size1, lhs_data1);
	printM(mat1);
	int d3;
	detM(mat1, &d3);
	printf("\n%d\n", d3);
	//testn
	int ndim4 = 2;
	int dim4[2] = {5,5};
	Matrix * tmp3 = zerosM(ndim4, dim4);
	a = tmp3;
	int* lhs_data2 = i_to_i(a);
	counter = 0;
	for (int iter4 = 0; iter4 <= 4; iter4++) {
		for (int iter5 = 0; iter5 <= 4; iter5++) {
			counter = counter + 1;
			int tmp4 = (counter - 1) % 2;
			if ((tmp4 == 0)) {
				int tmp6 = (counter + iter4) % 7;
				int tmp5 = tmp6;
				int idx1 = convertSubscript(ndim4, dim4, counter);
				lhs_data2[idx1] = tmp5;
				
				} else {
				int tmp8 = (counter + iter5) % 7;
				int tmp7 = -1 * tmp8;
				int idx2 = convertSubscript(ndim4, dim4, counter);
				lhs_data2[idx2] = tmp7;
				
			
			}
		
		}
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter6 = 0 ; iter6 < ndim4; iter6++)
	{
		size2 *= dim4[iter6];
	}
	Matrix *mat2 = createM(ndim4, dim4, 0);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp9 = transposeM(mat2);
	a = tmp9;
	printM(a);
	int d4;
	detM(a, &d4);
	printf("\n%d\n", d4);
	//non_square
	int ndim5 = 2;
	int dim5[2] = {3, 2};
	Matrix * tmp10 = zerosM(ndim5, dim5);
	a = tmp10;
	complex* lhs_data3 = i_to_c(a);
	complex tmp11 = 26 + 1*I;
	int idx3 = convertSubscript(ndim5, dim5, 1);
	lhs_data3[idx3] = tmp11;
	complex tmp12 = 3 - 8*I;
	int idx4 = convertSubscript(ndim5, dim5, 2);
	lhs_data3[idx4] = tmp12;
	complex tmp13 = 20*I;
	int idx5 = convertSubscript(ndim5, dim5, 3);
	lhs_data3[idx5] = tmp13;
	complex tmp14 = 1 + 25*I;
	int idx6 = convertSubscript(ndim5, dim5, 4);
	lhs_data3[idx6] = tmp14;
	int tmp15 = 0;
	int idx7 = convertSubscript(ndim5, dim5, 5);
	lhs_data3[idx7] = tmp15;
	int tmp16 = 1;
	int idx8 = convertSubscript(ndim5, dim5, 6);
	lhs_data3[idx8] = tmp16;
	// Write matrix mat3
	int size3 = 1;
	for (int iter7 = 0 ; iter7 < ndim5; iter7++)
	{
		size3 *= dim5[iter7];
	}
	Matrix *mat3 = createM(ndim5, dim5, 2);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp17 = transposeM(mat3);
	a = tmp17;
	printM(a);
	return 0;
}
