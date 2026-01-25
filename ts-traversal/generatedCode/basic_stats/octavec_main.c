//
//  basic_stats.c
//  Octave-C
//
//  Created by Zach Taylor on 6/21/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int vec_stats_functions(Matrix *tmp)
{
	bool flag = true;

	int index = -1;
	Matrix *max = maxV(tmp, &index);
	printM(max);
	fprintf(stdout, "max index: %d\n", index);

	Matrix *min = minV(tmp, &index);
	printM(min);
	fprintf(stdout, "min index: %d\n", index);
	
	if (!destroyM(min))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(max))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}


	if (gettypeM(tmp) == INTEGER)
	{
		double mu = NAN;
		double sd = NAN;
		normfitM(tmp, &mu, &sd);
		printf("mean: %.3f\n", mu);
		printf("sd: %.3f\n", sd);

		int a = INT_MIN;
		int b = INT_MIN;
		unifitM(tmp, &a, &b);
		printf("a: %d\n", a);
		printf("b: %d\n", b);
	}
	else if (gettypeM(tmp) == DOUBLE)
	{
		double mu = NAN;
		double sd = NAN;
		normfitM(tmp, &mu, &sd);
		printf("mean: %.3f\n", mu);
		printf("sd: %.3f\n", sd);

		double a = NAN;
		double b = NAN;
		unifitM(tmp, &a, &b);
		printf("a: %.3f\n", a);
		printf("b: %.3f\n", b);
	}
	else if (gettypeM(tmp) == COMPLEX)
	{
		double complex mu = NAN + I*NAN;
		double complex sd = NAN + I*NAN;
		normfitM(tmp, &mu, &sd);
		printf("mean: %.3f + %.3fi\n", creal(mu), cimag(mu));
		printf("sd: %.3f + %.3fi\n", creal(sd), cimag(sd));

		double complex a = NAN + I*NAN;
		double complex b = NAN + I*NAN;
		unifitM(tmp, &a, &b);
		printf("a: %.3f + %.3fi\n", creal(a), cimag(a));
		printf("b: %.3f + %.3fi\n", creal(b), cimag(b));
	}
	
	return(flag);
}

int stats_functions(Matrix *tmp)
{
	bool flag = true;

	double fun_qs[] = {0, -1, 3, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17};

	Matrix *mean = meanM(tmp);
	Matrix *variance = varM(tmp);
	Matrix *pop_variance = popvarM(tmp);
	Matrix *stdev = stdM(tmp);
	Matrix *pop_stdev = popstdM(tmp);
	Matrix *sorted_a = sortM(tmp, ASCENDING);
	Matrix *sorted_d = sortM(tmp, DESCENDING);
	Matrix *median = medianM(tmp);
	Matrix *min = minM(tmp);
	Matrix *max = maxM(tmp);
	Matrix *quantiles = quantileM(tmp);
	Matrix *fun_quantiles = quantileM_vec(tmp, 10, fun_qs);

	printM(mean);
	printM(variance);
	printM(pop_variance);
	printM(stdev);
	printM(pop_stdev);
	printM(sorted_a);
	printM(sorted_d);
	printM(median);
	printM(min);
	printM(max);
	printM(quantiles);
	printM(fun_quantiles);

	
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(mean))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(variance))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(pop_variance))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(stdev))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(pop_stdev))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(sorted_a))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(sorted_d))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(median))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(min))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(max))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(quantiles))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(fun_quantiles))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Stats functions on row vectors
int row_vectors_i()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {1,4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
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

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
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

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
	return(flag);
}

// Stats functions on column vectors
int column_vectors_i()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {4};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-5,0,1};
	writeM(tmp, 4, input);
	printM(tmp);

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
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

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
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

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
	return(flag);
}

// Stats functions on matrices
int matrices_23_i()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
}

int matrices_23_d()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
}

int matrices_23_c()
{

	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
}

int matrices_32_i()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, INTEGER);
	int input[] = {3,-2,0,1,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
}

int matrices_32_d()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, DOUBLE);
	double input[] = {3.25,-2.0,0.0,1.0,5,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
}

int matrices_32_c()
{

	int ndim = 2;
	int dim[2] = {3,2};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	double complex input[] = {3.25,-2.0,0.0,1.0,5-I,10};
	writeM(tmp, 6, input);
	printM(tmp);

	return(stats_functions(tmp));
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

	return(stats_functions(tmp));
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

	return(stats_functions(tmp));
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

	return(stats_functions(tmp));
}

int basic_quantile_test()
{
	bool flag = true;


	// Integer
	int *vec_i = NULL;
	vec_i = malloc(100*sizeof(*vec_i));

	for (int i = 0 ; i < 100 ; i++)
	{
		vec_i[i] = i+1;
	}

	for (double i = 0 ; i < 1.01 ; i += 0.01)
	{
		double quantile = int_quantile(vec_i, 100, i);
		printf("%.4f\n", quantile);
	}
	free(vec_i);

	// Double
	double *vec_d = NULL;
	vec_d = malloc(1004*sizeof(*vec_d));

	for (int i = 0 ; i < 1004 ; i++)
	{
		vec_d[i] = (i+1)*(i+1)/17.0;
	}

	for (double i = 0 ; i < 1.01 ; i += 0.01)
	{
		double quantile = double_quantile(vec_d, 1004, i);
		printf("%.4f\n", quantile);
	}
	free(vec_d);

	// Complex
	double complex *vec_c = NULL;
	vec_c = malloc(57*sizeof(*vec_c));

	for (int i = 0 ; i < 57 ; i++)
	{
		vec_c[i] = (i+1)-(i+1)/17.0*I;
	}

	for (double i = 0 ; i < 1.01 ; i += 0.01)
	{
		double complex quantile = complex_quantile(vec_c, 57, i);
		printf("%.4f ", creal(quantile));
		if (cimag(quantile) >= 0)
		{
			printf("+%.4fi\n",cimag(quantile));
		}
		else
		{
			printf("%.4fi\n",cimag(quantile));
		}
	}
	free(vec_c);

	return(flag);
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

	if(!basic_quantile_test()){flag = false;}

	return(flag);
}