//
//  fourier.c
//  Octave-C
//
//  Created by Zach Taylor on 6/24/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int fourier_functions(Matrix *tmp)
{
	bool flag = true;

	Matrix *fourier = fftM(tmp);
	Matrix *fourier_inv = ifftM(tmp);

	printM(fourier);
	printM(fourier_inv);

	Matrix *sanity_check_1 = fftM(fourier_inv);
	Matrix *sanity_check_2 = ifftM(fourier);

	if (!isEqualM(tmp, sanity_check_1))
	{
		fprintf(stdout, "Failure: sanity check 1\n");
		flag = false;
	}

	if (!isEqualM(tmp, sanity_check_2))
	{
		fprintf(stdout, "Failure: sanity check 2\n");
		flag = false;
	}

	if (!destroyM(fourier))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(fourier_inv))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(sanity_check_1))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(sanity_check_2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int fourier_functions_vec(Matrix *tmp)
{
	bool flag = true;

	for (int i = 1 ; i < 21 ; i++)
	{
		Matrix *fourier = fftV(tmp, i);
		Matrix *fourier_inv = ifftV(tmp, i);

		printM(fourier);
		printM(fourier_inv);

		if (!destroyM(fourier))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		if (!destroyM(fourier_inv))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
	}
	
	return(flag);
}

// Fourier functions on row vectors
int row_vectors_i()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);
}

int row_vectors_d()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,4};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2,0,10.1};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);
}

int row_vectors_c()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,4};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2,0,1-I};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);
}

// Fourier functions on column vectors
int column_vectors_i()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);}

int column_vectors_d()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2,0,10.1};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);
}

int column_vectors_c()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2,0,1-I};
	writeM(tmp, 4, input);
	printM(tmp);

	if(!fourier_functions_vec(tmp)){flag = false;}
	if(!fourier_functions(tmp)){flag = false;}

	return(flag);
}

// Fourier functions on matrices
int matrices_23_i()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_23_d()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_23_c()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_32_i()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_32_d()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_32_c()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_97_i()
{

	int ndim = 2;
	int dim[2] = {9,7};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = createM(ndim, dim, INTEGER);


	int *input = NULL;
	input = malloc(size*sizeof(*input));

	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (int)pow(-1, i+1)*(i+1)*(i+1);
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_97_d()
{

	int ndim = 2;
	int dim[2] = {9,7};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = createM(ndim, dim, DOUBLE);


	double *input = NULL;
	input = malloc(size*sizeof(*input));

	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (int)pow(-1, i+1)*(i+1)*(i+1)/17.0;
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int matrices_97_c()
{

	int ndim = 2;
	int dim[2] = {9,7};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = createM(ndim, dim, COMPLEX);


	double complex *input = NULL;
	input = malloc(size*sizeof(*input));

	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (int)pow(-1, i+1)*(i+1)-(i+1)/17.0*I;
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	return(fourier_functions(tmp));
}

int main()
{
	bool flag = true;

	if(!row_vectors_i()){flag = false;}
	if(!row_vectors_d()){flag = false;}
	if(!row_vectors_c()){flag = false;}

	if(!column_vectors_i()){flag = false;}
	if(!column_vectors_d()){flag = false;}
	if(!column_vectors_c()){flag = false;}

	if(!matrices_23_i()){flag = false;}
	if(!matrices_23_d()){flag = false;}
	if(!matrices_23_c()){flag = false;}

	if(!matrices_32_i()){flag = false;}
	if(!matrices_32_d()){flag = false;}
	if(!matrices_32_c()){flag = false;}

	if(!matrices_97_i()){flag = false;}
	if(!matrices_97_d()){flag = false;}
	if(!matrices_97_c()){flag = false;}

	return(flag);
}