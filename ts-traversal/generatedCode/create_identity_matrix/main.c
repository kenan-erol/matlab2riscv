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
	Matrix * tmp1 = identityM(8);
	Matrix * a = tmp1;
	Matrix * tmp2 = identityM(4);
	Matrix * b = tmp2;
	Matrix * tmp3 = identityM(1);
	Matrix * c = tmp3;
	printM(a);
	printM(b);
	printM(c);
	return 0;
}
