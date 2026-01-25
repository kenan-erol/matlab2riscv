//
//  advanced_stats.c
//  Octave-C
//
//  Created by Zach Taylor on 7/6/21.
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
	fprintf(stdout, "max index: %d\n\n", index);

	double max_val = NAN;
	switch(gettypeM(tmp))
	{
		case INTEGER: ;
			int max_val_i = INT_MIN;
			indexM(tmp, &max_val_i, 1, index);
			max_val = (double) max_val_i;
			break;

		case DOUBLE: ;
			indexM(tmp, &max_val, 1, index);
			break;
	}

	double max_val_check = NAN;
	switch(gettypeM(max))
	{
		case INTEGER: ;
			int max_val_i = INT_MIN;
			indexM(max, &max_val_i, 1, 1);
			max_val_check = (double) max_val_i;
			break;

		case DOUBLE: ;
			indexM(max, &max_val_check, 1, 1);
			break;
	}

	if (max_val != max_val_check)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	index = -1;
	Matrix *min = minV(tmp, &index);
	printM(min);
	fprintf(stdout, "min index: %d\n\n", index);
	
	double min_val = NAN;
	switch(gettypeM(tmp))
	{
		case INTEGER: ;
			int min_val_i = INT_MIN;
			indexM(tmp, &min_val_i, 1, index);
			min_val = (double) min_val_i;
			break;

		case DOUBLE: ;
			indexM(tmp, &min_val, 1, index);
			break;
	}

	double min_val_check = NAN;
	switch(gettypeM(min))
	{
		case INTEGER: ;
			int min_val_i = INT_MIN;
			indexM(min, &min_val_i, 1, 1);
			min_val_check = (double) min_val_i;
			break;

		case DOUBLE: ;
			indexM(min, &min_val_check, 1, 1);
			break;
	}

	if (min_val != min_val_check)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	Matrix *popstdev = popstdM(tmp);
	double s = NAN;
	indexM(popstdev, &s, 1, 1);

	for (double i = min_val ; i <= max_val ; i+=0.5)
	{
		printf("mu: %.3f\n", i);

		bool h = false;
		double pval = NAN;
		double *ci = NULL;
		ci = malloc(2*sizeof(*ci));
		double tstat = NAN;
		double df = NAN;
		double sd = NAN;
		ttestM(tmp, i, &h, &pval, &ci, &tstat, &df, &sd);

		// printf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", 
		// 	h, pval, ci[0], ci[1], tstat, df, sd);

		free(ci);

		h = false;
		pval = NAN;
		ci = NULL;
		ci = malloc(2*sizeof(*ci));
		double z = NAN;
		double zcrit = NAN;
		ztestM(tmp, i, s, &h, &pval, &ci, &z, &zcrit);

		// printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\nzcrit: %.3f\n", 
		// 	h, pval, ci[0], ci[1], z, zcrit);
		
		free(ci);
	}

	Matrix *popvar = popvarM(tmp);
	double var = NAN;
	indexM(popvar, &var, 1, 1);

	for (double i = var-5 ; i <= var+5 ; i++)
	{
		printf("v: %.3f\n", i);

		bool h = false;
		double pval = NAN;
		double *ci = NULL;
		ci = malloc(2*sizeof(*ci));
		double chisqstat = NAN;
		double df = NAN;
		vartestM(tmp, i, &h, &pval, &ci, &chisqstat, &df);

		// printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", 
		// 	h, pval, ci[0], ci[1], chisqstat, df);

		// printf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", 
		// 	h, pval, chisqstat, df);

		free(ci);
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

	if (!destroyM(popstdev))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(popvar))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	return(flag);
}

