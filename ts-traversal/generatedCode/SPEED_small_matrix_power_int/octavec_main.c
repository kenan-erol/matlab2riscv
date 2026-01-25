//
//  SPEED_small_matrix_power_int.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Speedtest: raise a complex matrix to an integer power
int main() 
{
	bool flag = true;

	// Initialize matrix metadata
	int ndim = 2;
	int dim[2] = {2,2};

	// Initialize the matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Create the matrix data and write it
	double complex *input = NULL;
	input = malloc(4*sizeof(*input));
	input[0] = 2.1+0.5*I;
	input[1] = 1;
	input[2] = 0;
	input[3] = 2.1+0.5*I;

	writeM(tmp, 4, input);
	free(input); // writeM copies the input into the matrix, so it can be freed immediately
	printM(tmp);


	// The meat of the speed test
	int exponent = 30;
	int iterations = 100000;
	Matrix **products = NULL;
	products = malloc(iterations*sizeof(*products));

	Matrix *initial_product = mpowerM(tmp, &exponent, INTEGER);
	printM(initial_product);
	if (!destroyM(initial_product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	// Multiply the two matrices together <iterations> number of times
	// Multiplying two matrices together generates a third matrix
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

	// Clean up the original matrix
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}