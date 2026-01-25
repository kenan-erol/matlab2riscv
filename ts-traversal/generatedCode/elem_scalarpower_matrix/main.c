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
	//ii_test
	int exponent = 3;
	int ndim1 = 2;
	int dim1[2] = {3,3};
	Matrix * tmp1 = zerosM(ndim1, dim1);
	Matrix * a = tmp1;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 9; iter1++) {
		int tmp3 = pow((-1), (iter1 + 1));
		int tmp2 = (tmp3) * iter1;
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
	int ndim2 = 2;
	int dim2[2] = {3,3};
	Matrix * tmp5 = zerosM(ndim2, dim2);
	Matrix * b = tmp5;
	int* lhs_data2 = i_to_i(b);
	for (int iter3 = 1; iter3 <= 9; iter3++) {
		int tmp7 = pow((-1), (iter3 + 1));
		int tmp8 = pow(((tmp7) * iter3), exponent);
		int tmp6 = tmp8;
		int idx2 = convertSubscript(ndim2, dim2, iter3);
		lhs_data2[idx2] = tmp6;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim2; iter4++)
	{
		size2 *= dim2[iter4];
	}
	Matrix *mat2 = createM(ndim2, dim2, 0);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp9 = transposeM(mat2);
	b = tmp9;
	printM(b);
	Matrix * tmp10 = scalarpowerM(a, &exponent, 0);
	Matrix * c = tmp10;
	printM(c);
	//id_test
	double exponent1 = 1.2;
	int ndim3 = 2;
	int dim3[2] = {3,3};
	Matrix * tmp11 = zerosM(ndim3, dim3);
	a = tmp11;
	int* lhs_data3 = i_to_i(a);
	for (int iter5 = 1; iter5 <= 9; iter5++) {
		int tmp12 = iter5;
		int idx3 = convertSubscript(ndim3, dim3, iter5);
		lhs_data3[idx3] = tmp12;
	
	}
	// Write matrix mat3
	int size3 = 1;
	for (int iter6 = 0 ; iter6 < ndim3; iter6++)
	{
		size3 *= dim3[iter6];
	}
	Matrix *mat3 = createM(ndim3, dim3, 0);
	writeM(mat3, size3, lhs_data3);
	Matrix * tmp13 = transposeM(mat3);
	a = tmp13;
	printM(a);
	int ndim4 = 2;
	int dim4[2] = {3,3};
	Matrix * tmp14 = zerosM(ndim4, dim4);
	b = tmp14;
	complex* lhs_data4 = i_to_c(b);
	for (int iter7 = 1; iter7 <= 9; iter7++) {
		complex tmp16 = cpow(iter7, exponent1);
		complex tmp15 = tmp16;
		int idx4 = convertSubscript(ndim4, dim4, iter7);
		lhs_data4[idx4] = tmp15;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter8 = 0 ; iter8 < ndim4; iter8++)
	{
		size4 *= dim4[iter8];
	}
	Matrix *mat4 = createM(ndim4, dim4, 2);
	writeM(mat4, size4, lhs_data4);
	Matrix * tmp17 = transposeM(mat4);
	b = tmp17;
	printM(b);
	Matrix * tmp18 = scalarpowerM(a, &exponent1, 1);
	c = tmp18;
	printM(c);
	//neg_id_test
	exponent1 = 1.2;
	int ndim5 = 2;
	int dim5[2] = {3,3};
	Matrix * tmp19 = zerosM(ndim5, dim5);
	a = tmp19;
	int* lhs_data5 = i_to_i(a);
	for (int iter9 = 1; iter9 <= 9; iter9++) {
		int tmp21 = pow((-1), (iter9 + 1));
		int tmp20 = (tmp21) * iter9;
		int idx5 = convertSubscript(ndim5, dim5, iter9);
		lhs_data5[idx5] = tmp20;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter10 = 0 ; iter10 < ndim5; iter10++)
	{
		size5 *= dim5[iter10];
	}
	Matrix *mat5 = createM(ndim5, dim5, 0);
	writeM(mat5, size5, lhs_data5);
	Matrix * tmp22 = transposeM(mat5);
	a = tmp22;
	printM(a);
	int ndim6 = 2;
	int dim6[2] = {3,3};
	Matrix * tmp23 = zerosM(ndim6, dim6);
	b = tmp23;
	complex* lhs_data6 = i_to_c(b);
	for (int iter11 = 1; iter11 <= 9; iter11++) {
		int tmp25 = pow((-1), (iter11 + 1));
		complex tmp26 = cpow(((tmp25) * iter11), exponent1);
		complex tmp24 = tmp26;
		int idx6 = convertSubscript(ndim6, dim6, iter11);
		lhs_data6[idx6] = tmp24;
	
	}
	// Write matrix mat6
	int size6 = 1;
	for (int iter12 = 0 ; iter12 < ndim6; iter12++)
	{
		size6 *= dim6[iter12];
	}
	Matrix *mat6 = createM(ndim6, dim6, 2);
	writeM(mat6, size6, lhs_data6);
	Matrix * tmp27 = transposeM(mat6);
	b = tmp27;
	printM(b);
	Matrix * tmp28 = scalarpowerM(a, &exponent1, 1);
	c = tmp28;
	printM(c);
	//ic_test
	complex exponent2 = 4 + 0.3*I;
	int ndim7 = 2;
	int dim7[2] = {3,3};
	Matrix * tmp29 = zerosM(ndim7, dim7);
	a = tmp29;
	int* lhs_data7 = i_to_i(a);
	for (int iter13 = 1; iter13 <= 9; iter13++) {
		int tmp30 = iter13;
		int idx7 = convertSubscript(ndim7, dim7, iter13);
		lhs_data7[idx7] = tmp30;
	
	}
	// Write matrix mat7
	int size7 = 1;
	for (int iter14 = 0 ; iter14 < ndim7; iter14++)
	{
		size7 *= dim7[iter14];
	}
	Matrix *mat7 = createM(ndim7, dim7, 0);
	writeM(mat7, size7, lhs_data7);
	Matrix * tmp31 = transposeM(mat7);
	a = tmp31;
	printM(a);
	int ndim8 = 2;
	int dim8[2] = {3,3};
	Matrix * tmp32 = zerosM(ndim8, dim8);
	b = tmp32;
	complex* lhs_data8 = i_to_c(b);
	for (int iter15 = 1; iter15 <= 9; iter15++) {
		complex tmp34 = cpow(iter15, exponent2);
		complex tmp33 = tmp34;
		int idx8 = convertSubscript(ndim8, dim8, iter15);
		lhs_data8[idx8] = tmp33;
	
	}
	// Write matrix mat8
	int size8 = 1;
	for (int iter16 = 0 ; iter16 < ndim8; iter16++)
	{
		size8 *= dim8[iter16];
	}
	Matrix *mat8 = createM(ndim8, dim8, 2);
	writeM(mat8, size8, lhs_data8);
	Matrix * tmp35 = transposeM(mat8);
	b = tmp35;
	printM(b);
	Matrix * tmp36 = scalarpowerM(a, &exponent2, 2);
	c = tmp36;
	printM(c);
	//di_test
	int exponent3 = 5;
	int ndim9 = 2;
	int dim9[2] = {3,3};
	Matrix * tmp37 = zerosM(ndim9, dim9);
	a = tmp37;
	double* lhs_data9 = i_to_d(a);
	for (int iter17 = 1; iter17 <= 9; iter17++) {
		int tmp39 = pow((-1), (iter17 + 1));
		double tmp38 = (tmp39) * (iter17 + 0.4);
		int idx9 = convertSubscript(ndim9, dim9, iter17);
		lhs_data9[idx9] = tmp38;
	
	}
	// Write matrix mat9
	int size9 = 1;
	for (int iter18 = 0 ; iter18 < ndim9; iter18++)
	{
		size9 *= dim9[iter18];
	}
	Matrix *mat9 = createM(ndim9, dim9, 1);
	writeM(mat9, size9, lhs_data9);
	Matrix * tmp40 = transposeM(mat9);
	a = tmp40;
	printM(a);
	int ndim10 = 2;
	int dim10[2] = {3,3};
	Matrix * tmp41 = zerosM(ndim10, dim10);
	b = tmp41;
	double* lhs_data10 = i_to_d(b);
	for (int iter19 = 1; iter19 <= 9; iter19++) {
		int tmp43 = pow((-1), (iter19 + 1));
		double tmp44 = pow(((tmp43) * (iter19 + 0.4)), exponent3);
		double tmp42 = tmp44;
		int idx10 = convertSubscript(ndim10, dim10, iter19);
		lhs_data10[idx10] = tmp42;
	
	}
	// Write matrix mat10
	int size10 = 1;
	for (int iter20 = 0 ; iter20 < ndim10; iter20++)
	{
		size10 *= dim10[iter20];
	}
	Matrix *mat10 = createM(ndim10, dim10, 1);
	writeM(mat10, size10, lhs_data10);
	Matrix * tmp45 = transposeM(mat10);
	b = tmp45;
	printM(b);
	Matrix * tmp46 = scalarpowerM(a, &exponent3, 0);
	c = tmp46;
	printM(c);
	//dd_test
	double exponent4 = 1.4;
	int ndim11 = 2;
	int dim11[2] = {3,3};
	Matrix * tmp47 = zerosM(ndim11, dim11);
	a = tmp47;
	double* lhs_data11 = i_to_d(a);
	for (int iter21 = 1; iter21 <= 9; iter21++) {
		double tmp48 = (iter21 + 0.4);
		int idx11 = convertSubscript(ndim11, dim11, iter21);
		lhs_data11[idx11] = tmp48;
	
	}
	// Write matrix mat11
	int size11 = 1;
	for (int iter22 = 0 ; iter22 < ndim11; iter22++)
	{
		size11 *= dim11[iter22];
	}
	Matrix *mat11 = createM(ndim11, dim11, 1);
	writeM(mat11, size11, lhs_data11);
	Matrix * tmp49 = transposeM(mat11);
	a = tmp49;
	printM(a);
	int ndim12 = 2;
	int dim12[2] = {3,3};
	Matrix * tmp50 = zerosM(ndim12, dim12);
	b = tmp50;
	complex* lhs_data12 = i_to_c(b);
	for (int iter23 = 1; iter23 <= 9; iter23++) {
		complex tmp52 = cpow((iter23 + 0.4), exponent4);
		complex tmp51 = tmp52;
		int idx12 = convertSubscript(ndim12, dim12, iter23);
		lhs_data12[idx12] = tmp51;
	
	}
	// Write matrix mat12
	int size12 = 1;
	for (int iter24 = 0 ; iter24 < ndim12; iter24++)
	{
		size12 *= dim12[iter24];
	}
	Matrix *mat12 = createM(ndim12, dim12, 2);
	writeM(mat12, size12, lhs_data12);
	Matrix * tmp53 = transposeM(mat12);
	b = tmp53;
	printM(b);
	Matrix * tmp54 = scalarpowerM(a, &exponent4, 1);
	c = tmp54;
	printM(c);
	//neg_dd_test
	exponent4 = 1.4;
	int ndim13 = 2;
	int dim13[2] = {3,3};
	Matrix * tmp55 = zerosM(ndim13, dim13);
	a = tmp55;
	double* lhs_data13 = i_to_d(a);
	for (int iter25 = 1; iter25 <= 9; iter25++) {
		int tmp57 = pow((-1), (iter25 + 1));
		double tmp56 = (tmp57) * (iter25 + 0.4);
		int idx13 = convertSubscript(ndim13, dim13, iter25);
		lhs_data13[idx13] = tmp56;
	
	}
	// Write matrix mat13
	int size13 = 1;
	for (int iter26 = 0 ; iter26 < ndim13; iter26++)
	{
		size13 *= dim13[iter26];
	}
	Matrix *mat13 = createM(ndim13, dim13, 1);
	writeM(mat13, size13, lhs_data13);
	Matrix * tmp58 = transposeM(mat13);
	a = tmp58;
	printM(a);
	int ndim14 = 2;
	int dim14[2] = {3,3};
	Matrix * tmp59 = zerosM(ndim14, dim14);
	b = tmp59;
	complex* lhs_data14 = i_to_c(b);
	for (int iter27 = 1; iter27 <= 9; iter27++) {
		int tmp61 = pow((-1), (iter27 + 1));
		complex tmp62 = cpow(((tmp61) * (iter27 + 0.4)), exponent4);
		complex tmp60 = tmp62;
		int idx14 = convertSubscript(ndim14, dim14, iter27);
		lhs_data14[idx14] = tmp60;
	
	}
	// Write matrix mat14
	int size14 = 1;
	for (int iter28 = 0 ; iter28 < ndim14; iter28++)
	{
		size14 *= dim14[iter28];
	}
	Matrix *mat14 = createM(ndim14, dim14, 2);
	writeM(mat14, size14, lhs_data14);
	Matrix * tmp63 = transposeM(mat14);
	b = tmp63;
	printM(b);
	Matrix * tmp64 = scalarpowerM(a, &exponent4, 1);
	c = tmp64;
	printM(c);
	//dc_test
	complex exponent5 = -0.5*I;
	int ndim15 = 2;
	int dim15[2] = {3,3};
	Matrix * tmp65 = zerosM(ndim15, dim15);
	a = tmp65;
	double* lhs_data15 = i_to_d(a);
	for (int iter29 = 1; iter29 <= 9; iter29++) {
		double tmp66 = iter29 + 0.4;
		int idx15 = convertSubscript(ndim15, dim15, iter29);
		lhs_data15[idx15] = tmp66;
	
	}
	// Write matrix mat15
	int size15 = 1;
	for (int iter30 = 0 ; iter30 < ndim15; iter30++)
	{
		size15 *= dim15[iter30];
	}
	Matrix *mat15 = createM(ndim15, dim15, 1);
	writeM(mat15, size15, lhs_data15);
	Matrix * tmp67 = transposeM(mat15);
	a = tmp67;
	printM(a);
	int ndim16 = 2;
	int dim16[2] = {3,3};
	Matrix * tmp68 = zerosM(ndim16, dim16);
	b = tmp68;
	complex* lhs_data16 = i_to_c(b);
	for (int iter31 = 1; iter31 <= 9; iter31++) {
		complex tmp70 = cpow((iter31 + 0.4), exponent5);
		complex tmp69 = tmp70;
		int idx16 = convertSubscript(ndim16, dim16, iter31);
		lhs_data16[idx16] = tmp69;
	
	}
	// Write matrix mat16
	int size16 = 1;
	for (int iter32 = 0 ; iter32 < ndim16; iter32++)
	{
		size16 *= dim16[iter32];
	}
	Matrix *mat16 = createM(ndim16, dim16, 2);
	writeM(mat16, size16, lhs_data16);
	Matrix * tmp71 = transposeM(mat16);
	b = tmp71;
	printM(b);
	Matrix * tmp72 = scalarpowerM(a, &exponent5, 2);
	c = tmp72;
	printM(c);
	//ci_test
	int exponent6 = 3;
	int ndim17 = 2;
	int dim17[2] = {3,3};
	Matrix * tmp73 = zerosM(ndim17, dim17);
	a = tmp73;
	complex* lhs_data17 = i_to_c(a);
	for (int iter33 = 1; iter33 <= 9; iter33++) {
		complex tmp74 = iter33 + 0.5*I;
		int idx17 = convertSubscript(ndim17, dim17, iter33);
		lhs_data17[idx17] = tmp74;
	
	}
	// Write matrix mat17
	int size17 = 1;
	for (int iter34 = 0 ; iter34 < ndim17; iter34++)
	{
		size17 *= dim17[iter34];
	}
	Matrix *mat17 = createM(ndim17, dim17, 2);
	writeM(mat17, size17, lhs_data17);
	Matrix * tmp75 = transposeM(mat17);
	a = tmp75;
	printM(a);
	int ndim18 = 2;
	int dim18[2] = {3,3};
	Matrix * tmp76 = zerosM(ndim18, dim18);
	b = tmp76;
	complex* lhs_data18 = i_to_c(b);
	for (int iter35 = 1; iter35 <= 9; iter35++) {
		complex tmp78 = cpow((iter35 + 0.5*I), exponent6);
		complex tmp77 = tmp78;
		int idx18 = convertSubscript(ndim18, dim18, iter35);
		lhs_data18[idx18] = tmp77;
	
	}
	// Write matrix mat18
	int size18 = 1;
	for (int iter36 = 0 ; iter36 < ndim18; iter36++)
	{
		size18 *= dim18[iter36];
	}
	Matrix *mat18 = createM(ndim18, dim18, 2);
	writeM(mat18, size18, lhs_data18);
	Matrix * tmp79 = transposeM(mat18);
	b = tmp79;
	printM(b);
	Matrix * tmp80 = scalarpowerM(a, &exponent6, 0);
	c = tmp80;
	printM(c);
	//cd_test
	double exponent7 = -0.9;
	int ndim19 = 2;
	int dim19[2] = {3,3};
	Matrix * tmp81 = zerosM(ndim19, dim19);
	a = tmp81;
	complex* lhs_data19 = i_to_c(a);
	for (int iter37 = 1; iter37 <= 9; iter37++) {
		complex tmp82 = iter37 + 0.5*I;
		int idx19 = convertSubscript(ndim19, dim19, iter37);
		lhs_data19[idx19] = tmp82;
	
	}
	// Write matrix mat19
	int size19 = 1;
	for (int iter38 = 0 ; iter38 < ndim19; iter38++)
	{
		size19 *= dim19[iter38];
	}
	Matrix *mat19 = createM(ndim19, dim19, 2);
	writeM(mat19, size19, lhs_data19);
	Matrix * tmp83 = transposeM(mat19);
	a = tmp83;
	printM(a);
	int ndim20 = 2;
	int dim20[2] = {3,3};
	Matrix * tmp84 = zerosM(ndim20, dim20);
	b = tmp84;
	complex* lhs_data20 = i_to_c(b);
	for (int iter39 = 1; iter39 <= 9; iter39++) {
		complex tmp86 = cpow((iter39 + 0.5*I), exponent7);
		complex tmp85 = tmp86;
		int idx20 = convertSubscript(ndim20, dim20, iter39);
		lhs_data20[idx20] = tmp85;
	
	}
	// Write matrix mat20
	int size20 = 1;
	for (int iter40 = 0 ; iter40 < ndim20; iter40++)
	{
		size20 *= dim20[iter40];
	}
	Matrix *mat20 = createM(ndim20, dim20, 2);
	writeM(mat20, size20, lhs_data20);
	Matrix * tmp87 = transposeM(mat20);
	b = tmp87;
	printM(b);
	Matrix * tmp88 = scalarpowerM(a, &exponent7, 1);
	c = tmp88;
	printM(c);
	//cc_test
	complex exponent8 = 2 - 2*I;
	int ndim21 = 2;
	int dim21[2] = {3,3};
	Matrix * tmp89 = zerosM(ndim21, dim21);
	a = tmp89;
	complex* lhs_data21 = i_to_c(a);
	for (int iter41 = 1; iter41 <= 9; iter41++) {
		complex tmp90 = iter41 + 0.5*I;
		int idx21 = convertSubscript(ndim21, dim21, iter41);
		lhs_data21[idx21] = tmp90;
	
	}
	// Write matrix mat21
	int size21 = 1;
	for (int iter42 = 0 ; iter42 < ndim21; iter42++)
	{
		size21 *= dim21[iter42];
	}
	Matrix *mat21 = createM(ndim21, dim21, 2);
	writeM(mat21, size21, lhs_data21);
	Matrix * tmp91 = transposeM(mat21);
	a = tmp91;
	printM(a);
	int ndim22 = 2;
	int dim22[2] = {3,3};
	Matrix * tmp92 = zerosM(ndim22, dim22);
	b = tmp92;
	complex* lhs_data22 = i_to_c(b);
	for (int iter43 = 1; iter43 <= 9; iter43++) {
		complex tmp94 = cpow((iter43 + 0.5*I), exponent8);
		complex tmp93 = tmp94;
		int idx22 = convertSubscript(ndim22, dim22, iter43);
		lhs_data22[idx22] = tmp93;
	
	}
	// Write matrix mat22
	int size22 = 1;
	for (int iter44 = 0 ; iter44 < ndim22; iter44++)
	{
		size22 *= dim22[iter44];
	}
	Matrix *mat22 = createM(ndim22, dim22, 2);
	writeM(mat22, size22, lhs_data22);
	Matrix * tmp95 = transposeM(mat22);
	b = tmp95;
	printM(b);
	Matrix * tmp96 = scalarpowerM(a, &exponent8, 2);
	c = tmp96;
	printM(c);
	//overflow_test
	//exponent=2;
	//a = zeros(3);
	//for i=1:9
	//	a(i) = INT_MAX;
	//end
	//a=a.';
	//disp(a);
	//b = zeros(3);
	//for i=1:9
	//	b(i) = INT_MAX^exponent;
	//end
	//b=b.';
	//disp(b);
	//c = a.^exponent;
	//disp(c);
	//underflow_test
	//exponent=2;
	//a = zeros(3);
	//for i=1:9
	//	a(i) = INT_MIN;
	//end
	//a=a.';
	//disp(a);
	//b = zeros(3);
	//for i=1:9
	//	b(i) = INT_MIN^exponent;
	//end
	//b=b.';
	//disp(b);
	//c = a.^exponent;
	//disp(c);
	return 0;
}
