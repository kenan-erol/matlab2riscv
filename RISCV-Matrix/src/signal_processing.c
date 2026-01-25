//
//  signal_processing.c
//  Octave-C
//
//  Created by Zach Taylor on 12/4/22
//

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

/*
* =========================================
* 
*  Matrix signal processing operations
* 
* =========================================
*/

double * hamming(int n)
// Return the coefficients of a hamming window
{
	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	double *output = NULL;
	output = malloc(n*sizeof(*output));

	if (n == 1)
	{
		output[0] = 1;
		return(output);
	}

	for (register int i = 0 ; i < n ; i++)
	{
		output[i] = 0.54 - 0.46*cos(2*M_PI*i/(n-1));
	}

	return(output);
}

double * periodichamming(int n)
// Return the coefficients of a periodic hamming window of size n+1, with last one removed
{
	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	double *output = NULL;
	output = malloc(n*sizeof(*output));

	if (n == 1)
	{
		output[0] = 1;
		return(output);
	}

	for (register int i = 0 ; i < n ; i++)
	{
		output[i] = 0.54 - 0.46*cos(2*M_PI*i/n);
	}

	return(output);
}

double * hanning(int n)
// Return the coefficients of a hanning window
{
	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	double *output = NULL;
	output = malloc(n*sizeof(*output));

	if (n == 1)
	{
		output[0] = 1;
		return(output);
	}

	for (register int i = 0 ; i < n ; i++)
	{
		output[i] = 0.5 * (1 - cos(2*M_PI*i/(n-1)));
	}

	return(output);
}

double * periodichanning(int n)
// Return the coefficients of a periodic hanning window of size n+1, with last one removed
{
	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	double *output = NULL;
	output = malloc(n*sizeof(*output));

	if (n == 1)
	{
		output[0] = 1;
		return(output);
	}

	for (register int i = 0 ; i < n ; i++)
	{
		output[i] = 0.5 * (1 - cos(2*M_PI*i/n));
	}

	return(output);
}

Matrix * dftM(Matrix* restrict m)
// If m is a row vector, returns a row vector with the discrete fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are discrete fourier transformed
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Fourier transform only supported for 1D and 2D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;


	Matrix *output = createM(m->ndim, m_dim, COMPLEX);
	double complex *output_data_data = ((double complex *)(output->data->data));

	// Row or column vector
	if ((m->ndim == 2 && m_dim[0] == 1) || m->ndim == 1)
	{
		// Casting
		double complex *input = NULL;
		switch(m->data->datatype)
		{
			case INTEGER: ;
				input = i_to_c(m);
				break;

			case DOUBLE: ;
				input = d_to_c(m);
				break;

			case COMPLEX: ;
				input = c_to_c(m);
				break;
		}
		
		// Discrete Fourier Transform
		for (register int i = 0 ; i < m_data_size ; i++)
		{
			output_data_data[i] = 0;
			for (register int j = 0 ; j < m_data_size ; j++)
			{
				output_data_data[i] += input[j] * exp(-2*M_PI*I/(m_data_size)*i*j);
			}
		}

		free(input);
		return(output);
	}

	// Matrix
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((int *)(m_data_data))[j*(m_dim[1])+col] * exp(-2*M_PI*I/(m_dim[0])*i*j);
					}
				}
			}
			break;

		case DOUBLE: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((double *)(m_data_data))[j*(m_dim[1])+col] * exp(-2*M_PI*I/(m_dim[0])*i*j);
					}
				}
			}
			break;

		case COMPLEX: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((double complex *)(m_data_data))[j*(m_dim[1])+col] * exp(-2*M_PI*I/(m_dim[0])*i*j);
					}
				}
			}
			break;
	}

	return(output);
}


