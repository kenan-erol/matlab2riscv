//
//  cd_matrix_multiplication.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Multiplies a complex matrix by a double matrix
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

	// Create the double matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	
	double *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5;
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	// Create the complex matrix
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	input2[0] = 2.1+0.5*I;
	input2[1] = 0;
	input2[2] = 0;
	input2[3] = 2.1+0.5*I;
	input2[4] = 0;
	input2[5] = 0;

	writeM(tmp2, size, input2);
	free(input2);

    Matrix *tmp2_transpose = transposeM(tmp2);
	printM(tmp2_transpose);

	// Multiply the two matrices
	Matrix *product = mtimesM(tmp2_transpose, tmp);
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