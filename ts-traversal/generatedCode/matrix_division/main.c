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
	//ldivide_test1
	
	int ndim1 = 2;
	int dim1[2] = {2,2};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 4*sizeof(*input1));
	input1[0] = 1;
	input1[1] = 4;
	input1[2] = 9;
	input1[3] = 16;
	writeM( a, 4, input1);
	free(input1);
	
	
	int ndim2 = 2;
	int dim2[2] = {2,2};
	Matrix * b = createM(ndim2, dim2, 2);
	complex *input2 = NULL;
	input2 = malloc( 4*sizeof(*input2));
	input2[0] = 2.1 + 0.5 * 1*I;
	input2[1] = 0;
	input2[2] = 0;
	input2[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input2);
	free(input2);
	
	Matrix * tmp1 = mldivideM(a, b);
	Matrix * c = tmp1;
	printM(a);
	printM(b);
	printM(c);
	//ldivide_test2
	Matrix * tmp2 = identityM(2);
	a = tmp2;
	
	int ndim3 = 2;
	int dim3[2] = {2,2};
	b = createM(ndim3, dim3, 2);
	complex *input3 = NULL;
	input3 = malloc( 4*sizeof(*input3));
	input3[0] = 2.1 + 0.5 * 1*I;
	input3[1] = 0;
	input3[2] = 0;
	input3[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input3);
	free(input3);
	
	Matrix * tmp3 = mldivideM(a, b);
	c = tmp3;
	printM(a);
	printM(b);
	printM(c);
	//ldivide_test3
	
	int ndim4 = 2;
	int dim4[2] = {2,2};
	a = createM(ndim4, dim4, 2);
	complex *input4 = NULL;
	input4 = malloc( 4*sizeof(*input4));
	input4[0] = 2.1 + 0.5 * 1*I;
	input4[1] = 0;
	input4[2] = 0;
	input4[3] = 2.1 + 0.5 * 1*I;
	writeM( a, 4, input4);
	free(input4);
	
	
	int ndim5 = 2;
	int dim5[2] = {2,2};
	b = createM(ndim5, dim5, 2);
	complex *input5 = NULL;
	input5 = malloc( 4*sizeof(*input5));
	input5[0] = 2.1 + 0.5 * 1*I;
	input5[1] = 0;
	input5[2] = 0;
	input5[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input5);
	free(input5);
	
	Matrix * tmp4 = mldivideM(a, b);
	c = tmp4;
	printM(a);
	printM(b);
	printM(c);
	//rdivide_test1
	
	int ndim6 = 2;
	int dim6[2] = {2,2};
	a = createM(ndim6, dim6, 0);
	int *input6 = NULL;
	input6 = malloc( 4*sizeof(*input6));
	input6[0] = 1;
	input6[1] = 4;
	input6[2] = 9;
	input6[3] = 16;
	writeM( a, 4, input6);
	free(input6);
	
	
	int ndim7 = 2;
	int dim7[2] = {2,2};
	b = createM(ndim7, dim7, 2);
	complex *input7 = NULL;
	input7 = malloc( 4*sizeof(*input7));
	input7[0] = 2.1 + 0.5 * 1*I;
	input7[1] = 0;
	input7[2] = 0;
	input7[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input7);
	free(input7);
	
	Matrix * tmp5 = mrdivideM(a, b);
	c = tmp5;
	printM(a);
	printM(b);
	printM(c);
	//rdivide_test2
	Matrix * tmp6 = identityM(2);
	a = tmp6;
	
	int ndim8 = 2;
	int dim8[2] = {2,2};
	b = createM(ndim8, dim8, 2);
	complex *input8 = NULL;
	input8 = malloc( 4*sizeof(*input8));
	input8[0] = 2.1 + 0.5 * 1*I;
	input8[1] = 0;
	input8[2] = 0;
	input8[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input8);
	free(input8);
	
	Matrix * tmp7 = mrdivideM(a, b);
	c = tmp7;
	printM(a);
	printM(b);
	printM(c);
	//rdivide_test3
	
	int ndim9 = 2;
	int dim9[2] = {2,2};
	a = createM(ndim9, dim9, 2);
	complex *input9 = NULL;
	input9 = malloc( 4*sizeof(*input9));
	input9[0] = 2.1 + 0.5 * 1*I;
	input9[1] = 0;
	input9[2] = 0;
	input9[3] = 2.1 + 0.5 * 1*I;
	writeM( a, 4, input9);
	free(input9);
	
	
	int ndim10 = 2;
	int dim10[2] = {2,2};
	b = createM(ndim10, dim10, 2);
	complex *input10 = NULL;
	input10 = malloc( 4*sizeof(*input10));
	input10[0] = 2.1 + 0.5 * 1*I;
	input10[1] = 0;
	input10[2] = 0;
	input10[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input10);
	free(input10);
	
	Matrix * tmp8 = mrdivideM(a, b);
	c = tmp8;
	printM(a);
	printM(b);
	printM(c);
	//divide_by_zero
	int ndim11 = 2;
	int dim11[2] = {2,2};
	Matrix * tmp9 = zerosM(ndim11, dim11);
	a = tmp9;
	
	int ndim12 = 2;
	int dim12[2] = {2,2};
	b = createM(ndim12, dim12, 2);
	complex *input11 = NULL;
	input11 = malloc( 4*sizeof(*input11));
	input11[0] = 2.1 + 0.5 * 1*I;
	input11[1] = 0;
	input11[2] = 0;
	input11[3] = 2.1 + 0.5 * 1*I;
	writeM( b, 4, input11);
	free(input11);
	
	Matrix * tmp10 = mrdivideM(b, a);
	c = tmp10;
	printM(a);
	printM(b);
	return 0;
}
