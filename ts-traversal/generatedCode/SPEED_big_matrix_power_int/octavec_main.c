//
//  SPEED_big_matrix_power_int.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Speedtest: raise a large double matrix to an int power
int main() 
{
	bool flag = true;
	int exponent = 10;

	// Initialize matrix metadata
	int ndim = 2;
	int dim[2] = {1000,1000};

	// Initialize matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Create the matrix data and write it
	int size = 1000*1000;
	double *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5;
	}

	writeM(tmp, size, input);
	free(input); // writeM copies the input into the matrix, so it can be freed immediately


	// The meat of the speed test
	int iterations = 1;
	Matrix **products = NULL;
	products = malloc(iterations*sizeof(*products));

	// Raise the matrix to the exponent <iterations> number of times
	// Raising a matrix generates a second matrix
	for (int i = 0 ; i < iterations ; i++)
	{
		products[i] = mpowerM(tmp, &exponent, INTEGER);
		//printM(products[i]);
	}

	// Clean up all the product matrices created in the loop
	for (int i = 0 ; i < iterations ; i++)
	{
		if (!destroyM(products[i]))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
	}
	free(products);

	// Clean up the original matrices
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}