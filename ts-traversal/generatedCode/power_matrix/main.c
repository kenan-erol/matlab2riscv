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
	
	// i_zero
	
	printf("\n%s\n", "i_zero");
	Matrix * tmp1 = identityM(3);
	Matrix * a = tmp1;
	printM(a);
	int exponent1 = 0;
	Matrix * tmp2 = mpowerM(a, &exponent1, 0);
	printM(tmp2);
	// d_zero
	
	printf("\n%s\n", "d_zero");
	int ndim1 = 2;
	int dim1[2] = {3, 3};
	Matrix * tmp3 = zerosM(ndim1, dim1);
	a = tmp3;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp4 = iter1 * iter1;
		int idx1 = convertSubscript(ndim1, dim1, iter1);
		lhs_data1[idx1] = tmp4;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim1; iter2++)
	{
		size1 *= dim1[iter2];
	}
	Matrix *mat1 = createM(ndim1, dim1, 0);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp5 = transposeM(mat1);
	a = tmp5;
	printM(a);
	int exponent2 = 0;
	Matrix * tmp6 = mpowerM(a, &exponent2, 0);
	printM(tmp6);
	// c_zero
	
	printf("\n%s\n", "c_zero");
	int ndim2 = 2;
	int dim2[2] = {3, 3};
	Matrix * tmp7 = zerosM(ndim2, dim2);
	a = tmp7;
	complex* lhs_data2 = i_to_c(a);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		complex tmp8 = iter3 * iter3 + 0.5*I;
		int idx2 = convertSubscript(ndim2, dim2, iter3);
		lhs_data2[idx2] = tmp8;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim2; iter4++)
	{
		size2 *= dim2[iter4];
	}
	Matrix *mat2 = createM(ndim2, dim2, 2);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp9 = transposeM(mat2);
	a = tmp9;
	printM(a);
	int exponent3 = 0;
	Matrix * tmp10 = mpowerM(a, &exponent3, 0);
	printM(tmp10);
	// i_one
	
	printf("\n%s\n", "i_one");
	Matrix * tmp11 = identityM(3);
	a = tmp11;
	printM(a);
	int exponent4 = 1;
	Matrix * tmp12 = mpowerM(a, &exponent4, 0);
	printM(tmp12);
	// d_one
	
	printf("\n%s\n", "d_one");
	int ndim3 = 2;
	int dim3[2] = {3, 3};
	Matrix * tmp13 = zerosM(ndim3, dim3);
	a = tmp13;
	int* lhs_data3 = i_to_i(a);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		int tmp14 = iter5 * iter5;
		int idx3 = convertSubscript(ndim3, dim3, iter5);
		lhs_data3[idx3] = tmp14;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim3; iter6++)
	{
		size3 *= dim3[iter6];
	}
	Matrix *mat3 = createM(ndim3, dim3, 0);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp15 = transposeM(mat3);
	a = tmp15;
	printM(a);
	int exponent5 = 1;
	Matrix * tmp16 = mpowerM(a, &exponent5, 0);
	printM(tmp16);
	// c_one
	
	printf("\n%s\n", "c_one");
	int ndim4 = 2;
	int dim4[2] = {3, 3};
	Matrix * tmp17 = zerosM(ndim4, dim4);
	a = tmp17;
	complex* lhs_data4 = i_to_c(a);
	for (int iter7 = 1; iter7 <= 9; iter7++) {
		complex tmp18 = iter7 * iter7 + 0.5*I;
		int idx4 = convertSubscript(ndim4, dim4, iter7);
		lhs_data4[idx4] = tmp18;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter8 = 0 ; iter8 < ndim4; iter8++)
	{
		size4 *= dim4[iter8];
	}
	Matrix *mat4 = createM(ndim4, dim4, 2);
	writeM(mat4, size4, lhs_data4);
	Matrix * tmp19 = transposeM(mat4);
	a = tmp19;
	printM(a);
	int exponent6 = 1;
	Matrix * tmp20 = mpowerM(a, &exponent6, 0);
	printM(tmp20);
	// i_large
	
	printf("\n%s\n", "i_large");
	int ndim5 = 2;
	int dim5[2] = {3, 3};
	Matrix * tmp21 = zerosM(ndim5, dim5);
	a = tmp21;
	int* lhs_data5 = i_to_i(a);
	for (int iter9 = 1; iter9 <= 9; iter9++) {
		int tmp22 = iter9 * iter9;
		int idx5 = convertSubscript(ndim5, dim5, iter9);
		lhs_data5[idx5] = tmp22;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter10 = 0 ; iter10 < ndim5; iter10++)
	{
		size5 *= dim5[iter10];
	}
	Matrix *mat5 = createM(ndim5, dim5, 0);
	writeM(mat5, size5, lhs_data5);
	Matrix * tmp23 = transposeM(mat5);
	a = tmp23;
	printM(a);
	int exponent7 = 20;
	Matrix * tmp24 = mpowerM(a, &exponent7, 0);
	printM(tmp24);
	// i_negative
	
	printf("\n%s\n", "i_neg");
	int ndim6 = 2;
	int dim6[2] = {3, 3};
	Matrix * tmp25 = zerosM(ndim6, dim6);
	a = tmp25;
	int* lhs_data6 = i_to_i(a);
	for (int iter11 = 1; iter11 <= 9; iter11++) {
		//a(i) = i*i;
		
		int tmp27 = pow((-1), iter11);
		int tmp26 = (tmp27) * iter11 * iter11;
		int idx6 = convertSubscript(ndim6, dim6, iter11);
		lhs_data6[idx6] = tmp26;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter12 = 0 ; iter12 < ndim6; iter12++)
	{
		size6 *= dim6[iter12];
	}
	Matrix *mat6 = createM(ndim6, dim6, 0);
	writeM(mat6, size6, lhs_data6);
	Matrix * tmp28 = transposeM(mat6);
	a = tmp28;
	printM(a);
	int exponent8 = -2;
	Matrix * tmp29 = mpowerM(a, &exponent8, 0);
	Matrix * tmp30 = floorM(tmp29);
	printM(tmp30);
	// d_small
	
	printf("\n%s\n", "d_small");
	int ndim7 = 2;
	int dim7[2] = {3, 3};
	Matrix * tmp31 = zerosM(ndim7, dim7);
	a = tmp31;
	int* lhs_data7 = i_to_i(a);
	for (int iter13 = 1; iter13 <= 9; iter13++) {
		int tmp32 = iter13 * iter13;
		int idx7 = convertSubscript(ndim7, dim7, iter13);
		lhs_data7[idx7] = tmp32;
	
	}
	// Write matrix mat7
	int size7 = 1;
	for (int iter14 = 0 ; iter14 < ndim7; iter14++)
	{
		size7 *= dim7[iter14];
	}
	Matrix *mat7 = createM(ndim7, dim7, 0);
	writeM(mat7, size7, lhs_data7);
	Matrix * tmp33 = transposeM(mat7);
	a = tmp33;
	printM(a);
	double exponent9 = 0.05;
	Matrix * tmp34 = mpowerM(a, &exponent9, 1);
	printM(tmp34);
	// d_negative
	
	printf("\n%s\n", "d_neg");
	int ndim8 = 2;
	int dim8[2] = {3, 3};
	Matrix * tmp35 = zerosM(ndim8, dim8);
	a = tmp35;
	int* lhs_data8 = i_to_i(a);
	for (int iter15 = 1; iter15 <= 9; iter15++) {
		int tmp37 = pow((-1), iter15);
		int tmp36 = (tmp37) * iter15 * iter15;
		int idx8 = convertSubscript(ndim8, dim8, iter15);
		lhs_data8[idx8] = tmp36;
	
	}
	// Write matrix mat8
	int size8 = 1;
	for (int iter16 = 0 ; iter16 < ndim8; iter16++)
	{
		size8 *= dim8[iter16];
	}
	Matrix *mat8 = createM(ndim8, dim8, 0);
	writeM(mat8, size8, lhs_data8);
	Matrix * tmp38 = transposeM(mat8);
	a = tmp38;
	printM(a);
	double exponent10 = -1.1;
	Matrix * tmp39 = mpowerM(a, &exponent10, 1);
	printM(tmp39);
	// c_large
	
	int ndim9 = 2;
	int dim9[2] = {3, 3};
	Matrix * tmp40 = zerosM(ndim9, dim9);
	a = tmp40;
	complex* lhs_data9 = i_to_c(a);
	for (int iter17 = 1; iter17 <= 9; iter17++) {
		complex tmp41 = iter17 * iter17 + 0.5*I;
		int idx9 = convertSubscript(ndim9, dim9, iter17);
		lhs_data9[idx9] = tmp41;
	
	}
	// Write matrix mat9
	int size9 = 1;
	for (int iter18 = 0 ; iter18 < ndim9; iter18++)
	{
		size9 *= dim9[iter18];
	}
	Matrix *mat9 = createM(ndim9, dim9, 2);
	writeM(mat9, size9, lhs_data9);
	Matrix * tmp42 = transposeM(mat9);
	a = tmp42;
	printM(a);
	complex exponent11 = (-10 + 7.8*I);
	Matrix * tmp43 = mpowerM(a, &exponent11, 2);
	printM(tmp43);
	// c_small
	
	int ndim10 = 2;
	int dim10[2] = {3, 3};
	Matrix * tmp44 = zerosM(ndim10, dim10);
	a = tmp44;
	complex* lhs_data10 = i_to_c(a);
	for (int iter19 = 1; iter19 <= 9; iter19++) {
		complex tmp45 = iter19 * iter19 + 0.5*I;
		int idx10 = convertSubscript(ndim10, dim10, iter19);
		lhs_data10[idx10] = tmp45;
	
	}
	// Write matrix mat10
	int size10 = 1;
	for (int iter20 = 0 ; iter20 < ndim10; iter20++)
	{
		size10 *= dim10[iter20];
	}
	Matrix *mat10 = createM(ndim10, dim10, 2);
	writeM(mat10, size10, lhs_data10);
	Matrix * tmp46 = transposeM(mat10);
	a = tmp46;
	printM(a);
	complex exponent12 = (-0.8*I);
	Matrix * tmp47 = mpowerM(a, &exponent12, 2);
	printM(tmp47);
	// brutal_test
	
	
	Matrix **matrices = NULL;
	matrices = malloc(11*sizeof(*matrices));
		        
	int ndim11 = 2;
	int dim11[2] = {11,1};
	int idx11 = convertSubscript(ndim11, dim11, 1);
	int ndim12 = 2;
	int dim12[2] = {3,3};
	Matrix * tmp48 = zerosM(ndim12, dim12);
	matrices[idx11] = tmp48;
	int idx12 = convertSubscript(ndim11, dim11, 2);
	int ndim13 = 2;
	int dim13[2] = {3,3};
	Matrix * tmp49 = onesM(ndim13, dim13);
	matrices[idx12] = tmp49;
	int idx13 = convertSubscript(ndim11, dim11, 3);
	Matrix * tmp50 = identityM(3);
	matrices[idx13] = tmp50;
	int idx14 = convertSubscript(ndim11, dim11, 4);
	Matrix * tmp51 = identityM(3);
	complex scalar1 = (4.2 - 0.03*I);
	Matrix * tmp52 = scaleM(tmp51, &scalar1, 2);
	matrices[idx14] = tmp52;
	int idx15 = convertSubscript(ndim11, dim11, 5);
	int ndim14 = 2;
	int dim14[2] = {3,3};
	Matrix * tmp53 = zerosM(ndim14, dim14);
	matrices[idx15] = tmp53;
	int* lhs_data11 = i_to_i(matrices[idx15]);
	for (int iter21 = 1; iter21 <= 9; iter21++) {
		int tmp54 = iter21 * iter21;
		int idx16 = convertSubscript(ndim14, dim14, iter21);
		lhs_data11[idx16] = tmp54;
	
	}
	// Write matrix mat11
	int size11 = 1;
	for (int iter22 = 0 ; iter22 < ndim14; iter22++)
	{
		size11 *= dim14[iter22];
	}
	Matrix *mat11 = createM(ndim14, dim14, 0);
	writeM(mat11, size11, lhs_data11);
	matrices[idx15] = mat11;
	Matrix * tmp55 = transposeM(matrices[idx15]);
	matrices[idx15] = tmp55;
	int idx17 = convertSubscript(ndim11, dim11, 6);
	int ndim15 = 2;
	int dim15[2] = {3,3};
	Matrix * tmp56 = zerosM(ndim15, dim15);
	matrices[idx17] = tmp56;
	double* lhs_data12 = i_to_d(matrices[idx17]);
	for (int iter23 = 1; iter23 <= 9; iter23++) {
		double tmp57 = iter23 * iter23 + 0.5;
		int idx18 = convertSubscript(ndim15, dim15, iter23);
		lhs_data12[idx18] = tmp57;
	
	}
	// Write matrix mat12
	int size12 = 1;
	for (int iter24 = 0 ; iter24 < ndim15; iter24++)
	{
		size12 *= dim15[iter24];
	}
	Matrix *mat12 = createM(ndim15, dim15, 1);
	writeM(mat12, size12, lhs_data12);
	matrices[idx17] = mat12;
	Matrix * tmp58 = transposeM(matrices[idx17]);
	matrices[idx17] = tmp58;
	int idx19 = convertSubscript(ndim11, dim11, 7);
	int ndim16 = 2;
	int dim16[2] = {3,3};
	Matrix * tmp59 = zerosM(ndim16, dim16);
	matrices[idx19] = tmp59;
	complex* lhs_data13 = i_to_c(matrices[idx19]);
	for (int iter25 = 1; iter25 <= 9; iter25++) {
		complex tmp60 = iter25 * iter25 + 0.5*I;
		int idx20 = convertSubscript(ndim16, dim16, iter25);
		lhs_data13[idx20] = tmp60;
	
	}
	// Write matrix mat13
	int size13 = 1;
	for (int iter26 = 0 ; iter26 < ndim16; iter26++)
	{
		size13 *= dim16[iter26];
	}
	Matrix *mat13 = createM(ndim16, dim16, 2);
	writeM(mat13, size13, lhs_data13);
	matrices[idx19] = mat13;
	Matrix * tmp61 = transposeM(matrices[idx19]);
	matrices[idx19] = tmp61;
	int idx21 = convertSubscript(ndim11, dim11, 8);
	int ndim17 = 2;
	int dim17[2] = {3,3};
	Matrix * tmp62 = zerosM(ndim17, dim17);
	matrices[idx21] = tmp62;
	int* lhs_data14 = i_to_i(matrices[idx21]);
	for (int iter27 = 1; iter27 <= 9; iter27++) {
		int tmp63 = (iter27 - 5) * iter27;
		int idx22 = convertSubscript(ndim17, dim17, iter27);
		lhs_data14[idx22] = tmp63;
	
	}
	// Write matrix mat14
	int size14 = 1;
	for (int iter28 = 0 ; iter28 < ndim17; iter28++)
	{
		size14 *= dim17[iter28];
	}
	Matrix *mat14 = createM(ndim17, dim17, 0);
	writeM(mat14, size14, lhs_data14);
	matrices[idx21] = mat14;
	Matrix * tmp64 = transposeM(matrices[idx21]);
	matrices[idx21] = tmp64;
	int idx23 = convertSubscript(ndim11, dim11, 9);
	int ndim18 = 2;
	int dim18[2] = {3,3};
	Matrix * tmp65 = zerosM(ndim18, dim18);
	matrices[idx23] = tmp65;
	double* lhs_data15 = i_to_d(matrices[idx23]);
	for (int iter29 = 1; iter29 <= 9; iter29++) {
		double tmp66 = (iter29 - 8.2) * iter29 + 0.5;
		int idx24 = convertSubscript(ndim18, dim18, iter29);
		lhs_data15[idx24] = tmp66;
	
	}
	// Write matrix mat15
	int size15 = 1;
	for (int iter30 = 0 ; iter30 < ndim18; iter30++)
	{
		size15 *= dim18[iter30];
	}
	Matrix *mat15 = createM(ndim18, dim18, 1);
	writeM(mat15, size15, lhs_data15);
	matrices[idx23] = mat15;
	Matrix * tmp67 = transposeM(matrices[idx23]);
	matrices[idx23] = tmp67;
	int idx25 = convertSubscript(ndim11, dim11, 10);
	int ndim19 = 2;
	int dim19[2] = {3,3};
	Matrix * tmp68 = zerosM(ndim19, dim19);
	matrices[idx25] = tmp68;
	complex* lhs_data16 = i_to_c(matrices[idx25]);
	for (int iter31 = 1; iter31 <= 9; iter31++) {
		complex tmp69 = (iter31 - 5.89) * (iter31) + ((0.5) * (4 - iter31)) * 1*I;
		int idx26 = convertSubscript(ndim19, dim19, iter31);
		lhs_data16[idx26] = tmp69;
	
	}
	// Write matrix mat16
	int size16 = 1;
	for (int iter32 = 0 ; iter32 < ndim19; iter32++)
	{
		size16 *= dim19[iter32];
	}
	Matrix *mat16 = createM(ndim19, dim19, 2);
	writeM(mat16, size16, lhs_data16);
	matrices[idx25] = mat16;
	Matrix * tmp70 = transposeM(matrices[idx25]);
	matrices[idx25] = tmp70;
	int idx27 = convertSubscript(ndim11, dim11, 11);
	
	int ndim20 = 2;
	int dim20[2] = {3,3};
	matrices[idx27] = createM(ndim20, dim20, 0);
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
	writeM( matrices[idx27], 9, input1);
	free(input1);
	
	int idx28 = convertSubscript(ndim11, dim11, 12);
	
	int ndim21 = 2;
	int dim21[2] = {3,3};
	matrices[idx28] = createM(ndim21, dim21, 1);
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
	writeM( matrices[idx28], 9, input2);
	free(input2);
	
	//for index=3:12 -> removed 12 because yields small rounding error
	
	for (int iter33 = 3; iter33 <= 11; iter33++) {
		printf("\n%s\n", "Original\n");
		int idx29 = convertSubscript(ndim11, dim11, iter33);
		printM(matrices[idx29]);
		printf("\n%s\n", "Integer exponents\n");
		for (int iter34 = -4; iter34 <= 4; iter34++) {
			Matrix * tmp71 = mpowerM(matrices[idx29], &iter34, 0);
			printM(tmp71);
		
		}
		printf("\n%s\n", "Double exponents\n");
		for (double iter35 = 0; iter35 < 25; iter35++) {
			double i = -3 + iter35*0.2;
			printf("Exponent: %.4f\n", i);
			Matrix * tmp72 = mpowerM(matrices[idx29], &i, 1);
			printM(tmp72);
		
		}
		printf("\n%s\n", "Complex exponents\n");
		for (double iter36 = 0; iter36 < 31; iter36++) {
			double i = -3 + iter36*0.2;
			for (double iter37 = 0; iter37 < 31; iter37++) {
				double j = -3 + iter37*0.2;
				if (fabs(j - 0) > 1e-6) {
					char *mystr1 = NULL;
					mystr1 = malloc(50*sizeof(*mystr1));
					sprintf(mystr1, "Exponent: %.4f + %.4fi\n", i, j);
					printf("\n%s\n", mystr1);
					complex exponent13 = (i + j * 1*I);
					Matrix * tmp73 = mpowerM(matrices[idx29], &exponent13, 2);
					printM(tmp73);
					
					
					
				
				}
			
			}
		
		}
	
	}
	// % non_diag1
	
	// a = [1,1;0,1];
	
	// disp(a);
	
	// disp(a^1.5);
	
	// % non_diag2
	
	// a = [3,4,3;-1,0,-1;1,2,3];
	
	// disp(a);
	
	// disp(a^-4.25);
	return 0;
}
