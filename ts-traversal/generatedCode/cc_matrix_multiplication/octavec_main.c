//
//  cc_matrix_multiplication.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>


// Multiply two complex matrices
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create matrix 1
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(size*sizeof(*input));
	input[0] = 1+I;
	input[1] = 0.4*I;
	input[2] = 9-0.5*I;
	input[3] = 16;
	input[4] = 10 + 17*I;	
    input[5] = 35 - 9*I;

	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Create matrix 2
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	input2[0] = 2;
	input2[1] = 0;
	input2[2] = 0;
	input2[3] = I;
	input2[4] = 10 + 17*I;	
    input2[5] = 35 - 9*I;

	writeM(tmp2, size, input2);
	free(input2);

    Matrix *tmp2_transpose = transposeM(tmp2);
	printM(tmp2_transpose);

	// Multiply matrix 1 by matrix 2
	Matrix *product = mtimesM(tmp, tmp2_transpose);
	printM(product);

	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp2_transpose))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
    
	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}