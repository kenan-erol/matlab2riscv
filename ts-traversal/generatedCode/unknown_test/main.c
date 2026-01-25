// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"
#include "../unknownTypes.h"

	
Matrix * myfun(Matrix * A) {
	int *A_dim = getDimsM(A);
	int A_dim0 = A_dim[0];
	int A_dim1 = A_dim[1];
	int A_type = gettypeM(A);
	Matrix * tmp1 = ctransposeM(A);
	Matrix * B = tmp1;
	int ndim1 = getnDimM(B);
	int *dim1 = getDimsM(B);
	int idx1 = convertSubscript(ndim1, dim1, (A_dim1)*(A_dim0));
	struct generic_val tmp2;
	unknown_indexM(&tmp2, A_type, B, 1, idx1+1);
	unknown_printf(&tmp2);
	struct generic_val tmp3;
	unknown_indexM(&tmp3, A_type, B, 1, (1-1) + (A_dim1-1)*A_dim0 + (1-1)*A_dim1*A_dim0 + (1-1)*A_dim1*A_dim0*1);
	unknown_printf(&tmp3);
	return B;
}