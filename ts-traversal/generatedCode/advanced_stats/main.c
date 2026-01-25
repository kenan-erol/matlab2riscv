// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"

// Function declarations
void two_t_test_intint(Matrix * a, Matrix * b);
void two_t_test_doubleint(Matrix * a, Matrix * b);
void int_vec_stats(Matrix * a);
void double_vec_stats(Matrix * a);
void double_stats_int(Matrix * a);
void double_stats_double(Matrix * a);

// Entry-point function
int main(void) {

	//more off
	
	//format short
	
	//source octaveIncludes.m;
	
	//pkg load statistics;
	
	//row_vectors_i
	
	printf("\n%s\n", "row_vectors_i");
	
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
	int * dim2 = getDimsM(a);
	int ndim2 = 2;
	Matrix * tmp2 = onesM(ndim2, dim2);
	two_t_test_intint(a, tmp2);
	int_vec_stats(a);
	double_stats_int(a);
	printf("\n%s\n", "--------------------");
	//row_vectors_d
	
	printf("\n%s\n", "row_vectors_d");
	
	int ndim3 = 2;
	int dim3[2] = {1,4};
	a = createM(ndim3, dim3, 1);
	double *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 0.5;
	input2[1] = 0.25;
	input2[2] = 0;
	input2[3] = 0.6;
	writeM( a, 4, input2);
	free(input2);
	
	printM(a);
	int * dim4 = getDimsM(a);
	int ndim4 = 2;
	Matrix * tmp4 = onesM(ndim4, dim4);
	two_t_test_doubleint(a, tmp4);
	double_vec_stats(a);
	double_stats_double(a);
	printf("\n%s\n", "--------------------");
	//column_vectors_i
	
	printf("\n%s\n", "column_vectors_i");
	
	int ndim5 = 2;
	int dim5[2] = {4,1};
	a = createM(ndim5, dim5, 0);
	int *input3 = NULL;
	input3 = malloc( 4*sizeof(*input3));
	input3[0] = 3;
	input3[1] = -5;
	input3[2] = 0;
	input3[3] = 1;
	writeM( a, 4, input3);
	free(input3);
	
	printM(a);
	int_vec_stats(a);
	double_stats_int(a);
	printf("\n%s\n", "--------------------");
	//column_vectors_d
	
	printf("\n%s\n", "column_vectors_d");
	
	int ndim6 = 2;
	int dim6[2] = {4,1};
	a = createM(ndim6, dim6, 1);
	double *input4 = NULL;
	input4 = malloc( 4*sizeof(*input4));
	input4[0] = 0.25;
	input4[1] = 0.5;
	input4[2] = 0;
	input4[3] = 0.6;
	writeM( a, 4, input4);
	free(input4);
	
	printM(a);
	double_vec_stats(a);
	double_stats_double(a);
	printf("\n%s\n", "--------------------");
	//matrices_23_i
	
	printf("\n%s\n", "matrices_23_i");
	
	int ndim7 = 2;
	int dim7[2] = {2,3};
	a = createM(ndim7, dim7, 0);
	int *input5 = NULL;
	input5 = malloc( 6*sizeof(*input5));
	input5[0] = 3;
	input5[1] = -2;
	input5[2] = 0;
	input5[3] = 1;
	input5[4] = 5;
	input5[5] = 10;
	writeM( a, 6, input5);
	free(input5);
	
	printM(a);
	double_stats_int(a);
	printf("\n%s\n", "--------------------");
	//matrices_23_d
	
	printf("\n%s\n", "matrices_23_d");
	
	int ndim8 = 2;
	int dim8[2] = {2,3};
	a = createM(ndim8, dim8, 1);
	double *input6 = NULL;
	input6 = malloc( 6*sizeof(*input6));
	input6[0] = 3.25;
	input6[1] = -2;
	input6[2] = 0;
	input6[3] = 1;
	input6[4] = 5;
	input6[5] = 10;
	writeM( a, 6, input6);
	free(input6);
	
	printM(a);
	double_stats_double(a);
	printf("\n%s\n", "--------------------");
	//matrices_32_i
	
	printf("\n%s\n", "matrices_32_i");
	
	int ndim9 = 2;
	int dim9[2] = {3,2};
	a = createM(ndim9, dim9, 0);
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
	double_stats_int(a);
	printf("\n%s\n", "--------------------");
	//matrices_32_d
	
	printf("\n%s\n", "matrices_32_d");
	
	int ndim10 = 2;
	int dim10[2] = {3,2};
	a = createM(ndim10, dim10, 1);
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
	double_stats_double(a);
	printf("\n%s\n", "--------------------");
	//matrices_97_i
	
	int ndim11 = 2;
	int dim11[2] = {7, 9};
	Matrix * tmp5 = zerosM(ndim11, dim11);
	a = tmp5;
	int* lhs_data1 = i_to_i(a);
	for (int iter1 = 1; iter1 <= 63; iter1++) {
		int tmp7 = pow((-1), iter1);
		int tmp8 = pow(iter1, 2);
		int tmp6 = tmp7 * tmp8;
		int idx1 = convertSubscript(ndim11, dim11, iter1);
		lhs_data1[idx1] = tmp6;
	
	}
	// Write matrix mat1
	int size1 = 1;
	for (int iter2 = 0 ; iter2 < ndim11; iter2++)
	{
		size1 *= dim11[iter2];
	}
	Matrix *mat1 = createM(ndim11, dim11, 0);
	writeM(mat1, size1, lhs_data1);
	Matrix * tmp9 = transposeM(mat1);
	a = tmp9;
	printM(a);
	double_stats_int(a);
	//matrices_97_d
	
	int ndim12 = 2;
	int dim12[2] = {7, 9};
	Matrix * tmp10 = zerosM(ndim12, dim12);
	a = tmp10;
	double* lhs_data2 = i_to_d(a);
	for (int iter3 = 1; iter3 <= 63; iter3++) {
		int tmp12 = pow((-1), iter3);
		int tmp13 = pow(iter3, 2);
		double tmp11 = (double) tmp12 * tmp13 / 17;
		int idx2 = convertSubscript(ndim12, dim12, iter3);
		lhs_data2[idx2] = tmp11;
	
	}
	// Write matrix mat2
	int size2 = 1;
	for (int iter4 = 0 ; iter4 < ndim12; iter4++)
	{
		size2 *= dim12[iter4];
	}
	Matrix *mat2 = createM(ndim12, dim12, 1);
	writeM(mat2, size2, lhs_data2);
	Matrix * tmp14 = transposeM(mat2);
	a = tmp14;
	printM(a);
	double_stats_double(a);
	// %big_matrix
	
	// a=ones(32,32);
	
	// disp(a);
	
	// double_stats(a);
	
	// 
	
	// %big_vector
	
	// a=ones(1010,1);
	
	// disp(a);
	
	// int_vec_stats(a);
	
	// double_stats(a);
	
	return 0;
}


