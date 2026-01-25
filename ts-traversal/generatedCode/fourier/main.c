// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"

// Function declarations
void fourier_script_int(Matrix * a);
void fourier_script_double(Matrix * a);
void fourier_script_complex(Matrix * a);
void fourier_vec_script_int(Matrix * a);
void fourier_vec_script_double(Matrix * a);
void fourier_vec_script_complex(Matrix * a);

// Entry-point function
int main(void) {

	//more off
	
	//format short
	
	//source octaveIncludes.m;
	
	//row_vectors_i
	
	
	int ndim1 = 2;
	int dim1[2] = {1,4};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 4*sizeof(*input1));
	input1[0] = 3;
	input1[1] = -5;
	input1[2] = 0;
	input1[3] = 1;
	writeM( a, 4, input1);
	free(input1);
	
	printM(a);
	fourier_vec_script_int(a);
	fourier_script_int(a);
	//row_vectors_d
	
	
	int ndim2 = 2;
	int dim2[2] = {1,4};
	a = createM(ndim2, dim2, 1);
	double *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 3.25;
	input2[1] = -2;
	input2[2] = 0;
	input2[3] = 10.1;
	writeM( a, 4, input2);
	free(input2);
	
	printM(a);
	fourier_vec_script_double(a);
	fourier_script_double(a);
	//row_vectors_c
	
	
	int ndim3 = 2;
	int dim3[2] = {1,4};
	a = createM(ndim3, dim3, 2);
	complex *input3 = NULL;
	input3 = malloc( 4*sizeof(*input3));
	input3[0] = 3.25;
	input3[1] = -2;
	input3[2] = 0;
	input3[3] = 1 - 1*I;
	writeM( a, 4, input3);
	free(input3);
	
	printM(a);
	fourier_vec_script_complex(a);
	fourier_script_complex(a);
	//column_vectors_i
	
	
	int ndim4 = 2;
	int dim4[2] = {4,1};
	a = createM(ndim4, dim4, 0);
	int *input4 = NULL;
	input4 = malloc( 4*sizeof(*input4));
	input4[0] = 3;
	input4[1] = -5;
	input4[2] = 0;
	input4[3] = 1;
	writeM( a, 4, input4);
	free(input4);
	
	printM(a);
	fourier_vec_script_int(a);
	fourier_script_int(a);
	//column_vectors_d
	
	
	int ndim5 = 2;
	int dim5[2] = {4,1};
	a = createM(ndim5, dim5, 1);
	double *input5 = NULL;
	input5 = malloc( 4*sizeof(*input5));
	input5[0] = 3.25;
	input5[1] = -2;
	input5[2] = 0;
	input5[3] = 10.1;
	writeM( a, 4, input5);
	free(input5);
	
	printM(a);
	fourier_vec_script_double(a);
	fourier_script_double(a);
	//column_vectors_c
	
	
	int ndim6 = 2;
	int dim6[2] = {4,1};
	a = createM(ndim6, dim6, 2);
	complex *input6 = NULL;
	input6 = malloc( 4*sizeof(*input6));
	input6[0] = 3.25;
	input6[1] = -2;
	input6[2] = 0;
	input6[3] = 1 - 1*I;
	writeM( a, 4, input6);
	free(input6);
	
	printM(a);
	fourier_vec_script_complex(a);
	fourier_script_complex(a);
	//matrices_23_i
	
	
	int ndim7 = 2;
	int dim7[2] = {2,3};
	a = createM(ndim7, dim7, 0);
	int *input7 = NULL;
	input7 = malloc( 6*sizeof(*input7));
	input7[0] = 3;
	input7[1] = -2;
	input7[2] = 0;
	input7[3] = 1;
	input7[4] = 5;
	input7[5] = 10;
	writeM( a, 6, input7);
	free(input7);
	
	printM(a);
	fourier_script_int(a);
	//matrices_23_d
	
	
	int ndim8 = 2;
	int dim8[2] = {2,3};
	a = createM(ndim8, dim8, 1);
	double *input8 = NULL;
	input8 = malloc( 6*sizeof(*input8));
	input8[0] = 3.25;
	input8[1] = -2;
	input8[2] = 0;
	input8[3] = 1;
	input8[4] = 5;
	input8[5] = 10;
	writeM( a, 6, input8);
	free(input8);
	
	printM(a);
	fourier_script_double(a);
	//matrices_23_c
	
	
	int ndim9 = 2;
	int dim9[2] = {2,3};
	a = createM(ndim9, dim9, 2);
	complex *input9 = NULL;
	input9 = malloc( 6*sizeof(*input9));
	input9[0] = 3.25;
	input9[1] = -2;
	input9[2] = 0;
	input9[3] = 1;
	input9[4] = 5 - 1*I;
	input9[5] = 10;
	writeM( a, 6, input9);
	free(input9);
	
	printM(a);
	fourier_script_complex(a);
	//matrices_32_i
	
	
	int ndim10 = 2;
	int dim10[2] = {3,2};
	a = createM(ndim10, dim10, 0);
	int *input10 = NULL;
	input10 = malloc( 6*sizeof(*input10));
	input10[0] = 3;
	input10[1] = -2;
	input10[2] = 0;
	input10[3] = 1;
	input10[4] = 5;
	input10[5] = 10;
	writeM( a, 6, input10);
	free(input10);
	
	printM(a);
	fourier_script_int(a);
	//matrices_32_d
	
	
	int ndim11 = 2;
	int dim11[2] = {3,2};
	a = createM(ndim11, dim11, 1);
	double *input11 = NULL;
	input11 = malloc( 6*sizeof(*input11));
	input11[0] = 3.25;
	input11[1] = -2;
	input11[2] = 0;
	input11[3] = 1;
	input11[4] = 5;
	input11[5] = 10;
	writeM( a, 6, input11);
	free(input11);
	
	printM(a);
	fourier_script_double(a);
	//matrices_32_c
	
	
	int ndim12 = 2;
	int dim12[2] = {3,2};
	a = createM(ndim12, dim12, 2);
	complex *input12 = NULL;
	input12 = malloc( 6*sizeof(*input12));
	input12[0] = 3.25;
	input12[1] = -2;
	input12[2] = 0;
	input12[3] = 1;
	input12[4] = 5 - 1*I;
	input12[5] = 10;
	writeM( a, 6, input12);
	free(input12);
	
	printM(a);
	fourier_script_complex(a);
	//matrices_97_i
	
	int ndim13 = 2;
	int dim13[2] = {7, 9};
	Matrix * tmp1 = zerosM(ndim13, dim13);
	a = tmp1;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 63; iter1++) {
		int tmp3 = pow((-1), iter1);
		int tmp4 = pow(iter1, 2);
		int tmp2 = tmp3 * tmp4;
		int idx1 = convertSubscript(ndim13, dim13, iter1);
		lhs_data1[idx1] = tmp2;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim13; iter2++)
	{
		size1 *= dim13[iter2];
	}
	Matrix *mat1 = createM(ndim13, dim13, 0);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp5 = transposeM(mat1);
	a = tmp5;
	printM(a);
	fourier_script_int(a);
	//matrices_97_d
	
	int ndim14 = 2;
	int dim14[2] = {7, 9};
	Matrix * tmp6 = zerosM(ndim14, dim14);
	a = tmp6;
	double* lhs_data2 = i_to_d(a);
	for (int iter3 = 1; iter3 <= 63; iter3++) {
		int tmp8 = pow((-1), iter3);
		int tmp9 = pow(iter3, 2);
		double tmp7 = (double) tmp8 * tmp9 / 17;
		int idx2 = convertSubscript(ndim14, dim14, iter3);
		lhs_data2[idx2] = tmp7;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim14; iter4++)
	{
		size2 *= dim14[iter4];
	}
	Matrix *mat2 = createM(ndim14, dim14, 1);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp10 = transposeM(mat2);
	a = tmp10;
	printM(a);
	fourier_script_double(a);
	//matrices_97_c
	
	int ndim15 = 2;
	int dim15[2] = {7, 9};
	Matrix * tmp11 = zerosM(ndim15, dim15);
	a = tmp11;
	complex* lhs_data3 = i_to_c(a);
	for (int iter5 = 1; iter5 <= 63; iter5++) {
		int tmp13 = pow((-1), iter5);
		complex tmp12 = tmp13 * iter5 - ((complex) iter5) / (17*I);
		int idx3 = convertSubscript(ndim15, dim15, iter5);
		lhs_data3[idx3] = tmp12;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim15; iter6++)
	{
		size3 *= dim15[iter6];
	}
	Matrix *mat3 = createM(ndim15, dim15, 2);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp14 = transposeM(mat3);
	a = tmp14;
	printM(a);
	fourier_script_complex(a);
	return 0;
}


