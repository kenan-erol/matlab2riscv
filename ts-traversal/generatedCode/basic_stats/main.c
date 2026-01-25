// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"

// Function declarations
void normfit_int(Matrix * a, Matrix ** p_mu, Matrix ** p_sd);
void normfit_double(Matrix * a, Matrix ** p_mu, Matrix ** p_sd);
void normfit_complex(Matrix * a, Matrix ** p_mu, Matrix ** p_sd);
void unifit_int(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat);
void unifit_double(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat);
void unifit_complex(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat);
void int_vec_stats(Matrix * a);
void double_vec_stats(Matrix * a);
void complex_vec_stats(Matrix * a);
void int_stats(Matrix * a);
void double_stats(Matrix * a);
void complex_stats(Matrix * a);

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
	int_vec_stats(a);
	int_stats(a);
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
	double_vec_stats(a);
	double_stats(a);
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
	complex_vec_stats(a);
	complex_stats(a);
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
	int_vec_stats(a);
	int_stats(a);
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
	double_vec_stats(a);
	double_stats(a);
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
	complex_vec_stats(a);
	complex_stats(a);
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
	int_stats(a);
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
	double_stats(a);
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
	complex_stats(a);
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
	int_stats(a);
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
	double_stats(a);
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
	complex_stats(a);
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
	int_stats(a);
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
	double_stats(a);
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
	complex_stats(a);
	//basic_quantile_test
	
	int vec1[100];
	
	for (int iter7 = 0; iter7 < 100; iter7++) {
		vec1[iter7] = 1 + (1)*iter7;
	}
	
	int ndim16 = 2;
	int dim16[2] = {1,100};
	a = createM(ndim16, dim16, 0);
	int *input13 = NULL;
	input13 = malloc( 100*sizeof(*input13));
	for (int iter8 = 0; iter8 < 100; iter8++) {
	   input13[0 + iter8] = vec1[iter8];
	}
	writeM( a, 100, input13);
	free(input13);
	
	double vec2[101];
	
	for (int iter9 = 0; iter9 < 101; iter9++) {
		vec2[iter9] = 0 + (0.01)*iter9;
	}
	Matrix * tmp15 = quantileM_vec(a, 101, vec2);
	Matrix * tmp16 = transposeM(tmp15);
	printM(tmp16);
	int ndim17 = 2;
	int dim17[2] = {1, 1004};
	Matrix * tmp17 = zerosM(ndim17, dim17);
	Matrix * b = tmp17;
	double* lhs_data4 = i_to_d(b);
	for (int iter10 = 1; iter10 <= 1004; iter10++) {
		double tmp18 = (double) iter10 * iter10 / 17;
		int idx4 = convertSubscript(ndim17, dim17, iter10);
		lhs_data4[idx4] = tmp18;
	
	}
	// Write matrix mat4
	int size4 = 1;
	for (int iter11 = 0 ; iter11 < ndim17; iter11++)
	{
		size4 *= dim17[iter11];
	}
	Matrix *mat4 = createM(ndim17, dim17, 1);
	writeM(mat4, size4, lhs_data4);
	Matrix * tmp19 = transposeM(mat4);
	b = tmp19;
	double vec3[101];
	
	for (int iter12 = 0; iter12 < 101; iter12++) {
		vec3[iter12] = 0 + (0.01)*iter12;
	}
	Matrix * tmp20 = quantileM_vec(b, 101, vec3);
	Matrix * tmp21 = transposeM(tmp20);
	printM(tmp21);
	int ndim18 = 2;
	int dim18[2] = {1, 57};
	Matrix * tmp22 = zerosM(ndim18, dim18);
	Matrix * c = tmp22;
	complex* lhs_data5 = i_to_c(c);
	for (int iter13 = 1; iter13 <= 57; iter13++) {
		complex tmp23 = iter13 - ((complex) iter13) / (17*I);
		int idx5 = convertSubscript(ndim18, dim18, iter13);
		lhs_data5[idx5] = tmp23;
	
	}
	// Write matrix mat5
	int size5 = 1;
	for (int iter14 = 0 ; iter14 < ndim18; iter14++)
	{
		size5 *= dim18[iter14];
	}
	Matrix *mat5 = createM(ndim18, dim18, 2);
	writeM(mat5, size5, lhs_data5);
	Matrix * tmp24 = transposeM(mat5);
	c = tmp24;
	//disp(quantile(c, 0:0.01:1).');
	
	return 0;
}


// Subprograms
	
void normfit_int(Matrix * a, Matrix ** p_mu, Matrix ** p_sd) {
	Matrix * tmp25 = meanM(a);
	Matrix * mu = tmp25;
	Matrix * tmp26 = stdM(a);
	Matrix * sd = tmp26;
	*p_mu = mu;
	*p_sd = sd;
}
	
void normfit_double(Matrix * a, Matrix ** p_mu, Matrix ** p_sd) {
	Matrix * tmp27 = meanM(a);
	Matrix * mu = tmp27;
	Matrix * tmp28 = stdM(a);
	Matrix * sd = tmp28;
	*p_mu = mu;
	*p_sd = sd;
}
	
