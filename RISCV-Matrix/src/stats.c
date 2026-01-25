//
//  stats.c
//  Octave-C
//
//  Created by Zach Taylor on 12/4/22
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

void sort(void* restrict data, int N, int type, int direction)
// Sorts a list of numbers in place
{
	switch(type)
	{
		case INTEGER: ;
			if (direction == ASCENDING)
			{
				qsort(data, N, sizeof(int), int_comp_asc);
			}
			else
			{
				qsort(data, N, sizeof(int), int_comp_desc);
			}
			break;

		case DOUBLE: ;
			if (direction == ASCENDING)
			{
				qsort(data, N, sizeof(double), double_comp_asc);
			}
			else
			{
				qsort(data, N, sizeof(double), double_comp_desc);
			}
			break;

		case COMPLEX: ;
			if (direction == ASCENDING)
			{
				qsort(data, N, sizeof(double complex), complex_comp_asc);
			}
			else
			{
				qsort(data, N, sizeof(double complex), complex_comp_desc);
			}
			break;
	}
}

Matrix * tsortM(Matrix* restrict m, int direction)
// If m is a row or column vector, returns a row or column vector with sorted elements
// If m is a matrix, returns a matrix with each column sorted
// Returns the output matrix transposed 
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;

	// Sort column and row vectors
	if ((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1))
	{
		output = full_duplicateM(m);
		sort(output->data->data, output->data->size, output->data->datatype, direction);
		return(output);
	}

	// For matrices, return a row vector with mean value of each column
	output = transposeM(m);
	int output_data_size = output->data->size;
	int output_data_datatype = output->data->datatype;
	int *output_dim = output->dim;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:
			for (register int i = 0 ; i < output_data_size ; i += output_dim[1])
			{
				sort( &(((int *)(output_data_data))[i]), output_dim[1], output_data_datatype, direction);
			}
			break;

		case DOUBLE:
			for (register int i = 0 ; i < output_data_size ; i += output_dim[1])
			{
				sort( &(((double *)(output_data_data))[i]), output_dim[1], output_data_datatype, direction);
			}
			break;

		case COMPLEX:
			for (register int i = 0 ; i < output_data_size ; i += output_dim[1])
			{
				sort( &(((double complex *)(output_data_data))[i]), output_dim[1], output_data_datatype, direction);
			}
			break;
	}

	return(output);
}

Matrix * sortM(Matrix* restrict m, int direction)
// If m is a row or column vector, returns a row or column vector with sorted elements
// If m is a matrix, returns a matrix with each column sorted
{
	Matrix *sorted = tsortM(m, direction);

	if ((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1))
	{
		return(sorted);
	}
	
	Matrix *final = transposeM(sorted);

	if (!destroyM(sorted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	return(final);
}

void mean(void* restrict output, int type, int N, void* restrict data)
// Returns the average of a list of numbers
{
	switch(type)
	{
		case INTEGER: ;
			int sum_i = 0;
			for (register int i = 0; i < N; i++)
			{
				sum_i += ((int *)(data))[i];
			}

			*(double *) output = (double) sum_i / (double) N;
			break;

		case DOUBLE: ;

			double sum_d = 0;
			for (register int i = 0; i < N; i++)
			{
				sum_d += ((double *)(data))[i];
			}
			*(double *) output = (double) sum_d / (double) N;
			break;

		case COMPLEX: ;

			double complex sum_c = 0;
			for (register int i = 0; i < N; i++)
			{
				sum_c += ((double complex *)(data))[i];
			}
			*(double complex *) output = (double complex) sum_c / (double complex) N;
			break;
	}
}

Matrix * meanM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the average of the values
// If m is a matrix, returns a row vector with the mean value of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	void *m_data_data = m->data->data;
	int *m_dim = m->dim;
	int m_data_size = m->data->size;


	Matrix *output = NULL;
	int one[] = {1};

	// For row or column vectors, take the average of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m_dim[0] == 1))
	{
		switch(m->data->datatype)
		{
			case INTEGER: ;
			case DOUBLE: ;
				output = createM(1, one, DOUBLE);
				mean(&(((double *)(output->data->data))[0]), m->data->datatype, m_data_size, m_data_data);
				break;

			case COMPLEX: ;
				output = createM(1, one, COMPLEX);
				mean(&(((double complex *)(output->data->data))[0]), m->data->datatype, m_data_size, m_data_data);
				break;

		}
		return(output);
	}

	// For matrices, return a row vector with mean value of each column
	int columns[] = {1,m_dim[1]};
	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(2, columns, DOUBLE);
			void *output_data_data = output->data->data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double *)(output_data_data))[i] = (double) ((int *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i%m_dim[1]] += (double) ((int *)(m_data_data))[i];
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double *)(output_data_data))[i] /= (double) m_dim[0];
			}
			break;

		case DOUBLE: ;
			output = createM(2, columns, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double *)(output_data_data))[i] = ((double *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i%m_dim[1]] += ((double *)(m_data_data))[i];
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double *)(output_data_data))[i] /= (double) m_dim[0];
			}
			break;

		case COMPLEX: ;
			output = createM(2, columns, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i%m_dim[1]] += ((double complex *)(m_data_data))[i];
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				((double complex *)(output_data_data))[i] /= (double complex) m_dim[0];
			}

			break;
	}

	return(output);
}

