//
//  reindex.c
//  Octave-C
//
//  Created by Zach Taylor on 7/13/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int reindexing_tests(Matrix *tmp)
{
	bool flag = true;

	Matrix *reindexing0 = reindexM(tmp, 1, 1);
	Matrix *reindexing1 = reindexM(tmp, 1, 4);
	Matrix *reindexing2 = reindexM(tmp, 4, 1,2,3,4);
	Matrix *reindexing3 = reindexM(tmp, 4, 4,3,2,1);
	Matrix *reindexing4 = reindexM(tmp, 3, 2,2,2);
	Matrix *reindexing5 = reindexM(tmp, 10, 1,2,2,3,3,3,4,4,4,4);
		
	printM(reindexing0);
	printM(reindexing1);
	printM(reindexing2);
	printM(reindexing3);
	printM(reindexing4);
	printM(reindexing5);

	if (!destroyM(reindexing0))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(reindexing1))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(reindexing2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(reindexing3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(reindexing4))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(reindexing5))
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

// Reindexing on row vectors
int row_vectors_i()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if (!reindexing_tests(tmp)){flag = false;}
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

	if (!reindexing_tests(tmp)){flag = false;}
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

	if (!reindexing_tests(tmp)){flag = false;}
	return(flag);
}

// Reindexing on column vectors
int column_vectors_i()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if (!reindexing_tests(tmp)){flag = false;}
	return(flag);
}

int column_vectors_d()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2,0,10.1};
	writeM(tmp, 4, input);
	printM(tmp);

	if (!reindexing_tests(tmp)){flag = false;}
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

	if (!reindexing_tests(tmp)){flag = false;}
	return(flag);
}

// Reindexing on matrices
int matrices_23_i()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
}

int matrices_23_d()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
}

int matrices_23_c()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
}

int matrices_32_i()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
}

int matrices_32_d()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
}

int matrices_32_c()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(reindexing_tests(tmp));
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

	return(reindexing_tests(tmp));
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

	return(reindexing_tests(tmp));
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

	return(reindexing_tests(tmp));
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