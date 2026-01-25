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
	int iterations = 100;
	int size = 100;
	int ndim1 = 2;
	int dim1[2] = {100,100};
	Matrix * tmp1 = onesM(ndim1, dim1);
	Matrix * a = tmp1;
	complex* lhs_data1 = i_to_c(a);
	for (int iter1 = 1; iter1 <= size; iter1++) {
		for (int iter2 = 1; iter2 <= size; iter2++) {
			complex tmp3 = cpow(((iter1 - 1) * size + iter2), 2.1);
			int tmp4 = ((iter1 - 1) * size + iter2) % 7;
			complex tmp2 = tmp3 + 0.5 + tmp4;
			lhs_data1[(iter2-1) + (iter1-1)*100 + (1-1)*100*100 + (1-1)*100*100*1] = tmp2;
		
		}
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter3 = 0 ; iter3 < ndim1; iter3++)
	{
		size1 *= dim1[iter3];
	}
	Matrix *mat1 = createM(ndim1, dim1, 2);
	writeM(mat1, size1, lhs_data1);
	for (int iter4 = 1; iter4 <= iterations; iter4++) {
		complex complex_one = 1;
		Matrix * V1 = NULL;
		Matrix * lambda1 = NULL;
		Matrix * evals1 = NULL;
		Matrix * evecs1 = NULL;
		eigM(mat1, &evals1, &evecs1);
		lambda1 = scaleM(evals1, &complex_one, COMPLEX);
		V1 = scaleM(evecs1, &complex_one, COMPLEX);
	
	}
	return 0;
}
