// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"

// Function declarations
void fourier_vec_script(Matrix * a);

// Entry-point function
int main(void) {

	//more off
	//format short
	//source octaveIncludes.m;
	//function fourier_vec_script(a)
	//
	//	for win_size=1:9
	//		for inc=1:9
	//			for num_coef=2:9
	//				for win_type=1:3
	//					disp(sprintf("win_size: %d, inc: %d, num_coef: %d, win_type: %d\n", win_size, inc, num_coef, win_type));
	//					y = stft(a, win_size, inc, num_coef, win_type);
	//					disp(y);
	//				end
	//			end
	//		end
	//	end
	//end
	//row_vectors_i
	int vec1[10];
	
	for (int iter4 = 0; iter4 < 10; iter4++) {
		vec1[iter4] = -4 + (1)*iter4;
	}
	
	int ndim1 = 2;
	int dim1[2] = {1,10};
	Matrix * a = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 10*sizeof(*input1));
	for (int iter5 = 0; iter5 < 10; iter5++) {
	   input1[0 + iter5] = vec1[iter5];
	}
	writeM( a, 10, input1);
	free(input1);
	
	printM(a);
	fourier_vec_script(a);
	// %row_vectors_d
	// a = [-4:0.5:1.5];
	// disp(a);
	// fourier_vec_script(a);
	// %row_vectors_c
	// a = zeros(1,100);
	// for i=1:100
	// 	a(i) = 101-i + (i-1)*1i;
	// end
	// disp(a);
	// fourier_vec_script(a);
	// %column_vectors_i
	// a = [-4:5].';
	// disp(a);
	// fourier_vec_script(a);
	// %column_vectors_d
	// a = [-4:0.5:1.5].';
	// disp(a);
	// fourier_vec_script(a);
	// %column_vectors_c
	// a = zeros(100,1);
	// for i=1:100
	// 	a(i) = 101-i + (i-1)*1i;
	// end
	// disp(a);
	// fourier_vec_script(a);
	return 0;
}


// Subprograms
	
void fourier_vec_script(Matrix * a) {
	for (int iter1 = 4; iter1 <= 4; iter1++) {
		for (int iter2 = 8; iter2 <= 8; iter2++) {
			for (int iter3 = 2; iter3 <= 2; iter3++) {
				printf("win_size: %d, overlap: %d, num_coef: %d\n", iter1, iter3, iter2);
				int inc1 = iter1 - iter3;
				int num_coef1 = iter2/2;
				Matrix * tmp1 = stftV(a, iter1, inc1, num_coef1, 2);
				Matrix * y = tmp1;
				printM(y);
			
			}
		
		}
	
	}
}