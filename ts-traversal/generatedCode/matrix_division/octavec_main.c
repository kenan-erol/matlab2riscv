//
//  matrix_division.c
//  Octave-C
//
//  Created by Zach Taylor on 5/4/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Left division
int ldivide_test1() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};


	// Create a matrix
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	int size = 4;
	int *input = NULL;
	input = malloc(4*sizeof(*input));
	for (int i = 0 ; i < 4 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}



	// Create a second matrix
	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);


	// Matrix left division
	Matrix *product = mldivideM(tmp, tmp2);
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

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Left division
int ldivide_test2() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *tmp = idM(2);
	printM(tmp);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, 4, input2);
	free(input2);
	printM(tmp2);

	Matrix *product = mldivideM(tmp, tmp2);
	printM(product);

	if (!isEqualM(product, tmp2))
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

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Left division
int ldivide_test3() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *id2 = idM(2);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, 4, input2);
	free(input2);

	Matrix *tmp = duplicateM(tmp2);

	printM(tmp);
	printM(tmp2);

	Matrix *product = mldivideM(tmp, tmp2);
	printM(product);

	if (!isEqualM(product, id2))
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

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(id2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Right division
int rdivide_test1() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	int size = 4;
	int *input = NULL;
	input = malloc(4*sizeof(*input));
	for (int i = 0 ; i < 4 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}

	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);

	Matrix *product = mrdivideM(tmp, tmp2);
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

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Right division
int rdivide_test2() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *tmp = idM(2);
	printM(tmp);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, 4, input2);
	free(input2);
	printM(tmp2);

	Matrix *product = mrdivideM(tmp, tmp2);
	printM(product);

	Matrix *tmp3 = invM(tmp2);

	if (!isEqualM(product, tmp3))
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

	if (!destroyM(product))
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

// Right division
int rdivide_test3() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *id2 = idM(2);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, 4, input2);
	free(input2);

	Matrix *tmp = duplicateM(tmp2);

	printM(tmp);
	printM(tmp2);

	Matrix *product = mrdivideM(tmp, tmp2);
	printM(product);

	if (!isEqualM(product, id2))
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

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(id2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Checks that it can't divide by zero
int divide_by_zero() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *zeros = zerosM(ndim, dim);
	printM(zeros);

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input2 = NULL;
	input2 = malloc(4*sizeof(*input2));
	for (int i = 0 ; i < 4 ; i++)
	{
		input2[i] = 0;
		if (i == 0 || i == 3)
		{
			input2[i] = 2.1+0.5*I;
		}
	}

	writeM(tmp2, 4, input2);
	free(input2);
	printM(tmp2);

	Matrix *product = mrdivideM(tmp2, zeros);
	printM(product);

	if (product != NULL)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(product))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(zeros))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

int main()
{
	bool flag = true;

	if(!ldivide_test1()){flag = false;}
	if(!ldivide_test2()){flag = false;}
	if(!ldivide_test3()){flag = false;}
	if(!rdivide_test1()){flag = false;}
	if(!rdivide_test2()){flag = false;}
	if(!rdivide_test3()){flag = false;}
	if(!divide_by_zero()){flag = false;}

	return(flag);
}