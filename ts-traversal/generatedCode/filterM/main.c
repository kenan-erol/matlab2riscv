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
	matrices = malloc(13*sizeof(*matrices));
		        
	int ndim1 = 2;
	int dim1[2] = {13,1};
	int idx1 = convertSubscript(ndim1, dim1, 1);
	int ndim2 = 2;
	int dim2[2] = {1, 10};
	Matrix * tmp1 = zerosM(ndim2, dim2);
	matrices[idx1] = tmp1;
	int* lhs_data1 = i_to_i(matrices[idx1]);
	int tmp2 = 1;
	int idx2 = convertSubscript(ndim2, dim2, 1);
	lhs_data1[idx2] = tmp2;
	// Write matrix mat1
	int size1 = 1;
	for (int iter1 = 0 ; iter1 < ndim2; iter1++)
	{
		size1 *= dim2[iter1];
	}
	Matrix *mat1 = createM(ndim2, dim2, 0);
	writeM(mat1, size1, lhs_data1);
	matrices[idx1] = mat1;
	int idx3 = convertSubscript(ndim1, dim1, 2);
	int ndim3 = 2;
	int dim3[2] = {20, 1};
	Matrix * tmp3 = onesM(ndim3, dim3);
	matrices[idx3] = tmp3;
	int idx4 = convertSubscript(ndim1, dim1, 3);
	int ndim4 = 2;
	int dim4[2] = {1, 10};
	Matrix * tmp4 = onesM(ndim4, dim4);
	matrices[idx4] = tmp4;
	int idx5 = convertSubscript(ndim1, dim1, 4);
	int ndim5 = 2;
	int dim5[2] = {20, 1};
	Matrix * tmp5 = onesM(ndim5, dim5);
	complex scalar1 = (4.5 - 0.5*I);
	Matrix * tmp6 = scaleM(tmp5, &scalar1, 2);
	matrices[idx5] = tmp6;
	int idx6 = convertSubscript(ndim1, dim1, 5);
	int ndim6 = 2;
	int dim6[2] = {1, 10};
	Matrix * tmp7 = zerosM(ndim6, dim6);
	matrices[idx6] = tmp7;
	int* lhs_data2 = i_to_i(matrices[idx6]);
	for (int iter2 = 1; iter2 <= 10; iter2++) {
		int tmp8 = iter2;
		int idx7 = convertSubscript(ndim6, dim6, iter2);
		lhs_data2[idx7] = tmp8;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter3 = 0 ; iter3 < ndim6; iter3++)
	{
		size2 *= dim6[iter3];
	}
	Matrix *mat2 = createM(ndim6, dim6, 0);
	writeM(mat2, size2, lhs_data2);
	matrices[idx6] = mat2;
	matrices[idx6] = matrices[idx6];
	int idx8 = convertSubscript(ndim1, dim1, 6);
	int ndim7 = 2;
	int dim7[2] = {1, 10};
	Matrix * tmp9 = zerosM(ndim7, dim7);
	matrices[idx8] = tmp9;
	double* lhs_data3 = i_to_d(matrices[idx8]);
	for (int iter4 = 1; iter4 <= 10; iter4++) {
		double tmp10 = iter4 * iter4 + 0.5;
		int idx9 = convertSubscript(ndim7, dim7, iter4);
		lhs_data3[idx9] = tmp10;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter5 = 0 ; iter5 < ndim7; iter5++)
	{
		size3 *= dim7[iter5];
	}
	Matrix *mat3 = createM(ndim7, dim7, 1);
	writeM(mat3, size3, lhs_data3);
	matrices[idx8] = mat3;
	matrices[idx8] = matrices[idx8];
	int idx10 = convertSubscript(ndim1, dim1, 7);
	int ndim8 = 2;
	int dim8[2] = {20, 1};
	Matrix * tmp11 = onesM(ndim8, dim8);
	matrices[idx10] = tmp11;
	complex* lhs_data4 = i_to_c(matrices[idx10]);
	for (int iter6 = 1; iter6 <= 20; iter6++) {
		complex tmp12 = iter6 * iter6 + 0.5*I;
		int idx11 = convertSubscript(ndim8, dim8, iter6);
		lhs_data4[idx11] = tmp12;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter7 = 0 ; iter7 < ndim8; iter7++)
	{
		size4 *= dim8[iter7];
	}
	Matrix *mat4 = createM(ndim8, dim8, 2);
	writeM(mat4, size4, lhs_data4);
	matrices[idx10] = mat4;
	matrices[idx10] = matrices[idx10];
	int idx12 = convertSubscript(ndim1, dim1, 8);
	int ndim9 = 2;
	int dim9[2] = {20, 1};
	Matrix * tmp13 = onesM(ndim9, dim9);
	matrices[idx12] = tmp13;
	int* lhs_data5 = i_to_i(matrices[idx12]);
	for (int iter8 = 1; iter8 <= 20; iter8++) {
		int tmp14 = (iter8 - 5) * iter8;
		int idx13 = convertSubscript(ndim9, dim9, iter8);
		lhs_data5[idx13] = tmp14;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter9 = 0 ; iter9 < ndim9; iter9++)
	{
		size5 *= dim9[iter9];
	}
	Matrix *mat5 = createM(ndim9, dim9, 0);
	writeM(mat5, size5, lhs_data5);
	matrices[idx12] = mat5;
	matrices[idx12] = matrices[idx12];
	int idx14 = convertSubscript(ndim1, dim1, 9);
	int ndim10 = 2;
	int dim10[2] = {20, 1};
	Matrix * tmp15 = onesM(ndim10, dim10);
	matrices[idx14] = tmp15;
	double* lhs_data6 = i_to_d(matrices[idx14]);
	for (int iter10 = 1; iter10 <= 20; iter10++) {
		double tmp16 = (iter10 - 8.5) * iter10 + 0.5;
		int idx15 = convertSubscript(ndim10, dim10, iter10);
		lhs_data6[idx15] = tmp16;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter11 = 0 ; iter11 < ndim10; iter11++)
	{
		size6 *= dim10[iter11];
	}
	Matrix *mat6 = createM(ndim10, dim10, 1);
	writeM(mat6, size6, lhs_data6);
	matrices[idx14] = mat6;
	matrices[idx14] = matrices[idx14];
	int idx16 = convertSubscript(ndim1, dim1, 10);
	int ndim11 = 2;
	int dim11[2] = {1, 10};
	Matrix * tmp17 = zerosM(ndim11, dim11);
	matrices[idx16] = tmp17;
	complex* lhs_data7 = i_to_c(matrices[idx16]);
	for (int iter12 = 1; iter12 <= 10; iter12++) {
		complex tmp18 = (iter12 - 5.5) * (iter12) + ((0.5) * (4 - iter12)) * 1*I;
		int idx17 = convertSubscript(ndim11, dim11, iter12);
		lhs_data7[idx17] = tmp18;
	
	}
	// Write matrix mat7
	int size7 = 1;
	for (int iter13 = 0 ; iter13 < ndim11; iter13++)
	{
		size7 *= dim11[iter13];
	}
	Matrix *mat7 = createM(ndim11, dim11, 2);
	writeM(mat7, size7, lhs_data7);
	matrices[idx16] = mat7;
	matrices[idx16] = matrices[idx16];
	int idx18 = convertSubscript(ndim1, dim1, 11);
	
	int ndim12 = 2;
	int dim12[2] = {1,10};
	matrices[idx18] = createM(ndim12, dim12, 1);
	double *input1 = NULL;
	input1 = malloc( 10*sizeof(*input1));
	input1[0] = 3;
	input1[1] = -2;
	input1[2] = 0;
	input1[3] = 4;
	input1[4] = -1;
	input1[5] = 0;
	input1[6] = 0;
	input1[7] = 0;
	input1[8] = 1;
	input1[9] = 2.5;
	writeM( matrices[idx18], 10, input1);
	free(input1);
	
	int idx19 = convertSubscript(ndim1, dim1, 12);
	
	int ndim13 = 2;
	int dim13[2] = {1,10};
	matrices[idx19] = createM(ndim13, dim13, 1);
	double *input2 = NULL;
	input2 = malloc( 10*sizeof(*input2));
	input2[0] = 3;
	input2[1] = -2;
	input2[2] = 0;
	input2[3] = 4;
	input2[4] = -1;
	input2[5] = 0;
	input2[6] = 0;
	input2[7] = 0;
	input2[8] = 1;
	input2[9] = 2.5;
	writeM( matrices[idx19], 10, input2);
	free(input2);
	
	int idx20 = convertSubscript(ndim1, dim1, 13);
	
	int ndim14 = 2;
	int dim14[2] = {1,5};
	matrices[idx20] = createM(ndim14, dim14, 0);
	int *input3 = NULL;
	input3 = malloc( 5*sizeof(*input3));
	input3[0] = 1;
	input3[1] = 2;
	input3[2] = 3;
	input3[3] = 4;
	input3[4] = 5;
	writeM( matrices[idx20], 5, input3);
	free(input3);
	
	for (int iter14 = 1; iter14 <= 13; iter14++) {
		printf("\n%s\n", "b\n");
		int idx21 = convertSubscript(ndim1, dim1, iter14);
		printM(matrices[idx21]);
		for (int iter15 = 1; iter15 <= 13; iter15++) {
			printf("\n%s\n", "\na\n");
			int idx22 = convertSubscript(ndim1, dim1, iter15);
			printM(matrices[idx22]);
			for (int iter16 = 1; iter16 <= 13; iter16++) {
				printf("\n%s\n", "\nx\n");
				int idx23 = convertSubscript(ndim1, dim1, iter16);
				printM(matrices[idx23]);
				printf("\n%s\n", "\n");
				//[y, sf] = filter(matrices{i}, matrices{j}, matrices{k});
				int state_size1[1] = {(int) fmax(getsizeM(matrices[idx21]), getsizeM(matrices[idx22])) - 1};
				Matrix * zero1 = zerosM(1, state_size1);
				Matrix * tmp19 = filterM(matrices[idx21], matrices[idx22], matrices[idx23], &zero1);
				Matrix * y = tmp19;
				printf("\n%s\n", "\ny\n");
				printM(y);
				printf("\n%s\n", "\n");
				//disp(sf);
			
			}
		
		}
	
	}
	return 0;
}
