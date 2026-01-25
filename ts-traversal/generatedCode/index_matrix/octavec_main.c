//
//  index_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Demonstrates matrix indexing
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}


	// Create a matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	input[0] = 0;
	input[1] = 10;
	input[2] = 10*I;

	input[3] = 10.102;
	input[4] = 10.102 + 0.5*I;
	input[5] = -12*I;

	input[6] = -0.0002-0.1*I;
	input[7] = -100.01*I;
	input[8] = 81;

	writeM(tmp, size, input);
	free(input);
	printM(tmp);



	// Flat-indexing of the matrix
	for (int i = 1 ; i <= size ; i++)
	{
		double complex index = 0;
		indexM(tmp, &index, 1, i);

		printf("%.5f ", creal(index));		

		if (cimag(index) >= 0)
		{
			printf("+%.5fi \n", cimag(index));
		}
		else
		{
			printf("%.5fi \n", cimag(index));
		}
	}

	// Indexing the matrix by dimension
	for (int i = 1 ; i <= 3 ; i++)
	{
		for (int j = 1 ; j <= 3 ; j++)
		{
			double complex index = 0;
			indexM(tmp, &index, 2, i, j);

			printf("%.5f ", creal(index));		

			if (cimag(index) >= 0)
			{
				printf("+%.5fi \n", cimag(index));
			}
			else
			{
				printf("%.5fi \n", cimag(index));
			}
		}	
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}
