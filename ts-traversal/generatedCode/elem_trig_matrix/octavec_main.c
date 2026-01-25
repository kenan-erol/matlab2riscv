//
//  elem_trig_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 6/18/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Trig functions of an integer matrix
int int_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	// Create an integer matrix
	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (int)(pow(-1, i))*(i);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	// Run all the trig functions on the matrix
	Matrix **trig_outputs = NULL;
	trig_outputs = malloc(6*sizeof(*trig_outputs));

	trig_outputs[0] = sinM(tmp);
	trig_outputs[1] = sindM(tmp);
	trig_outputs[2] = cosM(tmp);
	trig_outputs[3] = cosdM(tmp);
	trig_outputs[4] = tanM(tmp);
	trig_outputs[5] = tandM(tmp);

	// Print out all the trig results
	for (int i = 0 ; i < 6 ; i++)
	{
		printM(trig_outputs[i]);
	}


	// Clean up
	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	for (int i = 0 ; i < 6 ; i++)
	{
		if (!destroyM(trig_outputs[i]))
		{
			flag = false;
			fprintf(stderr, "Failed to destroy matrix\n");
		}
	}
	free(trig_outputs);

	return(flag);
}

// Trig functions of a double matrix
int double_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (pow(-1, i))*(i+1.4);
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	// Run all the trig functions on the matrix
	Matrix **trig_outputs = NULL;
	trig_outputs = malloc(6*sizeof(*trig_outputs));

	trig_outputs[0] = sinM(tmp);
	trig_outputs[1] = sindM(tmp);
	trig_outputs[2] = cosM(tmp);
	trig_outputs[3] = cosdM(tmp);
	trig_outputs[4] = tanM(tmp);
	trig_outputs[5] = tandM(tmp);

	// Print out all the trig results
	for (int i = 0 ; i < 6 ; i++)
	{
		printM(trig_outputs[i]);
	}


	// Clean up
	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	for (int i = 0 ; i < 6 ; i++)
	{
		if (!destroyM(trig_outputs[i]))
		{
			flag = false;
			fprintf(stderr, "Failed to destroy matrix\n");
		}
	}
	free(trig_outputs);

	return(flag);
}

// Trig functions of a complex matrix
int complex_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.5*I;
	}
	
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	// Run all the trig functions on the matrix
	Matrix **trig_outputs = NULL;
	trig_outputs = malloc(6*sizeof(*trig_outputs));

	trig_outputs[0] = sinM(tmp);
	trig_outputs[1] = sindM(tmp);
	trig_outputs[2] = cosM(tmp);
	trig_outputs[3] = cosdM(tmp);
	trig_outputs[4] = tanM(tmp);
	trig_outputs[5] = tandM(tmp);

	// Print out all the trig results
	for (int i = 0 ; i < 6 ; i++)
	{
		printM(trig_outputs[i]);
	}


	// Clean up
	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	for (int i = 0 ; i < 6 ; i++)
	{
		if (!destroyM(trig_outputs[i]))
		{
			flag = false;
			fprintf(stderr, "Failed to destroy matrix\n");
		}
	}
	free(trig_outputs);

	return(flag);
}

int main()
{
	bool flag = true;
	
	if(!int_test()){flag = false;}
	if(!double_test()){flag = false;}
	if(!complex_test()){flag = false;}

	return(flag);
}