// Subprograms
	
void two_t_test_intint(Matrix * a, Matrix * b) {
	bool h1;
	double pval1;
	double *ci1 = NULL;
	ci1 = malloc(2*sizeof(*ci1));
	double tstat1;
	double df1;
	double sd1;
	ttestM_xy(a, b, &h1, &pval1, &ci1, &tstat1, &df1, &sd1);
	printf("h: %d\npval: %.2f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h1, pval1, tstat1, df1, sd1);
}
	
void two_t_test_doubleint(Matrix * a, Matrix * b) {
	bool h2;
	double pval2;
	double *ci2 = NULL;
	ci2 = malloc(2*sizeof(*ci2));
	double tstat2;
	double df2;
	double sd2;
	ttestM_xy(a, b, &h2, &pval2, &ci2, &tstat2, &df2, &sd2);
	printf("h: %d\npval: %.2f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h2, pval2, tstat2, df2, sd2);
}
	
void int_vec_stats(Matrix * a) {
	int index1;
	Matrix * tmp15 = maxV(a, &index1);
	Matrix * greatest = tmp15;
	printM(tmp15);
	printf("max index: %d\n\n", index1);
	int index2;
	Matrix * tmp16 = minV(a, &index2);
	Matrix * least = tmp16;
	printM(tmp16);
	printf("min index: %d\n\n", index2);
	int * tmp17 = i_to_i(tmp16);
	int * tmp18 = i_to_i(tmp15);
	for (double iter5 = 0; iter5 < (int) floor((tmp18[0]-tmp17[0])/0.5) + 1; iter5++) {
		double i = tmp17[0] + iter5*0.5;
		printf("mu: %.3f\n", i);
		bool h3;
		double pval3;
		double *ci3 = NULL;
		ci3 = malloc(2*sizeof(*ci3));
		double tstat3;
		double df3;
		double sd3;
		ttestM(a, i, &h3, &pval3, &ci3, &tstat3, &df3, &sd3);
		printf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h3, pval3, ci3[1 - 1], ci3[2 - 1], tstat3, df3, sd3);
		Matrix * tmp19 = stdM(a);
		double * tmp20 = d_to_d(tmp19);
		bool h4;
		double pval4;
		double *ci4 = NULL;
		ci4 = malloc(2*sizeof(*ci4));
		double z1;
		double zcrit1;
		ztestM(a, i, tmp20[0], &h4, &pval4, &ci4, &z1, &zcrit1);
		printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\n", h4, pval4, ci4[1 - 1], ci4[2 - 1], z1);
	
	}
	//if var(a, 0, "all") == 0
	
	//    i_range = [0:1.0:(var(a, 0, "all")+5)];
	
	//else 
	
	//    i_range = [0:1.0:2*var(a, 0, "all")];
	
	//end
	
	Matrix * tmp21 = varM(a);
	double * tmp22 = d_to_d(tmp21);
	double vec1[(1)*((int) floor((2 * tmp22[0]-0)/1.0) + 1)];
	
	for (int iter6 = 0; iter6 < (int) floor((2 * tmp22[0]-0)/1.0) + 1; iter6++) {
		vec1[iter6] = 0 + (1.0)*iter6;
	}
	
	int ndim13 = 2;
	int dim13[2] = {1,0+(int) floor((2 * tmp22[0]-0)/1.0) + 1};
	Matrix * i_range = createM(ndim13, dim13, 1);
	double *input9 = NULL;
	input9 = malloc( (1)*(0+(int) floor((2 * tmp22[0]-0)/1.0) + 1)*sizeof(*input9));
	for (int iter7 = 0; iter7 < (1)*((int) floor((2 * tmp22[0]-0)/1.0) + 1); iter7++) {
	   input9[0 + iter7] = vec1[iter7];
	}
	writeM( i_range, (1)*(0+(int) floor((2 * tmp22[0]-0)/1.0) + 1), input9);
	free(input9);
	
	
	double i;
	for (int iter8 = 1; iter8 < (1)*(0+(int) floor((2 * tmp22[0]-0)/1.0) + 1); iter8++ ) {
	indexM(i_range, &i, 1, iter8);
		printf("v: %.3f\n", i);
		//[h, pval, ci, stats] = vartest(a, i);
		
		// disp(sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df));
		
		// disp(sprintf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df));
		
	
	}
}
	
void double_vec_stats(Matrix * a) {
	int index3;
	Matrix * tmp23 = maxV(a, &index3);
	Matrix * greatest = tmp23;
	printM(tmp23);
	printf("max index: %d\n", index3);
	int index4;
	Matrix * tmp24 = minV(a, &index4);
	Matrix * least = tmp24;
	printM(tmp24);
	printf("min index: %d\n", index4);
	double * tmp25 = d_to_d(tmp24);
	double * tmp26 = d_to_d(tmp23);
	for (double iter9 = 0; iter9 < (int) floor((tmp26[0]-tmp25[0])/0.5) + 1; iter9++) {
		double i = tmp25[0] + iter9*0.5;
		printf("mu: %.3f\n", i);
		bool h5;
		double pval5;
		double *ci5 = NULL;
		ci5 = malloc(2*sizeof(*ci5));
		double tstat4;
		double df4;
		double sd4;
		ttestM(a, i, &h5, &pval5, &ci5, &tstat4, &df4, &sd4);
		printf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h5, pval5, ci5[1 - 1], ci5[2 - 1], tstat4, df4, sd4);
		Matrix * tmp27 = stdM(a);
		double * tmp28 = d_to_d(tmp27);
		bool h6;
		double pval6;
		double *ci6 = NULL;
		ci6 = malloc(2*sizeof(*ci6));
		double z2;
		double zcrit2;
		ztestM(a, i, tmp28[0], &h6, &pval6, &ci6, &z2, &zcrit2);
		printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\n", h6, pval6, ci6[1 - 1], ci6[2 - 1], z2);
	
	}
	Matrix * tmp29 = varM(a);
	double * tmp30 = d_to_d(tmp29);
	Matrix * tmp31 = varM(a);
	double * tmp32 = d_to_d(tmp31);
	for (double iter10 = 0; iter10 < (int) floor(((tmp32[0] + 0.05)-(tmp30[0] - 0.05))/0.01) + 1; iter10++) {
		double i = (tmp30[0] - 0.05) + iter10*0.01;
		printf("v: %.3f\n", i);
		bool h7;
		double pval7;
		double *ci7 = NULL;
		ci7 = malloc(2*sizeof(*ci7));
		double chisqstat1;
		double df5;
		vartestM(a, i, &h7, &pval7, &ci7, &chisqstat1, &df5);
		// disp(sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df));
		
		// disp(sprintf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df));
		
	
	}
}
	
