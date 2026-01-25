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
	int ndim1 = 3;
	int dim1[3] = {2, 3, 5};
	Matrix * tmp1 = zerosM(ndim1, dim1);
	Matrix * a = tmp1;
	double* lhs_data1 = i_to_d(a);
	int counter = 0;
	// Method 1 to create 3D matrix
	// Creates the matrix row-major to match C's implementation
	// Note that in Octave, the i (row) loop is outside the j (column) loop. this is
	// because Octave is natively column-major, so we must assign carefully.
	for (int iter1 = 1; iter1 <= 5; iter1++) {
		for (int iter2 = 1; iter2 <= 2; iter2++) {
			for (int iter3 = 1; iter3 <= 3; iter3++) {
				double tmp2 = counter * counter + 0.5;
				lhs_data1[(iter3-1) + (iter2-1)*3 + (iter1-1)*2*3 + (1-1)*2*3*1] = tmp2;
				counter = counter + 1;
			
			}
		
		}
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter4 = 0 ; iter4 < ndim1; iter4++)
	{
		size1 *= dim1[iter4];
	}
	Matrix *mat1 = createM(ndim1, dim1, 1);
	writeM(mat1, size1, lhs_data1);
	printM(mat1);
	// Flat indexing in C must be must be matched in Octave by flipping the row & column iteration
	for (int iter5 = 1; iter5 <= 5; iter5++) {
		for (int iter6 = 1; iter6 <= 2; iter6++) {
			for (int iter7 = 1; iter7 <= 3; iter7++) {
				double tmp3;
				indexM(mat1, &tmp3, 3, iter6, iter7, iter5);
				printf("\n%.4f\n", tmp3);
			
			}
		
		}
	
	}
	printf("\n%s\n", "\n");
	// Normal indexing in C and normal indexing in Octave are the same
	for (int iter8 = 1; iter8 <= 5; iter8++) {
		for (int iter9 = 1; iter9 <= 3; iter9++) {
			for (int iter10 = 1; iter10 <= 2; iter10++) {
				double tmp4;
				indexM(mat1, &tmp4, 3, iter10, iter9, iter8);
				printf("\n%.4f\n", tmp4);
			
			}
		
		}
	
	}
	printf("\n%s\n", "\n");
	// Flat indexing in Octave must be matched by normal indexing in C
	for (int iter11 = 1; iter11 <= 30; iter11++) {
		int idx1 = convertSubscript(ndim1, dim1, iter11);
		double tmp5;
		indexM(mat1, &tmp5, 1, idx1+1);
		printf("\n%.4f\n", tmp5);
	
	}
	printf("\n%s\n", "\n");
	// Method 2 to create 3D matrix
	// Creates the matrix column-major to match Octave's implementation
	int ndim2 = 3;
	int dim2[3] = {2, 3, 5};
	Matrix * tmp6 = zerosM(ndim2, dim2);
	a = tmp6;
	double* lhs_data2 = i_to_d(a);
	counter = 0;
	for (int iter12 = 1; iter12 <= 30; iter12++) {
		double tmp7 = counter * counter + 0.5;
		int idx2 = convertSubscript(ndim2, dim2, iter12);
		lhs_data2[idx2] = tmp7;
		counter = counter + 1;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter13 = 0 ; iter13 < ndim2; iter13++)
	{
		size2 *= dim2[iter13];
	}
	Matrix *mat2 = createM(ndim2, dim2, 1);
	writeM(mat2, size2, lhs_data2);
	printM(mat2);
	// Flat indexing in C must be must be matched in Octave by flipping the row & column iteration
	for (int iter14 = 1; iter14 <= 5; iter14++) {
		for (int iter15 = 1; iter15 <= 2; iter15++) {
			for (int iter16 = 1; iter16 <= 3; iter16++) {
				double tmp8;
				indexM(mat2, &tmp8, 3, iter15, iter16, iter14);
				printf("\n%.4f\n", tmp8);
			
			}
		
		}
	
	}
	printf("\n%s\n", "\n");
	// Normal indexing in C and normal indexing in Octave are the same
	for (int iter17 = 1; iter17 <= 5; iter17++) {
		for (int iter18 = 1; iter18 <= 3; iter18++) {
			for (int iter19 = 1; iter19 <= 2; iter19++) {
				double tmp9;
				indexM(mat2, &tmp9, 3, iter19, iter18, iter17);
				printf("\n%.4f\n", tmp9);
			
			}
		
		}
	
	}
	printf("\n%s\n", "\n");
	// Flat indexing in Octave must be matched by normal indexing in C
	for (int iter20 = 1; iter20 <= 30; iter20++) {
		int idx3 = convertSubscript(ndim1, dim1, iter20);
		double tmp10;
		indexM(mat2, &tmp10, 1, idx3+1);
		printf("\n%.4f\n", tmp10);
	
	}
	printf("\n%s\n", "\n");
	return 0;
}