void normfit_complex(Matrix * a, Matrix ** p_mu, Matrix ** p_sd) {
	Matrix * tmp29 = meanM(a);
	Matrix * mu = tmp29;
	Matrix * tmp30 = stdM(a);
	Matrix * sd = tmp30;
	*p_mu = mu;
	*p_sd = sd;
}
	
void unifit_int(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat) {
	Matrix * tmp31 = minM(a);
	Matrix * ahat = tmp31;
	Matrix * tmp32 = maxM(a);
	Matrix * bhat = tmp32;
	*p_ahat = ahat;
	*p_bhat = bhat;
}
	
void unifit_double(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat) {
	Matrix * tmp33 = minM(a);
	Matrix * ahat = tmp33;
	Matrix * tmp34 = maxM(a);
	Matrix * bhat = tmp34;
	*p_ahat = ahat;
	*p_bhat = bhat;
}
	
void unifit_complex(Matrix * a, Matrix ** p_ahat, Matrix ** p_bhat) {
	Matrix * tmp35 = minM(a);
	Matrix * ahat = tmp35;
	Matrix * tmp36 = maxM(a);
	Matrix * bhat = tmp36;
	*p_ahat = ahat;
	*p_bhat = bhat;
}
	
void int_vec_stats(Matrix * a) {
	int index1;
	Matrix * tmp37 = maxV(a, &index1);
	Matrix * greatest = tmp37;
	printM(tmp37);
	printf("max index: %d\n", index1);
	int index2;
	Matrix * tmp38 = minV(a, &index2);
	Matrix * least = tmp38;
	printM(tmp38);
	printf("min index: %d\n", index2);
	Matrix * mu1 = NULL;
	Matrix * sd1 = NULL;
	normfit_int(a, &mu1, &sd1);
	double * tmp39 = d_to_d(mu1);
	printf("mean: %.3f\n", tmp39[0]);
	double * tmp40 = d_to_d(sd1);
	printf("sd: %.3f\n", tmp40[0]);
	Matrix * ahat1 = NULL;
	Matrix * bhat1 = NULL;
	unifit_int(a, &ahat1, &bhat1);
	int * tmp41 = i_to_i(ahat1);
	printf("a: %d\n", tmp41[0]);
	int * tmp42 = i_to_i(bhat1);
	printf("b: %d\n", tmp42[0]);
}
	
void double_vec_stats(Matrix * a) {
	int index3;
	Matrix * tmp43 = maxV(a, &index3);
	Matrix * greatest = tmp43;
	printM(tmp43);
	printf("max index: %d\n", index3);
	int index4;
	Matrix * tmp44 = minV(a, &index4);
	Matrix * least = tmp44;
	printM(tmp44);
	printf("min index: %d\n", index4);
	Matrix * mu2 = NULL;
	Matrix * sd2 = NULL;
	normfit_double(a, &mu2, &sd2);
	double * tmp45 = d_to_d(mu2);
	printf("mean: %.3f\n", tmp45[0]);
	double * tmp46 = d_to_d(sd2);
	printf("sd: %.3f\n", tmp46[0]);
	Matrix * ahat2 = NULL;
	Matrix * bhat2 = NULL;
	unifit_double(a, &ahat2, &bhat2);
	double * tmp47 = d_to_d(ahat2);
	printf("a: %.3f\n", tmp47[0]);
	double * tmp48 = d_to_d(bhat2);
	printf("b: %.3f\n", tmp48[0]);
}
	
void complex_vec_stats(Matrix * a) {
	int index5;
	Matrix * tmp49 = maxV(a, &index5);
	Matrix * greatest = tmp49;
	printM(tmp49);
	printf("max index: %d\n", index5);
	int index6;
	Matrix * tmp50 = minV(a, &index6);
	Matrix * least = tmp50;
	printM(tmp50);
	printf("min index: %d\n", index6);
	Matrix * mu3 = NULL;
	Matrix * sd3 = NULL;
	normfit_complex(a, &mu3, &sd3);
	complex * tmp51 = c_to_c(mu3);
	double tmp52 = creal(tmp51[0]);
	double tmp53 = cimag(tmp51[0]);
	printf("mean: %.3f + %.3fi\n", tmp52, tmp53);
	complex * tmp54 = c_to_c(sd3);
	double tmp55 = creal(tmp54[0]);
	double tmp56 = cimag(tmp54[0]);
	printf("sd: %.3f + %.3fi\n", tmp55, tmp56);
	Matrix * ahat3 = NULL;
	Matrix * bhat3 = NULL;
	unifit_complex(a, &ahat3, &bhat3);
	complex * tmp57 = c_to_c(ahat3);
	double tmp58 = creal(tmp57[0]);
	double tmp59 = cimag(tmp57[0]);
	printf("a: %.3f + %.3fi\n", tmp58, tmp59);
	complex * tmp60 = c_to_c(bhat3);
	double tmp61 = creal(tmp60[0]);
	double tmp62 = cimag(tmp60[0]);
	printf("b: %.3f + %.3fi\n", tmp61, tmp62);
}
	