Matrix * idftM(Matrix* restrict m)
// If m is a row vector, returns a row vector with the inverse discrete fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse discrete fourier transformed
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Fourier transform only supported for 1D and 2D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;


	Matrix *output = createM(m->ndim, m_dim, COMPLEX);
	double complex *output_data_data = ((double complex *)(output->data->data));

	// Row or column vector
	if ((m->ndim == 2 && m_dim[0] == 1) || m->ndim == 1)
	{
		// Casting
		double complex *input = NULL;
		switch(m->data->datatype)
		{
			case INTEGER: ;
				input = i_to_c(m);
				break;

			case DOUBLE: ;
				input = d_to_c(m);
				break;

			case COMPLEX: ;
				input = c_to_c(m);
				break;
		}
		
		// Discrete Inverse Fourier Transform
		for (register int i = 0 ; i < m_data_size ; i++)
		{
			output_data_data[i] = 0;
			for (register int j = 0 ; j < m_data_size ; j++)
			{
				output_data_data[i] += input[j] * exp(2*M_PI*I/(m_data_size)*i*j);
			}
			output_data_data[i] /= m_data_size;
		}

		free(input);
		return(output);
	}

	// Matrix
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((int *)(m_data_data))[j*(m_dim[1])+col] * exp(2*M_PI*I/(m_dim[0])*i*j);
					}
					output_data_data[i*(m_dim[1])+col] /= m_dim[0];
				}
			}
			break;

		case DOUBLE: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((double *)(m_data_data))[j*(m_dim[1])+col] * exp(2*M_PI*I/(m_dim[0])*i*j);
					}
					output_data_data[i*(m_dim[1])+col] /= m_dim[0];
				}
			}
			break;

		case COMPLEX: ;
			for (register int col = 0 ; col < m_dim[1] ; col++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					output_data_data[i*(m_dim[1])+col] = 0;
					for (register int j = 0 ; j < m_dim[0] ; j++)
					{
						output_data_data[i*(m_dim[1])+col] += ((double complex *)(m_data_data))[j*(m_dim[1])+col] * exp(2*M_PI*I/(m_dim[0])*i*j);
					}
					output_data_data[i*(m_dim[1])+col] /= m_dim[0];
				}
			}
			break;
	}

	return(output);
}

Matrix * fftM(Matrix* restrict m)
// If m is a row vector, returns a row vector with the fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are fast fourier transformed
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Fourier transform only supported for 1D and 2D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;


	Matrix *output = createM(m->ndim, m_dim, COMPLEX);
	double complex *output_data_data = ((double complex *)(output->data->data));

	// Row or column vector
	if ((m->ndim == 2 && m_dim[0] == 1) || m->ndim == 1)
	{
		switch(m->data->datatype)
		{
			case INTEGER: ;
				double complex *input_i = NULL;
				input_i = malloc((m_data_size)*sizeof(*input_i));

				fftw_plan p_i = fftw_plan_dft_1d(m_data_size, input_i, output_data_data, FFTW_FORWARD, FFTW_ESTIMATE);

				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_i[i] = (double complex)((int *)(m_data_data))[i];
				}

		        fftw_execute(p_i);

		        fftw_destroy_plan(p_i);
				free(input_i);
				break;

			case DOUBLE: ;
				double complex *input_d = NULL;
				input_d = malloc((m_data_size)*sizeof(*input_d));

				fftw_plan p_d = fftw_plan_dft_1d(m_data_size, input_d, output_data_data, FFTW_FORWARD, FFTW_ESTIMATE);

				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_d[i] = (double complex)((double *)(m_data_data))[i];
				}

		        fftw_execute(p_d);

		        fftw_destroy_plan(p_d);
				free(input_d);
				break;

			case COMPLEX: ;
				double complex *input_c = NULL;
				input_c = malloc((m_data_size)*sizeof(*input_c));

				fftw_plan p_c = fftw_plan_dft_1d(m_data_size, input_c, output_data_data, FFTW_FORWARD, FFTW_ESTIMATE);

				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_c[i] = (double complex)((double complex *)(m_data_data))[i];
				}

		        fftw_execute(p_c);

		        fftw_destroy_plan(p_c);
				free(input_c);
				break;
		}
				
		return(output);
	}

	// Matrix
	double complex *input = malloc((m_data_size)*sizeof(*input));
    fftw_plan p = fftw_plan_many_dft(1, &(m_dim[0]), m_dim[1], input, &(m_dim[0]), m_dim[1], 1, output_data_data, &(m_dim[0]), m_dim[1], 1, FFTW_FORWARD, FFTW_ESTIMATE);
    
    switch(m->data->datatype)
    {
    	case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((int *)(m_data_data))[i];
		    }
	    	break;

    	case DOUBLE: ;
	    	for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((double *)(m_data_data))[i];
		    }
	    	break;

    	case COMPLEX: ;
	    	for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((double complex *)(m_data_data))[i];
		    }
	    	break;
    }
   
    fftw_execute(p);
    fftw_destroy_plan(p);
    free(input);

	return(output);
}


