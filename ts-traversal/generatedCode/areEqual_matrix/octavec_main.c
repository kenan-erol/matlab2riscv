//
//  areEqual_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Checks that multiple matrices are all equal to each other, even if they're of different type
int trueTest() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create matrix 1
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Matrix 2 is a duplicate of matrix 1
	Matrix *tmp2 = duplicateM(tmp);
	printM(tmp2);


	// Matrix 3 is a (double) duplicate of matrix 1
	double *input3 = NULL;
	input3 = malloc(size*sizeof(*input3));
	for (int i = 0 ; i < size ; i++)
	{
		input3[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp3 = createM(ndim, dim, DOUBLE);
	if ( tmp3 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		flag = false;
	}
	writeM(tmp3, size, input3);
	free(input3);
	printM(tmp3);


	// Matrix 4 is a (double complex) duplicate of matrix 1
	double complex *input4 = NULL;
	input4 = malloc(size*sizeof(*input4));
	for (int i = 0 ; i < size ; i++)
	{
		input4[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp4 = createM(ndim, dim, COMPLEX);
	if ( tmp4 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		flag = false;
	}
	writeM(tmp4, size, input4);
	free(input4);
	printM(tmp4);



	// Make sure that all the matrices are equal
	Matrix *final = areEqualM(4, tmp, tmp2, tmp3, tmp4);
	printM(final);

	Matrix *ones = onesM(ndim, dim);

	// If all the matrices are fully equal, then final will be a matrix of ones
	if(!isEqualM(final, ones))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}
	



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

	if (!destroyM(tmp3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp4))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(final))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(ones))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int falseTest()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);


	// Create matrix 1
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);



	// Matrix 2 is a duplicate of matrix 1
	Matrix *tmp2 = duplicateM(tmp);
	printM(tmp2);


	// Matrix 3 is different than matrix 1 (the lower triangle, excluding the diagonal)
	double *input3 = NULL;
	input3 = malloc(size*sizeof(*input3));
	for (int i = 0 ; i < size ; i++)
	{
		input3[i] = (i+1)*(i+1);
	}
	input3[1] = 10;
	input3[2] = 11;
	input3[5] = 12;
	
	Matrix *tmp3 = createM(ndim, dim, DOUBLE);
	if ( tmp3 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		flag = false;
	}
	writeM(tmp3, size, input3);
	free(input3);
	printM(tmp3);


	// Matrix 4 is different than matrix 1 (the upper triangle, excluding the diagonal)
	double complex *input4 = NULL;
	input4 = malloc(size*sizeof(*input4));
	for (int i = 0 ; i < size ; i++)
	{
		input4[i] = (i+1)*(i+1);
	}
	input4[3] = 13;
	input4[6] = 14;
	input4[7] = 15;
	
	Matrix *tmp4 = createM(ndim, dim, COMPLEX);
	if ( tmp4 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		flag = false;
	}
	writeM(tmp4, size, input4);
	free(input4);
	printM(tmp4);

	// Should return an identity matrix
	Matrix *final = areEqualM(4, tmp, tmp2, tmp3, tmp4);
	printM(final);

	if(!isEqualM(final, id3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}
	
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

	if (!destroyM(tmp3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp4))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(final))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int main()
{
	bool flag = true;
	if(!trueTest()){flag = false;}
	if(!falseTest()){flag = false;}
	return(flag);
}