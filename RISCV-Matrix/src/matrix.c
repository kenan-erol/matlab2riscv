//
//  matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 2/26/21.
//

#define _XOPEN_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include "tgmath.h"
#include <limits.h>
#include <string.h>
#include "../riscv-blas/INCLUDE/cblas.h"
#include "../riscv-blas/INCLUDE/lapacke.h"
#include "../fftw-3.3.10/api/fftw3.h"

#include "../BSC-RVV/common/vector_defines.h"
#include "../BSC-RVV/common/trace_macros.h"

#include "matrix.h"

short unsigned int seed[] = {1.0, 1.0, 1.0};

/*
* =========================================
* 
*  Functions to create & destroy matrices
* 
* =========================================
*/
Matrix * createM(int ndim, int dim[ndim], int type)
// Initialize a matrix
{
	// Error checking: type must be 0, 1, or 2
	if (type < 0 || type > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// Error checking: ndim must be 1,2,3,or 4D.
	if (ndim < 1 || ndim > 4)
	{
		fprintf(stderr, "%s: %s (%d): Invalid ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	
	// Error checking: dimensions must be valid sizes
	int nExpectedValues = 1;
	for (register int i = 0 ; i < ndim ; i++)
	{
		nExpectedValues *= dim[i];
		if (dim[i] < 1) {
			fprintf(stderr, "%s: %s (%d): Invalid dim\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	
	// Make some room
	Matrix *m = NULL;
	m = malloc(sizeof(*m));
	Vector *datavec = NULL;
	datavec = malloc(sizeof(*datavec));
	
	if (datavec && m)
	{
		datavec->readOnly = 0;
		datavec->datatype = type;
		datavec->size = nExpectedValues;

		switch(type)
		{
			case INTEGER:
				datavec->data = malloc(nExpectedValues*sizeof(int));
				break;

			case DOUBLE:
				datavec->data = malloc(nExpectedValues*sizeof(double));
				break;

			case COMPLEX:
				datavec->data = malloc(nExpectedValues*sizeof(double complex));
				break;
		}
			
		
		m->data = datavec;
		m->ndim = ndim;

		int *m_dim = m->dim;
		for (register int i = 0 ; i < 4 ; i++)
		{
			if (i >= ndim)
			{
				m_dim[i] = 1;
			}
			else
			{
				m_dim[i] = dim[i];
			}
		}
		return(m);
	}
	return(NULL);
}

Matrix * duplicateM(Matrix* restrict m)
// Duplicates a matrix
// Does not duplicate the matrix's internal data, but instead acts as a second user
{
	// Make some room
	Matrix *output = NULL;
	output = malloc(sizeof(*output));
	
	if (output)
	{
		// Copy over the information
		output->data = m->data;
		output->ndim = m->ndim;

		int *output_dim = output->dim;
		int *m_dim = m->dim;

		for (register int i = 0 ; i < 4 ; i++)
		{
			(output_dim)[i] = (m_dim)[i];
		}

		// Since now there are multiple matrices accessing the data
		(output->data->readOnly)++;
		
		return(output);
	}

	fprintf(stderr, "%s: %s (%d): Failed to create duplicate matrix\n", __FILE__, __func__, __LINE__);

	return(NULL);
}

Matrix * full_duplicateM_vec(Matrix* restrict m)
// Duplicates a matrix
// Does duplicate the matrix's internal data
{
	// Make some room
	Matrix *output = createM(m->ndim, m->dim, m->data->datatype);
	if(output == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create duplicate matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	if( !writeM_vec(output, m->data->size, m->data->data))
	{
		fprintf(stderr, "%s: %s (%d): Failed to write to duplicated matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * full_duplicateM_ref(Matrix* restrict m)
// Duplicates a matrix
// Does duplicate the matrix's internal data
{
	// Make some room
	Matrix *output = createM(m->ndim, m->dim, m->data->datatype);
	if(output == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create duplicate matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	if( !writeM_ref(output, m->data->size, m->data->data))
	{
		fprintf(stderr, "%s: %s (%d): Failed to write to duplicated matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * full_duplicateM(Matrix* restrict m)
// Duplicates a matrix
// Does duplicate the matrix's internal data
{
	if (MODE == VEC) {return full_duplicateM_vec(m);}
    else if (MODE == REF) {return full_duplicateM_ref(m);}

	// Default mode
    return full_duplicateM_ref(m);
}

Matrix * identityM(int size)
// Initialize a square identity matrix
{
	if (size < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid size\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int dim[2] = {size,size};

	Matrix *result = zerosM(2, dim);
	if (result == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int *result_data_data = result->data->data;

	for (register int i = 0 ; i < size ; i++)
	{
		((int *)(result_data_data))[i*size + i] = 1;
	}

	return(result);
}

Matrix * zerosM(int ndim, int dim[ndim])
// Initialize a matrix of zeroes
{
	// Error checking: ndim must be 1,2,3,or 4D.
	if (ndim < 1 || ndim > 4)
	{
		fprintf(stderr, "%s: %s (%d): Invalid ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	
	// Error checking: dimensions must be valid sizes
	int nExpectedValues = 1;
	for (register int i = 0 ; i < ndim ; i++)
	{
		nExpectedValues *= dim[i];
		if (dim[i] < 1) {
			fprintf(stderr, "%s: %s (%d): Invalid dim\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}

	// Make some room
	Matrix *m = NULL;
	m = malloc(sizeof(*m));
	Vector *datavec = NULL;
	datavec = malloc(sizeof(*datavec));
	
	if (datavec && m)
	{
		datavec->readOnly = 0;
		datavec->datatype = INTEGER;
		datavec->size = nExpectedValues;

		datavec->data = calloc(nExpectedValues,sizeof(int));

		m->data = datavec;
		m->ndim = ndim;

		int *m_dim = m->dim;
		for (register int i = 0 ; i < 4 ; i++)
		{
			if (i >= ndim)
			{
				m_dim[i] = 1;
			}
			else
			{
				m_dim[i] = dim[i];
			}
		}
		return(m);
	}
	return(NULL);
}

void onesM_vec_i(int* restrict y, int n)
{
    // outputs are ints (SEW e32)
    // two input args (LMUL = 8)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of ones
        vint32m8_t v_res  = vmv_v_x_i32m8 (1, trueVL);

        // Store
		vse32_v_i32m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * onesM_vec(int ndim, int dim[ndim])
{
	// Error checking: ndim must be 1,2,3,or 4D.
	if (ndim < 1 || ndim > 4)
	{
		fprintf(stderr, "%s: %s (%d): Invalid ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	
	// Error checking: dimensions must be valid sizes
	int nExpectedValues = 1;
	for (register int i = 0 ; i < ndim ; i++)
	{
		nExpectedValues *= dim[i];
		if (dim[i] < 1) {
			fprintf(stderr, "%s: %s (%d): Invalid dim\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	
	// Make some room
	Matrix *m = NULL;
	m = malloc(sizeof(*m));
	Vector *datavec = NULL;
	datavec = malloc(sizeof(*datavec));
	
	if (datavec && m)
	{
		datavec->readOnly = 0;
		datavec->datatype = INTEGER;
		datavec->size = nExpectedValues;

		datavec->data = malloc(nExpectedValues*sizeof(int));

		int *datavec_data = datavec->data;

		onesM_vec_i(datavec_data, nExpectedValues);

		m->data = datavec;
		m->ndim = ndim;

		int *m_dim = m->dim;
		for (register int i = 0 ; i < 4 ; i++)
		{
			if (i >= ndim)
			{
				m_dim[i] = 1;
			}
			else
			{
				m_dim[i] = dim[i];
			}
		}
		return(m);
	}
	return(NULL);
}

Matrix * onesM_ref(int ndim, int dim[ndim])
{
	// Error checking: ndim must be 1,2,3,or 4D.
	if (ndim < 1 || ndim > 4)
	{
		fprintf(stderr, "%s: %s (%d): Invalid ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	
	// Error checking: dimensions must be valid sizes
	int nExpectedValues = 1;
	for (register int i = 0 ; i < ndim ; i++)
	{
		nExpectedValues *= dim[i];
		if (dim[i] < 1) {
			fprintf(stderr, "%s: %s (%d): Invalid dim\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	
	// Make some room
	Matrix *m = NULL;
	m = malloc(sizeof(*m));
	Vector *datavec = NULL;
	datavec = malloc(sizeof(*datavec));
	
	if (datavec && m)
	{
		datavec->readOnly = 0;
		datavec->datatype = INTEGER;
		datavec->size = nExpectedValues;

		datavec->data = malloc(nExpectedValues*sizeof(int));

		int *datavec_data = datavec->data;

		for (register int i = 0 ; i < nExpectedValues ; i++)
		{
			datavec_data[i] = 1;
		}

		m->data = datavec;
		m->ndim = ndim;

		int *m_dim = m->dim;
		for (register int i = 0 ; i < 4 ; i++)
		{
			if (i >= ndim)
			{
				m_dim[i] = 1;
			}
			else
			{
				m_dim[i] = dim[i];
			}
		}
		return(m);
	}
	return(NULL);
}

Matrix * onesM(int ndim, int dim[ndim])
// Initialize a matrix of ones
{
    if (MODE == REF){return(onesM_ref(ndim, dim));}
    else if (MODE == VEC){return(onesM_vec(ndim, dim));}

    // Default case
	return(onesM_ref(ndim, dim));
}

void randomSeed()
// Randomizes the seed
{
	for (register int i = 0 ; i < 3 ; i++)
	{
		seed[i] = rand();
	}
}

void resetSeed()
// Resets the seed to default
{
	for (register int i = 0 ; i < 3 ; i++)
	{
		seed[i] = 1;
	}
}

Matrix * randM(int ndim, int dim[ndim])
// Creates a double matrix of random elements uniformly distributed on the interval [0, 1)
{
	Matrix *output = createM(ndim, dim, DOUBLE);

	if (!output)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int output_data_size = output->data->size;
	double *output_data_data = output->data->data;

	// Assign random values
	for (register int i = 0 ; i < output_data_size ; i++)
	{
		// Generate random value and update seed
		output_data_data[i] = erand48(seed);
	}
	return(output);
}

Matrix * randiM(int ndim, int dim[ndim], int type, int imax)
// Creates a matrix of random elements uniformly distributed on the interval [0, max(2^31, imax))
{
	if (imax < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid imax\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = createM(ndim, dim, type);
	if (!output)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int output_data_size = output->data->size;
	void *output_data_data = output->data->data;

	// Assign random values
	// Generate random value and update seed
	switch(type)
	{
		case INTEGER:
			for (register int i = 0 ; i < output_data_size ; i++)
			{
				((int *)(output_data_data))[i] = (int) nrand48(seed) % imax;
			}
			break;

		case DOUBLE:
			for (register int i = 0 ; i < output_data_size ; i++)
			{
				((double *)(output_data_data))[i] = nrand48(seed) % imax;
			}
			break;

		case COMPLEX:
			for (register int i = 0 ; i < output_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = ((double complex)(nrand48(seed) % imax)) + ((double complex)(nrand48(seed) % imax))*I;
			}
			break;
	}

	return(output);
}

Matrix * randnM(int ndim, int dim[ndim])
// Creates a matrix of random elements uniformly distributed with zero mean and one variance
// Uses Box-Muller to transform from uniform to normal ditribution
{
	Matrix *output = createM(ndim, dim, DOUBLE);
	if (!output)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int output_data_size = output->data->size;
	double *output_data_data = ((double *)(output->data->data));

	// Assign random values
	for (register int i = 0 ; i < output_data_size ; i++)
	{
		// Generate two uniformly-distributed random values and update seed
		double sample1 = (double) erand48(seed);
		double sample2 = (double) erand48(seed);

		double mag = sqrt(-2 * log(sample1));
		output_data_data[i] = mag * cos(2*M_PI*sample2);

		if (i+1 < output_data_size)
		{
			output_data_data[i+1] = mag * sin(2*M_PI*sample2);
		}
	}
	return(output);
}

Matrix * reindexM(Matrix* restrict m, int size, ...)
// Creates a matrix out of pieces of another matrix
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (size < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid size\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int output_dims[] = {size};
	Matrix *output = createM(1, output_dims, m->data->datatype);

	if (output == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	va_list indices;

	va_start(indices, size);

	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < size ; i++)
			{
				indexM(m, &(((int *)(output_data_data))[i]), 1, va_arg(indices, int));
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < size ; i++)
			{
				indexM(m, &(((double *)(output_data_data))[i]), 1, va_arg(indices, int));
			}
			break;

		case COMPLEX: ;
			for (register int i = 0 ; i < size ; i++)
			{
				indexM(m, &(((double complex *)(output_data_data))[i]), 1, va_arg(indices, int));
			}
			break;
	}

	va_end(indices);

	return(output);
}

bool destroyM(Matrix* restrict m)
// Destroy a matrix & free the space.
{
	if (m == NULL)
	{
		return(true);
	}

	if(m->data->readOnly == 0)
	{
		free(m->data->data);
		free(m->data);
	}
	else
	{
		(m->data->readOnly)--;
	}

	free(m);
	m = NULL;
	if(m)
	{
		return(false);
	}
	return(true);
}












/*
* =========================================
* 
*  Functions to read & write to matrices
* 
* =========================================
*/

void writeM_vec_i(int* restrict y, int* restrict x, int n)
// Assign the values of y to the values of x
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vse32_v_i32m8(&y[i], v_x, trueVL); // Store the ints in the output

		i += trueVL;
	}

	FENCE();
}

void writeM_vec_d(double* restrict y, double* restrict x, int n)
// Assign the values of y to the values of x
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vse64_v_f64m8(&y[i], v_x, trueVL); // Store the doubles in the output

		i += trueVL;
	}

	FENCE();
}

void writeM_vec_c(double complex* restrict y, double complex* restrict x, int n)
// Assign the values of y to the values of x
{
	writeM_vec_d(((double *)(y)), ((double *)(x)), 2*n);
}

bool writeM_vec(Matrix* restrict m, int size, void* restrict input)
// Assign data to a matrix
{
	if(m->data->readOnly > 0)
	{
		fprintf(stderr, "%s: %s (%d): Matrix is readOnly\n", __FILE__, __func__, __LINE__);
		return(false);
	}

	if(size != m->data->size)
	{
		fprintf(stderr, "%s: %s (%d): size arg (%d) does not match matrix size (%d)\n", __FILE__, __func__, __LINE__, size, m->data->size);
		return(false);
	}

	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:
			writeM_vec_i(((int *)(m_data_data)), ((int *)(input)), size);
			break;

		case DOUBLE:
			writeM_vec_d(((double *)(m_data_data)), ((double *)(input)), size);
			break;

		case COMPLEX:
			writeM_vec_c(((double complex *)(m_data_data)), ((double complex *)(input)), size);
			break;
	}		

	return(true);
}

bool writeM_ref(Matrix* restrict m, int size, void* restrict input)
// Assign data to a matrix
{
	if(m->data->readOnly > 0)
	{
		fprintf(stderr, "%s: %s (%d): Matrix is readOnly\n", __FILE__, __func__, __LINE__);
		return(false);
	}

	if(size != m->data->size)
	{
		fprintf(stderr, "%s: %s (%d): size arg (%d) does not match matrix size (%d)\n", __FILE__, __func__, __LINE__, size, m->data->size);
		return(false);
	}

	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:
			for (register int i = 0 ; i < size ; i++)
			{
				((int *)(m_data_data))[i] = ((int *)(input))[i];
			}
			break;

		case DOUBLE:
			for (register int i = 0 ; i < size ; i++) 
			{
				((double *)(m_data_data))[i] = ((double *)(input))[i];
			}
			break;

		case COMPLEX:
			for (register int i = 0 ; i < size ; i++) 
			{
				((double complex *)(m_data_data))[i] = ((double complex *)(input))[i];
			}
			break;
	}		

	return(true);
}

bool writeM(Matrix* restrict m, int size, void* restrict input)
// Assign data to a matrix
{
    if (MODE == VEC) {return writeM_vec(m, size, input);}
    else if (MODE == REF) {return writeM_ref(m, size, input);}

	// Default mode
    return writeM_ref(m, size, input);
}

void indexM(Matrix* restrict m, void* restrict output, int ndim, ...)
// Takes a variable number of inputs for different dimensionalities.

// Return the value of a matrix at a certain index
// Note: indexM is *** One-indexed *** , to match Octave
// Example: 
// 		double foo;
// 		indexM(m, &foo, m->ndim, row, column[, index3[, index4]])
// 		// foo is now equal to the value of the specified index.

// Inputting ndim=1 and only one dimension will be interpreted as a flat index (i.e., the 1D index)
// This is especially useful when iterating through a whole matrix
// Example: 
// 		double foo;
// 		indexM(m, &foo, 1, 1); 
// 		// foo is now equal to the value of row 1, column 1(, dimension-three 1(, dimension-four 1)) of matrix m.
{
	if (m == NULL)
	{
		return;
	}

	if (ndim != m->ndim && ndim != 1)
	{
		fprintf(stderr, "%s: %s (%d): %dD matrix cannot be %dD-indexed\n", __FILE__, __func__, __LINE__, m->ndim, ndim);
		return;
	}

	int row = 0;
	int column = 0;
	int index3 = 0;
	int index4 = 0;

	int *m_dim = m->dim;


	// Parse & validate the index dimenion inputs
	va_list indices;

	va_start(indices, ndim);
	row = va_arg(indices, int)-1;

	if( (ndim == 1 && (row < 0 || row >= (m->data->size))) 
		||  (ndim != 1 && (row < 0 || row >= m_dim[0])) )
	{
		fprintf(stderr, "%s: %s (%d): Invalid row dimension: %d\nRemember that indexM is one-indexed, like Octave\n", __FILE__, __func__, __LINE__, row);
		return;
	}
	if (ndim > 1)
	{
		column = va_arg(indices, int)-1;
		if(column < 0 || column >= m_dim[1]){fprintf(stderr, "%s: %s (%d): Invalid column dimension: %d\nRemember that indexM is one-indexed, like Octave\n", __FILE__, __func__, __LINE__, column);va_end(indices);return;}

		if (ndim > 2)
		{
			index3 = va_arg(indices, int)-1;
			if(index3 < 0 || index3 >= m_dim[2]){fprintf(stderr, "%s: %s (%d): Invalid index3 dimension: %d\nRemember that indexM is one-indexed, like Octave\n", __FILE__, __func__, __LINE__, index3);va_end(indices);return;}

			if (ndim > 3)
			{
				index4 = va_arg(indices, int)-1;
				if(index4 < 0 || index4 >= m_dim[3]){fprintf(stderr, "%s: %s (%d): Invalid index4 dimension: %d\nRemember that indexM is one-indexed, like Octave\n", __FILE__, __func__, __LINE__, index4);va_end(indices);return;}
			}
		}
	}

	va_end(indices);


	int flat_index = row; // Inputting only one dimenion is interpretted as a flat dimension

	// The formula for finding the flat index of input dimensions 
	// [r, c, x, y] in a matrix that has n_row rows, n_col columns, and 
	// n_x third-dimension-index is:
	// (c) + (r)*(n_col) + (x)*(n_row)*(n_col) + (y)*(n_row)*(n_col)*(n_x)
	if (ndim > 1)
	{
		flat_index = column;
		flat_index += (row)*(m_dim[1]);

		if (ndim > 2)
		{
			flat_index += (index3)*(m_dim[0])*(m_dim[1]);

			if (ndim > 3)
			{
				flat_index += (index4)*(m_dim[0])*(m_dim[1])*(m_dim[2]);
			}
		}
	}

	void *m_data_data = m->data->data;


	switch(m->data->datatype)
	{
		case INTEGER:
			*(int *)output = (((int *)(m_data_data))[flat_index]);
			return;

		case DOUBLE:
			*(double *)output = (((double *)(m_data_data))[flat_index]);
			return;

		case COMPLEX:
			*(double complex *)output = (((double complex *)(m_data_data))[flat_index]);
			return;
	}
}

void printM(Matrix* restrict m)
// Prints out the Matrix
{
    fprintM(stdout, m);
}

void fprintM(FILE *fptr, Matrix* restrict m)
// Prints out the Matrix
{
	if(m == NULL){return;}

	char dims[256];
	int index = 0;
	int m_ndim = m->ndim;
	int *m_dim = m->dim;

	index += sprintf(&dims[index], "%dD, ", (m_ndim));
	for (register int i = 0 ; i < m_ndim ; i++) {
		index += sprintf(&dims[index], "%d", (m_dim)[i]);
		if (i < m_ndim-1)
		{
			index += sprintf(&dims[index], "x");
		}
	}
	index += sprintf(&dims[index], " dimension matrix:");
	//fprintf(stdout, "\n%s\n", dims);
	
	void *m_data_data = m->data->data;
	int m_data_datatype = m->data->datatype;

	for (register int i = 0 ; i < m_dim[0] ; i++)
	{
		for (register int j = 0 ; j < m_dim[1] ; j++)
		{
			switch(m_data_datatype)
			{
				case INTEGER:
				fprintf(fptr, "%d \t", (((int *)(m_data_data))[i*(m_dim[1]) + j]));
					break;

				case DOUBLE:
				fprintf(fptr, "%.4f \t", (((double *)(m_data_data))[i*(m_dim[1]) + j]));
					break;

				case COMPLEX:
					if (almostEqual_di(creal(((double complex *)(m_data_data))[i*(m_dim[1]) + j]), 0))
					{
					fprintf(fptr, "%.4f ", 0.0); // Removes negative zero
					}
					else
					{
					fprintf(fptr, "%.4f ", (creal(((double complex *)(m_data_data))[i*(m_dim[1]) + j])));
					}

					if (isnan((cimag(((double complex *)(m_data_data))[i*(m_dim[1]) + j]))))
					{
					fprintf(fptr, "+ NaNi\t");
					}
					else if ( (((double)(cimag(((double complex *)(m_data_data))[i*(m_dim[1]) + j]))) >= 0) ||
							   almostEqual_di(((double)(cimag(((double complex *)(m_data_data))[i*(m_dim[1]) + j]))), 0) )
					{
					fprintf(fptr, "+%.4fi \t", fabs((cimag(((double complex *)(m_data_data))[i*(m_dim[1]) + j]))));
					}
					else
					{
					fprintf(fptr, "%.4fi \t", (cimag(((double complex *)(m_data_data))[i*(m_dim[1]) + j])));
					}
					break;
			}
			
		}
	fprintf(fptr, "\n");
	}
    fprintf(fptr, "\n");
}

int getnDimM(Matrix* restrict m)
{
	return(m->ndim);
}

int * getDimsM(Matrix* restrict m)
{
	return(m->dim);
}

void * getdataM(Matrix* restrict m)
{
	return(m->data->data);
}

int gettypeM(Matrix* restrict m)
{
	return(m->data->datatype);
}

int getsizeM(Matrix* restrict m)
{
	return(m->data->size);
}

int getreadonlyM(Matrix* restrict m)
{
	return(m->data->readOnly);
}

bool reshapeM(Matrix* restrict m, int ndim, int dim[ndim])
// Reshapes a matrix
{
	if (m == NULL)
	{
		return(false);
	}

	// Error checking: ndim must be 1,2,3,or 4D.
	if (ndim < 1 || ndim > 4)
	{
		fprintf(stderr, "%s: %s (%d): Invalid ndim\n", __FILE__, __func__, __LINE__);		
		return(false);
	}

	// Error checking: dimensions must be valid sizes
	// Error checking: resize dimensions must hold same number of values
	int nActualValues = 1;
	for (register int i = 0 ; i < ndim ; i++)
	{
		nActualValues *= dim[i];
		if (dim[i] < 1) {
			fprintf(stderr, "%s: %s (%d): Invalid dim\n", __FILE__, __func__, __LINE__);
			return(false);
		}
	}

	if (m->data->size != nActualValues)
	{
		fprintf(stderr, "%s: %s (%d): Invalid resize dimensions\n", __FILE__, __func__, __LINE__);
		return(false);
	}

	m->ndim = ndim;
	int *m_dim = m->dim;
	for (register int i = 0 ; i < 4 ; i++)
	{
		if (i >= ndim)
		{
			m_dim[i] = 1;
		}
		else
		{
			m_dim[i] = dim[i];
		}
	}

	return(true);
}