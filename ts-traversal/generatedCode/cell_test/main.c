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

	// Structure for cell arrays
	struct cell {
		int type;
		union {
			int ival;
			double dval;
			complex double cval;
			char chval[20];
		} data;
	};
	int A = 1;
	
	int ndim1 = 2;
	int dim1[2] = {1,2};
	Matrix * B = createM(ndim1, dim1, 2);
	complex *input1 = NULL;
	input1 = malloc( 2*sizeof(*input1));
	input1[0] = 2.0;
	input1[1] = 3 + 5*I;
	writeM( B, 2, input1);
	free(input1);
	
	struct cell C[3];
	C[0].type = 3;
	strcpy(C[0].data.chval, "hello world");
	C[1].type = 0;
	C[1].data.ival = 1;
	C[2].type = 1;
	C[2].data.dval = 2.0;
	
	for (int iter1 = 1; iter1 <= 3; iter1++) {
		int ndim2 = 2;
		int dim2[2] = {1,15};
		int idx1 = convertSubscript(ndim2, dim2, iter1);
		struct cell tmp1 = C[idx1];
		switch(tmp1.type) {
			case 0:
			printf("\n%d\n", tmp1.data.ival);
			break;
		        
			case 1:
			printf("\n%f\n", tmp1.data.dval);
			break;
		        
			case 2:
			printf("\n%f\n", tmp1.data.cval);
			break;
		        
			case 3:
			printf("\n%s\n", tmp1.data.chval);
			break;
		}
	
	}
	//disp(C{1});
	return 0;
}