Matrix * medianM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the median of the values
// If m is a matrix, returns a row vector with the median value of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *sorted = sortM(m, ASCENDING);
	void *sorted_data_data = sorted->data->data;
	int sorted_data_size = sorted->data->size;
	int *sorted_dim = sorted->dim;

	Matrix *output = NULL;
	void *output_data_data = NULL;
	
	// For row or column vectors, take the average of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1))
	{
		int one[] = {1};

		// Edge case where an integer vector has a double median
		if ( sorted->data->datatype == INTEGER && 
			sorted_data_size % 2 == 0 && 
			(((int *)(sorted_data_data))[(sorted_data_size) / 2] + ((int *)(sorted_data_data))[(sorted_data_size) / 2 - 1]) % 2 == 1)
		{
			output = createM(1, one, DOUBLE);
			output_data_data = output->data->data;
			((double *)(output_data_data))[0] = 0.5 * (double)((((int *)(sorted_data_data))[(sorted_data_size) / 2]) + ((int *)(sorted_data_data))[(sorted_data_size) / 2 - 1]);
			if(!destroyM(sorted))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
				return(NULL);
			}
			return(output);
		}

		output = createM(1, one, m->data->datatype);
		output_data_data = output->data->data;

		switch(m->data->datatype)
		{
			case INTEGER: ;
				if (sorted_data_size % 2 == 0)
				{
					((int *)(output_data_data))[0] = (int) (0.5 * ((((int *)(sorted_data_data))[(sorted_data_size) / 2]) + ((int *)(sorted_data_data))[(sorted_data_size) / 2 - 1]));
				}
				else
				{
					((int *)(output_data_data))[0] = ((int *)(sorted_data_data))[(int) (ceil((sorted_data_size) / 2))];
				}
				break;

			case DOUBLE: ;
				if (sorted_data_size % 2 == 0)
				{
					((double *)(output_data_data))[0] = 0.5 * ((((double *)(sorted_data_data))[(sorted_data_size) / 2]) + ((double *)(sorted_data_data))[(sorted_data_size) / 2 - 1]);
				}
				else
				{
					((double *)(output_data_data))[0] = ((double *)(sorted_data_data))[(int) (ceil((sorted_data_size) / 2))];
				}
				break;

			case COMPLEX: ;
				if (sorted_data_size % 2 == 0)
				{
					((double complex *)(output_data_data))[0] = ((double complex)(0.5)) * ((((double complex *)(sorted_data_data))[(sorted_data_size) / 2]) + ((double complex *)(sorted_data_data))[(sorted_data_size) / 2 - 1]);
				}
				else
				{
					((double complex *)(output_data_data))[0] = ((double complex *)(sorted_data_data))[(int) (ceil((sorted_data_size) / 2))];
				}
				break;

		}

		if(!destroyM(sorted))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
		return(output);
	}

	int output_data_datatype = DOUBLE;
	if (m->data->datatype == COMPLEX) {output_data_datatype = COMPLEX;}

	// For matrices, return a row vector with mean value of each column
	int columns[] = {1,m->dim[1]};
	output = createM(2, columns, output_data_datatype);
	output_data_data = output->data->data;
	int output_data_size = output->data->size;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			if (m->dim[0] % 2 == 0)
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double *)(output_data_data))[i] = ((double) ((int *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])] +
														  (double) ((int *)(sorted_data_data))[i + ((sorted_dim[0])/2-1) * (sorted_dim[1])])/2;
				}
			}
			else
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double *)(output_data_data))[i] = (double) ((int *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])];
				}
			}
			break;

		case DOUBLE: ;
			if (m->dim[0] % 2 == 0)
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double *)(output_data_data))[i] = ((double) ((double *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])] +
														  (double) ((double *)(sorted_data_data))[i + ((sorted_dim[0])/2-1) * (sorted_dim[1])])/2;
				}
			}
			else
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double *)(output_data_data))[i] = (double) ((double *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])];
				}
			}
			break;

		case COMPLEX: ;
			if (m->dim[0] % 2 == 0)
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double complex *)(output_data_data))[i] = (((double complex *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])] +
																  ((double complex *)(sorted_data_data))[i + ((sorted_dim[0])/2-1) * (sorted_dim[1])])/2;
				}
			}
			else
			{
				for (register int i = 0 ; i < output_data_size ; i++)
				{
					((double complex *)(output_data_data))[i] = ((double complex *)(sorted_data_data))[i + (sorted_dim[0])/2 * (sorted_dim[1])];
				}
			}
			break;
	}

	if(!destroyM(sorted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * minM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the minimum of the values
// If m is a matrix, returns a row vector with the minimum value of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;

	Matrix *output = NULL;
	void *output_data_data = NULL;

	// For row or column vectors, take the minimum of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m_dim[0] == 1))
	{
		int one[] = {1};
		output = createM(1, one, m->data->datatype);
		output_data_data = output->data->data;

		switch(m->data->datatype)
		{
			case INTEGER: ;
				int min_val_i = ((int *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if (((int *)(m_data_data))[i] < min_val_i)
					{
						min_val_i = ((int *)(m_data_data))[i];
					}
				}
				((int *)(output_data_data))[0] = min_val_i;				
				break;

			case DOUBLE: ;
				double min_val_d = ((double *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if (((double *)(m_data_data))[i] < min_val_d)
					{
						min_val_d = ((double *)(m_data_data))[i];
					}
				}
				((double *)(output_data_data))[0] = min_val_d;	
				break;

			case COMPLEX: ;
				double complex min_val_c = ((double complex *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if ( complex_comp_desc(&(((double complex *)(m_data_data))[i]), &min_val_c) == 1 )
					{
						min_val_c = ((double complex *)(m_data_data))[i];
					}
				}
				((double complex *)(output_data_data))[0] = min_val_c;	
				break;
		}

		
		return(output);
	}

	// For matrices, return a row vector with min value of each column
	int columns[] = {1,m_dim[1]};
	output = createM(2, columns, m->data->datatype);
	output_data_data = output->data->data;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			int *int_output = ((int *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				int_output[i] = ((int *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] < int_output[i%(m_dim[1])])
				{
					int_output[i%(m_dim[1])] = ((int *)(m_data_data))[i];
				}
			}
			break;

		case DOUBLE: ;
			double *double_output = ((double *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				double_output[i] = ((double *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] < double_output[i%(m_dim[1])])
				{
					double_output[i%(m_dim[1])] = ((double *)(m_data_data))[i];
				}
			}
			break;

		case COMPLEX: ;
			double complex *complex_output = ((double complex *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				complex_output[i] = ((double complex *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if ( complex_comp_desc(&(((double complex *)(m_data_data))[i]) , &(complex_output[i%(m_dim[1])])) == 1)
				{
					complex_output[i%(m_dim[1])] = ((double complex *)(m_data_data))[i];
				}
			}
			break;
	}
	return(output);
}

Matrix * maxM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the maximum of the values
// If m is a matrix, returns a row vector with the maximum value of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;

	Matrix *output = NULL;
	void *output_data_data = NULL;

	// For row or column vectors, take the maximum of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m_dim[0] == 1))
	{
		int one[] = {1};
		output = createM(1, one, m->data->datatype);
		output_data_data = output->data->data;

		switch(m->data->datatype)
		{
			case INTEGER: ;
				int max_val_i = ((int *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if (((int *)(m_data_data))[i] > max_val_i)
					{
						max_val_i = ((int *)(m_data_data))[i];
					}
				}
				((int *)(output_data_data))[0] = max_val_i;				
				break;

			case DOUBLE: ;
				double max_val_d = ((double *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if (((double *)(m_data_data))[i] > max_val_d)
					{
						max_val_d = ((double *)(m_data_data))[i];
					}
				}
				((double *)(output_data_data))[0] = max_val_d;	
				break;

			case COMPLEX: ;
				double complex max_val_c = ((double complex *)(m_data_data))[0];
				for (register int i = 1 ; i < m_data_size ; i++)
				{
					if ( complex_comp_asc(&(((double complex *)(m_data_data))[i]), &max_val_c) == 1 )
					{
						max_val_c = ((double complex *)(m_data_data))[i];
					}
				}
				((double complex *)(output_data_data))[0] = max_val_c;	
				break;
		}

		
		return(output);
	}

	// For matrices, return a row vector with max value of each column
	int columns[] = {1,m_dim[1]};
	output = createM(2, columns, m->data->datatype);
	output_data_data = output->data->data;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			int *int_output = ((int *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				int_output[i] = ((int *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] > int_output[i%(m_dim[1])])
				{
					int_output[i%(m_dim[1])] = ((int *)(m_data_data))[i];
				}
			}
			break;

		case DOUBLE: ;
			double *double_output = ((double *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				double_output[i] = ((double *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] > double_output[i%(m_dim[1])])
				{
					double_output[i%(m_dim[1])] = ((double *)(m_data_data))[i];
				}
			}
			break;

		case COMPLEX: ;
			double complex *complex_output = ((double complex *)(output_data_data));

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				complex_output[i] = ((double complex *)(m_data_data))[i];
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				if ( complex_comp_asc(&(((double complex *)(m_data_data))[i]) , &(complex_output[i%(m_dim[1])])) == 1)
				{
					complex_output[i%(m_dim[1])] = ((double complex *)(m_data_data))[i];
				}
			}
			break;
	}
	return(output);
}

Matrix * minV(Matrix* restrict m, int* restrict index)
// If m is a row or column vector, returns a 1x1 matrix with the minimum of the values
// Returns the address of the index of the minimum value in index
// Note: *** the index is ONE-BASED *** to match Octave
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): m must be a vector\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	// For row or column vectors, take the minimum of the elements
	int one[] = {1};
	Matrix *output = createM(1, one, m->data->datatype);
	void *output_data_data = output->data->data;
	*index = 1;

	switch(m->data->datatype)
	{
		case INTEGER: ;

			int min_val_i = ((int *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] < min_val_i)
				{
					min_val_i = ((int *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((int *)(output_data_data))[0] = min_val_i;
			
			break;

		case DOUBLE: ;
			double min_val_d = ((double *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] < min_val_d)
				{
					min_val_d = ((double *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((double *)(output_data_data))[0] = min_val_d;
			break;

		case COMPLEX: ;
			double complex min_val_c = ((double complex *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if ( complex_comp_desc(&(((double complex *)(m_data_data))[i]), &min_val_c) == 1 )
				{
					min_val_c = ((double complex *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((double complex *)(output_data_data))[0] = min_val_c;
			break;
	}

	return(output);
}

Matrix * maxV(Matrix* restrict m, int* restrict index)
// If m is a row or column vector, returns a 1x1 matrix with the maximum of the values
// Returns the address of the index of the maximum value in index
// Note: *** the index is ONE-BASED *** to match Octave
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): m must be a vector\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	// For row or column vectors, take the maximum of the elements
	int one[] = {1};
	Matrix *output = createM(1, one, m->data->datatype);
	void *output_data_data = output->data->data;
	*index = 1;

	switch(m->data->datatype)
	{
		case INTEGER: ;

			int max_val_i = ((int *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] > max_val_i)
				{
					max_val_i = ((int *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((int *)(output_data_data))[0] = max_val_i;
			
			break;

		case DOUBLE: ;
			double max_val_d = ((double *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] > max_val_d)
				{
					max_val_d = ((double *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((double *)(output_data_data))[0] = max_val_d;
			break;

		case COMPLEX: ;
			double complex max_val_c = ((double complex *)(m_data_data))[0];
			for (register int i = 1 ; i < m_data_size ; i++)
			{
				if ( complex_comp_asc(&(((double complex *)(m_data_data))[i]), &max_val_c) == 1 )
				{
					max_val_c = ((double complex *)(m_data_data))[i];
					*index = i+1;
				}
			}
			((double complex *)(output_data_data))[0] = max_val_c;
			break;
	}

	return(output);
}

Matrix * varM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the unbiased sample variance of the values
// If m is a matrix, returns a row vector with the unbiased sample variance of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;

	Matrix *mean_vect = meanM(m);
	void *mean_vect_data_data = mean_vect->data->data;

	Matrix *output = NULL;
	int output_data_datatype = DOUBLE;
	if (m->data->datatype == COMPLEX) {output_data_datatype = COMPLEX;}
	int one[] = {1};

	// For row or column vectors, take the unbiased sample variance of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m_dim[0] == 1))
	{
		output = createM(1, one, output_data_datatype);
		switch(m->data->datatype)
		{
			case INTEGER: ;
				double mean = 0;
				indexM(mean_vect, &mean, 1, 1);

				double sum_of_squares = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares += pow((((int *)(m_data_data))[i] - mean), 2);
				}
				sum_of_squares /= (double) ((m_data_size)-1);

				((double *)(output->data->data))[0] = sum_of_squares;

				break;

			case DOUBLE: ;
				mean = 0;
				indexM(mean_vect, &mean, 1, 1);

				sum_of_squares = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares += pow((((double *)(m_data_data))[i] - mean), 2);
				}
				sum_of_squares /= (double) ((m_data_size)-1);

				((double *)(output->data->data))[0] = sum_of_squares;

				break;

			case COMPLEX: ;
				double complex mean_c = 0;
				indexM(mean_vect, &mean_c, 1, 1);

				double complex sum_of_squares_c = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares_c += (((double complex *)(m_data_data))[i] - mean_c) * (conj(((double complex *)(m_data_data))[i] - mean_c));
				}

				sum_of_squares_c /= (double complex) ((m_data_size)-1);

				((double complex *)(output->data->data))[0] = sum_of_squares_c;

				break;

		}

		if (!destroyM(mean_vect))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
		return(output);
	}

	// For matrices, return a row vector with mean value of each column
	int columns[] = {1,m_dim[1]};
	output = createM(2, columns, output_data_datatype);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			double *col_vars_i = output_data_data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_i[i] = (double) pow( ((int *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i], 2);
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_i[i%m_dim[1]] += (double) pow( ((int *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i%m_dim[1]], 2);
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_i[i] /= (double) ((m_dim[0])-1);
			}
			break;

		case DOUBLE: ;
			double *col_vars_d = output_data_data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_d[i] = (double) pow( ((double *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i], 2);
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_d[i%m_dim[1]] += (double) pow( ((double *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i%m_dim[1]], 2);
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_d[i] /= (double) ((m_dim[0])-1);
			}
			break;

		case COMPLEX: ;
			double complex *col_vars_c = output_data_data;			

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_c[i] = ( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i]) * (conj( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i]));
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_c[i%m_dim[1]] += ( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i%m_dim[1]]) * (conj( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i%m_dim[1]]));
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_c[i] /= (double complex) ((m_dim[0])-1);
			}
			break;
	}

	if (!destroyM(mean_vect))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * popvarM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the population variance of the values
// If m is a matrix, returns a row vector with the population variance of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;

	Matrix *mean_vect = meanM(m);
	void *mean_vect_data_data = mean_vect->data->data;

	Matrix *output = NULL;
	int output_data_datatype = DOUBLE;
	if (m->data->datatype == COMPLEX) {output_data_datatype = COMPLEX;}
	int one[] = {1};

	// For row or column vectors, take the unbiased sample variance of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m_dim[0] == 1))
	{
		output = createM(1, one, output_data_datatype);
		switch(m->data->datatype)
		{
			case INTEGER: ;
				double mean = 0;
				indexM(mean_vect, &mean, 1, 1);

				double sum_of_squares = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares += pow((((int *)(m_data_data))[i] - mean), 2);
				}
				sum_of_squares /= (double) ((m_data_size));

				((double *)(output->data->data))[0] = sum_of_squares;

				break;

			case DOUBLE: ;
				mean = 0;
				indexM(mean_vect, &mean, 1, 1);

				sum_of_squares = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares += pow((((double *)(m_data_data))[i] - mean), 2);
				}
				sum_of_squares /= (double) ((m_data_size));

				((double *)(output->data->data))[0] = sum_of_squares;

				break;

			case COMPLEX: ;
				double complex mean_c = 0;
				indexM(mean_vect, &mean_c, 1, 1);

				double complex sum_of_squares_c = 0;
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					sum_of_squares_c += (((double complex *)(m_data_data))[i] - mean_c) * (conj(((double complex *)(m_data_data))[i] - mean_c));
				}

				sum_of_squares_c /= (double complex) ((m_data_size));

				((double complex *)(output->data->data))[0] = sum_of_squares_c;

				break;

		}

		if (!destroyM(mean_vect))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
		return(output);
	}

	// For matrices, return a row vector with mean value of each column
	int columns[] = {1,m_dim[1]};
	output = createM(2, columns, output_data_datatype);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			double *col_vars_i = output_data_data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_i[i] = (double) pow( ((int *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i], 2);
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_i[i%m_dim[1]] += (double) pow( ((int *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i%m_dim[1]], 2);
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_i[i] /= (double) ((m_dim[0]));
			}
			break;

		case DOUBLE: ;
			double *col_vars_d = output_data_data;

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_d[i] = (double) pow( ((double *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i], 2);
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_d[i%m_dim[1]] += (double) pow( ((double *)(m_data_data))[i] - ((double *)(mean_vect_data_data))[i%m_dim[1]], 2);
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_d[i] /= (double) ((m_dim[0]));
			}
			break;

		case COMPLEX: ;
			double complex *col_vars_c = output_data_data;			

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_c[i] = ( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i]) * (conj( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i]));
			}

			for (register int i = m_dim[1] ; i < m_data_size ; i++)
			{
				col_vars_c[i%m_dim[1]] += ( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i%m_dim[1]]) * (conj( ((double complex *)(m_data_data))[i] - ((double complex *)(mean_vect_data_data))[i%m_dim[1]]));
			}

			for (register int i = 0 ; i < m_dim[1] ; i++)
			{
				col_vars_c[i] /= (double complex) ((m_dim[0]));
			}
			break;
	}

	if (!destroyM(mean_vect))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * stdM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the unbiased sample variance of the values
// If m is a matrix, returns a row vector with the unbiased sample variance of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *variance = varM(m);
	if (!m)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	double onehalf = 0.5;
	Matrix *output = scalarpowerM(variance, &onehalf, DOUBLE);

	if (!output)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(variance))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(output);
}

Matrix * popstdM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x1 matrix with the population standard deviation of the values
// If m is a matrix, returns a row vector with the population standard deviation of each column
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *variance = popvarM(m);
	if (!m)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	double onehalf = 0.5;
	Matrix *output = scalarpowerM(variance, &onehalf, DOUBLE);

	if (!output)
	{
		fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(variance))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(output);
}

double int_quantile(int* restrict vec, int N, double q)
// Finds the quantile q of an integer vector vec of length N
{
	// Error checking
	if (N < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid N\n", __FILE__, __func__, __LINE__);
		return(NAN);
	}

	if (q > 1 && !almostEqual_id(1, q))
	{
		return(INFINITY);
	}

	if (q < 0 && !almostEqual_id(0, q))
	{
		return(-INFINITY);
	}

	double output = NAN;

	// Make a copy of vec to play with
	int *v = NULL;
	v = malloc(N*sizeof(*v));
	for (register int i = 0 ; i < N ; i++)
	{
		v[i] = vec[i];
	}
	sort(v, N, INTEGER, ASCENDING);

	// Minimum
	if (q <= 0.5/(double) N)
	{
		output = (double) v[0];
		free(v); 
		return(output);
	}

	// Maximum
	if (q >= (double)(N - 0.5)/N)
	{
		output = (double) v[N-1];
		free(v);
		return(output);
	}

	// The i-th element of the sorted vector corresponds to the (i+0.5)/N -th quantile
	// If the desired quantile isn't exactly in the vector, linear interpolation is used between the nearest two points
	// 0.5/N < q < (N-0.5)/N
	double quantile_low = 0.5/(double) N;
	for (register int i = 0 ; i < N ; i++)
	{
		// If q is the i-th quantile, return the i-th vector item
		if (q == quantile_low)
		{
			output = (double) v[i];
			break;
		}

		// Get the (i+1)-th quantile
		double quantile_high = ((i+1)+0.5)/N;

		// Linear interpolation
		if (q > quantile_low && q < quantile_high)
		{
			output = v[i] + (q*N - (i+0.5))*(v[i+1] - v[i]);
			break;
		}

		quantile_low = quantile_high;
	}

	free(v);
	return(output);
}

double double_quantile(double* restrict vec, int N, double q)
// Finds the quantile q of a double vector vec of length N
{
	// Error checking
	if (N < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid N\n", __FILE__, __func__, __LINE__);
		return(NAN);
	}

	if (q > 1 && !almostEqual_id(1, q))
	{
		return(INFINITY);
	}

	if (q < 0 && !almostEqual_id(0, q))
	{
		return(-INFINITY);
	}

	double output = NAN;

	// Make a copy of vec to play with
	double *v = NULL;
	v = malloc(N*sizeof(*v));
	for (register int i = 0 ; i < N ; i++)
	{
		v[i] = vec[i];
	}
	sort(v, N, DOUBLE, ASCENDING);

	// Minimum
	if (q <= 0.5/(double) N)
	{
		output = v[0];
		free(v); 
		return(output);
	}

	// Maximum
	if (q >= (double)(N - 0.5)/N)
	{
		output = v[N-1];
		free(v);
		return(output);
	}

	// The i-th element of the sorted vector corresponds to the (i+0.5)/N -th quantile
	// If the desired quantile isn't exactly in the vector, linear interpolation is used between the nearest two points
	// 0.5/N < q < (N-0.5)/N
	double quantile_low = 0.5/(double) N;
	for (register int i = 0 ; i < N ; i++)
	{
		// If q is the i-th quantile, return the i-th vector item
		if (q == quantile_low)
		{
			output = v[i];
			break;
		}

		// Get the (i+1)-th quantile
		double quantile_high = ((i+1)+0.5)/N;

		// Linear interpolation
		if (q > quantile_low && q < quantile_high)
		{
			output = v[i] + (q*N - (i+0.5))*(v[i+1] - v[i]);
			break;
		}

		quantile_low = quantile_high;
	}

	free(v);
	return(output);
}

double complex complex_quantile(double complex * restrict vec, int N, double q)
// Finds the quantile q of a complex vector vec of length N
{
	// Error checking
	if (N < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid N\n", __FILE__, __func__, __LINE__);
		return(NAN);
	}

	if (q > 1 && !almostEqual_id(1, q))
	{
		return(INFINITY);
	}

	if (q < 0 && !almostEqual_id(0, q))
	{
		return(-INFINITY);
	}

	double complex output = NAN;

	// Make a copy of vec to play with
	double complex *v = NULL;
	v = malloc(N*sizeof(*v));
	for (register int i = 0 ; i < N ; i++)
	{
		v[i] = vec[i];
	}
	sort(v, N, COMPLEX, ASCENDING);

	// Minimum
	if (q <= 0.5/(double) N)
	{
		output = v[0];
		free(v); 
		return(output);
	}

	// Maximum
	if (q >= (double)(N - 0.5)/N)
	{
		output = v[N-1];
		free(v);
		return(output);
	}

	// The i-th element of the sorted vector corresponds to the (i+0.5)/N -th quantile
	// If the desired quantile isn't exactly in the vector, linear interpolation is used between the nearest two points
	// 0.5/N < q < (N-0.5)/N
	double quantile_low = 0.5/(double) N;
	for (register int i = 0 ; i < N ; i++)
	{
		// If q is the i-th quantile, return the i-th vector item
		if (q == quantile_low)
		{
			output = v[i];
			break;
		}

		// Get the (i+1)-th quantile
		double quantile_high = ((i+1)+0.5)/N;

		// Linear interpolation
		if (q > quantile_low && q < quantile_high)
		{
			output = v[i] + (q*N - (i+0.5))*(v[i+1] - v[i]);
			break;
		}

		quantile_low = quantile_high;
	}

	free(v);
	return(output);
}

Matrix * quantileM_vect(Matrix* restrict m, int N, double* restrict quantiles)
// If m is a row or column vector, returns a 1xN matrix with the specified  quantiles
// If m is a matrix of size rxc, returns a Nxc matrix with the specified quantiles of each column 
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Matrix statistical functions do not support 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *sorted = tsortM(m, ASCENDING);
	int sorted_data_size = sorted->data->size;
	void *sorted_data_data = sorted->data->data;
	int *sorted_dim = sorted->dim;

	Matrix *output = NULL;
	int onebyN[] = {1,N};
	int output_data_datatype = DOUBLE;
	if (m->data->datatype == COMPLEX) {output_data_datatype = COMPLEX;}
	void *output_data_data = NULL;

	// For row or column vectors, take the average of the elements
	if ((m->ndim == 1) || (m->ndim == 2 && m->dim[0] == 1))
	{
		output = createM(2, onebyN, output_data_datatype);
		output_data_data = output->data->data;

		switch(m->data->datatype)
		{
			case INTEGER: ;
				for (register int i = 0 ; i < N ; i++)
				{
					((double *)(output_data_data))[i] = int_quantile(((int *)(sorted_data_data)), sorted_data_size, quantiles[i]);
				}
				
				break;

			case DOUBLE: ;
				for (register int i = 0 ; i < N ; i++)
				{
					((double *)(output_data_data))[i] = double_quantile(((double *)(sorted_data_data)), sorted_data_size, quantiles[i]);
				}

				break;

			case COMPLEX: ;
				for (register int i = 0 ; i < N ; i++)
				{
					((double complex *)(output_data_data))[i] = complex_quantile(((double complex *)(sorted_data_data)), sorted_data_size, quantiles[i]);
				}

				break;
		}

		if(!destroyM(sorted))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
		return(output);
	}

	// For matrices, return a 5xC matrix with the quantiles of each column
	int NbyC[] = {N,m->dim[1]};
	output = createM(2, NbyC, output_data_datatype);
	output_data_data = output->data->data;
	int *output_dim = output->dim;
	
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < sorted_dim[0] ; i++)
			{
				int *int_vect = NULL;
				int_vect = malloc((sorted_dim[1])*sizeof(*int_vect));

				for (register int j = 0 ; j < sorted_dim[1] ; j++)
				{
					int_vect[j] = ((int *)(sorted_data_data))[i*(sorted_dim[1]) + j];
				}

				for (register int j = 0 ; j < output_dim[0] ; j++)
				{
					((double *)(output_data_data))[i+j*(output_dim[1])] = int_quantile(int_vect, sorted_dim[1], quantiles[j]);
				}
				free(int_vect);
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < sorted_dim[0] ; i++)
			{
				double *double_vect = NULL;
				double_vect = malloc((sorted_dim[1])*sizeof(*double_vect));

				for (register int j = 0 ; j < sorted_dim[1] ; j++)
				{
					double_vect[j] = ((double *)(sorted_data_data))[i*(sorted_dim[1]) + j];
				}

				for (register int j = 0 ; j < output_dim[0] ; j++)
				{
					((double *)(output_data_data))[i+j*(output_dim[1])] = double_quantile(double_vect, sorted_dim[1], quantiles[j]);
				}
				free(double_vect);
			}
			break;

		case COMPLEX: ;
			for (register int i = 0 ; i < sorted_dim[0] ; i++)
			{
				double complex *complex_vect = NULL;
				complex_vect = malloc((sorted_dim[1])*sizeof(*complex_vect));

				for (register int j = 0 ; j < sorted_dim[1] ; j++)
				{
					complex_vect[j] = ((double complex *)(sorted_data_data))[i*(sorted_dim[1]) + j];
				}

				for (register int j = 0 ; j < output_dim[0] ; j++)
				{
					((double complex *)(output_data_data))[i+j*(output_dim[1])] = complex_quantile(complex_vect, sorted_dim[1], quantiles[j]);
				}
				free(complex_vect);
			}
			break;
	}

	if(!destroyM(sorted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * quantileM(Matrix* restrict m)
// If m is a row or column vector, returns a 1x5 matrix with the [0, 0.25, 0.5, 0.75, 1] quantiles
// If m is a matrix of size rxc, returns a 5xc matrix with the [0, 0.25, 0.5, 0.75, 1] quantiles of each column 
{
	double default_quantiles[] = {0, 0.25, 0.5, 0.75, 1};
	return(quantileM_vect(m, 5, default_quantiles));
}

double incbeta(double a, double b, double x) 
// Adapted from zlib and matlab
// https://github.com/DynareTeam/dynare/blob/master/matlab/missing/stats/betapdf.m
// https://codeplea.com/incomplete-beta-function-c
{
    if (almostEqual_id(0, x) || almostEqual_id(1, x))
    {
    	return(1.0/0.0);
    }
    if (almostEqual_id(0, a) || almostEqual_id(0, b))
    {
    	return(NAN);
    }
    if (x < 0.0 || x > 1.0)
    {
    	return(0);
    }
    
    // Find the first part before the continued fraction.
    const double lbeta_ab = (lgamma(a))+(lgamma(b))-(lgamma(a+b));
    const double front = exp(log(x)*(a-1)+log(1.0-x)*(b-1)-lbeta_ab);

    return(front);
}

Matrix * betapdfM(Matrix* restrict m, double a, double b)
// Computes the beta pdf at each value in m using the parameters a and b
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if ((a < 0 && !almostEqual_id(0, a)) || (b < 0 && !almostEqual_id(0, b)))
	{
		fprintf(stderr, "%s: %s (%d): Invalid a or b\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = incbeta(a, b, (double) ((int *)(m_data_data))[i]);
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = incbeta(a, b, ((double *)(m_data_data))[i]);
			}
			break;
	}
	
	return(output);
}

Matrix * exppdfM(Matrix* restrict m, double lambda)
// Computes the exponential pdf at each value in m using the mean lambda
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (lambda <= 0 && !almostEqual_id(0, lambda))
	{
		fprintf(stderr, "%s: %s (%d): Invalid lambda\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] < 0)
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = 1/lambda * exp(-1/lambda * ((int *)(m_data_data))[i]);
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] < 0)
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = 1/lambda * exp(-1/lambda * ((double *)(m_data_data))[i]);
				}
			}
			break;
	}
	

	return(output);
}

