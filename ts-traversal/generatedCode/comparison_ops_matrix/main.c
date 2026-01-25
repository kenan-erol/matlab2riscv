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
	matrices = malloc(12*sizeof(*matrices));
		        
	int ndim1 = 2;
	int dim1[2] = {12,1};
	int idx1 = convertSubscript(ndim1, dim1, 1);
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim2, dim2);
	matrices[idx1] = tmp1;
	int idx2 = convertSubscript(ndim1, dim1, 2);
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp2 = onesM(ndim3, dim3);
	matrices[idx2] = tmp2;
	int idx3 = convertSubscript(ndim1, dim1, 3);
	Matrix * tmp3 = identityM(3);
	matrices[idx3] = tmp3;
	int idx4 = convertSubscript(ndim1, dim1, 4);
	Matrix * tmp4 = identityM(3);
	complex scalar1 = (4.2 - 0.03*I);
	Matrix * tmp5 = scaleM(tmp4, &scalar1, 2);
	matrices[idx4] = tmp5;
	int idx5 = convertSubscript(ndim1, dim1, 5);
	int ndim4 = 2;
	int dim4[2] = {3,3};
	Matrix * tmp6 = zerosM(ndim4, dim4);
	matrices[idx5] = tmp6;
	int* lhs_data1 = i_to_i(matrices[idx5]);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp7 = iter1 * iter1;
		int idx6 = convertSubscript(ndim4, dim4, iter1);
		lhs_data1[idx6] = tmp7;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim4; iter2++)
	{
		size1 *= dim4[iter2];
	}
	Matrix *mat1 = createM(ndim4, dim4, 0);
	writeM(mat1, size1, lhs_data1);
	matrices[idx5] = mat1;
	Matrix * tmp8 = transposeM(matrices[idx5]);
	matrices[idx5] = tmp8;
	int idx7 = convertSubscript(ndim1, dim1, 6);
	int ndim5 = 2;
	int dim5[2] = {3,3};
	Matrix * tmp9 = zerosM(ndim5, dim5);
	matrices[idx7] = tmp9;
	double* lhs_data2 = i_to_d(matrices[idx7]);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		double tmp10 = iter3 * iter3 + 0.5;
		int idx8 = convertSubscript(ndim5, dim5, iter3);
		lhs_data2[idx8] = tmp10;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim5; iter4++)
	{
		size2 *= dim5[iter4];
	}
	Matrix *mat2 = createM(ndim5, dim5, 1);
	writeM(mat2, size2, lhs_data2);
	matrices[idx7] = mat2;
	Matrix * tmp11 = transposeM(matrices[idx7]);
	matrices[idx7] = tmp11;
	int idx9 = convertSubscript(ndim1, dim1, 7);
	int ndim6 = 2;
	int dim6[2] = {3,3};
	Matrix * tmp12 = zerosM(ndim6, dim6);
	matrices[idx9] = tmp12;
	complex* lhs_data3 = i_to_c(matrices[idx9]);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		complex tmp13 = iter5 * iter5 + 0.5*I;
		int idx10 = convertSubscript(ndim6, dim6, iter5);
		lhs_data3[idx10] = tmp13;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim6; iter6++)
	{
		size3 *= dim6[iter6];
	}
	Matrix *mat3 = createM(ndim6, dim6, 2);
	writeM(mat3, size3, lhs_data3);
	matrices[idx9] = mat3;
	Matrix * tmp14 = transposeM(matrices[idx9]);
	matrices[idx9] = tmp14;
	int idx11 = convertSubscript(ndim1, dim1, 8);
	int ndim7 = 2;
	int dim7[2] = {3,3};
	Matrix * tmp15 = zerosM(ndim7, dim7);
	matrices[idx11] = tmp15;
	int* lhs_data4 = i_to_i(matrices[idx11]);
	for (int iter7 = 1; iter7 <= 9; iter7++) {
		int tmp16 = (iter7 - 5) * iter7;
		int idx12 = convertSubscript(ndim7, dim7, iter7);
		lhs_data4[idx12] = tmp16;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter8 = 0 ; iter8 < ndim7; iter8++)
	{
		size4 *= dim7[iter8];
	}
	Matrix *mat4 = createM(ndim7, dim7, 0);
	writeM(mat4, size4, lhs_data4);
	matrices[idx11] = mat4;
	Matrix * tmp17 = transposeM(matrices[idx11]);
	matrices[idx11] = tmp17;
	int idx13 = convertSubscript(ndim1, dim1, 9);
	int ndim8 = 2;
	int dim8[2] = {3,3};
	Matrix * tmp18 = zerosM(ndim8, dim8);
	matrices[idx13] = tmp18;
	double* lhs_data5 = i_to_d(matrices[idx13]);
	for (int iter9 = 1; iter9 <= 9; iter9++) {
		double tmp19 = (iter9 - 8.2) * iter9 + 0.5;
		int idx14 = convertSubscript(ndim8, dim8, iter9);
		lhs_data5[idx14] = tmp19;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter10 = 0 ; iter10 < ndim8; iter10++)
	{
		size5 *= dim8[iter10];
	}
	Matrix *mat5 = createM(ndim8, dim8, 1);
	writeM(mat5, size5, lhs_data5);
	matrices[idx13] = mat5;
	Matrix * tmp20 = transposeM(matrices[idx13]);
	matrices[idx13] = tmp20;
	int idx15 = convertSubscript(ndim1, dim1, 10);
	int ndim9 = 2;
	int dim9[2] = {3,3};
	Matrix * tmp21 = zerosM(ndim9, dim9);
	matrices[idx15] = tmp21;
	complex* lhs_data6 = i_to_c(matrices[idx15]);
	for (int iter11 = 1; iter11 <= 9; iter11++) {
		complex tmp22 = (iter11 - 5.89) * (iter11) + ((0.5) * (4 - iter11)) * 1*I;
		int idx16 = convertSubscript(ndim9, dim9, iter11);
		lhs_data6[idx16] = tmp22;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter12 = 0 ; iter12 < ndim9; iter12++)
	{
		size6 *= dim9[iter12];
	}
	Matrix *mat6 = createM(ndim9, dim9, 2);
	writeM(mat6, size6, lhs_data6);
	matrices[idx15] = mat6;
	Matrix * tmp23 = transposeM(matrices[idx15]);
	matrices[idx15] = tmp23;
	int idx17 = convertSubscript(ndim1, dim1, 11);
	
	int ndim10 = 2;
	int dim10[2] = {3,3};
	matrices[idx17] = createM(ndim10, dim10, 0);
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
	
	int ndim11 = 2;
	int dim11[2] = {3,3};
	matrices[idx18] = createM(ndim11, dim11, 1);
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
	
	for (int iter13 = 1; iter13 <= 12; iter13++) {
		for (int iter14 = 1; iter14 <= 12; iter14++) {
			printf("i = %d, j = %d\n", iter13, iter14);
			int idx19 = convertSubscript(ndim1, dim1, iter13);
			int idx20 = convertSubscript(ndim1, dim1, iter14);
			Matrix * tmp24 = matlab_ltM(matrices[idx19], matrices[idx20]);
			printM(tmp24);
			Matrix * tmp25 = matlab_leM(matrices[idx19], matrices[idx20]);
			printM(tmp25);
			Matrix * tmp26 = matlab_gtM(matrices[idx19], matrices[idx20]);
			printM(tmp26);
			Matrix * tmp27 = matlab_geM(matrices[idx19], matrices[idx20]);
			printM(tmp27);
			Matrix * tmp28 = neM(matrices[idx19], matrices[idx20]);
			printM(tmp28);
			Matrix * tmp29 = equalM(matrices[idx19], matrices[idx20]);
			printM(tmp29);
			Matrix * tmp30 = pairwise_maxM(matrices[idx19], matrices[idx20]);
			printM(tmp30);
			Matrix * tmp31 = pairwise_minM(matrices[idx19], matrices[idx20]);
			printM(tmp31);
			if ((iter13 == 4 || iter13 == 7 || iter13 == 10 || iter14 == 4 || iter14 == 7 || iter14 == 10)) {
				Matrix * tmp32 = pairwise_maxM(matrices[idx19], matrices[idx20]);
				printM(tmp32);
				
				//disp(min(matrices{i} , matrices{j}));
				} else if ((iter13 == 6 || iter13 == 9 || iter13 == 12 || iter14 == 6 || iter14 == 9 || iter14 == 12)) {
				Matrix * tmp33 = pairwise_minM(matrices[idx19], matrices[idx20]);
				printM(tmp33);
				//disp(max(matrices{i} , matrices{j}));
				
				} else {
				Matrix * tmp34 = pairwise_maxM(matrices[idx19], matrices[idx20]);
				printM(tmp34);
				Matrix * tmp35 = pairwise_minM(matrices[idx19], matrices[idx20]);
				printM(tmp35);

				
			
			}
			printf("\n%s\n", "\n-------------------------\n");
		
		}
	
	}
	return 0;
}
