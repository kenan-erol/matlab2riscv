//
//  transpose_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Transposes an int matrix
int int_test() 
{
	int ndim = 2;
	int dim[2] = {3,6};


	// Create a matrix
	int *input = NULL;
	input = malloc(18*sizeof(*input));
	for (int i = 0 ; i < 18 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 18, input);
	free(input);
	printM(tmp);


	// Transpose a matrix
	Matrix *tmp_t = transposeM(tmp);
	printM(tmp_t);
	
	// Transpose the transpose
	Matrix *tmp_t_t = transposeM(tmp_t);
	printM(tmp_t_t);

	if (!isEqualM(tmp, tmp_t_t))
	{
		fprintf(stdout, "Matrix & double transposed aren't equal\n");
		return(false);
	}
	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}
	
	return(true);
}

int double_test() 
{
	int ndim = 2;
	int dim[2] = {3,6};



	// Create a double matrix
	double *input = NULL;
	input = malloc(18*sizeof(*input));
	for (int i = 0 ; i < 18 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5;
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 18, input);
	free(input);
	printM(tmp);

	// Transpose the matrix
	Matrix *tmp_t = transposeM(tmp);
	printM(tmp_t);

	// Transpose the transpose
	Matrix *tmp_t_t = transposeM(tmp_t);
	printM(tmp_t_t);

	if (!isEqualM(tmp, tmp_t_t))
	{
		fprintf(stdout, "Matrix & double transposed aren't equal\n");
		return(false);
	}
	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}
	
	return(true);
}

int complex_test() 
{
	int ndim = 2;
	int dim[2] = {3,6};



	// Create a complex matrix
	double complex *input = NULL;
	input = malloc(18*sizeof(*input));
	for (int i = 0 ; i < 18 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5 - 0.5*I;
	}
	
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 18, input);
	free(input);
	printM(tmp);

	// Transpose the matrix
	Matrix *tmp_t = transposeM(tmp);
	printM(tmp_t);

	// Transpose the transpose
	Matrix *tmp_t_t = transposeM(tmp_t);	
	printM(tmp_t_t);

	if (!isEqualM(tmp, tmp_t_t))
	{
		fprintf(stdout, "Matrix & double transposed aren't equal\n");
		return(false);
	}
	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}
	
	return(true);
}

int complex_conjugate_test() 
{
	int ndim = 2;
	int dim[2] = {3,6};



	// Create a complex matrix
	double complex *input = NULL;
	input = malloc(18*sizeof(*input));
	for (int i = 0 ; i < 18 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5 - 0.5*I;
	}
	
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 18, input);
	free(input);
	printM(tmp);


	// Complex conjugate transpose
	Matrix *tmp_t = ctransposeM(tmp);
	printM(tmp_t);

	// Complex conjugate transpose the transpose
	Matrix *tmp_t_t = ctransposeM(tmp_t);
	printM(tmp_t_t);

	if (!isEqualM(tmp, tmp_t_t))
	{
		fprintf(stdout, "Matrix & double transposed aren't equal\n");
		return(false);
	}
	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}

	if (!destroyM(tmp_t_t))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		return(false);
	}
	
	return(true);
}

int main()
{
	bool flag = true;

	if(!int_test()){flag = false;}
	if(!double_test()){flag = false;}
	if(!complex_test()){flag = false;}
	if(!complex_conjugate_test()){flag = false;}

	return(flag);
}
