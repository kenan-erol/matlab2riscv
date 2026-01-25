//
//  invert_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 6/2/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Invert an integer matrix
int int_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}




	// Create an integer matrix
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1);
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Invert the matrix
	Matrix *tmp2 = invertM(tmp);
	printM(tmp2);


	// Invert the inverse
	Matrix *tmp3 = invM(tmp2);
	printM(tmp3);

	if(!isEqualM(tmp, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
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

	return(flag);
}

// Invert a double matrix
int double_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}


	// Create a double matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(4*sizeof(*input));
	for (int i = 0 ; i < 4 ; i++)
	{
		input[i] = (i+1)+0.5;
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Invert the matrix
	Matrix *tmp2 = invertM(tmp);
	printM(tmp2);

	// Invert the inverse
	Matrix *tmp3 = invM(tmp2);
	printM(tmp3);

	if(!isEqualM(tmp, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
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

	return(flag);
}

// Invert a complex matrix
int complex_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}


	// Create a complex matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(4*sizeof(*input));
	for (int i = 0 ; i < 4 ; i++)
	{
		input[i] = (i+1)+0.5 + (i+1)*I;
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);


	// Invert the matrix
	Matrix *tmp2 = invertM(tmp);
	printM(tmp2);


	// Invert the inverse
	Matrix *tmp3 = invM(tmp2);
	printM(tmp3);

	if(!isEqualM(tmp, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
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

	return(flag);
}

// Should fail to invert a singular matrix
int singular_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *tmp = onesM(ndim, dim);
	printM(tmp);

	Matrix *tmp2 = invertM(tmp);
	if (tmp2)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}


// Should fail to invert a non-square matrix
int non_square_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = onesM(ndim, dim);
	printM(tmp);

	Matrix *tmp2 = invertM(tmp);
	if (tmp2)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

int main()
{
	bool flag = true;

	if(!int_test()){flag = false;}
	if(!double_test()){flag = false;}
	if(!complex_test()){flag = false;}

	if(!singular_test()){flag = false;}
	if(!non_square_test()){flag = false;}

	return(flag);
}