void double_stats_int(Matrix * a) {
	//disp("double stats");
	
	
	int ndim14 = 2;
	int dim14[2] = {1,10};
	Matrix * fun_qs = createM(ndim14, dim14, 1);
	double *input10 = NULL;
	input10 = malloc( 10*sizeof(*input10));
	input10[0] = 0;
	input10[1] = -1;
	input10[2] = 3;
	input10[3] = 0.2;
	input10[4] = 0.9;
	input10[5] = 0.53;
	input10[6] = 0.75;
	input10[7] = 1;
	input10[8] = 0.34;
	input10[9] = 0.17;
	writeM( fun_qs, 10, input10);
	free(input10);
	
	// Beta PDF
	
	for (double iter11 = 0; iter11 < 19; iter11++) {
		double i = 0 + iter11*0.05;
		for (double iter12 = 0; iter12 < 19; iter12++) {
			double j = 0 + iter12*0.05;
			//disp(sprintf("beta, i: %.3f, j: %.3f\n", i, j));
			
			Matrix * tmp33 = betapdfM(a, i, j);
			printM(tmp33);
		
		}
	
	}
	// Exponential PDF
	
	for (double iter13 = 0; iter13 < 99; iter13++) {
		double lambda = 0.05 + iter13*0.05;
		//disp(sprintf("lamvda, l: %.3f\n", lambda));
		
		Matrix * tmp34 = exppdfM(a, lambda);
		printM(tmp34);
	
	}
	// Chi-square PDF
	
	for (double iter14 = 0; iter14 < 99; iter14++) {
		double n = 0.05 + iter14*0.05;
		//disp(sprintf("chi, n: %.3f\n", n));
		
		//disp(sprintf("n = %.3f\n", n));
		
		Matrix * tmp35 = chi2pdfM(a, n);
		printM(tmp35);
	
	}
	// Gamma PDF
	
	for (double iter15 = 0; iter15 < 7; iter15++) {
		double i = 0.25 + iter15*0.25;
		for (double iter16 = 0; iter16 < 7; iter16++) {
			double j = 0.25 + iter16*0.25;
			//disp(sprintf("gamma, i: %.3f, j: %.3f\n", i, j));
			
			Matrix * tmp36 = gampdfM(a, i, j);
			printM(tmp36);
		
		}
	
	}
	// Lognormal PDF
	
	for (double iter17 = 0; iter17 < 15; iter17++) {
		double mu = -2 + iter17*0.5;
		for (double iter18 = 0; iter18 < 10; iter18++) {
			double sd = 0.5 + iter18*0.5;
			Matrix * tmp37 = lognpdfM(a, mu, sd);
			printM(tmp37);
		
		}
	
	}
	// Normal PDF
	
	for (double iter19 = 0; iter19 < 15; iter19++) {
		double mu = -2 + iter19*0.5;
		for (double iter20 = 0; iter20 < 10; iter20++) {
			double sd = 0.5 + iter20*0.5;
			Matrix * tmp38 = normpdfM(a, mu, sd);
			printM(tmp38);
		
		}
	
	}
	// Uniform discrete PDF
	
	for (int iter21 = 1; iter21 <= 9; iter21++) {
		printf("n = %d\n", iter21);
		Matrix * tmp39 = unidpdfM(a, iter21);
		printM(tmp39);
	
	}
}
	
