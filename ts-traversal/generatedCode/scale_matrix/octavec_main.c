//
//  scale_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Multiplies a matrix by a scalar
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	// Create a matrix of ones
	Matrix *ones = onesM(ndim, dim);
	printM(ones);



	// Create a matrix
	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	// Create another matrix
	double complex *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+(i+1)*I;
	}
	
	Matrix *tmp1 = createM(ndim, dim, COMPLEX);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);

	

	int scalar_int = 2;
	double scalar_double = 2.1;
	double complex scalar_complex = 2.1 + I;

	// Multiply the ones by different scalars
	Matrix *out1 = scaleM(ones, &scalar_int, INTEGER);
	Matrix *out2 = scaleM(ones, &scalar_double, DOUBLE);
	Matrix *out3 = scaleM(ones, &scalar_complex, COMPLEX);

	printM(out1);
	printM(out2);
	printM(out3);

	// Multiply the first matrix by different scalars
	Matrix *out4 = scaleM(tmp, &scalar_int, INTEGER);
	Matrix *out5 = scaleM(tmp, &scalar_double, DOUBLE);
	Matrix *out6 = scaleM(tmp, &scalar_complex, COMPLEX);

	printM(out4);
	printM(out5);
	printM(out6);

	// Multiply the second matrix by different scalars
	Matrix *out7 = scaleM(tmp1, &scalar_int, INTEGER);
	Matrix *out8 = scaleM(tmp1, &scalar_double, DOUBLE);
	Matrix *out9 = scaleM(tmp1, &scalar_complex, COMPLEX);

	printM(out7);
	printM(out8);
	printM(out9);


	// Multiplying an int matrix by something that will make it overflow will give a double matrix
	int max = INT_MAX;
	Matrix *overflow_out = scaleM(out1, &max, INTEGER);
	printM(overflow_out);

	int min = INT_MIN;
	Matrix *underflow_out = scaleM(out1, &min, INTEGER);
	printM(underflow_out);

	

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(ones))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out4))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out5))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out6))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out7))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out8))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(out9))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(overflow_out))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(underflow_out))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}