int stats_functions(Matrix *tmp)
{
	bool flag = true;

	// Beta PDF
	for (double a = 0 ; a < 1 ; a+=0.05)
	{
		for (double b = 0 ; b < 1 ; b+=0.05)
		{
			Matrix *beta = betapdfM(tmp, a, b);
			printM(beta);

			if (!destroyM(beta))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
				break;
			}
		}
	}

	// Exponential PDF
	for (double lambda = 0.05 ; lambda < 4.99 ; lambda+= 0.05)
	{
		Matrix *exponential = exppdfM(tmp, lambda);
		printM(exponential);

		if (!destroyM(exponential))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
			break;
		}
	}

	// Chi-square PDF
	for (double n = 0.05 ; n < 4.99 ; n+= 0.05)
	{
		Matrix *chi2 = chi2pdfM(tmp, n);
		printf("n = %.3f\n", n);
		printM(chi2);

		if (!destroyM(chi2))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
			break;
		}
	}

	// Gamma PDF
	for (double a = 0.25 ; a < 1.99 ; a+=0.25)
	{
		for (double b = 0.25 ; b < 1.99 ; b+=0.25)
		{
			Matrix *gamma = gampdfM(tmp, a, b);
			printM(gamma);

			if (!destroyM(gamma))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
				break;
			}
		}
	}

	// Lognormal PDF
	for (double mu = -2 ; mu < 5.1 ; mu+=0.5)
	{
		for (double sigma = 0.5 ; sigma < 5.1 ; sigma+=0.5)
		{
			Matrix *lognormal = lognpdfM(tmp, mu, sigma);
			printM(lognormal);

			if (!destroyM(lognormal))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
				break;
			}
		}
	}

	// Normal PDF
	for (double mu = -2 ; mu < 5.1 ; mu+=0.5)
	{
		for (double sigma = 0.5 ; sigma < 5.1 ; sigma+=0.5)
		{
			Matrix *normal = normpdfM(tmp, mu, sigma);
			printM(normal);

			if (!destroyM(normal))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
				break;
			}
		}
	}
	
	// Uniform discrete PDF
	for (int n = 1 ; n < 10 ; n++)
	{
		printf("n = %d\n",n);
		Matrix *unid = unidpdfM(tmp, n);
		printM(unid);

		if (!destroyM(unid))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
			break;
		}
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

int two_t_test(Matrix *tmp, Matrix *tmp2)
{
	bool flag = true;

	bool h = false;
	double pval = NAN;
	double *ci = NULL;
	ci = malloc(2*sizeof(*ci));
	double tstat = NAN;
	double df = NAN;
	double sd = NAN;
	ttestM_xy(tmp, tmp2, &h, &pval, &ci, &tstat, &df, &sd);

	printf("h: %d\npval: %.2f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", 
		h, pval, tstat, df, sd);
	
	free(ci);

	if (!destroyM(tmp2))
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

	Matrix *tmp2 = createM(ndim, dim, INTEGER);
	int input2[] = {1,1,1,1};
	writeM(tmp2, 4, input2);

	if (!two_t_test(tmp, tmp2)){flag = false;}
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
	double input[] = {0.5,0.25,0,0.6};
	writeM(tmp, 4, input);
	printM(tmp);

	Matrix *tmp2 = createM(ndim, dim, INTEGER);
	int input2[] = {1,1,1,1};
	writeM(tmp2, 4, input2);

	if (!two_t_test(tmp, tmp2)){flag = false;}
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
	double input[] = {0.25,0.5,0,0.6};
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

int big_matrix()
{

	int ndim = 2;
	int dim[2] = {32,32};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = onesM(ndim, dim);

	printM(tmp);

	return(stats_functions(tmp));
}

int big_vector()
{
	bool flag = true;

	int ndim = 1;
	int dim[1] = {1010};

	Matrix *tmp = onesM(ndim, dim);

	printM(tmp);

	if (!vec_stats_functions(tmp)){flag = false;}
	if (!stats_functions(tmp)){flag = false;}
	return(flag);
}

int main()
{
	bool flag = true;

	if(!row_vectors_i()){flag = false;}
	if(!row_vectors_d()){flag = false;}

	if(!column_vectors_i()){flag = false;}
	if(!column_vectors_d()){flag = false;}

	if(!matrices_23_i()){flag = false;}
	if(!matrices_23_d()){flag = false;}

	if(!matrices_32_i()){flag = false;}
	if(!matrices_32_d()){flag = false;}

	if(!matrices_97_i()){flag = false;}
	if(!matrices_97_d()){flag = false;}

	if(!big_matrix()){flag = false;}
	if(!big_vector()){flag = false;}



	return(flag);
}