Matrix * chi2pdfM(Matrix* restrict m, double n)
// Computes the chi-square pdf at each value in m using the n degrees of freedom
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] < 0)
				{
					((double *)(output_data_data))[i] = 0;
				}
				else if (almostEqual_ii(0, ((int *)(m_data_data))[i]) && almostEqual_id(2, n)) // Miscellaneous indeterminate form
				{
					((double *)(output_data_data))[i] = 0.5;
				}
				else if (almostEqual_ii(0, ((int *)(m_data_data))[i]) && (double) n/2.0 - 1 < 0) // Guarantee positive infinity
				{
					((double *)(output_data_data))[i] = INFINITY;
				}
				else
				{
					((double *)(output_data_data))[i] = (pow((double) ((int *)(m_data_data))[i] , (double) n/2.0 - 1) * exp(-0.5 * (double)((int *)(m_data_data))[i]))/(pow(2.0, (double) n/2.0) * exp(lgamma( (double) n/2.0)));
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] < 0)
				{
					((double *)(output_data_data))[i] = 0;
				}
				else if (almostEqual_id(0, ((double *)(m_data_data))[i]) && almostEqual_id(2, n)) // Miscellaneous indeterminate form
				{
					((double *)(output_data_data))[i] = 0.5;
				}
				else if (almostEqual_id(0, ((double *)(m_data_data))[i]) && (double) n/2.0 - 1 < 0) // Guarantee positive infinity
				{
					((double *)(output_data_data))[i] = INFINITY;
				}
				else
				{
					((double *)(output_data_data))[i] = (pow(((double *)(m_data_data))[i] , (double) n/2.0 - 1) * exp(-0.5 * ((double *)(m_data_data))[i]))/(pow(2.0, (double) n/2.0) * exp(lgamma( (double) n/2.0)));
				}
			}
			break;
	}
	

	return(output);
}

Matrix * gampdfM(Matrix* restrict m, double a, double b)
// Computes the gamma pdf at each value in m using shape parameter a and rate parameter b
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if ((a <= 0 || almostEqual_id(0, a)) || (b <= 0 || almostEqual_id(0, b)))
	{
		fprintf(stderr, "%s: %s (%d): Invalid a or b\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] < 0 && !almostEqual_ii(0, ((int *)(m_data_data))[i]))
				{
					((double *)(output_data_data))[i] = 0;
				}
				else if (almostEqual_ii(((int *)(m_data_data))[i], 0) && a-1 < 0) // Guarantee positive infinity
				{
					((double *)(output_data_data))[i] = INFINITY;
				}
				else
				{
					((double *)(output_data_data))[i] = (pow(b, -a) * pow(((int *)(m_data_data))[i] , a-1) * exp(-1/b * ((int *)(m_data_data))[i]) ) / (exp(lgamma(a)));					
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] < 0 && !almostEqual_id(0, ((double *)(m_data_data))[i]))
				{
					((double *)(output_data_data))[i] = 0;
				}
				else if (almostEqual_di(((double *)(m_data_data))[i], 0) && a-1 < 0)  // Guarantee positive infinity
				{
					((double *)(output_data_data))[i] = INFINITY;
				}
				else
				{
					((double *)(output_data_data))[i] = (pow(b, -a) * pow(((double *)(m_data_data))[i] , a-1) * exp(-1/b * ((double *)(m_data_data))[i]) ) / (exp(lgamma(a)));					
				}
			}
			break;
	}
	

	return(output);
}

Matrix * lognpdfM(Matrix* restrict m, double mu, double sigma)
// Computes the lognormal pdf at each value in m using mean mu and standard deviation sigma
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (sigma <= 0 || almostEqual_id(0, sigma))
	{
		fprintf(stderr, "%s: %s (%d): Invalid sigma\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] <= 0 || almostEqual_ii(0, ((int *)(m_data_data))[i]))
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = 1/(((int *)(m_data_data))[i] * sigma * sqrt(2*M_PI)) * exp(-(pow(log(((int *)(m_data_data))[i]) - mu , 2))/(2*pow(sigma, 2)));
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] <= 0 || almostEqual_id(0, ((double *)(m_data_data))[i]))
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = 1/(((double *)(m_data_data))[i] * sigma * sqrt(2*M_PI)) * exp(-(pow(log(((double *)(m_data_data))[i]) - mu , 2))/(2*pow(sigma, 2)));
				}			
			}
			break;
	}
	

	return(output);
}

Matrix * normpdfM(Matrix* restrict m, double mu, double sigma)
// Computes the normal pdf at each value in m using mean mu and standard deviation sigma
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (sigma <= 0 || almostEqual_id(0, sigma))
	{
		fprintf(stderr, "%s: %s (%d): Invalid sigma\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = 1/(sigma * sqrt(2*M_PI)) * exp(-(pow(((int *)(m_data_data))[i] - mu , 2))/(2*pow(sigma, 2)));
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = 1/(sigma * sqrt(2*M_PI)) * exp(-(pow(((double *)(m_data_data))[i] - mu , 2))/(2*pow(sigma, 2)));
			}
			break;
	}
	
	return(output);
}

Matrix * unidpdfM(Matrix* restrict m, int n)
// Computes the uniform discrete pdf at each value in m using the maximum observable value n
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (n <= 0 || almostEqual_id(0, n))
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	double *output_data_data = output->data->data;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] <= 0 || almostEqual_ii(0, ((int *)(m_data_data))[i]) 
					|| ((int *)(m_data_data))[i] > n)
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = (double) 1 / (double) n;
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] <= 0 || almostEqual_id(0, ((double *)(m_data_data))[i]) 
					|| ((double *)(m_data_data))[i] > n
					|| ((double *)(m_data_data))[i] != floor(((double *)(m_data_data))[i]))
				{
					((double *)(output_data_data))[i] = 0;
				}
				else
				{
					((double *)(output_data_data))[i] = (double) 1 / (double) n;
				}			
			}
			break;
	}
	

	return(output);
}

double gammadiv(double x)
// Returns the gamma function fraction used by the hypergeometric function
// Returns gamma((x+1)/2) / gamma(x/2)
{
	double numerator = lgamma(0.5*(x+1));
	double denominator = lgamma(0.5*x);
	double output = exp(numerator - denominator);

	// exp should never return a negative value, so if it's negative, there was an overflow
	if (output < 0)
	{
		return(DBL_MAX);
	}
	return((double) output);
}

