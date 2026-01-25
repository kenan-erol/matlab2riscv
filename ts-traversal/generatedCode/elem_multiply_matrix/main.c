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
	Matrix * tmp1 = identityM(3);
	complex scalar1 = (0.5 + 1*I);
	Matrix * tmp2 = scaleM(tmp1, &scalar1, 2);
	Matrix * a = tmp2;
	printM(a);
	Matrix * tmp3 = identityM(3);
	complex scalar2 = (0.4 - 0.8*I);
	Matrix * tmp4 = scaleM(tmp3, &scalar2, 2);
	Matrix * b = tmp4;
	Matrix * tmp5 = transposeM(b);
	b = tmp5;
	printM(b);
	Matrix * tmp6 = timesM(a, b);
	Matrix * c = tmp6;
	printM(c);
	Matrix * tmp7 = identityM(3);
	Matrix * tmp8 = timesM(tmp7, a);
	Matrix * d = tmp8;
	printM(d);
	Matrix * tmp9 = identityM(3);
	int scalar3 = 2;
	Matrix * tmp10 = scaleM(tmp9, &scalar3, 0);
	Matrix * e = tmp10;
	printM(e);
	return 0;
}