void double_stats_double(Matrix * a) {
	//disp("double stats");
	
	
	int ndim15 = 2;
	int dim15[2] = {1,10};
	Matrix * fun_qs = createM(ndim15, dim15, 1);
	double *input11 = NULL;
	input11 = malloc( 10*sizeof(*input11));
	input11[0] = 0;
	input11[1] = -1;
	input11[2] = 3;
	input11[3] = 0.2;
	input11[4] = 0.9;
	input11[5] = 0.53;
	input11[6] = 0.75;
	input11[7] = 1;
	input11[8] = 0.34;
	input11[9] = 0.17;
	writeM( fun_qs, 10, input11);
	free(input11);
	
	// Beta PDF
	
	for (double iter22 = 0; iter22 < 19; iter22++) {
		double i = 0 + iter22*0.05;
		for (double iter23 = 0; iter23 < 19; iter23++) {
			double j = 0 + iter23*0.05;
			//disp(sprintf("beta, i: %.3f, j: %.3f\n", i, j));
			
			Matrix * tmp40 = betapdfM(a, i, j);
			printM(tmp40);
		
		}
	
	}
	// Exponential PDF
	
	for (double iter24 = 0; iter24 < 99; iter24++) {
		double lambda = 0.05 + iter24*0.05;
		//disp(sprintf("lamvda, l: %.3f\n", lambda));
		
		Matrix * tmp41 = exppdfM(a, lambda);
		printM(tmp41);
	
	}
	// Chi-square PDF
	
	for (double iter25 = 0; iter25 < 99; iter25++) {
		double n = 0.05 + iter25*0.05;
		//disp(sprintf("chi, n: %.3f\n", n));
		
		//disp(sprintf("n = %.3f\n", n));
		
		Matrix * tmp42 = chi2pdfM(a, n);
		printM(tmp42);
	
	}
	// Gamma PDF
	
	for (double iter26 = 0; iter26 < 7; iter26++) {
		double i = 0.25 + iter26*0.25;
		for (double iter27 = 0; iter27 < 7; iter27++) {
			double j = 0.25 + iter27*0.25;
			//disp(sprintf("gamma, i: %.3f, j: %.3f\n", i, j));
			
			Matrix * tmp43 = gampdfM(a, i, j);
			printM(tmp43);
		
		}
	
	}
	// Lognormal PDF
	
	for (double iter28 = 0; iter28 < 15; iter28++) {
		double mu = -2 + iter28*0.5;
		for (double iter29 = 0; iter29 < 10; iter29++) {
			double sd = 0.5 + iter29*0.5;
			Matrix * tmp44 = lognpdfM(a, mu, sd);
			printM(tmp44);
		
		}
	
	}
	// Normal PDF
	
	for (double iter30 = 0; iter30 < 15; iter30++) {
		double mu = -2 + iter30*0.5;
		for (double iter31 = 0; iter31 < 10; iter31++) {
			double sd = 0.5 + iter31*0.5;
			Matrix * tmp45 = normpdfM(a, mu, sd);
			printM(tmp45);
		
		}
	
	}
	// Uniform discrete PDF
	
	for (int iter32 = 1; iter32 <= 9; iter32++) {
		printf("n = %d\n", iter32);
		Matrix * tmp46 = unidpdfM(a, iter32);
		printM(tmp46);
	
	}
}