void int_stats(Matrix * a) {
	
	int ndim19 = 2;
	int dim19[2] = {1,8};
	Matrix * fun_qs = createM(ndim19, dim19, 1);
	double *input14 = NULL;
	input14 = malloc( 8*sizeof(*input14));
	input14[0] = 0;
	input14[1] = 0.2;
	input14[2] = 0.9;
	input14[3] = 0.53;
	input14[4] = 0.75;
	input14[5] = 1;
	input14[6] = 0.34;
	input14[7] = 0.17;
	writeM( fun_qs, 8, input14);
	free(input14);
	
	Matrix * tmp63 = meanM(a);
	printM(tmp63);
	Matrix * tmp64 = varM(a);
	printM(tmp64);
	Matrix * tmp65 = popvarM(a);
	printM(tmp65);
	Matrix * tmp66 = stdM(a);
	printM(tmp66);
	Matrix * tmp67 = popstdM(a);
	printM(tmp67);
	Matrix * tmp68 = sortM(a, 0);
	printM(tmp68);
	Matrix * tmp69 = sortM(a, 1);
	printM(tmp69);
	Matrix * tmp70 = medianM(a);
	printM(tmp70);
	Matrix * tmp71 = minM(a);
	printM(tmp71);
	Matrix * tmp72 = maxM(a);
	printM(tmp72);
	double vec4[4] = {};
	
	for (int i = 1; 0.2*i < 1; i ++) {
	    vec4[i-1] = 0.2*i;
	}
	                
	Matrix * tmp73 = quantileM_vec(a, 4, vec4);
	printM(tmp73);
	double * vec5 = d_to_d(fun_qs);
	Matrix * tmp74 = quantileM_vec(a, 8, vec5);
	printM(tmp74);
}
	
void double_stats(Matrix * a) {
	
	int ndim20 = 2;
	int dim20[2] = {1,8};
	Matrix * fun_qs = createM(ndim20, dim20, 1);
	double *input15 = NULL;
	input15 = malloc( 8*sizeof(*input15));
	input15[0] = 0;
	input15[1] = 0.2;
	input15[2] = 0.9;
	input15[3] = 0.53;
	input15[4] = 0.75;
	input15[5] = 1;
	input15[6] = 0.34;
	input15[7] = 0.17;
	writeM( fun_qs, 8, input15);
	free(input15);
	
	Matrix * tmp75 = meanM(a);
	printM(tmp75);
	Matrix * tmp76 = varM(a);
	printM(tmp76);
	Matrix * tmp77 = popvarM(a);
	printM(tmp77);
	Matrix * tmp78 = stdM(a);
	printM(tmp78);
	Matrix * tmp79 = popstdM(a);
	printM(tmp79);
	Matrix * tmp80 = sortM(a, 0);
	printM(tmp80);
	Matrix * tmp81 = sortM(a, 1);
	printM(tmp81);
	Matrix * tmp82 = medianM(a);
	printM(tmp82);
	Matrix * tmp83 = minM(a);
	printM(tmp83);
	Matrix * tmp84 = maxM(a);
	printM(tmp84);
	double vec6[4] = {};
	
	for (int i = 1; 0.2*i < 1; i ++) {
	    vec6[i-1] = 0.2*i;
	}
	                
	Matrix * tmp85 = quantileM_vec(a, 4, vec6);
	printM(tmp85);
	double * vec7 = d_to_d(fun_qs);
	Matrix * tmp86 = quantileM_vec(a, 8, vec7);
	printM(tmp86);
}
	
void complex_stats(Matrix * a) {
	
	int ndim21 = 2;
	int dim21[2] = {1,8};
	Matrix * fun_qs = createM(ndim21, dim21, 1);
	double *input16 = NULL;
	input16 = malloc( 8*sizeof(*input16));
	input16[0] = 0;
	input16[1] = 0.2;
	input16[2] = 0.9;
	input16[3] = 0.53;
	input16[4] = 0.75;
	input16[5] = 1;
	input16[6] = 0.34;
	input16[7] = 0.17;
	writeM( fun_qs, 8, input16);
	free(input16);
	
	Matrix * tmp87 = meanM(a);
	printM(tmp87);
	Matrix * tmp88 = varM(a);
	printM(tmp88);
	Matrix * tmp89 = popvarM(a);
	printM(tmp89);
	Matrix * tmp90 = stdM(a);
	printM(tmp90);
	Matrix * tmp91 = popstdM(a);
	printM(tmp91);
	Matrix * tmp92 = sortM(a, 0);
	printM(tmp92);
	Matrix * tmp93 = sortM(a, 1);
	printM(tmp93);
	Matrix * tmp94 = medianM(a);
	printM(tmp94);
	Matrix * tmp95 = minM(a);
	printM(tmp95);
	Matrix * tmp96 = maxM(a);
	printM(tmp96);
	//disp(quantile(a, 4));
	
	//disp(quantile(a, fun_qs));
	
}