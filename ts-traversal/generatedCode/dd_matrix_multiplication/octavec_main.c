//
//  dd_matrix_multiplication.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Multiplies two double matrices
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

	// Create the first matrix
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
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Create the second matrix
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2;
		}
	}

	writeM(tmp2, size, input2);
	free(input2);

    Matrix *tmp2_transpose = transposeM(tmp2);
	printM(tmp2_transpose);

	// Multiply the two matrices
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