Matrix * ifftM(Matrix* restrict m)
// If m is a row vector, returns a row vector with the inverse fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse fast fourier transformed
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Fourier transform only supported for 1D and 2D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	int *m_dim = m->dim;

	Matrix *output = createM(m->ndim, m_dim, COMPLEX);
	double complex *output_data_data = ((double complex *)(output->data->data));

	// Row or column vector
	if ((m->ndim == 2 && m_dim[0] == 1) || m->ndim == 1)
	{
		switch(m->data->datatype)
		{
			case INTEGER: ;
				double complex *input_i = NULL;
				input_i = malloc((m_data_size)*sizeof(*input_i));

				fftw_plan p_i = fftw_plan_dft_1d(m_data_size, input_i, output_data_data, FFTW_BACKWARD, FFTW_ESTIMATE);

				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_i[i] = (double complex)((int *)(m_data_data))[i];
				}

		        fftw_execute(p_i);

		        fftw_destroy_plan(p_i);
				free(input_i);
				break;

			case DOUBLE: ;
				double complex *input_d = NULL;
				input_d = malloc((m_data_size)*sizeof(*input_d));

				fftw_plan p_d = fftw_plan_dft_1d(m_data_size, input_d, output_data_data, FFTW_BACKWARD, FFTW_ESTIMATE);
				
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_d[i] = (double complex)((double *)(m_data_data))[i];
				}

		        fftw_execute(p_d);

		        fftw_destroy_plan(p_d);
				free(input_d);
				break;

			case COMPLEX: ;
				double complex *input_c = NULL;
				input_c = malloc((m_data_size)*sizeof(*input_c));

				fftw_plan p_c = fftw_plan_dft_1d(m_data_size, input_c, output_data_data, FFTW_BACKWARD, FFTW_ESTIMATE);

				for (register int i = 0 ; i < m_data_size ; i++)
				{
					input_c[i] = (double complex)((double complex *)(m_data_data))[i];
				}

		        fftw_execute(p_c);

		        fftw_destroy_plan(p_c);
				free(input_c);
				break;
		}    
        for (register int i = 0 ; i < m_data_size ; i++)
        {
            output_data_data[i] /= m_data_size;
        }

		return(output);
	}

	// Matrix
	double complex *input = malloc((m_data_size)*sizeof(*input));
    fftw_plan p = fftw_plan_many_dft(1, &(m_dim[0]), m_dim[1], input, &(m_dim[0]), m_dim[1], 1, output_data_data, &(m_dim[0]), m_dim[1], 1, FFTW_BACKWARD, FFTW_ESTIMATE);
    
    switch(m->data->datatype)
    {
    	case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((int *)(m_data_data))[i];
		    }
	    	break;

    	case DOUBLE: ;
	    	for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((double *)(m_data_data))[i];
		    }
	    	break;

    	case COMPLEX: ;
	    	for (register int i = 0 ; i < m_data_size ; i++)
		    {
		    	input[i] = (double complex)((double complex *)(m_data_data))[i];
		    }
	    	break;
    }
   
    fftw_execute(p);
    fftw_destroy_plan(p);
    free(input);
    
    for (register int i = 0 ; i < m_data_size ; i++)
    {
        output_data_data[i] /= m_dim[0];
    }

	return(output);
}

