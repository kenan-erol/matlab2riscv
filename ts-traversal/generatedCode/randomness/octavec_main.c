//
//  randomness.c
//  Octave-C
//
//  Created by Zach Taylor on 7/6/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int rand_test()
{
	bool flag = true;
	resetSeed();

	int ndim = 2;
	int dim[2] = {3,4};

	Matrix *m = randM(ndim, dim);
	// printM(m);
	// printf("seed: %u, %u, %u\n", seed[0], seed[1], seed[2]);

	resetSeed();
	
	Matrix *n = randM(ndim, dim);
	// printM(n);
	// printf("seed: %u, %u, %u\n", seed[0], seed[1], seed[2]);

	if (!isEqualM(m, n))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(m))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(n))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int randi_test()
{
	bool flag = true;
	randSeed();

	int ndim = 2;
	int dim[2] = {3,4};

	int ten = 10;

	Matrix *m = randiM(ndim, dim, INTEGER, ten);

	Matrix *n = onesM(ndim, dim);
	Matrix *tens = scaleM(n, &ten, INTEGER);
	Matrix *check = leM(m, tens);

	printM(check);

	if (!isEqualM(check, n))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(m))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(n))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tens))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(check))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int randn_test()
{
	bool flag = true;
	randSeed();

	int ndim = 1;
	int dim[1] = {1000};

	Matrix *m = randnM(ndim, dim);
	Matrix *meanMat = meanM(m);
	Matrix *varMat = varM(m);

	double mean = -1;
	indexM(meanMat, &mean, 1, 1);

	double var = -1;
	indexM(varMat, &var, 1, 1);

	if (mean < -0.4 || mean > 0.4 || var < 0.6 || var > 1.4)
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(m))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(meanMat))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(varMat))
	{
		fprintf(stderr, "failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int main()
{
	bool flag = true;

	if(!rand_test()){flag = false;}
	if(!randi_test()){flag = false;}
	if(!randn_test()){flag = false;}

	return(flag);
}