double tcdf_integral (double x, double v)
// Estimates integral from -inf to x of (1 + t^2/v)^(-0.5*(v+1)) dt
{
	double output = 0;
	double step_size = 0.001;
	for (register double num = x ; num > -50 ; num-=step_size)
	{
		output += step_size*pow(1 + num*num/v , -0.5*(v+1));
	}
	// printf("integral: %.3f\n", (double) output);
	return(output);
}

double tcdf (double x, double df)
// Get the t-distribution CDF at x with df degrees of freedom
{
	return(gammadiv(df)/sqrt(M_PI*df)*tcdf_integral(x, df));
}

void ttestM(Matrix* restrict m, double mu, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict tstat, double* restrict df, double* restrict sd)
// For a sample m from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (m) == mu'
// Uses alpha = 0.05
{	
	if (m == NULL)
	{
		return;
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (!(m->ndim == 1) && !(m->ndim == 2 && m->dim[0] == 1))
	{
		fprintf(stderr, "%s: %s (%d): m must not be a row or column vector\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Degrees of freedom
	*df = m->data->size - 1;

	if (*df == 0)
	{
		*h = false;
		*pval = NAN;
		(*ci)[0] = NAN;
		(*ci)[1] = NAN;
		*tstat = NAN;
		*sd = 0;
		return;
	}

	// Standard deviation
	Matrix *var_mat = samplevarM(m);
	*sd = sqrt(((double *)(var_mat->data->data))[0]);

	// Confidence Interval
	double mean_val = NAN;
	mean(&mean_val, m->data->datatype, m->data->size, m->data->data);
	double standard_error = *sd / sqrt(m->data->size);

	double se_multiplier = 1.96;
	if (*df <= 1001)
	{
		// Lookup table for the standard error multiplier for 95% confidence interval
		double se_multiplier_ci_005[] = {12.70620473617471,4.302652729749464,3.18244630528371,2.776445105197795,2.570581835636316,2.44691185114497,2.364624251592785,2.306004135204167,2.262157162798206,2.228138851986274,2.20098516009164,2.178812829667228,2.160368656462793,2.144786687917804,2.131449545559774,2.119905299221254,2.109815577833318,2.100922040241038,2.093024054408311,2.085963447265863,2.079613844727681,2.073873067904024,2.06865761041905,2.063898561628027,2.059538552753298,2.055529438642872,2.051830516480284,2.048407141795246,2.045229642132703,2.04227245630124,2.039513446396408,2.036933343460101,2.034515297449341,2.032244509317719,2.030107928250344,2.028094000980448,2.026192463029111,2.02439416391197,2.022690920036764,2.021075390306274,2.019540970441374,2.018081702818444,2.016692199227827,2.01536757444376,2.014103388880851,2.012895598919432,2.011740513729764,2.010634757624231,2.009575237129236,2.008559112100763,2.00758377031584,2.006646805061683,2.005745995317873,2.004879288188058,2.004044783289142,2.003240718847876,2.00246545929101,2.001717484145235,2.000995378088269,2.000297822014254,1.999623584994947,1.998971517033372,1.998340542520745,1.997729654317695,1.997137908392001,1.99656441895232,1.9960083540253,1.995468931429846,1.994945415107224,1.994437111771185,1.993943367845627,1.993463566661872,1.992997125889857,1.992543495180934,1.992102154002241,1.991672609644666,1.991254395388372,1.990847068811696,1.990450210230133,1.990063421254443,1.989686323456913,1.989318557136561,1.988959780175168,1.988609666975715,1.988267907477211,1.987934206239018,1.98760828158907,1.987289864831177,1.986978699506275,1.986674540703772,1.98637715441862,1.986086316951119,1.985801814345822,1.985523441866611,1.985251003505508,1.984984311522445,1.984723186013989,1.984467454508495,1.9842169515864,1.983971518523567,1.983731002955599,1.983495258562881,1.983264144773466,1.983037526483723,1.982815273795052,1.982597261765499,1.982383370175689,1.982173483307734,1.981967489736486,1.981765282132349,1.981566757074926,1.98137181487629,1.981180359414669,1.980992297975847,1.980807541103897,1.980626002459103,1.98044759868341,1.980272249272972,1.980099876456916,1.979930405082468,1.979763762505385,1.979599878486647,1.979438685093305,1.979280116604845,1.979124109423806,1.978970601990613,1.97881953470284,1.978670849837849,1.978524491479253,1.978380405447044,1.978238539230361,1.978098841924135,1.977961264167731,1.977825758087125,1.977692277239257,1.977560776558934,1.977431212308167,1.977303542027639,1.977177724490363,1.97705371965708,1.976931488634258,1.976810993632842,1.976692197929806,1.976575065830441,1.976459562632923,1.976345654593807,1.976233308895354,1.976122493613727,1.976013177689179,1.975905330896628,1.975798923817945,1.975693927815273,1.975590315005255,1.975488058234331,1.975387131055125,1.975287507703468,1.975189163076561,1.975092072712079,1.974996212767456,1.974901560000834,1.974808091751808,1.974715785923772,1.974624620966374,1.974534575858438,1.974445630092438,1.974357763658034,1.974270957028034,1.974185191143317,1.97410044739898,1.974016707630978,1.973933954103086,1.973852169494539,1.973771336887514,1.973691439756075,1.973612461954371,1.973534387706094,1.973457201593804,1.97338088854883,1.973305433841471,1.973230823071567,1.973157042159139,1.973084077335931,1.973011915136266,1.972940542389353,1.972869946210905,1.972800113995441,1.972731033408893,1.972662692381306,1.972595079099651,1.972528182001313,1.972461989767224,1.972396491315572,1.97233167579576,1.972267532582137,1.972204051268446,1.972141221662041,1.972079033778491,1.972017477836307,1.9719565442518,1.971896223633887,1.97183650677987,1.971777384669986,1.971718848463441,1.971660889493782,1.971603499264494,1.971546669445204,1.971490391867341,1.971434658520252,1.971379461547513,1.971324793243365,1.971270646048573,1.971217012547767,1.971163885465228,1.971111257662308,1.971059122133684,1.971007472004449,1.970956300527388,1.970905601079506,1.970855367159499,1.970805592384888,1.970756270489449,1.970707395320413,1.970658960835814,1.970610961102364,1.970563390292716,1.970516242683134,1.970469512651489,1.970423194674539,1.970377283326135,1.970331773275053,1.970286659282814,1.970241936201586,1.970197598972528,1.970153642623676,1.970110062268119,1.970066853102111,1.970024010403565,1.969981529529937,1.969939405916913,1.969897635076705,1.969856212596136,1.969815134135402,1.969774395426779,1.969733992272367,1.969693920543527,1.969654176178947,1.969614755183279,1.96957565362606,1.969536867640378,1.969498393421176,1.969460227224531,1.969422365366274,1.969384804220565,1.969347540219227,1.96931056984985,1.969273889655713,1.969237496233664,1.969201386233936,1.969165556358341,1.969130003360209,1.969094724042174,1.969059715256568,1.969024973903248,1.968990496929706,1.968956281329497,1.968922324141471,1.96888862244927,1.968855173380275,1.968821974104488,1.968789021834229,1.968756313823275,1.96872384736555,1.968691619795301,1.968659628485478,1.968627870847459,1.968596344330638,1.968565046420838,1.968533974640777,1.968503126548432,1.96847249973709,1.968442091834035,1.968411900500999,1.968381923432324,1.968352158355112,1.968322603028747,1.968293255243809,1.9682641128218,1.968235173614846,1.968206435504479,1.968177896402247,1.968149554247791,1.968121407009639,1.968093452683737,1.968065689294023,1.968038114890476,1.968010727550295,1.967983525376185,1.967956506496835,1.967929669065633,1.967903011261036,1.967876531285784,1.967850227365994,1.967824097752099,1.96779814071702,1.96777235455682,1.967746737589458,1.96772128815552,1.967696004616636,1.967670885355997,1.96764592877781,1.967621133306757,1.967596497387731,1.967572019485812,1.967547698085513,1.967523531690592,1.967499518823987,1.967475658027543,1.967451947860844,1.96742838690241,1.967404973747896,1.967381707011117,1.967358585322662,1.967335607330531,1.967312771699199,1.967290077109902,1.967267522259771,1.967245105862271,1.9672228266464,1.967200683356891,1.967178674753357,1.967156799610906,1.967135056719008,1.967113444882307,1.967091962918967,1.967070609662424,1.967049383958821,1.967028284669271,1.96700731066723,1.966986460840439,1.966965734089312,1.966945129327225,1.966924645480426,1.966904281487692,1.96688403630018,1.966863908881205,1.966843898206027,1.966824003262213,1.966804223048171,1.966784556574924,1.966765002863647,1.966745560947594,1.966726229870825,1.966707008687986,1.966687896464836,1.966668892277168,1.966649995211909,1.966631204365794,1.966612518845476,1.966593937768247,1.966575460260697,1.966557085459192,1.966538812509863,1.966520640568433,1.966502568799356,1.966484596376901,1.966466722483662,1.966448946312109,1.966431267062793,1.966413683945205,1.966396196177421,1.966378802985893,1.966361503605508,1.966344297279263,1.966327183258531,1.966310160802433,1.966293229178,1.966276387660327,1.966259635531834,1.96624297208291,1.966226396611173,1.96620990842178,1.966193506826933,1.966177191146653,1.96616096070721,1.966144814842512,1.966128752893402,1.96611277420701,1.966096878137809,1.966081064046715,1.966065331301084,1.966049679274944,1.966034107348791,1.96601861490905,1.966003201348893,1.965987866067317,1.965972608469543,1.96595742796685,1.965942323976279,1.965927295920825,1.965912343229518,1.965897465336493,1.965882661682298,1.965867931712173,1.965853274877783,1.965838690635618,1.965824178447795,1.965809737781367,1.965795368109377,1.965781068909134,1.965766839663692,1.965752679860909,1.965738588993801,1.965724566559941,1.965710612062303,1.965696725008048,1.965682904909613,1.965669151284025,1.965655463652555,1.965641841541645,1.965628284481746,1.96561479200823,1.96560136366033,1.965587998982121,1.965574697522145,1.965561458832493,1.965548282470029,1.965535167995602,1.965522114974408,1.965509122975211,1.965496191571453,1.965483320339707,1.965470508861421,1.965457756721215,1.965445063507861,1.965432428813933,1.965419852235523,1.965407333372643,1.965394871829,1.965382467211716,1.965370119131532,1.965357827202965,1.965345591043913,1.965333410275558,1.965321284522588,1.96530921341326,1.965297196578955,1.965285233654443,1.965273324277909,1.965261468090348,1.965249664736494,1.965237913863781,1.965226215123068,1.965214568168264,1.96520297265601,1.965191428246585,1.96517993460266,1.965168491390185,1.965157098278077,1.96514575493785,1.965134461044112,1.965123216274375,1.965112020308757,1.965100872830116,1.965089773524335,1.965078722079634,1.96506771818751,1.96505676154098,1.965045851837228,1.965034988774509,1.965024172054953,1.965013401382384,1.965002676463052,1.964991997006686,1.964981362724358,1.964970773330307,1.964960228540767,1.964949728074403,1.964939271652594,1.964928858998676,1.964918489838443,1.964908163899941,1.964897880913611,1.964887640611554,1.964877442729268,1.964867287002968,1.964857173172419,1.964847100978761,1.964837070165153,1.964827080477198,1.964817131662384,1.964807223470845,1.964797355653971,1.964787527965396,1.964777740160917,1.964767991998242,1.964758283237088,1.964748613638882,1.96473898296729,1.964729390987682,1.964719837467365,1.964710322175461,1.964700844882961,1.964691405362848,1.964682003389691,1.964672638739531,1.964663311191152,1.964654020524285,1.96464476652033,1.96463554896313,1.964626367637551,1.964617222330399,1.964608112830176,1.964599038927054,1.96459000041267,1.964580997080471,1.964572028725433,1.964563095144103,1.964554196134604,1.964545331496903,1.964536501031621,1.964527704542399,1.964518941832721,1.964510212708798,1.964501516977906,1.96449285444881,1.964484224931418,1.964475628237757,1.964467064180819,1.964458532574719,1.964450033236037,1.964441565981528,1.964433130630103,1.964424727001707,1.964416354917681,1.964408014200858,1.964399704675264,1.964391426166182,1.964383178500579,1.964374961505735,1.964366775011625,1.964358618848053,1.964350492847493,1.964342396842653,1.96433433066724,1.964326294157507,1.964318287149498,1.964310309481554,1.964302360992111,1.964294441522006,1.964286550912058,1.964278689005174,1.964270855644861,1.964263050675981,1.964255273944357,1.964247525297295,1.96423980458251,1.964232111649636,1.964224446348812,1.964216808531299,1.964209198050037,1.964201614757647,1.964194058509702,1.964186529161154,1.964179026568738,1.964171550590111,1.964164101084058,1.964156677910051,1.96414928092864,1.964141910001738,1.964134564991646,1.964127245762043,1.964119952177542,1.964112684103284,1.964105441405871,1.964098223952762,1.964091031611768,1.964083864252648,1.964076721745035,1.964069603959961,1.964062510769435,1.964055442045975,1.964048397663523,1.964041377496373,1.964034381419924,1.964027409310193,1.964020461044464,1.964013536500602,1.964006635557373,1.963999758094173,1.963992903991351,1.963986073130277,1.963979265392886,1.963972480661885,1.963965718820865,1.963958979754098,1.963952263347061,1.963945569485223,1.963938898055405,1.963932248945353,1.963925622042666,1.963919017236766,1.963912434417294,1.963905873473906,1.963899334298889,1.963892816783322,1.963886320819895,1.963879846302109,1.963873393123624,1.963866961179407,1.963860550364469,1.96385416057533,1.963847791708349,1.963841443660953,1.963835116331489,1.963828809618604,1.963822523421576,1.963816257640332,1.963810012175788,1.963803786929491,1.963797581802925,1.963791396699145,1.963785231521258,1.963779086173088,1.963772960558849,1.963766854584043,1.963760768154545,1.963754701175915,1.963748653555889,1.963742625201193,1.963736616020615,1.963730625922317,1.96372465481583,1.96371870261065,1.963712769217682,1.963706854547669,1.963700958511533,1.963695081022482,1.963689221992274,1.963683381334336,1.963677558962419,1.963671754790821,1.963665968734169,1.963660200708163,1.963654450627961,1.963648718410684,1.963643003972867,1.963637307231893,1.963631628105971,1.96362596651296,1.963620322372449,1.963614695603166,1.963609086125816,1.963603493860356,1.963597918727525,1.963592360649304,1.96358681954693,1.963581295342997,1.963575787960443,1.963570297322383,1.963564823352746,1.963559365975377,1.963553925115332,1.96354850069774,1.963543092647998,1.963537700891878,1.963532325356429,1.963526965968637,1.963521622654841,1.963516295343671,1.963510983963484,1.963505688442389,1.963500408709774,1.963495144694919,1.963489896328059,1.963484663539198,1.96347944625928,1.963474244419515,1.963469057951164,1.963463886786455,1.963458730857544,1.963453590097732,1.963448464439252,1.963443353816474,1.963438258163174,1.963433177413186,1.96342811150182,1.963423060363948,1.963418023934953,1.963413002150688,1.963407994947487,1.963403002261917,1.963398024030809,1.963393060191742,1.963388110681925,1.963383175440079,1.963378254403971,1.963373347512561,1.963368454705131,1.963363575920904,1.963358711099849,1.963353860181862,1.963349023107871,1.963344199818088,1.963339390254302,1.963334594357356,1.963329812069831,1.963325043333151,1.963320288090294,1.963315546283803,1.96331081785693,1.963306102753354,1.963301400916109,1.963296712290221,1.963292036819255,1.963287374448746,1.96328272512281,1.963278088787571,1.963273465388325,1.963268854870648,1.963264257181399,1.963259672266698,1.963255100073927,1.963250540549497,1.963245993640963,1.963241459296655,1.963236937463906,1.963232428091326,1.963227931127267,1.963223446520658,1.963218974220849,1.963214514176917,1.963210066338738,1.963205630656613,1.963201207079807,1.963196795559934,1.963192396046833,1.963188008492531,1.963183632847345,1.963179269063603,1.963174917092599,1.963170576886811,1.963166248398516,1.963161931579945,1.963157626384787,1.963153332765311,1.963149050675332,1.963144780068543,1.963140520898317,1.963136273119319,1.96313203668586,1.963127811552198,1.963123597673331,1.963119395004573,1.963115203500856,1.963111023118292,1.963106853812061,1.96310269553861,1.963098548254125,1.963094411914847,1.963090286478101,1.963086171900118,1.963082068138519,1.963077975150725,1.963073892894077,1.963069821326729,1.963065760406594,1.963061710092025,1.963057670341525,1.963053641113592,1.96304962236743,1.963045614062353,1.96304161615735,1.963037628612051,1.96303365138651,1.963029684440324,1.963025727733987,1.96302178122787,1.963017844882645,1.963013918658877,1.963010002517702,1.963006096420369,1.963002200328076,1.962998314202963,1.962994438006435,1.962990571700326,1.962986715247081,1.962982868609223,1.96297903174909,1.962975204629315,1.96297138721331,1.962967579463789,1.962963781344176,1.962959992818507,1.96295621384948,1.962952444401797,1.962948684439312,1.962944933926038,1.962941192826517,1.962937461105612,1.96293373872788,1.962930025658074,1.962926321861648,1.962922627303985,1.962918941950031,1.96291526576579,1.962911598717497,1.962907940770429,1.962904291890978,1.962900652045276,1.962897021200636,1.962893399322831,1.962889786378867,1.96288618233585,1.962882587161243,1.962879000821817,1.962875423285229,1.962871854519341,1.962868294491166,1.962864743169972,1.962861200522701,1.962857666517768,1.962854141124456,1.96285062431069,1.96284711604455,1.962843616296181,1.962840125033586,1.96283664222667,1.962833167844187,1.962829701855922,1.962826244231686,1.962822794940306,1.962819353953079,1.962815921238665,1.962812496768469,1.962809080512043,1.962805672440256,1.96280227252332,1.962798880732555,1.962795497038498,1.962792121412424,1.962788753824642,1.962785394248297,1.962782042652949,1.962778699010945,1.962775363294558,1.962772035474837,1.962768715523882,1.962765403414219,1.962762099117972,1.962758802606937,1.96275551385459,1.96275223283318,1.962748959514957,1.962745693873547,1.962742435881568,1.962739185512435,1.962735942739366,1.962732707535735,1.962729479874365,1.962726259730403,1.962723047076333,1.962719841886749,1.962716644135394,1.962713453796339,1.962710270843694,1.962707095252689,1.962703926996691,1.962700766051172,1.962697612390174,1.962694465989061,1.962691326822739,1.96268819486605,1.962685070094396,1.962681952482603,1.962678842007375,1.962675738642188,1.962672642364701,1.962669553149456,1.962666470972675,1.962663395810345,1.962660327638605,1.962657266433218,1.962654212171295,1.962651164828477,1.962648124381699,1.962645090807066,1.962642064082424,1.962639044183622,1.962636031087183,1.962633024771529,1.962630025212922,1.962627032388293,1.962624046276263,1.962621066852866,1.962618094096298,1.962615127984606,1.962612168494881,1.962609215604626,1.962606269293082,1.962603329536915,1.962600396315854,1.962597469606076,1.962594549387841,1.962591635638409,1.962588728337026,1.962585827461247,1.962582932991217,1.962580044904497,1.962577163181022,1.962574287798881,1.962571418738158,1.962568555976491,1.962565699494931,1.962562849271582,1.962560005286846,1.962557167519012,1.962554335949098,1.96255151055565,1.962548691319281,1.962545878219846,1.962543071236618,1.962540270350711,1.962537475541042,1.962534686789042,1.962531904074401,1.962529127377754,1.962526356679177,1.96252359195989,1.962520833200381,1.962518080381106,1.96251533348292,1.962512592487205,1.962509857374557,1.962507128125699,1.962504404722967,1.962501687146148,1.962498975378144,1.962496269398841,1.962493569190254,1.962490874734836,1.962488186012559,1.96248550300666,1.962482825697952,1.96248015406892,1.962477488101168,1.962474827776801,1.962472173077998,1.962469523986421,1.962466880485244,1.96246424255604,1.96246161018178,1.962458983344356,1.962456362026773,1.962453746211101,1.962451135880994,1.962448531017958,1.96244593160609,1.962443337627526,1.962440749065186,1.962438165902542,1.962435588122738,1.962433015708679,1.962430448643391,1.962427886910881,1.962425330494259,1.962422779376593,1.96242023354247,1.962417692973658,1.96241515765608,1.962412627571807,1.962410102705266,1.962407583040596,1.962405068561059,1.962402559251168,1.962400055094998,1.962397556076219,1.962395062179826,1.962392573389022,1.962390089689432,1.962387611064603,1.962385137498613,1.962382668977213,1.962380205484013,1.962377747004059,1.962375293521846,1.962372845022397,1.962370401490277,1.96236796291078,1.962365529268491,1.962363100548591,1.962360676735916,1.962358257816147,1.962355843773975,1.962353434594907,1.962351030264322,1.962348630767199,1.962346236089425,1.962343846216507,1.96234146113335,1.962339080826245,1.962336705281012};
		se_multiplier = se_multiplier_ci_005[(int)(*df) - 1];
	}

	(*ci)[0] = mean_val - se_multiplier*standard_error;
	(*ci)[1] = mean_val + se_multiplier*standard_error;

	// h
	*h = !(mu > (*ci)[0] && mu < (*ci)[1]);

	// tstat
	*tstat = (mean_val - mu)/(*sd / sqrt(m->data->size));

	// pval
	double raw_p = tcdf((*tstat), *df);
	*pval = 2.0 * (fmin(raw_p, 1-raw_p));


	if (!destroyM(var_mat))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	return;
}

void ttestM_xy(Matrix *x, Matrix *y, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict tstat, double* restrict df, double* restrict sd)
// For a sample x from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (x) - mean (y) == 0'
// Uses alpha = 0.05
{	
	if (!can_matrices_be_compared(x,y)) {return;}

	Matrix *m = minusM(x, y);
	if (!m)
	{
		return;
	}

	ttestM(m, 0, h, pval, ci, tstat, df, sd);

	if (!destroyM(m))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	return;
}

double normcdf (double x, double mu, double s)
// Get the normal distribution CDF at x with mean mu and standard deviation s
{
	return(0.5*(erfc(-x/sqrt(2))));
}

void ztestM(Matrix* restrict m, double mu, double s, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict z, double* restrict zcrit)
// For a sample m from a normal distribution with unknown mean and known standard deviation, perform a z-test of the null hypothesis 'mean (m) == mu'
// Uses alpha = 0.05
{	
	if (m == NULL)
	{
		return;
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (!(m->ndim == 1) && !(m->ndim == 2 && m->dim[0] == 1))
	{
		fprintf(stderr, "%s: %s (%d): m must not be a row or column vector\n", __FILE__, __func__, __LINE__);
		return;
	}

	// z critical value
	*zcrit = 1.959964;

	// Confidence Interval
	double mean_val = NAN;
	mean(&mean_val, m->data->datatype, m->data->size, m->data->data);

	double standard_error = s / sqrt(m->data->size);

	(*ci)[0] = mean_val - (*zcrit)*standard_error;
	(*ci)[1] = mean_val + (*zcrit)*standard_error;

	// h
	*h = !(mu > (*ci)[0] && mu < (*ci)[1]);

	// z
	*z = (mean_val - mu)/(s / sqrt(m->data->size));

	// pval
	*pval = 2*(1-normcdf(fabs(*z), 0, 1));

	return;
}

double lowerincgamma(double s, double x)
// Estimates the lower incomplete gamma function
{
	double output = 0;
	double step_size = 0.001;
	for (register double num = 0 ; num <= x ; num+=step_size)
	{
		output += step_size*pow(num, s-1)*exp(-num);
	}
	// printf("integral: %.3f\n", (double) output);
	return(output);
}

double chi2cdf(double chisqstat, double df)
// Estimates the Chi-square CDF at chisqstat with df degrees of freedom
{
	return(lowerincgamma(0.5*df, 0.5*chisqstat)/exp(lgamma(0.5*df)));
}

double gamcdf(double x, double a, double b)
// Estamates the gamma CDF at x with the parameters a and b
{
	double output = 0;

	double step_size = 0.001;
	for ( register double num = 0 ; num <= x ; num+=step_size)
	{
		output += step_size*pow(num, a-1)*exp(-num/b);
		if (output > DBL_MAX)
		{
			return(0);
		}
	}
	return(output/(pow(b, a)*exp(lgamma(a))));
}

double gampdf(double x, double a, double b)
// Returns the gamma PDF at x with the parameters a and b
{
	return(pow(x, a-1)*exp(-x/b)/(pow(b, a)*exp(lgamma(a))));
}

double gaminv(double x, double a, double b)
// Estimates the inverse gamma CDF
{
	double eps = 0.00000000000000022204;

	double y = sqrt(eps);
	double y_new = y;
	for (register int loopcount = 0 ; loopcount < 40 ; loopcount++)
	{
		double y_old = y_new;
		double h = (gamcdf(y_old, a, b) - x) / gampdf(y_old, a, b);
		y_new = y_old - h;

		if (y_new <= eps)
		{
			y_new = y_old/10;
			h = y_old - y_new;
		}

		if (fabs(h) < sqrt(eps))
		{
			break;
		}
	}
	return(y_new);
}

void vartestM(Matrix* restrict m, double v, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict chisqstat, double* restrict df)
// For a sample m from a perform a variance test of the null hypothesis 'var (m) == v'
// Uses alpha = 0.05
{	
	if (m == NULL)
	{
		return;
	}

	if (m->data->datatype == COMPLEX)
	{
		fprintf(stderr, "%s: %s (%d): m must not be complex\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (!(m->ndim == 1) && !(m->ndim == 2 && m->dim[0] == 1))
	{
		fprintf(stderr, "%s: %s (%d): m must not be a row or column vector\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Degrees of freedom
	*df = m->data->size - 1;

	if (*df == 0)
	{
		*h = false;
		*pval = NAN;
		(*ci)[0] = NAN;
		(*ci)[1] = NAN;
		*chisqstat = NAN;
		return;
	}

	// Calculate the chi-square statistic
	Matrix *var_mat = samplevarM(m);
	double s_var = ((double *)(var_mat->data->data))[0];
	*chisqstat = (*df)*s_var/v;


	// Confidence Interval
	double chi_sq_025_val = NAN;
	double chi_sq_975_val = NAN;

	if (*df <= 1001)
	{
		double chi_sq_025[] = {5.023886187314885,7.377758908227868,9.34840360449615,11.1432867818778,12.83250199403002,14.44937533544792,16.01276427462932,17.53454613948465,19.02276779864163,20.48317735080739,21.9200492610212,23.33666415864534,24.73560488493154,26.11894804503736,27.48839286344297,28.84535072340476,30.19100912163981,31.52637844038663,32.85232686172971,34.16960690283833,35.47887590572739,36.78071208403556,38.07562725035586,39.36407702660391,40.64646912027522,41.92317009635391,43.19451096615605,44.46079183631775,45.72228580417454,46.97924224367116,48.23188959445194,49.48043774297167,50.72508006628123,51.96599519512191,53.20334854205649,54.43729363181323,55.6679732642611,56.89552053505597,58.12005973468626,59.34170714317118,60.56057173484376,61.7767558053492,62.99035553110197,64.20146146988677,65.41015900999959,66.61652877425048,67.82064698425246,69.02258578966607,70.22241356643453,71.42019518750641,72.61599226908578,73.80986339506075,75.00186432192861,76.19204816625003,77.38046557641913,78.56716489032421,79.75219228029036,80.93559188653636,82.11740594023831,83.29767487717319,84.47643744280907,85.65373078961534,86.82959056728613,88.00405100649751,89.17714499675617,90.34890415884091,91.51935891228953,92.68853853833855,93.85647123868537,95.02318419040617,96.1887035973332,97.35305473816612,98.51626201156779,99.67834897847236,100.8393384018134,101.9992522838617,103.1581119013466,104.3159378385192,105.472750018303,106.6285677316657,107.7834096653345,108.9372939279681,110.0902380748887,111.2422591314698,112.3933736152682,113.5435975569813,114.6929465203061,115.8414356207673,116.9890795435814,118.1358925606155,119.2818885464957,120.4270809939176,121.5714830282068,122.715107421172,123.857966604295,125.0000726812939,126.1414374400959,127.2820723642545,128.4219886438403,129.5611971858366,130.6997086240659,131.8375333286736,132.9746814151919,134.1111627532076,135.2469869746538,136.3821634817458,137.5167014545793,138.6506098584085,139.7838974506205,140.9165727874214,142.0486442302478,143.1801199519188,144.3110079425394,145.4413160151699,146.571051811269,147.7002228059256,148.8288363128863,149.9568994893891,151.0844193408131,152.2114027251515,153.337856357317,154.4637868132866,155.5892005340935,156.7141038296721,157.8385028825647,158.9624037514925,160.0858123748009,161.208734573781,162.3311760558751,163.45314241777,164.5746391483825,165.6956716317431,166.8162451497809,167.9363648850136,169.056035923148,170.1752632555927,171.2940517818888,172.4124063120591,173.5303315688819,174.6478321900906,175.764912730502,176.8815776640779,177.9978313859197,179.1136782142012,180.2291223920411,181.3441680893159,182.4588194044188,183.5730803659631,184.6869549344345,185.8004470037932,186.9135604030285,188.0262988976655,189.1386661912291,190.2506659266632,191.3623016877095,192.4735770002458,193.5844953335853,194.6950601017396,195.8052746646443,196.9151423293515,198.0246663511871,199.1338499348769,200.2426962356411,201.3512083602581,202.4593893680993,203.5672422721363,204.6747700399197,205.7819755945322,206.888861815516,207.9954315397754,209.1016875624556,210.2076326377979,211.3132694799735,212.4186007638943,213.523629126004,214.6283571650476,215.7327874428223,216.8369224849086,217.9407647813829,219.0443167875129,220.1475809244345,221.2505595798129,222.3532551084865,223.4556698330954,224.5578060446947,225.6596660033522,226.7612519387322,227.8625660506653,228.9636105097037,230.0643874576645,231.1648990081592,232.2651472471111,233.3651342332601,234.464861998657,235.5643325491442,236.6635478648277,237.7625099005361,238.8612205862706,239.9596818276443,241.0578955063109,242.1558634803852,243.2535875848528,244.3510696319708,245.448311411661,246.5453146918926,247.6420812190575,248.7386127183373,249.8349108940623,250.9309774300624,252.0268139900108,253.12242221776,254.2178037376717,255.3129601549373,256.4078930558945,257.5026040083355,258.5970945618087,259.6913662479151,260.7854205805977,261.8792590564253,262.9728831548704,264.0662943385812,265.1594940536488,266.2524837298674,267.3452647809914,268.4378386049855,269.5302065842708,270.6223700859653,271.7143304621208,272.8060890499533,273.8976471720708,274.9890061366953,276.0801672378811,277.1711317557287,278.2619009565939,279.3524760932951,280.4428584053131,281.5330491189904,282.6230494477252,283.7128605921611,284.802483740375,285.8919200680597,286.9811707387045,288.0702369037714,289.1591197028684,290.24782026392,291.3363397033341,292.4246791261658,293.5128396262783,294.6008222865014,295.6886281787862,296.7762583643579,297.8637138938652,298.9509958075272,300.0381051352781,301.1250428969088,302.2118101022062,303.2984077510902,304.3848368337481,305.4710983307664,306.5571932132612,307.6431224430054,308.7288869725539,309.8144877453675,310.8999256959332,311.9852017498836,313.0703168241139,314.1552718268974,315.2400676579972,316.3247052087793,317.4091853623203,318.4935089935159,319.577676969186,320.6616901481794,321.7455493814753,322.8292555122842,323.9128093761469,324.9962118010317,326.0794636074302,327.1625656084518,328.2455186099153,329.3283234104416,330.410980801542,331.4934915677076,332.5758564864952,333.6580763286136,334.7401518580073,335.8220838319395,336.9038730010736,337.9855201095535,339.0670258950822,340.1483910890001,341.2296164163616,342.3107025960098,343.3916503406515,344.4724603569299,345.5531333454966,346.6336700010827,347.7140710125681,348.7943370630511,349.8744688299153,350.9544669848966,352.034332194149,353.1140651183093,354.193666412561,355.2731367266964,356.3524767051795,357.4316869872063,358.5107682067652,359.5897209926961,360.6685459687487,361.7472437536399,362.8258149611108,363.9042601999826,364.9825800742108,366.0607751829406,367.1388461205595,368.2167934767505,369.2946178365439,370.3723197803687,371.4498998841028,372.5273587191231,373.6046968523542,374.6819148463174,375.7590132591779,376.8359926447924,377.912853552755,378.9895965284432,380.0662221130631,381.142730843694,382.219123253332,383.2953998709338,384.371561221459,385.4476078259127,386.5235402013863,387.5993588610996,388.6750643144405,389.750657067005,390.8261376206369,391.9015064734667,392.9767641199494,394.0519110509032,395.1269477535458,396.2018747115322,397.2766924049909,398.3514013105591,399.426001901419,400.5004946473325,401.574880014675,402.649158466471,403.723330462426,404.7973964589611,405.8713569092447,406.9452122632258,408.0189629676652,409.0926094661672,410.1661521992112,411.2395916041818,412.3129281153996,413.3861621641508,414.4592941787172,415.532324584405,416.6052538035742,417.6780822556664,418.7508103572337,419.8234385219657,420.8959671607177,421.9683966815375,423.0407274896921,424.112959987694,425.185094575328,426.2571316496765,427.3290716051447,428.4009148334868,429.4726617238297,430.5443126626987,431.6158680340408,432.6873282192495,433.7586935971883,434.8299645442133,435.9011414341982,436.9722246385551,438.0432145262585,439.1141114638673,440.1849158155468,441.2556279430903,442.3262482059411,443.3967769612137,444.4672145637145,445.5375613659633,446.6078177182134,447.6779839684718,448.7480604625198,449.8180475439327,450.8879455540993,451.9577548322415,453.0274757154333,454.09710853862,455.1666536346372,456.2361113342285,457.305481966065,458.3747658567625,459.4439633308999,460.5130747110367,461.5821003177313,462.651040469557,463.7198954831203,464.7886656730774,465.8573513521511,466.925952831147,467.9944704189709,469.0629044226436,470.1312551473183,471.1995228962957,472.26770797104,473.3358106711939,474.4038312945949,475.47177013729,476.5396274935505,477.6074036558871,478.6750989150648,479.742713560117,480.8102478783604,481.8777021554089,482.9450766751879,484.0123717199481,485.0795875702794,486.1467245051242,487.2137828017916,488.2807627359701,489.3476645817414,490.4144886115927,491.4812350964307,492.5479043055932,493.614496506863,494.6810119664793,495.7474509491515,496.8138137180698,497.8801005349187,498.9463116598885,500.0124473516875,501.0785078675532,502.1444934632646,503.2104043931536,504.2762409101163,505.3420032656242,506.4076917097357,507.4733064911072,508.5388478570039,509.6043160533106,510.6697113245428,511.735033913857,512.8002840630616,513.8654620126267,514.9305680016956,515.9956022680936,517.0605650483396,518.1254565776546,519.1902770899729,520.2550268179514,521.3197059929794,522.3843148451882,523.4488536034611,524.5133224954417,525.5777217475448,526.6420515849647,527.7063122316844,528.770503910485,529.8346268429549,530.8986812494979,531.9626673493432,533.0265853605532,534.0904355000324,535.1542179835366,536.2179330256802,537.2815808399457,538.345161638692,539.4086756331619,540.4721230334904,541.5355040487142,542.598818886778,543.6620677545427,544.7252508577947,545.788368401252,546.8514205885732,547.9144076223643,548.9773297041864,550.0401870345642,551.102979812992,552.1657082379422,553.2283725068722,554.2909728162317,555.3535093614695,556.4159823370414,557.4783919364166,558.5407383520852,559.6030217755642,560.6652423974062,561.7274004072038,562.7894959935982,563.851529344285,564.9135006460216,565.9754100846321,567.0372578450163,568.0990441111536,569.1607690661111,570.2224328920494,571.2840357702288,572.3455778810154,573.4070594038875,574.4684805174418,575.5298413993993,576.5911422266107,577.6523831750638,578.7135644198879,579.7746861353605,580.8357484949131,581.8967516711363,582.9576958357859,584.0185811597888,585.0794078132485,586.1401759654498,587.2008857848654,588.2615374391613,589.322131095201,590.3826669190524,591.4431450759919,592.5035657305107,593.5639290463187,594.6242351863511,595.684484312773,596.7446765869834,597.8048121696223,598.8648912205741,599.9249138989734,600.9848803632088,602.0447907709297,603.1046452790491,604.1644440437498,605.2241872204884,606.2838749640005,607.3435074283049,608.4030847667086,609.4626071318115,610.5220746755103,611.5814875490038,612.6408459027971,613.7001498867054,614.7593996498601,615.818595340711,616.8777371070323,617.9368250959263,618.9958594538276,620.0548403265075,621.1137678590778,622.172642195996,623.2314634810679,624.2902318574533,625.3489474676687,626.4076104535923,627.4662209564674,628.5247791169066,629.5832850748963,630.6417389697989,631.7001409403588,632.758491124705,633.8167896603546,634.8750366842183,635.9332323326017,636.9913767412114,638.0494700451569,639.1075123789551,640.165503876534,641.2234446712359,642.2813348958216,643.3391746824732,644.3969641627978,645.4547034678313,646.5123927280418,647.5700320733332,648.6276216330481,649.6851615359709,650.7426519103328,651.8000928838135,652.8574845835448,653.9148271361146,654.9721206675694,656.0293653034181,657.0865611686349,658.1437083876622,659.2008070844145,660.2578573822807,661.314859404128,662.3718132723043,663.428719108642,664.4855770344602,665.5423871705684,666.5991496372692,667.6558645543611,668.7125320411418,669.7691522164112,670.8257251984736,671.8822511051413,672.9387300537376,673.9951621610987,675.0515475435777,676.1078863170459,677.1641785968974,678.2204244980504,679.2766241349509,680.3327776215746,681.3888850714297,682.4449465975606,683.5009623125494,684.5569323285189,685.6128567571358,686.668735709612,687.7245692967082,688.7803576287366,689.8361008155625,690.8917989666082,691.9474521908535,693.0030605968406,694.0586242926742,695.1141433860261,696.169617984136,697.2250481938148,698.2804341214467,699.3357758729916,700.3910735539874,701.4463272695527,702.5015371243888,703.5567032227819,704.6118256686055,705.6669045653233,706.7219400159905,707.7769321232566,708.8318809893675,709.8867867161674,710.9416494051023,711.9964691572201,713.0512460731748,714.1059802532269,715.1606717972473,716.2153208047179,717.2699273747343,718.3244916060082,719.3790135968693,720.433493445267,721.487931248773,722.5423271045826,723.5966811095177,724.6509933600281,725.7052639521937,726.7594929817265,727.8136805439723,728.8678267339133,729.9219316461697,730.9759953750014,732.0300180143098,733.0839996576402,734.1379403981839,735.191840328779,736.2456995419136,737.2995181297266,738.3532961840098,739.4070337962099,740.4607310574305,741.5143880584338,742.5680048896417,743.6215816411385,744.6751184026721,745.7286152636561,746.7820723131716,747.8354896399685,748.8888673324669,749.9422054787605,750.9955041666162,752.0487634834772,753.101983516464,754.1551643523762,755.2083060776946,756.2614087785823,757.314472540886,758.3674974501388,759.4204835915609,760.4734310500611,761.5263399102391,762.5792102563868,763.6320421724888,764.6848357422264,765.7375910489764,766.7903081758141,767.8429872055154,768.8956282205568,769.9482313031176,771.0007965350817,772.0533239980389,773.1058137732861,774.158265941829,775.2106805843836,776.2630577813775,777.3153976129513,778.3677001589605,779.4199654989761,780.4721937122872,781.5243848779007,782.5765390745445,783.6286563806676,784.6807368744421,785.7327806337644,786.7847877362564,787.8367582592674,788.8886922798748,789.9405898748857,790.9924511208382,792.0442760940026,793.0960648703833,794.1478175257193,795.1995341354859,796.2512147748956,797.3028595189004,798.3544684421919,799.4060416192028,800.4575791241091,801.50908103083,802.5605474130298,803.6119783441195,804.6633738972573,805.71473414535,806.7660591610547,807.8173490167793,808.8686037846842,809.9198235366832,810.9710083444452,812.0221582793943,813.0732734127115,814.1243538153373,815.1753995579697,816.2264107110689,817.2773873448551,818.3283295293127,819.3792373341886,820.4301108289958,821.4809500830123,822.5317551652845,823.5825261446259,824.6332630896201,825.6839660686206,826.734635149753,827.7852704009148,828.8358718897776,829.8864396837879,830.9369738501672,831.9874744559143,833.0379415678055,834.0883752523965,835.1387755760226,836.1891426047994,837.2394764046253,838.2897770411812,839.3400445799316,840.3902790861267,841.4404806248018,842.4906492607794,843.5407850586698,844.5908880828724,845.640958397576,846.6909960667601,847.7410011541964,848.7909737234487,849.8409138378744,850.890821560626,851.940696954651,852.9905400826933,854.0403510072941,855.0901297907931,856.1398764953288,857.18959118284,858.2392739150662,859.2889247535485,860.3385437596315,861.3881309944628,862.4376865189946,863.4872103939844,864.5367026799963,865.5861634374007,866.6355927263769,867.6849906069124,868.7343571388044,869.7836923816608,870.8329963949005,871.8822692377553,872.931510969269,873.9807216482999,875.0299013335209,876.07905008342,877.1281679563019,878.1772550102885,879.2263113033187,880.2753368931507,881.3243318373625,882.3732961933518,883.4222300183377,884.4711333693608,885.5200063032848,886.568848876796,887.6176611464058,888.6664431684497,889.7151949990891,890.7639166943122,891.8126083099338,892.8612699015972,893.909901524774,894.9585032347647,896.0070750867012,897.0556171355452,898.1041294360904,899.1526120429623,900.2010650106204,901.2494883933572,902.2978822452997,903.3462466204103,904.3945815724871,905.4428871551647,906.4911634219152,907.5394104260484,908.5876282207129,909.6358168588962,910.6839763934264,911.7321068769719,912.7802083620423,913.8282809009895,914.8763245460083,915.9243393491358,916.9723253622545,918.0202826370903,919.0682112252151,920.1161111780465,921.1639825468488,922.2118253827334,923.2596397366596,924.3074256594352,925.3551832017174,926.4029124140125,927.4506133466775,928.4982860499208,929.5459305738019,930.5935469682325,931.6411352829775,932.6886955676551,933.7362278717372,934.7837322445506,935.8312087352778,936.8786573929561,937.9260782664804,938.9734714046017,940.0208368559289,941.0681746689294,942.1154848919286,943.1627675731121,944.2100227605248,945.2572505020723,946.3044508455214,947.3516238384999,948.3987695284987,949.4458879628705,950.492979188832,951.5400432534633,952.5870802037089,953.6340900863785,954.6810729481468,955.7280288355553,956.774957795011,957.821859872789,958.8687351150312,959.9155835677483,960.9624052768191,962.0092002879923,963.0559686468855,964.1027103989869,965.1494255896556,966.1961142641219,967.2427764674876,968.2894122447273,969.336021640688,970.38260470009,971.4291614675275,972.4756919874691,973.522196304258,974.5686744621129,975.6151265051275,976.6615524772733,977.7079524223969,978.7543263842233,979.8006744063545,980.8469965322712,981.8932928053323,982.9395632687762,983.9858079657206,985.0320269391635,986.0782202319836,987.1243878869401,988.1705299466745,989.2166464537089,990.2627374504493,991.3088029791833,992.3548430820827,993.4008578012021,994.4468471784817,995.4928112557446,996.5387500747007,997.5846636769442,998.6305521039557,999.676415397102,1000.722253597637,1001.768066746702,1002.813854885326,1003.859618054425,1004.905356294805,1005.95106964716,1006.996758152075,1008.042421850022,1009.088060781366,1010.133674986362,1011.179264505154,1012.224829377781,1013.27036964417,1014.315885344144,1015.361376517416,1016.406843203594,1017.452285442178,1018.497703272562,1019.543096734036,1020.588465865783,1021.633810706882,1022.679131296307,1023.724427672928,1024.769699875511,1025.814947942719,1026.860171913113,1027.90537182515,1028.950547717185,1029.995699627472,1031.040827594162,1032.085931655307,1033.131011848857,1034.176068212662,1035.221100784473,1036.266109601939,1037.311094702612,1038.356056123945,1039.400993903291,1040.445908077907,1041.490798684951,1042.535665761483,1043.580509344467,1044.625329470771,1045.670126177165,1046.714899500323,1047.759649476826,1048.804376143156,1049.849079535703,1050.893759690762,1051.938416644532,1052.98305043312,1054.027661092538,1055.072248658706,1056.116813167451,1057.161354654506,1058.205873155514,1059.250368706024,1060.294841341496,1061.339291097295,1062.3837180087,1063.428122110894,1064.472503438975,1065.516862027948,1066.561197912728,1067.605511128142,1068.64980170893,1069.694069689739,1070.73831510513,1071.782537989578,1072.826738377466,1073.870916303093,1074.915071800669,1075.959204904319,1077.00331564808,1078.047404065904,1079.091470191656,1080.135514059117,1081.179535701981,1082.223535153859,1083.267512448275,1084.311467618671,1085.355400698403,1086.399311720745,1087.443200718887,1088.487067725935,1089.530912774914,1090.574735898763};
		double chi_sq_975[] = {0.0009820691171752575,0.05063561596857979,0.215795282623898,0.4844185570879305,0.8312116134866628,1.237344245791203,1.689869180677355,2.179730747252648,2.700389499980359,3.246972780236841,3.815748252236099,4.403788506981703,5.008750511810333,5.628726103039739,6.262137795043251,6.907664353496998,7.564186449577571,8.230746194756676,8.906516481987971,9.59077739226487,10.2828977825228,10.98232073447369,11.68855192245242,12.40115021744444,13.11972002493778,13.84390498200761,14.5733827308217,15.3078605526012,16.04707169536488,16.7907722655666,17.53873858147549,18.29076490728306,19.04666150317512,19.80625293921457,20.56937663074497,21.33588156079905,22.10562716116951,22.87848232873346,23.65432455759302,24.43303917080789,25.21451863811251,25.99866196815238,26.78537416553633,27.57456574445923,28.36615229185985,29.16005407408937,29.9561956819121,30.75450570937293,31.55491646266714,32.35736369565866,33.16178636912694,33.96812643119269,34.77632861669063,35.58634026352955,36.3981111443153,37.21159331171506,38.02674095621744,38.84351027509587,39.66185935151566,40.48174804284184,41.30313787730647,42.12599195828371,42.95027487549903,43.77595262256913,44.60299252032928,45.43136314545968,46.26103426397004,47.09197676914454,47.9241626235869,48.75756480503952,49.59215725568063,50.42791483463046,51.26481327342183,52.10282913421206,52.94193977053284,53.78212329039229,54.62335852155904,55.46562497887296,56.30890283343967,57.15317288357792,57.99841652739944,58.84461573691057,59.69175303353384,60.5398114649554,61.38877458321154,62.23862642393438,63.0893514866826,63.94093471628866,64.7933614851591,65.64661757646894,66.50068916819598,67.35556281794376,68.2112254485064,69.06766433413152,69.92486708744028,70.78282164696648,71.64151626527961,72.50093949765829,73.36108019128368,74.22192747492375,75.0834707490814,75.94569967658153,76.80860417357276,77.67217440092246,78.53640075598371,79.40127386471494,80.26678457413402,81.13292394508949,81.9996832453329,82.86705394287695,83.73502769962528,84.60359636526043,85.47275197137718,86.34248672584951,87.21279300741975,88.08366336049951,88.95509049017205,89.8270672573869,90.69958667433764,91.57264190001453,92.4462262359236,93.32033312196508,94.1949561324639,95.07008897234518,95.94572547344836,96.82185959097448,97.69848540005979,98.5755970924709,99.45318897341626,100.3312554584685,101.2097910705938,102.0887904372826,102.9682482877785,103.8481594504011,104.7285188499583,105.6093215052449,106.4905625266245,107.3722371136897,108.2543405529998,109.136868215891,110.0198155563571,110.9031781089981,111.7869514870331,112.6711313803767,113.5557135537748,114.4406938449983,115.3260681630929,116.211832486682,117.0979828623211,117.9845154029029,118.8714262861087,119.7587117529072,120.6463681060971,121.534391708893,122.422778983552,123.3115264100407,124.2006305247404,125.0900879191897,125.9798952388625,126.8700491819807,127.7605464983605,128.6513839882907,129.5425585014425,130.4340669358087,131.3259062366729,132.2180733956062,133.110565449491,134.003379479572,134.8965126105312,135.789962009589,136.6837248856278,137.57779848834,138.4721801073966,139.3668670716394,140.2618567482922,141.1571465421936,142.0527338950491,142.9486162847015,143.844791224421,144.7412562622119,145.6380089801378,146.5350469936621,147.4323679510067,148.3299695325248,149.2278494500897,150.1260054464987,151.0244352948912,151.9231367981804,152.8221077884996,153.7213461266603,154.6208497016246,155.5206164299889,156.4206442554803,157.3209311484647,158.2214751054656,159.1222741486953,160.0233263255959,160.9246297083908,161.8261823936469,162.7279825018463,163.6300281769679,164.5323175860779,165.4348489189302,166.337620387575,167.240630225976,168.1438766896368,169.0473580552342,169.9510726202609,170.8550187026748,171.7591946405563,172.6635987917729,173.568229533651,174.4730852626549,175.3781643940716,176.2834653617037,177.1889866175674,178.0947266315979,179.0006838913596,179.906856901764,180.8132441847914,181.7198442792204,182.6266557403609,183.5336771397941,184.4409070651168,185.3483441196911,186.2559869223995,187.1638341074039,188.0718843239108,188.9801362359397,189.8885885220967,190.797239875353,191.7060890028269,192.6151346255703,193.5243754783594,194.4338103094896,195.3434378805739,196.2532569663457,197.1632663544646,198.0734648453266,198.9838512518774,199.8944243994296,200.8051831254828,201.7161262795474,202.6272527229718,203.5385613287724,204.4500509814667,205.3617205769102,206.2735690221352,207.1855952351933,208.0977981450005,209.0101766911849,209.9227298239376,210.8354565038657,211.7483557018485,212.6614263988956,213.574667586008,214.4880782640417,215.401657443573,216.315404144767,217.2293173972481,218.1433962399721,219.0576397211016,219.9720468978829,220.8866168365249,221.8013486120803,222.7162413083291,223.6312940176633,224.5465058409743,225.4618758875419,226.3774032749248,227.2930871288538,228.2089265831258,229.1249207795003,230.0410688675972,230.9573700047961,231.873823356138,232.7904280942281,233.7071833991396,234.6240884583206,235.5411424665006,236.4583446256004,237.3756941446418,238.2931902396599,239.210832133616,240.1286190563127,241.0465502443094,241.9646249408399,242.8828423957309,243.8012018653219,244.719702612386,245.6383439060531,246.5571250217323,247.4760452410378,248.3951038517135,249.3143001475613,250.2336334283682,251.1531029998361,252.072708173512,252.9924482667193,253.9123226024897,254.8323305094977,255.7524713219939,256.6727443797408,257.5931490279493,258.5136846172156,259.4343505034598,260.3551460478645,261.2760706168154,262.1971235818415,263.1183043195575,264.0396122116058,264.9610466446003,265.8826070100704,266.8042927044064,267.7261031288045,268.6480376892145,269.5700957962862,270.492276865318,271.4145803162058,272.3370055733919,273.2595520658162,274.1822192268663,275.1050064943298,276.0279133103464,276.9509391213605,277.8740833780755,278.7973455354075,279.7207250524407,280.6442213923824,281.5678340225196,282.4915624141755,283.4154060426666,284.3393643872612,285.2634369311372,286.1876231613417,287.11192256875,288.0363346480263,288.9608588975838,289.8854948195464,290.8102419197099,291.7350997075044,292.6600676959574,293.5851454016561,294.5103323447119,295.4356280487239,296.3610320407442,297.2865438512424,298.2121630140712,299.1378890664327,300.0637215488444,300.9896600051064,301.9157039822683,302.8418530305973,303.7681067035462,304.694464557721,305.6209261528517,306.5474910517597,307.4741588203285,308.4009290274735,309.3278012451125,310.2547750481365,311.1818500143806,312.1090257245962,313.0363017624219,313.9636777143568,314.8911531697324,315.8187277206853,316.7464009621314,317.6741724917383,318.6020419099003,319.5300088197116,320.4580728269417,321.3862335400096,322.3144905699592,323.2428435304346,324.1712920376563,325.0998357103964,326.0284741699555,326.9572070401392,327.886033947235,328.814954519989,329.743968389584,330.6730751896166,331.6022745560754,332.5315661273193,333.4609495440557,334.3904244493192,335.3199904884509,336.2496473090771,337.1793945610892,338.1092318966232,339.0391589700392,339.9691754379022,340.8992809589623,341.8294751941348,342.7597578064816,343.6901284611919,344.6205868255635,345.5511325689845,346.4817653629146,347.4124848808672,348.3432907983911,349.2741827930535,350.2051605444219,351.1362237340467,352.0673720454445,352.998605164081,353.929922777354,354.8613245745768,355.7928102469625,356.7243794876068,357.6560319914727,358.5877674553743,359.5195855779612,360.4514860597028,361.3834686028734,362.3155329115366,363.2476786915304,364.1799056504523,365.1122134976446,366.0446019441803,366.9770707028478,367.9096194881375,368.8422480162274,369.774956004969,370.707743173874,371.6406092440999,372.5735539384375,373.5065769812966,374.4396780986932,375.3728570182368,376.3061134691165,377.2394471820894,378.1728578894667,379.1063453251024,380.0399092243795,380.9735493241992,381.9072653629676,382.8410570805842,383.7749242184298,384.708866519355,385.6428837276683,386.5769755891242,387.5111418509126,388.445382261647,389.3796965713531,390.3140845314581,391.2485458947798,392.1830804155152,393.1176878492301,394.0523679528486,394.9871204846423,395.92194520422,396.8568418725172,397.7918102517861,398.7268501055852,399.6619611987696,400.5971432974807,401.5323961691371,402.4677195824234,403.4031133072824,404.3385771149041,405.2741107777169,406.2097140693783,407.1453867647652,408.0811286399651,409.016939472267,409.9528190401521,410.8887671232853,411.8247835025057,412.7608679598185,413.6970202783866,414.6332402425205,415.5695276376717,416.5058822504232,417.4423038684812,418.3787922806671,419.3153472769094,420.251968648235,421.188656186762,422.1254096856912,423.0622289392984,423.9991137429265,424.9360638929778,425.8730791869064,426.8101594232106,427.7473044014252,428.6845139221143,429.621787786864,430.5591257982745,431.4965277599537,432.433993476509,433.3715227535415,434.3091153976374,435.2467712163625,436.1844900182541,437.122271612815,438.0601158105063,438.9980224227401,439.9359912618747,440.8740221412053,441.8121148749598,442.7502692782909,443.6884851672706,444.6267623588826,445.5651006710175,446.5034999224653,447.4419599329095,448.3804805229217,449.3190615139542,450.2577027283351,451.1964039892615,452.135165120794,453.073985947851,454.012866296202,454.9518059924629,455.8908048640892,456.8298627393715,457.7689794474284,458.7081548182025,459.6473886824535,460.5866808717537,461.5260312184818,462.4654395558179,463.4049057177383,464.3444295390098,465.2840108551846,466.223649502595,467.1633453183486,468.1030981403227,469.0429078071596,469.982774158261,470.9226970337837,471.8626762746342,472.802711722464,473.7428032196643,474.6829506093619,475.6231537354138,476.5634124424027,477.5037265756321,478.4440959811219,479.3845205056036,480.3249999965158,481.2655343019995,482.2061232708935,483.1467667527303,484.0874645977316,485.0282166568029,485.9690227815311,486.9098828241781,487.8507966376778,488.7917640756314,489.732784992303,490.6738592426159,491.6149866821482,492.5561671671282,493.497400554431,494.4386867015741,495.3800254667135,496.3214167086397,497.2628602867732,498.2043560611617,499.145903892475,500.0875036420021,501.0291551716464,501.9708583439229,502.9126130219536,503.8544190694641,504.7962763507802,505.7381847308234,506.6801440751081,507.6221542497373,508.5642151213993,509.5063265573643,510.4484884254807,511.3907005941709,512.3329629324294,513.2752753098175,514.2176375964614,515.1600496630474,516.1025113808201,517.0450226215777,517.9875832576689,518.9301931619905,519.872852207983,520.8155602696279,521.7583172214443,522.701122938486,523.6439772963378,524.5868801711131,525.5298314394495,526.4728309785073,527.4158786659652,528.3589743800177,529.3021179993718,530.2453094032447,531.1885484713599,532.1318350839446,533.0751691217268,534.0185504659327,534.9619789982829,535.9054546009903,536.8489771567573,537.7925465487721,538.7361626607067,539.6798253767139,540.6235345814245,541.5672901599443,542.5110919978517,543.454939981195,544.3988339964892,545.3427739307141,546.286759671311,547.23079110618,548.1748681236785,549.1189906126167,550.0631584622571,551.0073715623105,551.9516298029337,552.8959330747273,553.8402812687335,554.7846742764326,555.7291119897413,556.6735943010103,557.6181211030213,558.5626922889851,559.5073077525388,560.451967387744,561.3966710890838,562.3414187514608,563.2862102701947,564.23104554102,565.1759244600834,566.1208469239423,567.0658128295618,568.0108220743124,568.9558745559686,569.9009701727058,570.8461088230986,571.7912904061182,572.7365148211304,573.6817819678943,574.6270917465583,575.5724440576601,576.5178388021227,577.4632758812536,578.4087551967422,579.3542766506579,580.2998401454477,581.2454455839345,582.1910928693152,583.1367819051583,584.0825125954019,585.0282848443525,585.9740985566818,586.9199536374257,587.865849991982,588.8117875261084,589.7577661459211,590.7037857578919,591.6498462688471,592.5959475859657,593.5420896167772,594.4882722691594,595.4344954513374,596.3807590718815,597.3270630397045,598.2734072640613,599.2197916545463,600.1662161210917,601.1126805739658,602.0591849237712,603.0057290814432,603.9523129582482,604.8989364657812,605.845599515965,606.7923020210485,607.7390438936038,608.6858250465262,609.6326453930316,610.5795048466543,611.5264033212474,612.4733407309782,613.4203169903295,614.3673320140961,615.3143857173837,616.2614780156078,617.2086088244916,618.1557780600644,619.1029856386604,620.0502314769167,620.9975154917723,621.9448376004663,622.8921977205362,623.8395957698166,624.7870316664379,625.7345053288245,626.6820166756934,627.6295656260525,628.5771520992,629.5247760147216,630.4724372924904,631.4201358526647,632.3678716156867,633.315644502281,634.2634544334538,635.2113013304906,636.1591851149553,637.1071057086895,638.055063033809,639.0030570127053,639.951087568042,640.8991546227543,641.8472581000483,642.795397923398,643.7435740165461,644.6917863035003,645.6400347085346,646.5883191561858,647.5366395712534,648.484995878798,649.4333880041405,650.3818158728595,651.3302794107917,652.2787785440297,653.227313198921,654.1758833020666,655.1244887803201,656.073129560786,657.0218055708195,657.9705167380238,658.9192629902501,659.8680442555961,660.8168604624046,661.7657115392626,662.7145974150001,663.6635180186883,664.6124732796395,665.561463127406,666.5104874917773,667.459546302781,668.40863949068,669.3577669859734,670.3069287193929,671.2561246219036,672.2053546247023,673.1546186592162,674.1039166571023,675.0532485502456,676.0026142707586,676.9520137509805,677.9014469234754,678.8509137210317,679.8004140766608,680.7499479235966,681.6995151952938,682.649115825427,683.5987497478905,684.548416896796,685.4981172064726,686.4478506114652,687.3976170465342,688.3474164466534,689.2972487470101,690.2471138830039,691.1970117902448,692.146942404554,693.0969056619607,694.0469014987034,694.9969298512275,695.9469906561844,696.8970838504316,697.8472093710307,698.7973671552469,699.747557140548,700.6977792646038,701.6480334652848,702.598319680661,703.5486378490023,704.4989879087757,705.4493697986463,706.3997834574751,707.3502288243186,708.3007058384283,709.2512144392484,710.2017545664173,711.1523261597646,712.1029291593111,713.0535635052681,714.0042291380365,714.9549259982053,715.9056540265518,716.85641316404,717.80720335182,718.7580245312274,719.7088766437822,720.6597596311881,721.6106734353314,722.5616179982811,723.5125932622869,724.4635991697793,725.4146356633681,726.3657026858426,727.3168001801698,728.2679280894944,729.2190863571369,730.1702749265947,731.1214937415397,732.0727427458178,733.0240218834492,733.9753310986262,734.9266703357134,735.8780395392467,736.8294386539328,737.780867624648,738.7323263964374,739.6838149145151,740.6353331242625,741.586880971228,742.5384584011261,743.4900653598371,744.4417017934061,745.3933676480419,746.345062870117,747.2967874061667,748.2485412028882,749.2003242071401,750.1521363659416,751.1039776264718,752.0558479360692,753.0077472422311,753.9596754926126,754.9116326350261,755.8636186174406,756.8156333879808,757.7676768949276,758.7197490867158,759.6718499119341,760.6239793193255,761.5761372577847,762.5283236763595,763.4805385242482,764.4327817508007,765.3850533055167,766.3373531380457,767.2896811981859,768.2420374358844,769.1944218012356,770.1468342444812,771.0992747160092,772.051743166354,773.0042395461948,773.9567638063558,774.9093158978054,775.8618957716556,776.8145033791609,777.7671386717187,778.7198016008681,779.6724921182891,780.6252101758026,781.5779557253695,782.5307287190902,783.483529109204,784.4363568480888,785.3892118882601,786.3420941823704,787.2950036832092,788.2479403437022,789.2009041169106,790.1538949560306,791.106912814393,792.0599576454623,793.013029402837,793.9661280402478,794.9192535115582,795.8724057707633,796.8255847719896,797.7787904694948,798.7320228176659,799.6852817710204,800.6385672842049,801.5918793119946,802.5452178092926,803.4985827311303,804.4519740326662,805.4053916691847,806.3588355960974,807.3123057689407,808.2658021433773,809.219324675193,810.1728733202996,811.1264480347312,812.0800487746459,813.0336754963241,813.9873281561687,814.9410067107046,815.8947111165777,816.8484413305545,817.8021973095226,818.7559790104889,819.7097863905798,820.6636194070412,821.6174780172366,822.5713621786483,823.5252718488759,824.4792069856363,825.4331675467629,826.3871534902052,827.3411647740287,828.2952013564146,829.2492631956582,830.2033502501698,831.1574624784736,832.1115998392077,833.0657622911222,834.0199497930815,834.9741623040611,835.9283997831487,836.8826621895438,837.8369494825564,838.7912616216072,839.745598566227,840.6999602760569,841.6543467108465,842.6087578304546,843.5631935948489,844.5176539641047,845.4721388984054,846.4266483580409,847.3811823034091,848.3357406950131,849.2903234934631,850.2449306594747,851.1995621538687,852.1542179375704,853.10889797161,854.0636022171218,855.0183306353439,855.9730831876171,856.9278598353861,857.8826605401973,858.8374852636998,859.792333967644,860.7472066138822,861.7021031643676,862.6570235811536,863.6119678263946,864.5669358623446,865.521927651357,866.4769431558846,867.4319823384789,868.38704516179,869.3421315885657,870.2972415816521,871.2523751039923,872.2075321186264,873.1627125886914,874.1179164774204,875.0731437481423,876.0283943642822,876.9836682893599,877.93896548699,878.8942859208825,879.8496295548406,880.804996352762,881.7603862786377,882.7157992965523,883.6712353706824,884.626694465298,885.5821765447608,886.5376815735247,887.4932095161348,888.4487603372274,889.4043340015299,890.3599304738603,891.3155497191266,892.2711917023264,893.2268563885477,894.1825437429669,895.1382537308499,896.0939863175508,897.0497414685127,898.0055191492656,898.961319325428,899.9171419627058,900.8729870268913,901.8288544838641,902.78474429959,903.7406564401209,904.6965908715945,905.6525475602344,906.608526472349,907.5645275743316,908.5205508326603,909.4765962138977,910.4326636846897,911.388753211767,912.3448647619425,913.3009983021135,914.257153799259,915.2133312204414};

		chi_sq_025_val = chi_sq_025[(int)(*df) - 1];
		chi_sq_975_val = chi_sq_975[(int)(*df) - 1];
	}
	else
	{
		chi_sq_025_val = gaminv(1-0.05/2, (*df)/2, 2);
		chi_sq_975_val = gaminv(0.05/2, (*df)/2, 2);
	}

	(*ci)[0] = (*df)*s_var / chi_sq_025_val;
	(*ci)[1] = (*df)*s_var / chi_sq_975_val;

	// pval
	double raw_p = chi2cdf(*chisqstat, *df);
	*pval = 2.0 * (fmin(raw_p, 1-raw_p));

	// h
	*h = (*pval < 0.05);

	if (!destroyM(var_mat))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	return;
}

void normfitM(Matrix* restrict m, void* restrict mu, void* restrict sigma)
// Returns estimations of normal distribution parameters. mu is set to the mean, sigma is set to the standard deviation
{
	if (m == NULL)
	{
		return;
	}

	if (!(m->ndim == 1) && !(m->ndim == 2 && m->dim[0] == 1))
	{
		fprintf(stderr, "%s: %s (%d): m must not be a row or column vector\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (mu == NULL || sigma == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Invalid address for mu or sigma\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Calculate the mean
	mean(mu, m->data->datatype, m->data->size, m->data->data);

	// Calculate the variance
	Matrix *var_mat = samplevarM(m);
	if (var_mat->data->datatype == COMPLEX)
	{
		*(double complex *)sigma = sqrt(((double complex *)(var_mat->data->data))[0]);
	}
	else
	{
		*(double *)sigma = sqrt(((double *)(var_mat->data->data))[0]);
	}

	if (!destroyM(var_mat))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	return;
}

void unifitM(Matrix* restrict m, void* restrict a, void* restrict b)
// Returns estimations of uniform distribution parameters. a is set to the minimum, b is set to the maximum
{
	if (m == NULL)
	{
		return;
	}

	if (!(m->ndim == 1) && !(m->ndim == 2 && m->dim[0] == 1))
	{
		fprintf(stderr, "%s: %s (%d): m must not be a row or column vector\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (a == NULL || b == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Invalid address for a or b\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Calculate the min
	Matrix *min_mat = minM(m);

	switch(min_mat->data->datatype)
	{
		case INTEGER: ;
			*(int *)a = ((int *)(min_mat->data->data))[0];
			break;

		break;

		case DOUBLE: ;
			*(double *)a = ((double *)(min_mat->data->data))[0];
			break;

		case COMPLEX: ;
			*(double complex *)a = ((double complex *)(min_mat->data->data))[0];
			break;
	}

	if (!destroyM(min_mat))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Calculate the max
	Matrix *max_mat = maxM(m);
	switch(max_mat->data->datatype)
	{
		case INTEGER: ;
			*(int *)b = ((int *)(max_mat->data->data))[0];
			break;

		break;

		case DOUBLE: ;
			*(double *)b = ((double *)(max_mat->data->data))[0];
			break;

		case COMPLEX: ;
			*(double complex *)b = ((double complex *)(max_mat->data->data))[0];
			break;
	}

	if (!destroyM(max_mat))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	return;
}