Matrix * fftV(Matrix* restrict m, int n)
// m = a row or column vector
// n = a positive integer
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!(m->ndim == 1 || (m->ndim == 2 && m->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Only row and vectors are supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	if (n == m_data_size)
	{
		return(fftM(m));
	}

	Matrix *output = NULL;

	if (m->ndim == 1)
	{
		int output_dims[] = {n};
		output = createM(m->ndim, output_dims, COMPLEX);
	}
	else
	{
		int output_dims[] = {1, n};
		output = createM(m->ndim, output_dims, COMPLEX);
	}

	double complex *output_data_data = ((double complex *)(output->data->data));

	switch(m->data->datatype)
	{
		case INTEGER: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((int *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((int *)(m_data_data))[i];
				}
			}

			break;

		case DOUBLE: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((double *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((double *)(m_data_data))[i];
				}
			}

			break;

		case COMPLEX: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((double complex *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((double complex *)(m_data_data))[i];
				}
			}

			break;
	}

	Matrix *final = fftM(output);
	if (!destroyM(output))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(final);
}

Matrix * ifftV(Matrix* restrict m, int n)
// If m is a row vector, returns a row vector with the inverse fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse fast fourier transformed
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	if (n < 1)
	{
		fprintf(stderr, "%s: %s (%d): Invalid n\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!(m->ndim == 1 || (m->ndim == 2 && m->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Only row and vectors are supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	if (n == m_data_size)
	{
		return(ifftM(m));
	}

	Matrix *output = NULL;

	if (m->ndim == 1)
	{
		int output_dims[] = {n};
		output = createM(m->ndim, output_dims, COMPLEX);
	}
	else
	{
		int output_dims[] = {1, n};
		output = createM(m->ndim, output_dims, COMPLEX);
	}

	double complex *output_data_data = ((double complex *)(output->data->data));

	switch(m->data->datatype)
	{
		case INTEGER: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((int *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((int *)(m_data_data))[i];
				}
			}

			break;

		case DOUBLE: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((double *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((double *)(m_data_data))[i];
				}
			}

			break;

		case COMPLEX: ;
			if (n > m_data_size)
			{
				for (register int i = 0 ; i < m_data_size ; i++)
				{
					output_data_data[i] = (double complex)((double complex *)(m_data_data))[i];
				}

				for (register int i = m_data_size ; i < n ; i++)
				{
					output_data_data[i] = 0;
				}
			}

			else
			{
				for (register int i = 0 ; i < n ; i++)
				{
					output_data_data[i] = (double complex)((double complex *)(m_data_data))[i];
				}
			}

			break;
	}

	Matrix *final = ifftM(output);
	if (!destroyM(output))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(final);
}

Matrix * stftV(Matrix* restrict m, int win_size, int inc, int num_coef, int win_type)
// Compute the short-time Fourier transform of the vector m with num_coef coefficients by applying a window of win_size data points and an increment of inc points
// Translated into C from: https://searchcode.com/codesearch/view/20327749/
{
	// Error checking
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	if (!(m->ndim == 1 || (m->ndim == 2 && m->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Only row and vectors are supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (win_type < 1 || win_type > 3)
	{
		fprintf(stderr, "%s: %s (%d): Invalid win_type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (win_size > m_data_size)
	{
		fprintf(stderr, "%s: %s (%d): Invalid win_size\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (inc <= 0)
	{
		fprintf(stderr, "%s: %s (%d): Invalid inc\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (num_coef <= 0)
	{
		fprintf(stderr, "%s: %s (%d): Invalid num_coef\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int ncoef = 2*num_coef;
	if (win_size > ncoef)
	{
		win_size = ncoef;
		printf ("stft: window size adjusted to %d\n", win_size);
	}

	double *window = NULL;
	switch(win_type)
	{
		case HANNING: ;
			window = hanning(win_size);
			break;

		case HAMMING: ;
			window = hamming(win_size);
			break;

		case RECTANGLE: ;
			window = malloc(win_size*sizeof(*window));
			for (register int i = 0 ; i < win_size ; i++)
			{
				window[i] = 1.0;
			}
			break;
	}

	int num_win = floor((m_data_size - win_size)/inc);


	double complex one = 1;
	int output_dim_t[] = {num_win + 1, ncoef};
	Matrix *output_raw_t = zerosM(2, output_dim_t);
	Matrix *output_t = scaleM(output_raw_t, &one, COMPLEX);

	int output_t_data_size = output_t->data->size;
	double complex *output_t_data_data = ((double complex *)(output_t->data->data));

	if (!destroyM(output_raw_t))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	int start = 0;
	
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int row = 0 ; row < num_win+1 ; row++)
			{
				for (register int j = 0 ; j < win_size ; j++)
				{
					if (row*num_coef + j >= output_t_data_size)
					{
						break;
					}
					output_t_data_data[row*ncoef + j] = (double complex) ((int *)(m_data_data))[start+j] * (double complex) window[j];
				}
				start += inc;
			}
			break;

		case DOUBLE: ;
			for (register int row = 0 ; row < num_win+1 ; row++)
			{
				for (register int j = 0 ; j < win_size ; j++)
				{
					if (row*num_coef + j >= output_t_data_size)
					{
						break;
					}
					output_t_data_data[row*ncoef + j] = (double complex) ((double *)(m_data_data))[start+j] * (double complex) window[j];
				}
				start += inc;
			}
			break;

		case COMPLEX: ;
			for (register int row = 0 ; row < num_win+1 ; row++)
			{
				for (register int j = 0 ; j < win_size ; j++)
				{
					if (row*num_coef + j >= output_t_data_size)
					{
						break;
					}
					output_t_data_data[row*ncoef + j] = (double complex) ((double complex *)(m_data_data))[start+j] * (double complex) window[j];
				}
				start += inc;
			}
			break;
	}
	free(window);

	Matrix *output = transposeM(output_t);
	if (!destroyM(output_t))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	Matrix *fourier_transformed = fftM(output);
	if (!destroyM(output))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	Matrix *abs_fourier = absM(fourier_transformed);
	double *abs_fourier_data_data = ((double *)(abs_fourier->data->data));
	if (!destroyM(fourier_transformed))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	int final_dim[] = {num_coef, num_win + 1};
	Matrix *final = createM(2, final_dim, DOUBLE);
	int final_data_size = final->data->size;
	double *final_data_data = ((double *)(final->data->data));

	for (register int i = 0 ; i < final_data_size ; i++)
	{
		final_data_data[i] = abs_fourier_data_data[i];
	}
	if (!destroyM(abs_fourier))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(final);
}

Matrix * filterM_internal(Matrix* restrict b, Matrix* restrict a, Matrix* restrict x, Matrix* restrict *state)
{
	// First element of a must be nonzero
	switch(a->data->datatype)
	{
		case INTEGER: ;
			if (almostEqual_ii(0, ((int *)(a->data->data))[0]))
			{
				fprintf(stderr, "%s: %s (%d): First element of a must be nonzero\n", __FILE__, __func__, __LINE__);
				return(NULL);
			}
			break;

		case DOUBLE: ;
			if (almostEqual_id(0, ((double *)(a->data->data))[0]))
			{
				fprintf(stderr, "%s: %s (%d): First element of a must be nonzero\n", __FILE__, __func__, __LINE__);
				return(NULL);
			}
			break;

		case COMPLEX: ;
			if (almostEqual_ic(0, ((double complex *)(a->data->data))[0]))
			{
				fprintf(stderr, "%s: %s (%d): First element of a must be nonzero\n", __FILE__, __func__, __LINE__);
				return(NULL);
			}
			break;
	}

	// Initialize si if NULL
	bool return_state = true;
	Matrix *si = NULL;

	if (state == NULL || *state == NULL)
	{
		return_state = false;
		int state_dims[] = {((int) fmax(a->data->size, b->data->size))-1};
		si = zerosM(1, state_dims);
	}
	else
	{
		si = *state;

		// If si is passed in, must be a writeable vector
		if (!(si->ndim == 1 || (si->ndim == 2 && si->dim[0] == 1)) || si->data->readOnly > 0)
		{
			fprintf(stderr, "%s: %s (%d): Invalid si\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}

		// If si passed in, must already be of the correct length
		if (si->data->size != ((int) fmax(a->data->size, b->data->size))-1)
		{
			fprintf(stderr, "%s: %s (%d): si must be a vector of length max(len(a), len(b))-1 \n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	int si_data_size = si->data->size;
	void *si_data_data = NULL;

	// Determine output type
	int output_type = DOUBLE;
	if (b->data->datatype == COMPLEX || a->data->datatype == COMPLEX || x->data->datatype == COMPLEX || si->data->datatype == COMPLEX)
	{
		output_type = COMPLEX;
	}

	// Initialize the output
	Matrix *output = createM(x->ndim, x->dim, output_type);
	void *output_data_data = output->data->data;
	int output_data_size = output->data->size;

	Matrix *a_norm = NULL;
	int a_norm_data_size = -1;
	void *a_norm_data_data = NULL;

	Matrix *b_norm = NULL;
	int b_norm_data_size = -1;
	void *b_norm_data_data = NULL;

	Matrix *x_norm = NULL;	
	
	switch(output->data->datatype)
	{	
		case DOUBLE: ;

			// Cast si if necessary -- IN PLACE
			Matrix *si_d = NULL;
			if (si->data->datatype == INTEGER)
			{
				double one = 1;
				si_d = scaleM(si, &one, DOUBLE);
				if (!destroyM(si))
				{
					fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					return(NULL);
				}
				si = si_d;
			}
			si_data_data = si->data->data;

			// Create normalized & casted copies of a and b
			double first_a_d = 0;
			switch(a->data->datatype)
			{
				case INTEGER: ;
					first_a_d = 1 / (double) ((int *)(a->data->data))[0];
					break;

				case DOUBLE: ;
					first_a_d = 1 / (double) ((double *)(a->data->data))[0];
					break;
			}

			a_norm = scaleM(a, &first_a_d, DOUBLE);
			b_norm = scaleM(b, &first_a_d, DOUBLE);

			a_norm_data_size = a_norm->data->size;
			b_norm_data_size = b_norm->data->size;

			a_norm_data_data = a_norm->data->data;
			b_norm_data_data = b_norm->data->data;

			// Create casted copy of x
			double double_one = 1;
			x_norm = scaleM(x, &double_one, DOUBLE);
			double *x_norm_data_data_d = ((double *)(x_norm->data->data));

			// Extract b_0_d
			double b_0_d = ((double *)(b_norm_data_data))[0];

			for (register int i = 0 ; i < output_data_size ; i++)
			{
				double si_0 = ((double *)(si_data_data))[0];
				double x_i =  x_norm_data_data_d[i];

				// Apply the filter
				((double *)(output_data_data))[i] = 0;
				((double *)(output_data_data))[i] = b_0_d * x_i + si_0;

				// Update the state vector
				for (register int j = 1 ; j < si_data_size ; j++)
				{
					double b_j = 0;
					if (j < b_norm_data_size)
					{
						b_j = ((double *)(b_norm_data_data))[j];
					}

					double a_j = 0;
					if (j < a_norm_data_size)
					{
						a_j = ((double *)(a_norm_data_data))[j];
					}
					double si_j = ((double *)(si_data_data))[j];
				
				
					((double *)(si_data_data))[j-1] = b_j * x_i + si_j - a_j * ((double *)(output_data_data))[i];
				}

				double b_last = 0;
				if (si_data_size < b_norm_data_size)
				{
					b_last = ((double *)(b_norm_data_data))[si_data_size];
				}

				double a_last = 0;
				if (si_data_size < a_norm_data_size)
				{
					a_last = ((double *)(a_norm_data_data))[si_data_size];
				}
			
				((double *)(si_data_data))[si_data_size - 1] = b_last * x_i - a_last * ((double *)(output_data_data))[i];

			}
			break;

		case COMPLEX: ;

			// Cast si if necessary
			Matrix *si_c = NULL;
			if (si->data->datatype != COMPLEX)
			{
				double complex one = 1;
				si_c = scaleM(si, &one, COMPLEX);
				if (!destroyM(si))
				{
					fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					return(NULL);
				}
				si = si_c;
			}
			si_data_data = si->data->data;

			// Create normalized & casted copies of a and b
			double complex first_a_c = 0;
			switch(a->data->datatype)
			{
				case INTEGER: ;
					first_a_c = 1 / (double complex) ((int *)(a->data->data))[0];
					break;

				case DOUBLE: ;
					first_a_c = 1 / (double complex) ((double *)(a->data->data))[0];
					break;

				case COMPLEX: ;
					first_a_c = 1 / (double complex) ((double complex *)(a->data->data))[0];
					break;
			}
			b_norm = scaleM(b, &first_a_c, COMPLEX);
			a_norm = scaleM(a, &first_a_c, COMPLEX);

			a_norm_data_size = a_norm->data->size;
			b_norm_data_size = b_norm->data->size;

			a_norm_data_data = a_norm->data->data;
			b_norm_data_data = b_norm->data->data;

			// Create casted copy of x
			double complex complex_one = 1;
			x_norm = scaleM(x, &complex_one, COMPLEX);
			double complex *x_norm_data_data_c = ((double complex *)(x_norm->data->data));

			// Extract b_0
			double complex b_0_c = ((double complex *)(b_norm_data_data))[0];

			for (register int i = 0 ; i < output_data_size ; i++)
			{
				double complex si_0 = ((double complex *)(si_data_data))[0];
				double complex x_i =  x_norm_data_data_c[i];

				// Apply the filter
				((double complex *)(output_data_data))[i] = 0;
				((double complex *)(output_data_data))[i] = b_0_c * x_i + si_0;

				// Update the state vector
				for (register int j = 1 ; j < si_data_size ; j++)
				{
					double complex b_j = 0;
					if (j < b_norm_data_size)
					{
						b_j = ((double complex *)(b_norm_data_data))[j];
					}

					double complex a_j = 0;
					if (j < a_norm_data_size)
					{
						a_j = ((double complex *)(a_norm_data_data))[j];
					}
					double complex si_j = ((double complex *)(si_data_data))[j];
				
					((double complex *)(si_data_data))[j-1] = b_j * x_i + si_j - a_j * ((double complex *)(output_data_data))[i];
				}

				double complex b_last = 0;
				if (si_data_size < b_norm_data_size)
				{
					b_last = ((double complex *)(b_norm_data_data))[si_data_size];
				}

				double complex a_last = 0;
				if (si_data_size < a_norm_data_size)
				{
					a_last = ((double complex *)(a_norm_data_data))[si_data_size];
				}
			
				((double complex *)(si_data_data))[si_data_size - 1] = b_last * x_i - a_last * ((double complex *)(output_data_data))[i];

			}
			break;
	}

	// Clean up
	if (!destroyM(a_norm))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	if (!destroyM(b_norm))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	if (!destroyM(x_norm))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	if (!return_state)
	{
		if (!destroyM(si))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		}
	}
	*state = si;

	return(output);
}

Matrix * filterM(Matrix *b, Matrix *a, Matrix *x, Matrix **state)
// Apply a 1-D digital filter to the data x
{
	// Error checking
	if (b == NULL)
	{
		return(NULL);
	}

	// b must be a vector
	if (!(b->ndim == 1 || (b->ndim == 2 && b->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Invalid b\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (a == NULL)
	{
		return(NULL);
	}

	// a must be a vector
	if (!(a->ndim == 1 || (a->ndim == 2 && a->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Invalid a\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (x == NULL)
	{
		return(NULL);
	}

	// x must be a vector
	if (!(x->ndim == 1 || (x->ndim == 2 && x->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Invalid x\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// Overlap optimization: b == a
	if (a == b && (state == NULL || *state == NULL))
	{
		return(full_duplicateM(x));
	}

	// Overlap optimization: a == x
	if (a == x && (state == NULL || *state == NULL))
	{
		Matrix *output = createM(a->ndim, a->dim, b->data->datatype);
		void *output_data_data = output->data->data;
		void *b_data_data = b->data->data;

		int a_data_size = a->data->size;
		int b_data_size = b->data->size;

		switch(b->data->datatype)
		{
			case INTEGER: ;
				for (register int i = 0 ; i < a_data_size ; i++)
				{
					if (i > b_data_size)
					{
						((int *)(output_data_data))[i] = 0;
					}
					else
					{
						((int *)(output_data_data))[i] = ((int *)(b_data_data))[i];
					}
				}
				break;

			case DOUBLE: ;
				for (register int i = 0 ; i < a_data_size ; i++)
				{
					if (i > b_data_size)
					{
						((double *)(output_data_data))[i] = 0;
					}
					else
					{
						((double *)(output_data_data))[i] = ((double *)(b_data_data))[i];
					}
				}
				break;

			case COMPLEX: ;
				for (register int i = 0 ; i < a_data_size ; i++)
				{
					if (i > b_data_size)
					{
						((double complex *)(output_data_data))[i] = 0;
					}
					else
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(b_data_data))[i];
					}
				}
				break;
		}

		return(output);
	}

	// Ensure separate pointers to satisfy the restrict keyword
	Matrix *b_restricted = b;
	bool free_b = false;

	Matrix *a_restricted = a;
	bool free_a = false;

	Matrix *x_restricted = x;
	bool free_x = false;

	if (b == a || b == x || b == *state)
	{
		b_restricted = full_duplicateM(b);
		free_b = true;
	}

	if (a == x || a == *state)
	{
		a_restricted = full_duplicateM(a);
		free_a = true;
	}

	if (x == *state)
	{
		x_restricted = full_duplicateM(x);
		free_x = true;
	}

	Matrix *output = filterM_internal(b_restricted, a_restricted, x_restricted, state);

	// Cleanup if necessary
	if (free_b && !destroyM(b_restricted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	if (free_a && !destroyM(a_restricted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	if (free_x && !destroyM(x_restricted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(output);
}

// Internal functions for cross correlation
double internal_xcorrM_ii(int m, int *x, int x_l, int *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double) conj((double complex) internal_xcorrM_ii(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double first = internal_xcorrM_ii(0, x, x_l, x, x_l, 0);
			double second = internal_xcorrM_ii(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double internal_xcorrM_id(int m, int *x, int x_l, double *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double) conj((double complex) internal_xcorrM_di(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double first = internal_xcorrM_ii(0, x, x_l, x, x_l, 0);
			double second = internal_xcorrM_dd(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double complex internal_xcorrM_ic(int m, int *x, int x_l, double complex *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double complex) conj((double complex) internal_xcorrM_ci(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double complex output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double complex) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double complex first = (double complex) internal_xcorrM_ii(0, x, x_l, x, x_l, 0);
			double complex second = internal_xcorrM_cc(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double internal_xcorrM_di(int m, double *x, int x_l, int *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double) conj((double complex) internal_xcorrM_id(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double first = internal_xcorrM_dd(0, x, x_l, x, x_l, 0);
			double second = internal_xcorrM_ii(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double internal_xcorrM_dd(int m, double *x, int x_l, double *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double) conj((double complex) internal_xcorrM_dd(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double first = internal_xcorrM_dd(0, x, x_l, x, x_l, 0);
			double second = internal_xcorrM_dd(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double complex internal_xcorrM_dc(int m, double *x, int x_l, double complex *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double complex) conj((double complex) internal_xcorrM_cd(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double complex output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double complex) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double complex first = (double complex) internal_xcorrM_dd(0, x, x_l, x, x_l, 0);
			double complex second = internal_xcorrM_cc(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double complex internal_xcorrM_ci(int m, double complex *x, int x_l, int *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double complex) conj((double complex) internal_xcorrM_ic(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double complex output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double complex) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double complex first = internal_xcorrM_cc(0, x, x_l, x, x_l, 0);
			double complex second = (double complex) internal_xcorrM_ii(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double complex internal_xcorrM_cd(int m, double complex *x, int x_l, double *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double complex) conj((double complex) internal_xcorrM_dc(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double complex output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double complex) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double complex first = internal_xcorrM_cc(0, x, x_l, x, x_l, 0);
			double complex second = (double complex) internal_xcorrM_dd(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

double complex internal_xcorrM_cc(int m, double complex *x, int x_l, double complex *y, int y_l, int mode)
{
	if (m < 0)
	{
		return((double complex) conj((double complex) internal_xcorrM_cc(-m, y, y_l, x, x_l, mode)));
	}

	int N = (int) fmax(x_l, y_l);

	double complex output = 0;
	for (register int n = 0 ; n < N-m ; n++)
	{
		if (n >= y_l)
		{
			continue;
		}
		if (m+n >= x_l)
		{
			continue;
		}
		output += x[n+m] * (double complex) (conj((double complex) (y[n])));
	}

	switch(mode)
	{
		case 0: ; // NONE
			break;

		case 1: ; // UNBIASED
			output/=(N - (int)fabs(m));
			if (N == (int)fabs(m)){return(0);}
			break;

		case 2: ; // BIASED
			output/=N;
			break;

		case 3: ; // COEFF
			double complex first = internal_xcorrM_cc(0, x, x_l, x, x_l, 0);
			double complex second = internal_xcorrM_cc(0, y, y_l, y, y_l, 0);
			output/=(sqrt(first * second));
			break;
	}

	return(output);
}

Matrix * xcorrM_internal(Matrix* restrict x, Matrix* restrict y, int maxlag, char *scale)
{
	int x_data_size = x->data->size;
	int y_data_size = y->data->size;

	void *x_data_data = x->data->data;
	void *y_data_data = y->data->data;


	int post_processing_mode = 0;
	if (x_data_size == y_data_size)
	{
		if (!strcmp(scale, "none"))
		{
			post_processing_mode = 0;
		}
		else if (!strcmp(scale, "unbiased"))
		{
			post_processing_mode = 1;
		}
		else if (!strcmp(scale, "biased"))
		{
			post_processing_mode = 2;
		}
		else if (!strcmp(scale, "coeff"))
		{
			post_processing_mode = 3;
		}
		else
		{
			fprintf(stderr, "%s: %s (%d): Invalid scale\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	else
	{
		if (strcmp(scale, "none"))
		{
			fprintf(stderr, "%s: %s (%d): scale must be 'none' if length(x) != length(y)\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}
	}
	
	// Initialize output
	if (maxlag == 0)
	{
		maxlag = ((int) fmax(x_data_size, y_data_size)) - 1;
	}
	int output_data_size = 2*maxlag + 1;

	int output_type = DOUBLE;
	if (x->data->datatype == COMPLEX || y->data->datatype == COMPLEX)
	{
		output_type = COMPLEX;
	}

	Matrix *output = NULL;

	if (x->ndim == 2)
	{
		int output_dims[] = {1, output_data_size};
		output = createM(2, output_dims, output_type);
	}
	else
	{
		int output_dims[] = {output_data_size};
		output = createM(1, output_dims, output_type);
	}

	void *output_data_data = output->data->data;

	switch(x->data->datatype)
	{
		case INTEGER: ;
			switch(y->data->datatype)
			{
				case INTEGER: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double *)(output_data_data))[i] = internal_xcorrM_ii(i - maxlag, ((int *)(x_data_data)), x_data_size, ((int *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case DOUBLE: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double *)(output_data_data))[i] = internal_xcorrM_id(i - maxlag, ((int *)(x_data_data)), x_data_size, ((double *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case COMPLEX: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = internal_xcorrM_ic(i - maxlag, ((int *)(x_data_data)), x_data_size, ((double complex *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;
			}
			break;

		case DOUBLE: ;
			switch(y->data->datatype)
			{
				case INTEGER: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double *)(output_data_data))[i] = internal_xcorrM_di(i - maxlag, ((double *)(x_data_data)), x_data_size, ((int *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case DOUBLE: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double *)(output_data_data))[i] = internal_xcorrM_dd(i - maxlag, ((double *)(x_data_data)), x_data_size, ((double *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case COMPLEX: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = internal_xcorrM_dc(i - maxlag, ((double *)(x_data_data)), x_data_size, ((double complex *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;
			}
			break;

		case COMPLEX: ;
			switch(y->data->datatype)
			{
				case INTEGER: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = internal_xcorrM_ci(i - maxlag, ((double complex *)(x_data_data)), x_data_size, ((int *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case DOUBLE: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = internal_xcorrM_cd(i - maxlag, ((double complex *)(x_data_data)), x_data_size, ((double *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;

				case COMPLEX: ;
					for (register int i = 0 ; i < output_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = internal_xcorrM_cc(i - maxlag, ((double complex *)(x_data_data)), x_data_size, ((double complex *)(y_data_data)), y_data_size, post_processing_mode);
					}

					break;
			}
			break;
	}

	return(output);
}

Matrix * xcorrM(Matrix *x, Matrix *y, int maxlag, char *scale)
// Estimates cross correlation
{
	// Error checking
	if (maxlag < 0)
	{
		fprintf(stderr, "%s: %s (%d): Invalid maxlag\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (x == NULL)
	{
		return(NULL);
	}

	if (!(x->ndim == 1 || (x->ndim == 2 && x->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Only row and vectors are supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!(y->ndim == 1 || (y->ndim == 2 && y->dim[0] == 1)))
	{
		fprintf(stderr, "%s: %s (%d): Only row and vectors are supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	bool y_flag = false;
	Matrix *y_restricted = y;
	if (x == y || y == NULL)
	{
		y_restricted = full_duplicateM(x);
		y_flag = true;
	}

	Matrix *output = xcorrM_internal(x, y_restricted, maxlag, scale);

	// Cleanup
	if (y_flag && !destroyM(y_restricted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(output);
}
