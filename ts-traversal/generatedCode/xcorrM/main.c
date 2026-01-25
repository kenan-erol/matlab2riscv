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
	//pkg load signal;
	
	Matrix **matrices = NULL;
	matrices = malloc(12*sizeof(*matrices));
		        
	int ndim1 = 2;
	int dim1[2] = {12,1};
	int idx1 = convertSubscript(ndim1, dim1, 1);
	int ndim2 = 2;
	int dim2[2] = {1, 10};
	Matrix * tmp1 = zerosM(ndim2, dim2);
	matrices[idx1] = tmp1;
	int idx2 = convertSubscript(ndim1, dim1, 2);
	int ndim3 = 2;
	int dim3[2] = {20, 1};
	Matrix * tmp2 = onesM(ndim3, dim3);
	matrices[idx2] = tmp2;
	int idx3 = convertSubscript(ndim1, dim1, 3);
	int ndim4 = 2;
	int dim4[2] = {1, 10};
	Matrix * tmp3 = onesM(ndim4, dim4);
	matrices[idx3] = tmp3;
	int idx4 = convertSubscript(ndim1, dim1, 4);
	int ndim5 = 2;
	int dim5[2] = {20, 1};
	Matrix * tmp4 = onesM(ndim5, dim5);
	complex scalar1 = (4.5 - 0.5*I);
	Matrix * tmp5 = scaleM(tmp4, &scalar1, 2);
	matrices[idx4] = tmp5;
	int idx5 = convertSubscript(ndim1, dim1, 5);
	int ndim6 = 2;
	int dim6[2] = {1, 10};
	Matrix * tmp6 = zerosM(ndim6, dim6);
	matrices[idx5] = tmp6;
	int* lhs_data1 = i_to_i(matrices[idx5]);
	for (int iter1 = 1; iter1 <= 10; iter1++) {
		int tmp7 = iter1 * iter1;
		int idx6 = convertSubscript(ndim6, dim6, iter1);
		lhs_data1[idx6] = tmp7;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim6; iter2++)
	{
		size1 *= dim6[iter2];
	}
	Matrix *mat1 = createM(ndim6, dim6, 0);
	writeM(mat1, size1, lhs_data1);
	matrices[idx5] = mat1;
	matrices[idx5] = matrices[idx5];
	int idx7 = convertSubscript(ndim1, dim1, 6);
	int ndim7 = 2;
	int dim7[2] = {1, 10};
	Matrix * tmp8 = zerosM(ndim7, dim7);
	matrices[idx7] = tmp8;
	double* lhs_data2 = i_to_d(matrices[idx7]);
	for (int iter3 = 1; iter3 <= 10; iter3++) {
		double tmp9 = iter3 * iter3 + 0.5;
		int idx8 = convertSubscript(ndim7, dim7, iter3);
		lhs_data2[idx8] = tmp9;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim7; iter4++)
	{
		size2 *= dim7[iter4];
	}
	Matrix *mat2 = createM(ndim7, dim7, 1);
	writeM(mat2, size2, lhs_data2);
	matrices[idx7] = mat2;
	matrices[idx7] = matrices[idx7];
	int idx9 = convertSubscript(ndim1, dim1, 7);
	int ndim8 = 2;
	int dim8[2] = {20, 1};
	Matrix * tmp10 = onesM(ndim8, dim8);
	matrices[idx9] = tmp10;
	complex* lhs_data3 = i_to_c(matrices[idx9]);
	for (int iter5 = 1; iter5 <= 20; iter5++) {
		complex tmp11 = iter5 * iter5 + 0.5*I;
		int idx10 = convertSubscript(ndim8, dim8, iter5);
		lhs_data3[idx10] = tmp11;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim8; iter6++)
	{
		size3 *= dim8[iter6];
	}
	Matrix *mat3 = createM(ndim8, dim8, 2);
	writeM(mat3, size3, lhs_data3);
	matrices[idx9] = mat3;
	matrices[idx9] = matrices[idx9];
	int idx11 = convertSubscript(ndim1, dim1, 8);
	int ndim9 = 2;
	int dim9[2] = {20, 1};
	Matrix * tmp12 = onesM(ndim9, dim9);
	matrices[idx11] = tmp12;
	int* lhs_data4 = i_to_i(matrices[idx11]);
	for (int iter7 = 1; iter7 <= 20; iter7++) {
		int tmp13 = (iter7 - 5) * iter7;
		int idx12 = convertSubscript(ndim9, dim9, iter7);
		lhs_data4[idx12] = tmp13;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter8 = 0 ; iter8 < ndim9; iter8++)
	{
		size4 *= dim9[iter8];
	}
	Matrix *mat4 = createM(ndim9, dim9, 0);
	writeM(mat4, size4, lhs_data4);
	matrices[idx11] = mat4;
	matrices[idx11] = matrices[idx11];
	int idx13 = convertSubscript(ndim1, dim1, 9);
	int ndim10 = 2;
	int dim10[2] = {20, 1};
	Matrix * tmp14 = onesM(ndim10, dim10);
	matrices[idx13] = tmp14;
	double* lhs_data5 = i_to_d(matrices[idx13]);
	for (int iter9 = 1; iter9 <= 20; iter9++) {
		double tmp15 = (iter9 - 8.5) * iter9 + 0.5;
		int idx14 = convertSubscript(ndim10, dim10, iter9);
		lhs_data5[idx14] = tmp15;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter10 = 0 ; iter10 < ndim10; iter10++)
	{
		size5 *= dim10[iter10];
	}
	Matrix *mat5 = createM(ndim10, dim10, 1);
	writeM(mat5, size5, lhs_data5);
	matrices[idx13] = mat5;
	matrices[idx13] = matrices[idx13];
	int idx15 = convertSubscript(ndim1, dim1, 10);
	int ndim11 = 2;
	int dim11[2] = {1, 10};
	Matrix * tmp16 = zerosM(ndim11, dim11);
	matrices[idx15] = tmp16;
	complex* lhs_data6 = i_to_c(matrices[idx15]);
	for (int iter11 = 1; iter11 <= 10; iter11++) {
		complex tmp17 = (iter11 - 5.5) * (iter11) + ((0.5) * (4 - iter11)) * 1*I;
		int idx16 = convertSubscript(ndim11, dim11, iter11);
		lhs_data6[idx16] = tmp17;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter12 = 0 ; iter12 < ndim11; iter12++)
	{
		size6 *= dim11[iter12];
	}
	Matrix *mat6 = createM(ndim11, dim11, 2);
	writeM(mat6, size6, lhs_data6);
	matrices[idx15] = mat6;
	matrices[idx15] = matrices[idx15];
	int idx17 = convertSubscript(ndim1, dim1, 11);
	
	int ndim12 = 2;
	int dim12[2] = {1,10};
	matrices[idx17] = createM(ndim12, dim12, 1);
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
	writeM( matrices[idx17], 10, input1);
	free(input1);
	
	int idx18 = convertSubscript(ndim1, dim1, 12);
	
	int ndim13 = 2;
	int dim13[2] = {1,10};
	matrices[idx18] = createM(ndim13, dim13, 1);
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
	writeM( matrices[idx18], 10, input2);
	free(input2);
	
	for (int iter13 = 6; iter13 <= 6; iter13++) {
		printf("i = %d\n", iter13);
		int idx19 = convertSubscript(ndim1, dim1, iter13);
		printM(matrices[idx19]);
		for (int iter14 = 11; iter14 <= 11; iter14++) {
			printf("j = %d\n", iter14);
			int idx20 = convertSubscript(ndim1, dim1, iter14);
			printM(matrices[idx20]);
			printf("\n%s\n", "\n\n");
			Matrix * tmp18 = xcorrM(matrices[idx19], matrices[idx20], 0, "none");
			printM(tmp18);
			int * dim14 = getDimsM(matrices[idx19]);
			int * dim15 = getDimsM(matrices[idx19]);
			int * dim16 = getDimsM(matrices[idx20]);
			int * dim17 = getDimsM(matrices[idx20]);
			if ((iter13 > 1 && iter14 > 1 && dim14[0] * dim15[1] == dim16[0] * dim17[1])) {
				Matrix * tmp23 = xcorrM(matrices[idx19], matrices[idx20], 0, "unbiased");
				printM(tmp23);
				Matrix * tmp24 = xcorrM(matrices[idx19], matrices[idx20], 0, "biased");
				printM(tmp24);
				Matrix * tmp25 = xcorrM(matrices[idx19], matrices[idx20], 0, "coeff");
				printM(tmp25);
				
				
				
			
			}
			//for k=9:21
			//	disp(xcorr(matrices{index}, matrices{j}, k, 'none'));
			//	if (index > 1 && j > 1 && size(matrices{index},1)*size(matrices{index},2) == size(matrices{j},1)*size(matrices{j},2))
			//		disp(xcorr(matrices{index}, matrices{j}, k, 'unbiased'));
			//		disp(xcorr(matrices{index}, matrices{j}, k, 'biased'));
			//		disp(xcorr(matrices{index}, matrices{j}, k, 'coeff'));
			//	end
			//end
		
		}
	
	}
	return 0;
}
