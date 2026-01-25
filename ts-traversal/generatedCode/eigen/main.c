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
	
	Matrix **matrices = NULL;
	matrices = malloc(15*sizeof(*matrices));
		        
	int ndim1 = 2;
	int dim1[2] = {15,1};
	int idx1 = convertSubscript(ndim1, dim1, 1);
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim2, dim2);
	matrices[idx1] = tmp1;
	int idx2 = convertSubscript(ndim1, dim1, 2);
	Matrix * tmp2 = identityM(3);
	int scalar1 = 2;
	Matrix * tmp3 = scaleM(tmp2, &scalar1, 0);
	matrices[idx2] = tmp3;
	int idx3 = convertSubscript(ndim1, dim1, 3);
	Matrix * tmp4 = identityM(3);
	matrices[idx3] = tmp4;
	int idx4 = convertSubscript(ndim1, dim1, 4);
	Matrix * tmp5 = identityM(3);
	complex scalar2 = (4.2 - 0.03*I);
	Matrix * tmp6 = scaleM(tmp5, &scalar2, 2);
	matrices[idx4] = tmp6;
	int idx5 = convertSubscript(ndim1, dim1, 5);
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp7 = zerosM(ndim3, dim3);
	matrices[idx5] = tmp7;
	int* lhs_data1 = i_to_i(matrices[idx5]);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp8 = iter1 * iter1;
		int idx6 = convertSubscript(ndim3, dim3, iter1);
		lhs_data1[idx6] = tmp8;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim3; iter2++)
	{
		size1 *= dim3[iter2];
	}
	Matrix *mat1 = createM(ndim3, dim3, 0);
	writeM(mat1, size1, lhs_data1);
	matrices[idx5] = mat1;
	Matrix * tmp9 = transposeM(matrices[idx5]);
	matrices[idx5] = tmp9;
	int idx7 = convertSubscript(ndim1, dim1, 6);
	int ndim4 = 2;
	int dim4[2] = {3,3};
	Matrix * tmp10 = zerosM(ndim4, dim4);
	matrices[idx7] = tmp10;
	double* lhs_data2 = i_to_d(matrices[idx7]);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		double tmp11 = iter3 * iter3 + 0.5;
		int idx8 = convertSubscript(ndim4, dim4, iter3);
		lhs_data2[idx8] = tmp11;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim4; iter4++)
	{
		size2 *= dim4[iter4];
	}
	Matrix *mat2 = createM(ndim4, dim4, 1);
	writeM(mat2, size2, lhs_data2);
	matrices[idx7] = mat2;
	Matrix * tmp12 = transposeM(matrices[idx7]);
	matrices[idx7] = tmp12;
	int idx9 = convertSubscript(ndim1, dim1, 7);
	int ndim5 = 2;
	int dim5[2] = {3,3};
	Matrix * tmp13 = zerosM(ndim5, dim5);
	matrices[idx9] = tmp13;
	complex* lhs_data3 = i_to_c(matrices[idx9]);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		complex tmp14 = iter5 * iter5 + 0.5*I;
		int idx10 = convertSubscript(ndim5, dim5, iter5);
		lhs_data3[idx10] = tmp14;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim5; iter6++)
	{
		size3 *= dim5[iter6];
	}
	Matrix *mat3 = createM(ndim5, dim5, 2);
	writeM(mat3, size3, lhs_data3);
	matrices[idx9] = mat3;
	Matrix * tmp15 = transposeM(matrices[idx9]);
	matrices[idx9] = tmp15;
	int idx11 = convertSubscript(ndim1, dim1, 8);
	int ndim6 = 2;
	int dim6[2] = {3,3};
	Matrix * tmp16 = zerosM(ndim6, dim6);
	matrices[idx11] = tmp16;
	int* lhs_data4 = i_to_i(matrices[idx11]);
	for (int iter7 = 1; iter7 <= 9; iter7++) {
		int tmp17 = (iter7 - 5) * iter7;
		int idx12 = convertSubscript(ndim6, dim6, iter7);
		lhs_data4[idx12] = tmp17;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter8 = 0 ; iter8 < ndim6; iter8++)
	{
		size4 *= dim6[iter8];
	}
	Matrix *mat4 = createM(ndim6, dim6, 0);
	writeM(mat4, size4, lhs_data4);
	matrices[idx11] = mat4;
	Matrix * tmp18 = transposeM(matrices[idx11]);
	matrices[idx11] = tmp18;
	int idx13 = convertSubscript(ndim1, dim1, 9);
	int ndim7 = 2;
	int dim7[2] = {3,3};
	Matrix * tmp19 = zerosM(ndim7, dim7);
	matrices[idx13] = tmp19;
	double* lhs_data5 = i_to_d(matrices[idx13]);
	for (int iter9 = 1; iter9 <= 9; iter9++) {
		double tmp20 = (iter9 - 8.2) * iter9 + 0.5;
		int idx14 = convertSubscript(ndim7, dim7, iter9);
		lhs_data5[idx14] = tmp20;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter10 = 0 ; iter10 < ndim7; iter10++)
	{
		size5 *= dim7[iter10];
	}
	Matrix *mat5 = createM(ndim7, dim7, 1);
	writeM(mat5, size5, lhs_data5);
	matrices[idx13] = mat5;
	Matrix * tmp21 = transposeM(matrices[idx13]);
	matrices[idx13] = tmp21;
	int idx15 = convertSubscript(ndim1, dim1, 10);
	int ndim8 = 2;
	int dim8[2] = {3,3};
	Matrix * tmp22 = zerosM(ndim8, dim8);
	matrices[idx15] = tmp22;
	complex* lhs_data6 = i_to_c(matrices[idx15]);
	for (int iter11 = 1; iter11 <= 9; iter11++) {
		complex tmp23 = (iter11 - 5.89) * (iter11) + ((0.5) * (4 - iter11)) * 1*I;
		int idx16 = convertSubscript(ndim8, dim8, iter11);
		lhs_data6[idx16] = tmp23;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter12 = 0 ; iter12 < ndim8; iter12++)
	{
		size6 *= dim8[iter12];
	}
	Matrix *mat6 = createM(ndim8, dim8, 2);
	writeM(mat6, size6, lhs_data6);
	matrices[idx15] = mat6;
	Matrix * tmp24 = transposeM(matrices[idx15]);
	matrices[idx15] = tmp24;
	int idx17 = convertSubscript(ndim1, dim1, 11);
	
	int ndim9 = 2;
	int dim9[2] = {3,3};
	matrices[idx17] = createM(ndim9, dim9, 0);
	int *input1 = NULL;
	input1 = malloc( 9*sizeof(*input1));
	input1[0] = 3;
	input1[1] = -2;
	input1[2] = 0;
	input1[3] = 4;
	input1[4] = -1;
	input1[5] = 0;
	input1[6] = 0;
	input1[7] = 0;
	input1[8] = 1;
	writeM( matrices[idx17], 9, input1);
	free(input1);
	
	int idx18 = convertSubscript(ndim1, dim1, 12);
	
	int ndim10 = 2;
	int dim10[2] = {3,3};
	matrices[idx18] = createM(ndim10, dim10, 1);
	double *input2 = NULL;
	input2 = malloc( 9*sizeof(*input2));
	input2[0] = 11.25;
	input2[1] = -7.525;
	input2[2] = -1.45;
	input2[3] = 11;
	input2[4] = -6.9;
	input2[5] = -2.2;
	input2[6] = 5.5;
	input2[7] = -5.45;
	input2[8] = 2.9;
	writeM( matrices[idx18], 9, input2);
	free(input2);
	
	int idx19 = convertSubscript(ndim1, dim1, 13);
	int ndim11 = 2;
	int dim11[2] = {3,3};
	Matrix * tmp25 = zerosM(ndim11, dim11);
	matrices[idx19] = tmp25;
	int* lhs_data7 = i_to_i(matrices[idx19]);
	for (int iter13 = 1; iter13 <= 9; iter13++) {
		int tmp27 = pow((-1), iter13);
		int tmp26 = tmp27 * iter13 * iter13;
		int idx20 = convertSubscript(ndim11, dim11, iter13);
		lhs_data7[idx20] = tmp26;
	
	}
	// Write matrix mat7
	int size7 = 1;
	for (int iter14 = 0 ; iter14 < ndim11; iter14++)
	{
		size7 *= dim11[iter14];
	}
	Matrix *mat7 = createM(ndim11, dim11, 0);
	writeM(mat7, size7, lhs_data7);
	matrices[idx19] = mat7;
	Matrix * tmp28 = transposeM(matrices[idx19]);
	matrices[idx19] = tmp28;
	// Non-diagonalizeable matrices
	int idx21 = convertSubscript(ndim1, dim1, 14);
	
	int ndim12 = 2;
	int dim12[2] = {3,3};
	matrices[idx21] = createM(ndim12, dim12, 0);
	int *input3 = NULL;
	input3 = malloc( 9*sizeof(*input3));
	input3[0] = 1;
	input3[1] = 1;
	input3[2] = 0;
	input3[3] = 0;
	input3[4] = 1;
	input3[5] = 0;
	input3[6] = 0;
	input3[7] = 0;
	input3[8] = 0;
	writeM( matrices[idx21], 9, input3);
	free(input3);
	
	int idx22 = convertSubscript(ndim1, dim1, 15);
	
	int ndim13 = 2;
	int dim13[2] = {3,3};
	matrices[idx22] = createM(ndim13, dim13, 0);
	int *input4 = NULL;
	input4 = malloc( 9*sizeof(*input4));
	input4[0] = 3;
	input4[1] = 4;
	input4[2] = 3;
	input4[3] = -1;
	input4[4] = 0;
	input4[5] = -1;
	input4[6] = 1;
	input4[7] = 2;
	input4[8] = 3;
	writeM( matrices[idx22], 9, input4);
	free(input4);
	
	// Returns slightly different eigenvectors compared to the C output
	for (int iter15 = 1; iter15 <= 14; iter15++) {
		printf("\n%s\n", "Original\n");
		int idx23 = convertSubscript(ndim1, dim1, iter15);
		printM(matrices[idx23]);
		complex complex_one = 1;
		Matrix * V1 = NULL;
		Matrix * lambda1 = NULL;
		Matrix * evals1 = NULL;
		Matrix * evecs1 = NULL;
		eigM(matrices[idx23], &evals1, &evecs1);
		lambda1 = scaleM(evals1, &complex_one, COMPLEX);
		V1 = scaleM(evecs1, &complex_one, COMPLEX);
		printf("\n%s\n", "Eigenvalues:\n");
		printM(lambda1);
		printf("\n%s\n", "Eigenvectors:\n");
		printM(V1);
	
	}
	return 0;
}