// Subprograms
	
void fourier_script_int(Matrix * a) {
	Matrix * tmp15 = fftM(a);
	printM(tmp15);
	Matrix * tmp16 = ifftM(a);
	printM(tmp16);
}
	
void fourier_script_double(Matrix * a) {
	Matrix * tmp17 = fftM(a);
	printM(tmp17);
	Matrix * tmp18 = ifftM(a);
	printM(tmp18);
}
	
void fourier_script_complex(Matrix * a) {
	Matrix * tmp19 = fftM(a);
	printM(tmp19);
	Matrix * tmp20 = ifftM(a);
	printM(tmp20);
}
	
void fourier_vec_script_int(Matrix * a) {
	for (int iter7 = 1; iter7 <= 20; iter7++) {
		Matrix * tmp21 = fftV(a, iter7);
		printM(tmp21);
		Matrix * tmp22 = ifftV(a, iter7);
		printM(tmp22);
	
	}
}
	
void fourier_vec_script_double(Matrix * a) {
	for (int iter8 = 1; iter8 <= 20; iter8++) {
		Matrix * tmp23 = fftV(a, iter8);
		printM(tmp23);
		Matrix * tmp24 = ifftV(a, iter8);
		printM(tmp24);
	
	}
}
	
void fourier_vec_script_complex(Matrix * a) {
	for (int iter9 = 1; iter9 <= 20; iter9++) {
		Matrix * tmp25 = fftV(a, iter9);
		printM(tmp25);
		Matrix * tmp26 = ifftV(a, iter9);
		printM(tmp26);
	
	}
}