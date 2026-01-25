//
//  stftM.c
//  Octave-C
//
//  Created by Zach Taylor on 6/30/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int stft_functions(Matrix *tmp)
{
	bool flag = true;

	double one = 1;

	for (int win_size = 1 ; win_size < 10 ; win_size++)
	{
		for (int inc = 1 ; inc < 10 ; inc++)
		{
			for (int num_coef = 2 ; num_coef < 10 ; num_coef++)
			{
				for (int win_type = 1 ; win_type < 4 ; win_type++)
				{
					printf("win_size: %d, inc: %d, num_coef: %d, win_type: %d\n", win_size, inc, num_coef, win_type);
					Matrix *short_time = stftV(tmp, win_size, inc, num_coef, win_type);

					Matrix *short_time_double = scaleM(short_time, &one, DOUBLE);
					if (!destroyM(short_time))
					{
						flag = false;
						fprintf(stdout, "FAILURE\n");
					}

					printM(short_time_double);
					if (!destroyM(short_time_double))
					{
						flag = false;
						fprintf(stdout, "FAILURE\n");
					}
				}
			}
		}
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stdout, "FAILURE\n");
	}
	
	return(flag);
}

// Stft functions on row vectors
int row_vectors_i()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,10};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {-4,-3,-2,-1,0,1,2,3,4,5};
	writeM(tmp, 10, input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

int row_vectors_d()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,12};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {-4,-3.5,-3,-2.5, -2,-1.5,-1,-0.5, 0,0.5,1,1.5};
	writeM(tmp, 12, input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

int row_vectors_c()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,100};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex *input = NULL;
	input = malloc(100*sizeof(*input));
	for (int i = 0 ; i < 100 ; i++)
	{
		input[i] = 100-i + i*I;
	}
	writeM(tmp, 100, input);
	free(input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

// Stft functions on column vectors
int column_vectors_i()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {10};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {-4,-3,-2,-1,0,1,2,3,4,5};
	writeM(tmp, 10, input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

int column_vectors_d()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {12};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {-4,-3.5,-3,-2.5, -2,-1.5,-1,-0.5, 0,0.5,1,1.5};
	writeM(tmp, 12, input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

int column_vectors_c()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {100};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex *input = NULL;
	input = malloc(100*sizeof(*input));
	for (int i = 0 ; i < 100 ; i++)
	{
		input[i] = 100-i + i*I;
	}
	writeM(tmp, 100, input);
	free(input);
	printM(tmp);

	if(!stft_functions(tmp)){flag = false;}

	return(flag);
}

int main()
{
	bool flag = true;

	if(!row_vectors_i()){flag = false;}
	// if(!row_vectors_d()){flag = false;}
	// if(!row_vectors_c()){flag = false;}

	// if(!column_vectors_i()){flag = false;}
	// if(!column_vectors_d()){flag = false;}
	// if(!column_vectors_c()){flag = false;}

	return(flag);
}