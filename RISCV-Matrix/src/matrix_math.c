//
//  matrix_math.c
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

// TODO: Vectorize
Matrix * transpose_internal(Matrix* restrict m, bool conjugate)
// Transposes a 2D matrix
// Returns another matrix
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Transposing 3D and 4D matrices is not supported\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int output_ndim = 2;
	if (m->ndim == 2 && m->dim[0] == 1) 
	{
		output_ndim = 1;
	}

	int dims[2] = {m->dim[1], m->dim[0]};
	Matrix *output = createM(output_ndim, dims, m->data->datatype);
	
	int counter = 0;
	int *m_dim = m->dim;
	void *m_data_data = m->data->data;
	void *output_data_data = output->data->data;

	
	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int j = 0 ; j < m_dim[1] ; j++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					((int *)(output_data_data))[counter++] = (((int *)(m_data_data))[i*(m_dim[1]) + j]);
				}
			}
			break;

		case DOUBLE: ;
			for (register int j = 0 ; j < m_dim[1] ; j++)
			{
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					((double *)(output_data_data))[counter++] = (((double *)(m_data_data))[i*(m_dim[1]) + j]);
				}
			}
			break;

		case COMPLEX: ;
			if(conjugate)
			{
				for (register int j = 0 ; j < m_dim[1] ; j++)
				{
					for (register int i = 0 ; i < m_dim[0] ; i++)
					{
						((double complex *)(output_data_data))[counter++] = conj(((double complex *)(m_data_data))[i*(m_dim[1]) + j]);
					}
				}
			}
			else
			{
				for (register int j = 0 ; j < m_dim[1] ; j++)
				{
					for (register int i = 0 ; i < m_dim[0] ; i++)
					{
						((double complex *)(output_data_data))[counter++] = (((double complex *)(m_data_data))[i*(m_dim[1]) + j]);
					}
				}
			}
			break;
	}

	return(output);
}

Matrix * ctransposeM(Matrix* restrict m)
// Transposes a 2D matrix and gets the elementwise complex conjugate 
{
	return(transpose_internal(m, true));
}

Matrix * transposeM(Matrix* restrict m)
{
	return(transpose_internal(m, false));
}

void determinantM(Matrix* restrict m, void* restrict output)
// Finds the determinant of a matrix m
// Calculates the determinant by LU decomposition
{
	if (m == NULL)
	{
		output = NULL;
		return;
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Calculating the determinant of 3D and 4D matrices is not supported\n", __FILE__, __func__, __LINE__);
		output = NULL;
		return;
	}

	if (m->dim[0] != m->dim[1])
	{
		fprintf(stderr, "%s: %s (%d): Non-square matrix\n", __FILE__, __func__, __LINE__);
		output = NULL;
		return;
	}

	int *pivotArray = NULL;
	pivotArray = malloc((m->dim[0])*sizeof(*pivotArray));
	int success = 0;

	int *m_dim = m->dim;
	int m_data_size = m->data->size;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			double *m_data_double = i_to_d(m);

			Matrix *tmp_int = createM(m->ndim, m->dim, DOUBLE);
			writeM(tmp_int, m->data->size, m_data_double);
			free(m_data_double);

			double *tmp_int_data_data = ((double *)(tmp_int->data->data));

			// Factors m into P*L*U, where:
			// L is a lower triangular matrix with unit diagonal elements
			// U is an upper triangular matrix,
			// P is a permutation matrix
			LAPACK_dgetrf(
			    &(m->dim[0]), 
			    &(m->dim[0]),
			    tmp_int_data_data, 
			    &(m->dim[0]), 
			    pivotArray,
			    &success );

			// The absolute value of the determinant is the product of the diagonal entries
			int determinant_int = 1;
			for (register int i = 0 ; i < m_data_size ; i += (m_dim[0])+1)
			{
				determinant_int *= (int) (tmp_int_data_data[i]);
			}

			for (register int i = 0 ; i < m_dim[0] ; i++ )
			{
				if (i+1 != pivotArray[i])
				{
					determinant_int *= -1;
				}
			}

			*(int *) output = determinant_int;

			if (!destroyM(tmp_int))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			break;

		case DOUBLE: ;
			Matrix *tmp_double = full_duplicateM(m);
			double *tmp_double_data_data = ((double *)(tmp_double->data->data));

			// Factors m into P*L*U, where:
			// L is a lower triangular matrix with unit diagonal elements
			// U is an upper triangular matrix,
			// P is a permutation matrix
			LAPACK_dgetrf(
			    &(m->dim[0]), 
			    &(m->dim[0]),
			    tmp_double_data_data, 
			    &(m->dim[0]), 
			    pivotArray,
			    &success );

			// The absolute value of the determinant is the product of the diagonal entries
			double determinant_double = 1;
			for (register int i = 0 ; i < m_data_size ; i += (m_dim[0])+1)
			{
				determinant_double *= (tmp_double_data_data[i]);
			}

			for (register int i = 0 ; i < m_dim[0] ; i++ )
			{
				if (i+1 != pivotArray[i])
				{
					determinant_double *= -1;
				}
			}

			*(double *) output = determinant_double;

			if (!destroyM(tmp_double))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);

				
			}
			break;

		case COMPLEX: ;
			Matrix *tmp_complex = full_duplicateM(m);
			double complex *tmp_complex_data_data = ((double complex *)(tmp_complex->data->data));


			// Factors m into P*L*U, where:
			// L is a lower triangular matrix with unit diagonal elements
			// U is an upper triangular matrix,
			// P is a permutation matrix
			LAPACK_zgetrf(
			    &(m->dim[0]), 
			    &(m->dim[0]),
			    tmp_complex_data_data, 
			    &(m->dim[0]), 
			    pivotArray,
			    &success );

			// The absolute value of the determinant is the product of the diagonal entries
			double complex determinant_complex = 1;
			for (register int i = 0 ; i < m_data_size ; i += (m_dim[0])+1)
			{
				determinant_complex *= (tmp_complex_data_data[i]);
			}

			for (register int i = 0 ; i < m_dim[0] ; i++ )
			{
				if (i+1 != pivotArray[i])
				{
					determinant_complex *= -1;
				}
			}

			*(double complex *) output = determinant_complex;

			if (!destroyM(tmp_complex))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			break;

	}

	free(pivotArray);
	return;
}

Matrix * invertM(Matrix* restrict m)
// Inverts a matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Cannot invert 3D or 4D matrices\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (m->dim[0] != m->dim[1])
	{
		fprintf(stderr, "%s: %s (%d): Non-square matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;

	int *pivotArray = NULL;
	pivotArray = malloc((m->dim[0])*sizeof(*pivotArray));
	int success = 0;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			
			double *m_data_double = i_to_d(m);

			output = createM(m->ndim, m->dim, DOUBLE);
			writeM(output, output->data->size, m_data_double);
			free(m_data_double);

		case DOUBLE: ;
			if (m->data->datatype == DOUBLE)
			{
				output = full_duplicateM(m);
			}

		    double *WORK = NULL;
		    WORK = malloc((m->data->size)*sizeof(*WORK));

		    double *output_data_data_double = (double *) output->data->data;

		    LAPACK_dgetrf(
			    &(m->dim[0]), 
			    &(m->dim[0]),
			    output_data_data_double, 
			    &(m->dim[0]), 
			    pivotArray,
			    &success );

			if (success < 0)
			{
				fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_dgetrf was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(pivotArray);
		    	free(WORK);
		    	destroyM(output);
				return(NULL);
			}
			if (success > 0)
			{
				fprintf(stderr, "%s: %s (%d): Singular matrix\n", __FILE__, __func__, __LINE__);
				free(pivotArray);
		    	free(WORK);
		    	destroyM(output);
				return(NULL);
			}
		    
			LAPACK_dgetri(
			    &(m->dim[0]),
			    output_data_data_double, 
			    &(m->dim[0]),
			    pivotArray,
			    WORK, 
			    &(output->data->size),
			    &success );

			free(pivotArray);
		    free(WORK);

			if (success < 0)
			{
				fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_dgetri was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				destroyM(output);
				return(NULL);
			}
			if (success > 0)
			{
				fprintf(stderr, "%s: %s (%d): Singular matrix\n", __FILE__, __func__, __LINE__);
				destroyM(output);
				return(NULL);
			}

			break;

		case COMPLEX: ;
			output = full_duplicateM(m);

		    double complex *WORK_complex = NULL;
		    WORK_complex = malloc((m->data->size)*sizeof(*WORK_complex));

		    double complex *output_data_data_complex = (double complex *) output->data->data;

		    LAPACK_zgetrf(
			    &(m->dim[0]), 
			    &(m->dim[0]),
			    output_data_data_complex, 
			    &(m->dim[0]), 
			    pivotArray,
			    &success );

			if (success < 0)
			{
				fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_zgetrf was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(pivotArray);
		    	free(WORK_complex);
				return(NULL);
			}
			if (success > 0)
			{
				fprintf(stderr, "%s: %s (%d): Singular matrix\n", __FILE__, __func__, __LINE__);
				free(pivotArray);
		    	free(WORK_complex);
				return(NULL);
			}
		    
			LAPACK_zgetri(
			    &(m->dim[0]),
			    output_data_data_complex, 
			    &(m->dim[0]),
			    pivotArray,
			    WORK_complex, 
			    &(output->data->size),
			    &success );

			free(pivotArray);
		    free(WORK_complex);

			if (success < 0)
			{
				fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_zgetri was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				return(NULL);
			}
			if (success > 0)
			{
				fprintf(stderr, "%s: %s (%d): Singular matrix\n", __FILE__, __func__, __LINE__);
				return(NULL);
			}

			break;

	}

	return(output);
}

void eigM(Matrix* restrict m, Matrix* restrict *evals, Matrix* restrict *evecs)
// Eigendecomposition of a matrix m
{
	int m_ndim = m->ndim;
	int *m_dim = m->dim;

	// Error checking
	if (m == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Invalid m\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (m_ndim > 2 || m_dim[0] != m_dim[1])
	{
		fprintf(stderr, "%s: %s (%d): Matrix must be a 1D or 2D square matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	if (m == *evals || m == *evecs || (*evals == *evecs && *evals != NULL))
	{
		fprintf(stderr, "%s: %s (%d): Args must not reference overlapping memory\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Initialize the evals and evecs
	if (!destroyM(*evals))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}
	if (!destroyM(*evecs))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}
	*evals = NULL;
	*evecs = NULL;
	
	// Base case:
	// ndim == 1 ==> evals = the value; evecs = 1
	if (m_ndim == 1)
	{
		*evecs = onesM(1, m_dim);
		*evals = full_duplicateM(m);
		return;
	}

	int n = (m_dim)[0];

	// Base case:
	// Zero matrix ==> Eigenvalues are all 0; eigenvectors are identity
	Matrix *zero = zerosM(m_ndim, m_dim);
	if (isEqualM(zero, m))
	{
		*evals = zero;
		*evecs = idM(n);
		return;
	}
	if (!destroyM(zero))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	// Base case:
	// identity matrix ==> evals and evecs are both identity
	Matrix *id = idM(n);
	if (isEqualM(id, m))
	{
		*evals = id;
		*evecs = idM(n);
		return;
	}
	if (!destroyM(id))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return;
	}

	Matrix *m_copy = NULL;

	// Diagonalization
	char jobvl = 'N';
	char jobvr = 'V';
	int success = -1;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			
			double *m_data_double = i_to_d(m);

			m_copy = createM(m_ndim, m_dim, DOUBLE);
			writeM(m_copy, m_copy->data->size, m_data_double);
			free(m_data_double);

			// Don't break because now we handle it as a double

		case DOUBLE: ;

			// This if statement is necessary because the INTEGER case doesn't break
			if (m->data->datatype == DOUBLE)
			{
				m_copy = full_duplicateM(m);
			}

		    double *eigenvalues_r = NULL;
		    eigenvalues_r = malloc(n*sizeof(*eigenvalues_r));

		    double *eigenvalues_i = NULL;
		    eigenvalues_i = malloc(n*sizeof(*eigenvalues_i));

		    double *eigenvectors = NULL;
		    eigenvectors = malloc((m->data->size)*sizeof(*eigenvectors));

		    double *m_copy_data_data_double = (double *)(m_copy->data->data);

		    success = LAPACKE_dgeev(
		    	LAPACK_ROW_MAJOR,
		    	jobvl, 
		    	jobvr,
			    n,
			    m_copy_data_data_double, 
			    n,
			    eigenvalues_r,
			    eigenvalues_i,
			    NULL, 
			    n,
			    (double *) eigenvectors, 
			    n );

		    destroyM(m_copy);

		    // Error checking on the output
			if (success < 0)
			{
		    	fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_dgeev was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(eigenvectors);
		    	free(eigenvalues_r);
		    	free(eigenvalues_i);
				return;
			}
			if (success > 0)
			{
		    	fprintf(stderr, "%s: %s (%d): Failed to compute eigenvalues\n", __FILE__, __func__, __LINE__);
		    	free(eigenvectors);
		    	free(eigenvalues_r);
		    	free(eigenvalues_i);
				return;
			}

			// Check if any of the eigenvalues are complex or negative
			bool complex_eigenvalues_boolean = false;
			for (register int i = 0 ; i < m_dim[0] ; i++)
			{
				if ( !almostEqual_id(0, ((double *)(eigenvalues_i))[i]) )
				{
					complex_eigenvalues_boolean = true;
					break;
				}
			}

			// Construct the matrix of normalized eigenvectors
			// Construct the matrix of eigenvalues

			// If there were complex eigenvalues
			if (complex_eigenvalues_boolean)
			{
				// Initialize the evals to a complex zero matrix
				zero = zerosM(m_ndim, m_dim);
				double complex complex_one = 1;
				*evals = scaleM(zero, &complex_one, COMPLEX);
				if (!destroyM(zero))
				{
					fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
				}

				// Initialize the evecs
				*evecs = createM(m_ndim, m_dim, COMPLEX);

				double complex *evals_data_data_complex = ((double complex *)((*evals)->data->data));
				double complex *evecs_data_data_complex = ((double complex *)((*evecs)->data->data));

				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					// If this eigenvalue is real, put the eigenvalue on the diagonal and iterate over the eigenvectors
					if (almostEqual_ic(0, eigenvalues_i[i]))
					{
						evals_data_data_complex[i+i*n] = eigenvalues_r[i];

						for (register int j = 0 ; j < m_dim[1] ; j++)
						{
							evecs_data_data_complex[i+j*n] = (double complex)(eigenvectors[i+j*n]);
						}
					}

					// If this eigenvalue is complex, then the next eigenvalue will be its conjugate
					// This column of the eigenvectors will be the real parts, and the next column will be the complex parts
					else
					{
						evals_data_data_complex[i+i*n] = eigenvalues_r[i] + eigenvalues_i[i]*I;
						evals_data_data_complex[(i+1)+(i+1)*n] = eigenvalues_r[i] - eigenvalues_i[i]*I;

						for (register int j = 0 ; j < m_dim[1] ; j++)
						{
							evecs_data_data_complex[i+j*n] = eigenvectors[i+j*n] + (eigenvectors[(i+1)+j*n])*I;
							evecs_data_data_complex[(i+1)+j*n] = eigenvectors[i+j*n] - (eigenvectors[(i+1)+j*n])*I;
						}
						i++;
					}
				}
			}

			// All eigenvalues are real
			else
			{
				// Initialize the evals to a double zero matrix
				zero = zerosM(m_ndim, m_dim);
				double double_one = 1;
				*evals = scaleM(zero, &double_one, DOUBLE);
				if (!destroyM(zero))
				{
					fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
				}

				double *evals_data_data_double = ((double *)((*evals)->data->data));

				// Extract the eigenvalues
				for (register int i = 0 ; i < m_dim[0] ; i++)
				{
					evals_data_data_double[i+i*n] = eigenvalues_r[i];
				}

				// Eigenvectors
				*evecs = createM(m_ndim, m_dim, DOUBLE);
				writeM(*evecs, m->data->size, eigenvectors);
			}

			/*// Make sure the matrix is diagonalizable
			if (multiple_same_eigenvalues)
			{
				double determinant_of_eigenvectors = 0;
				detM(norm_eigenvectors, &determinant_of_eigenvectors);
				if (almostEqual_id(0, determinant_of_eigenvectors))
				{
					fprintf(stderr, "%s: %s (%d): Matrix is non-diagonalizable\n", __FILE__, __func__, __LINE__);
					output = NULL;

					free(eigenvectors);
					free(eigenvalues_r);
					free(eigenvalues_i);
					if(!destroyM(norm_eigenvectors))
					{
						fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					}

					break;
				}
			}*/

			// Clean up
			free(eigenvectors);
			free(eigenvalues_r);
			free(eigenvalues_i);
			break;

		case COMPLEX: ;
			m_copy = full_duplicateM(m);

			double complex *c_eigenvalues = NULL;
		    c_eigenvalues = malloc((m_dim[0])*sizeof(*c_eigenvalues));

		    double complex *c_eigenvectors = NULL;
		    c_eigenvectors = malloc((m->data->size)*sizeof(*c_eigenvectors));

		    double complex *m_copy_data_data_complex = (double complex *) m_copy->data->data;

		    success = LAPACKE_zgeev( 
		    	LAPACK_ROW_MAJOR,
		    	jobvl, 
		    	jobvr,
		    	n,
		    	m_copy_data_data_complex,
		    	n,
		    	c_eigenvalues,
		    	NULL,
		    	n,
		    	(double complex *) c_eigenvectors,
		    	n );

		    destroyM(m_copy);

			if (success < 0)
			{
		    	fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_zgeev was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(c_eigenvalues);
		    	free(c_eigenvectors);
				return;
			}
			if (success > 0)
			{
		    	fprintf(stderr, "%s: %s (%d): Failed to calculate eigenvalues\n", __FILE__, __func__, __LINE__);
				free(c_eigenvalues);
		    	free(c_eigenvectors);
				return;
			}

			/*// Check if any of the eigenvalues are repeats
			bool multiple_same_eigenvalues = false;
			for (register int i = 0 ; i < m->dim[0] ; i++)
			{
				for (register int j = 0 ; j < i ; j++)
				{
					if ( !almostEqual_cc( ((double complex *)(c_eigenvalues))[i], ((double complex *)(c_eigenvalues))[j] ) )
					{
						multiple_same_eigenvalues = true;
					}
				}
			}*/

			// Eigenvectors
			*evecs = createM(m_ndim, m_dim, COMPLEX);
			writeM(*evecs, m->data->size, c_eigenvectors);

			/*// Make sure the matrix is diagonalizable
			if (multiple_same_eigenvalues)
			{
				double determinant_of_eigenvectors = 0;
				detM(norm_eigenvectors, &determinant_of_eigenvectors);
				if (almostEqual_id(0, determinant_of_eigenvectors))
				{
			    	fprintf(stderr, "%s: %s (%d): Matrix is non-diagonalizable\n", __FILE__, __func__, __LINE__);
					output = NULL;

					free(c_eigenvectors);
					free(c_eigenvalues);
					if(!destroyM(norm_eigenvectors))
					{
						fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					}

					break;
				}
			}*/

			// Eigenvalues
			zero = zerosM(m_ndim, m_dim);
			double complex complex_one = 1;
			*evals = scaleM(zero, &complex_one, COMPLEX);
			if (!destroyM(zero))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}

			double complex *evals_data_data_complex = ((double complex *)((*evals)->data->data));

			// Extract the eigenvalues
			for (register int i = 0 ; i < m_dim[0] ; i++)
			{
				evals_data_data_complex[i+i*n] = c_eigenvalues[i];
			}

			// Clean up
			free(c_eigenvectors);
			free(c_eigenvalues);
			break;
	}

	return;
}

/*
* =========================================
* 
*  Matrix mathematical operations
*  i.e., NOT element-wise
* 
* =========================================
*/

Matrix * mtimesM_internal(Matrix* restrict m, Matrix* restrict n)
{
	int m_dim_0 = m->dim[0];
	int m_dim_1 = m->dim[1];
	// int n_dim_0 = n->dim[0];
	int n_dim_1 = n->dim[1];
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;

	// Initialize the metadata about the product matrix

	// Output matrix will be 2D unless matrix n is a 1D column matrix
	int output_ndim = 2;
	if (n_dim_1 == 1)
	{
		output_ndim = 1;
	}

	// If m is an axb matrix and n is a bxc matrix, mxn will be an axc matrix
	int output_dim[2] = {m_dim_0, n_dim_1};

	// The output matrix takes the type of the larger-type matrix
	// The output is at least a double size to account for overflow
	int output_type = DOUBLE;
	if (m->data->datatype == COMPLEX || n->data->datatype == COMPLEX)
	{
		output_type = COMPLEX;
	}

	// Initialize the output
	Matrix *output = createM(output_ndim, output_dim, output_type);
	void *output_data_data = output->data->data;

	switch(output_type)
	{
		case DOUBLE: ;
			bool free_m = false;
			double *m_data_double = (double *) m_data_data;

			bool free_n = false;
			double *n_data_double = (double *) n_data_data;

			if (m->data->datatype == INTEGER)
			{
				m_data_double = i_to_d_vec(m);
				free_m = true;
			}

			if (n->data->datatype == INTEGER)
			{
				n_data_double = i_to_d_vec(n);
				free_n = true;
			}
			cblas_dgemm(CblasRowMajor, CblasNoTrans, 
				CblasNoTrans, output_dim[0], output_dim[1], 
				m_dim_1, 1, m_data_double,
				m_dim_1, n_data_double, n_dim_1,
				0, output_data_data, output_dim[0]);
			if (free_m)
			{
				free(m_data_double);
			}

			if (free_n)
			{
				free(n_data_double);
			}
			break;

		case COMPLEX: ;

			double complex one = 1;
			double complex zero = 0;

			if (m->data->datatype == INTEGER)
			{
				double complex *m_data_complex = i_to_c_vec(m);
				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_complex,
					m_dim_1, n_data_data, n_dim_1,
					&zero, output_data_data, output_dim[0]);
				free(m_data_complex);
			}

			else if (m->data->datatype == DOUBLE)
			{
				double complex *m_data_complex = d_to_c_vec(m);
				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_complex,
					m_dim_1, n_data_data, n_dim_1,
					&zero, output_data_data, output_dim[0]);
				free(m_data_complex);
			}

			else if (n->data->datatype == INTEGER)
			{
				double complex *n_data_complex = i_to_c_vec(n);
				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_data,
					m_dim_1, n_data_complex, n_dim_1,
					&zero, output_data_data, output_dim[0]);
				free(n_data_complex);
			}

			else if (n->data->datatype == DOUBLE)
			{
				double complex *n_data_complex = d_to_c_vec(n);
				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_data,
					m_dim_1, n_data_complex, n_dim_1,
					&zero, output_data_data, output_dim[0]);
				free(n_data_complex);
			}

			else
			{
				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_data,
					m_dim_1, n_data_data, n_dim_1,
					&zero, output_data_data, output_dim[0]);
			}
			break;
	}

	return(output);
}

Matrix * mtimesM(Matrix *m, Matrix *n)
// Matrix multiplication (BLAS implementation)
// m * n
{
	// Error checking
	if(m == NULL || n == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2 || n->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid matrix ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (m->dim[1] != n->dim[0])
	{
		fprintf(stderr, "%s: %s (%d): Cannot multiply %dx%d matrix with %dx%d matrix\n", __FILE__, __func__, __LINE__, m->dim[0], m->dim[1], n->dim[0], n->dim[1]);
		return(NULL);
	}

	if (m == n)
	{
		int m_dim_0 = m->dim[0];
		int m_dim_1 = m->dim[1];
		void *m_data_data = m->data->data;

		// Initialize the metadata about the product matrix

		// Output matrix will be 2D unless matrix m is a 1x1 column matrix
		int output_ndim = m->ndim;

		int output_dim[2] = {m_dim_0, m_dim_1};

		// The output matrix takes the type of the larger-type matrix
		// The output is at least a double size to account for overflow
		int output_type = DOUBLE;
		if (m->data->datatype == COMPLEX)
		{
			output_type = COMPLEX;
		}

		// Initialize the output
		Matrix *output = createM(output_ndim, output_dim, output_type);
		void *output_data_data = output->data->data;

		switch(output_type)
		{
			case DOUBLE: ;
				bool free_m = false;
				double *m_data_double = (double *) m_data_data;

				if (m->data->datatype == INTEGER)
				{
					m_data_double = i_to_d_vec(m);
					free_m = true;
				}

				cblas_dgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, 1, m_data_double,
					m_dim_1, m_data_double, m_dim_1,
					0, output_data_data, output_dim[0]);

				if (free_m)
				{
					free(m_data_double);
				}
				break;

			case COMPLEX: ;

				double complex one = 1;
				double complex zero = 0;

				cblas_zgemm(CblasRowMajor, CblasNoTrans, 
					CblasNoTrans, output_dim[0], output_dim[1], 
					m_dim_1, &one, m_data_data,
					m_dim_1, m_data_data, m_dim_1,
					&zero, output_data_data, output_dim[0]);
				break;
		}

		return(output);
	}

	return(mtimesM_internal(m,n));
}
Matrix * naive_mtimesM_vec(Matrix* restrict m, Matrix* restrict n)
{
    // Error checking
	if(m == NULL || n == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2 || n->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid matrix ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (m->dim[1] != n->dim[0])
	{
		fprintf(stderr, "%s: %s (%d): Cannot multiply %dx%d matrix with %dx%d matrix\n", __FILE__, __func__, __LINE__, m->dim[0], m->dim[1], n->dim[0], n->dim[1]);
		return(NULL);
	}

	int m_dim_0 = m->dim[0];
	int m_dim_1 = m->dim[1];
	// int n_dim_0 = n->dim[0];
	int n_dim_1 = n->dim[1];
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;

	// Initialize the metadata about the product matrix

	// Output matrix will be 2D unless matrix n is a 1D column matrix
	int output_ndim = 2;
	if (n_dim_1 == 1)
	{
		output_ndim = 1;
	}

	// If m is an axb matrix and n is a bxc matrix, mxn will be an axc matrix
	int output_dim[2] = {m_dim_0, n_dim_1};
    // int output_dim_0 = m_dim_0;
    int output_dim_1 = n_dim_1;    

	// The output matrix takes the type of the larger-type matrix
	// The output is at least a double size to account for overflow
	int output_type = DOUBLE;
	if (m->data->datatype == COMPLEX || n->data->datatype == COMPLEX)
	{
		output_type = COMPLEX;
	}

	// Initialize the output
	Matrix *output = createM(output_ndim, output_dim, output_type);
	void *output_data_data = output->data->data;

	switch(output_type)
	{
		case DOUBLE: ;
			bool free_m = false;
			bool free_n = false;

			if (m->data->datatype == INTEGER)
			{
				m_data_data = i_to_d_vec(m);
				free_m = true;
			}

			if (n->data->datatype == INTEGER)
			{
				n_data_data = i_to_d_vec(n);
				free_n = true;
			}
            long vlm8 = vsetvlmax_e64m8();
            long vlm1 = vsetvlmax_e64m1();
            for (int i = 0; i < m_dim_0; ++i)
            {
                for (int j = 0; j < n_dim_1; ++j) 
                {
                    double *ptr_m = &(((double *)(m_data_data))[i*m_dim_1]);
                    double *ptr_n = &(((double *)(n_data_data))[j]);

                    vfloat64m1_t v_res = vfmv_v_f_f64m1(0, vlm1);
                    vfloat64m8_t v_s    = vfmv_v_f_f64m8(0, vlm8);

                    int k = m_dim_1;
                    for (size_t vl; k > 0; k -= vl, ptr_m += vl, ptr_n += n_dim_1*vl) 
                    // Adapted from: https://github.com/riscv-non-isa/rvv-intrinsic-doc/blob/master/examples/rvv_matmul.c
                    {
                        vl = vsetvl_e64m8(k);

                        // Load the ith row of matrix m
                        vfloat64m8_t v_m = vle64_v_f64m8(ptr_m, vl);

                        // Load the jth column of matrix n
                        vfloat64m8_t v_n = vlse64_v_f64m8(ptr_n, 8*(n_dim_1), vl);

                        // Elementwise multiply the ith row of matrix m and the jth column of matrix n
                        // Adding the results to whatever is already in v_s
                        v_s = vfmacc_vv_f64m8(v_s, v_m, v_n, vl);
                    }
                    v_res = vfredsum_vs_f64m8_f64m1(v_res, v_s, v_res, vlm8);
                    ((double *)(output_data_data))[i*output_dim_1 + j] = vfmv_f_s_f64m1_f64(v_res);
                }
            }
			if (free_m)
			{
				free(m_data_data);
			}

			if (free_n)
			{
				free(n_data_data);
			}
			break;

		case COMPLEX: ;

			// double complex one = 1;
			// double complex zero = 0;

            free_m = false;
            free_n = false;

			if (m->data->datatype == INTEGER)
			{
				m_data_data = i_to_c_vec(m);
                free_m = true;
			}

			else if (m->data->datatype == DOUBLE)
			{
				m_data_data = d_to_c_vec(m);
                free_m = true;
			}

			else if (n->data->datatype == INTEGER)
			{
				n_data_data = i_to_c_vec(n);
                free_n = true;
			}

			else if (n->data->datatype == DOUBLE)
			{
				n_data_data = d_to_c_vec(n);
                free_n = true;
			}

            vlm1 = vsetvlmax_e64m1();
            long vlm4 = vsetvlmax_e64m4();
            for (int i = 0; i < m_dim_0; ++i)
            {
                for (int j = 0; j < n_dim_1; ++j) 
                {
                    double *ptr_m = &(((double *)(m_data_data))[2*(i*m_dim_1)]);
                    double *ptr_n = &(((double *)(n_data_data))[2*(j)]);

                    vfloat64m1_t v_res_r = vfmv_v_f_f64m1(0, vlm1);
                    vfloat64m1_t v_res_i = vfmv_v_f_f64m1(0, vlm1);
                    vfloat64m4_t v_s_r    = vfmv_v_f_f64m4(0, vlm4);
                    vfloat64m4_t v_s_i    = vfmv_v_f_f64m4(0, vlm4);

                    int k = m_dim_1;
                    // int counter = 0;
                    for (size_t vl; k > 0; k -= vl, ptr_m += 2*vl, ptr_n += 2*n_dim_1*vl) 
                    // Adapted from: https://github.com/riscv-non-isa/rvv-intrinsic-doc/blob/master/examples/rvv_matmul.c
                    {
                        vl = vsetvl_e64m4(k);

                        // Load the real parts of the ith row of matrix m
                        vfloat64m4_t v_x_r = vlse64_v_f64m4(ptr_m, 16, vl);

                        // Load the imaginary parts of the ith row of matrix m
                        vfloat64m4_t v_x_i = vlse64_v_f64m4(ptr_m+1, 16, vl);

                        // Load the real parts of the jth column of matrix n
                        vfloat64m4_t v_y_r = vlse64_v_f64m4(ptr_n, 16*(n_dim_1), vl);

                        // Load the imaginary parts of the jth column of matrix n
                        vfloat64m4_t v_y_i = vlse64_v_f64m4(ptr_n+1, 16*(n_dim_1), vl);

                        // Real part multiplication
                        v_s_r = vfmacc_vv_f64m4(v_s_r, v_x_r, v_y_r, vl);
                        v_s_r = vfnmsac_vv_f64m4(v_s_r, v_x_i, v_y_i, vl);

                        // Imaginary part multiplication
                        v_s_i = vfmacc_vv_f64m4(v_s_i, v_x_i, v_y_r, vl);
                        v_s_i = vfmacc_vv_f64m4(v_s_i, v_x_r, v_y_i, vl);
                    }
                    v_res_r = vfredsum_vs_f64m4_f64m1(v_res_r, v_s_r, v_res_r, vlm4);
                    v_res_i = vfredsum_vs_f64m4_f64m1(v_res_i, v_s_i, v_res_i, vlm4);
                    ((double *)(output_data_data))[2*(i*output_dim_1 + j)] = vfmv_f_s_f64m1_f64(v_res_r);
                    ((double *)(output_data_data))[2*(i*output_dim_1 + j)+1] = vfmv_f_s_f64m1_f64(v_res_i);
                }
            }
            if (free_m)
			{
				free(m_data_data);
			}

			if (free_n)
			{
				free(n_data_data);
			}
			break;
	}

	return(output);
}

Matrix * naive_mtimesM_ref(Matrix* restrict m, Matrix* restrict n)
// Matrix multiplication (naive implementation)
// m * n
{
    // Error checking
	if(m == NULL || n == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2 || n->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid matrix ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (m->dim[1] != n->dim[0])
	{
		fprintf(stderr, "%s: %s (%d): Cannot multiply %dx%d matrix with %dx%d matrix\n", __FILE__, __func__, __LINE__, m->dim[0], m->dim[1], n->dim[0], n->dim[1]);
		return(NULL);
	}

	int m_dim_0 = m->dim[0];
	int m_dim_1 = m->dim[1];
	// int n_dim_0 = n->dim[0];
	int n_dim_1 = n->dim[1];
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;

	// Initialize the metadata about the product matrix

	// Output matrix will be 2D unless matrix n is a 1D column matrix
	int output_ndim = 2;
	if (n_dim_1 == 1)
	{
		output_ndim = 1;
	}

	// If m is an axb matrix and n is a bxc matrix, mxn will be an axc matrix
	int output_dim[2] = {m_dim_0, n_dim_1};
    // int output_dim_0 = m_dim_0;
    int output_dim_1 = n_dim_1;    

	// The output matrix takes the type of the larger-type matrix
	// The output is at least a double size to account for overflow
	int output_type = DOUBLE;
	if (m->data->datatype == COMPLEX || n->data->datatype == COMPLEX)
	{
		output_type = COMPLEX;
	}

	// Initialize the output
	Matrix *output = createM(output_ndim, output_dim, output_type);
	void *output_data_data = output->data->data;

	switch(output_type)
	{
		case DOUBLE: ;
			bool free_m = false;
			bool free_n = false;

			if (m->data->datatype == INTEGER)
			{
				m_data_data = i_to_d(m);
				free_m = true;
			}

			if (n->data->datatype == INTEGER)
			{
				n_data_data = i_to_d(n);
				free_n = true;
			}
            for (int i = 0; i < m_dim_0; ++i)
            {
                for (int j = 0; j < n_dim_1; ++j) 
                {
                    ((double *)(output_data_data))[i*output_dim_1 + j] = 0;
                    for (int k = 0; k < m_dim_1; ++k)
                    {
                        ((double *)(output_data_data))[i*output_dim_1 + j] += ((double *)(m_data_data))[i*m_dim_1 + k] * ((double *)(n_data_data))[k*n_dim_1 + j];
                    }
                }
            }
			if (free_m)
			{
				free(m_data_data);
			}

			if (free_n)
			{
				free(n_data_data);
			}
			break;

		case COMPLEX: ;

			// double complex one = 1;
			// double complex zero = 0;

            free_m = false;
            free_n = false;

			if (m->data->datatype == INTEGER)
			{
				m_data_data = i_to_c(m);
                free_m = true;
			}

			else if (m->data->datatype == DOUBLE)
			{
				m_data_data = d_to_c(m);
                free_m = true;
			}

			else if (n->data->datatype == INTEGER)
			{
				n_data_data = i_to_c(n);
                free_n = true;
			}

			else if (n->data->datatype == DOUBLE)
			{
				n_data_data = d_to_c(n);
                free_n = true;
			}
            for (int i = 0; i < m_dim_0; ++i)
            {
                for (int j = 0; j < n_dim_1; ++j) 
                {
                    ((double complex *)(output_data_data))[i*output_dim_1 + j] = 0;
                    for (int k = 0; k < m_dim_1; ++k)
                    {
                        ((double complex *)(output_data_data))[i*output_dim_1 + j] += ((double complex *)(m_data_data))[i*m_dim_1 + k] * ((double complex *)(n_data_data))[k*n_dim_1 + j];
                    }
                }
            }
            if (free_m)
			{
				free(m_data_data);
			}

			if (free_n)
			{
				free(n_data_data);
			}
			break;
	}

	return(output);
}

Matrix * mrdivideM_internal(Matrix* restrict m, Matrix* restrict n)
{
	// Invert n
	Matrix *n_inv = invertM(n);
	if (n_inv == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to invert matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// Check that m can be multiplied by inv(n)
	if (m->dim[1] != n_inv->dim[0])
	{
		fprintf(stderr, "%s: %s (%d): Cannot multiply %dx%d matrix with %dx%d matrix\n", __FILE__, __func__, __LINE__, m->dim[0], m->dim[1], n->dim[0], n->dim[1]);
		return(NULL);
	}

	// m * inv(n)
	Matrix *output = mtimesM_internal(m, n_inv);

	// Error checking
	if (output == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to rdivide matrices\n", __FILE__, __func__, __LINE__);
	}

	// Cleanup
	if (!destroyM(n_inv))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * mrdivideM(Matrix *m, Matrix *n)
// Matrix right division (/)
// output = m * inv(n)
{
	if (m == NULL || n == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2 || n->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid matrix ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// A matrix divided by itself is just:
	// m / m = m*inv(m) = Identity
	if (m == n)
	{
		return(identityM(m->dim[0]));
	}

	return(mrdivideM_internal(m, n));
}

Matrix * mldivideM_internal(Matrix* restrict m, Matrix* restrict n)
{
	// Invert m
	Matrix *m_inv = invertM(m);
	if (m_inv == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to invert matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// Check that inv(m) can be multiplied by n
	if (m_inv->dim[1] != n->dim[0])
	{
		fprintf(stderr, "%s: %s (%d): Cannot multiply %dx%d matrix with %dx%d matrix\n", __FILE__, __func__, __LINE__, m->dim[0], m->dim[1], n->dim[0], n->dim[1]);
		return(NULL);
	}

	// inv(m) * n
	Matrix *output = mtimesM_internal(m_inv, n);

	// Error checking
	if (output == NULL)
	{
		fprintf(stderr, "%s: %s (%d): Failed to rdivide matrices\n", __FILE__, __func__, __LINE__);
	}

	// Cleanup
	if (!destroyM(m_inv))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}
	return(output);
}

Matrix * mldivideM(Matrix *m, Matrix *n)
// Matrix left division (\)
// output = inv(m) * n
{
	if (m == NULL || n == NULL)
	{
		return(NULL);
	}

	if (m->ndim > 2 || n->ndim > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid matrix ndim\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// A matrix divided by itself is just:
	// m \ m = m*inv(m) = Identity
	if (m == n)
	{
		return(identityM(m->dim[0]));
	}

	return(mldivideM_internal(m, n));
}

Matrix * mpowerM(Matrix *m, void *scalar, int type)
// Matrix to a power
// output = m ^ scalar
// TODO: figure out how to raise defective matrices to non-integer powers
{
	// Error checking
	if (m == NULL || scalar == NULL)
	{
		return(NULL);
	}

	if (type < 0 || type > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	int m_dim_0 = m->dim[0];
	int m_dim_1 = m->dim[1];

	if (m->ndim > 2 || m_dim_0 != m_dim_1)
	{
		fprintf(stderr, "%s: %s (%d): Matrix must be a 1D or 2D square matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;

	// Base case:
	// ndim == 1 ==> Already diagonalized
	if (m->ndim == 1)
	{
		Matrix *one = onesM(1, m->dim);
		if (one == NULL)
		{
			fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}

		Matrix *exponent_mat = scaleM(one, scalar, type);
		if (exponent_mat == NULL)
		{
			fprintf(stderr, "%s: %s (%d): Failed to create matrix\n", __FILE__, __func__, __LINE__);
			if (!destroyM(one))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			return(NULL);
		}

		output = powerM(m, exponent_mat);

		if (!destroyM(one))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		}

		if (!destroyM(exponent_mat))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		}

		return(output);
	}

	// Base cases:
	// exponent == 0 ==> Return identity
	// exponent == 1 ==> Return itself
	switch(type)
	{
		case INTEGER:
			if(*(int *)scalar == 0)
			{
				output = identityM(m_dim_0);
				return(output);
			}
			if(*(int *)scalar == 1)
			{
				output = full_duplicateM(m);
				return(output);
			}
			break;

		case DOUBLE:
			if(*(double *)scalar == 0)
			{
				output = identityM(m_dim_0);
				return(output);
			}
			if(*(double *)scalar == 1)
			{
				output = full_duplicateM(m);
				return(output);
			}

			break;

		case COMPLEX:
			if((creal(*(double complex *)scalar)) == 0 && (cimag(*(double complex *)scalar)) == 0)
			{
				output = identityM(m_dim_0);
				return(output);
			}
			if((creal(*(double complex *)scalar)) == 1 && (cimag(*(double complex *)scalar)) == 0)
			{
				output = full_duplicateM(m);
				return(output);
			}

			break;
	}

	// Base cases:
	// Zero matrix ==> Return itself
	// identity matrix ==> Return itself
	Matrix *zero = zerosM(m->ndim, m->dim);
	if (isEqualM(zero, m))
	{
		return(zero);
	}
	if (!destroyM(zero))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *id = idM(m_dim_0);
	if (isEqualM(id, m))
	{
		return(id);
	}
	if (!destroyM(id))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	// INTEGER EXPONENTS
	// Exponentiation by binary expansion
	// At what point is it faster to diagonalize?
	if ((type == INTEGER) || 
		(type == DOUBLE && *(double *)scalar == floor(*(double *)scalar)) ||
		(type == COMPLEX && (creal(*(double complex *)scalar)) == floor((creal(*(double *)scalar))) && (cimag(*(double complex *)scalar)) == 0))
	{
		// Get the exponent in integer format
		// Exponent is an integer with magnitude greater than or equal to 2
		int power_int = 0;
		if (type == DOUBLE)
		{
			power_int = (int) (*(double *)scalar);
		}
		else if (type == COMPLEX)
		{
			power_int = (int) (creal(*(double complex *)scalar));
		}
		else
		{
			power_int = (*(int *)scalar);
		}

		// m^-x == invM(m)^x
		Matrix *m_to_use = NULL;
		if (power_int < 0)
		{
			m_to_use = invM(m);
			if (m_to_use == NULL)
			{
				fprintf(stderr, "%s: %s (%d): Cannot raise singular matrix to negative exponent\n", __FILE__, __func__, __LINE__);
			}
		}
		else
		{
			m_to_use = full_duplicateM(m);
		}
		power_int = (int) fabs(power_int);
		
		// Get the highest power of two of the exponent
		int highest_power_of_two = (int) floor(log2(power_int));

		// Get Matrix m's powers of two
		Matrix **powers_of_two = NULL;
		powers_of_two = malloc((highest_power_of_two+1) * sizeof(*powers_of_two));

		powers_of_two[0] = m_to_use;

		for (register int i = 1 ; i < (highest_power_of_two+1) ; i++)
		{
			// Can make this more efficient with direct BLAS call
			powers_of_two[i] = mtimesM(powers_of_two[i-1], powers_of_two[i-1]);
		}

		// Initial result is identity
		output = identityM(m_dim_0);

		// Iterate over the binary form of the exponent (LSB first)
		int counter = 0;
		for (register int exponent_bit = power_int ; exponent_bit > 0 ; exponent_bit = exponent_bit >> 1)
		{
			// If the current exponent bit is 1, multiply the result by the corresponding power of 2
			if ((exponent_bit & 1) == 1)
			{
				// Can make this more efficient with direct BLAS call
				Matrix *updated_output = mtimesM(output, powers_of_two[counter]);

				if (!destroyM(output))
				{
					fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					return(NULL);
				}

				output = updated_output;
			}

			counter++;
		}

		// Clean up the powers of two
		for (register int i = 0 ; i < (highest_power_of_two+1) ; i++)
		{
			destroyM(powers_of_two[i]);
		}
		free(powers_of_two);

		return(output);
	}

	// NON-INTEGER EXPONENTS, DIAGONALIZABLE MATRIX
	char jobvl = 'N';
	char jobvr = 'V';
	int success = -1;
	switch(m->data->datatype)
	{
		case INTEGER: ;
			
			double one = 1;
			output = scaleM(m, &one, DOUBLE);

		case DOUBLE: ;
			if (m->data->datatype == DOUBLE)
			{
				output = full_duplicateM(m);
			}

			double *output_data_data_double = ((double *)(output->data->data));

			// Will contain the real parts of the eigenvalues
		    double *eigenvalues_r = NULL;
		    eigenvalues_r = malloc((m_dim_0)*sizeof(*eigenvalues_r));

		    // Will contain the imaginary parts of the eigenvalues
		    double *eigenvalues_i = NULL;
		    eigenvalues_i = malloc((m_dim_0)*sizeof(*eigenvalues_i));

		    // Will contain a matrix where column i is the eigenvector corresponding to the i-th eigenvalue
		    double *eigenvectors = NULL;
		    eigenvectors = malloc((m->data->size)*sizeof(*eigenvectors));

		    success = LAPACKE_dgeev(
		    	LAPACK_ROW_MAJOR,
		    	jobvl, 
		    	jobvr,
			    (m_dim_0),
			    output_data_data_double, 
			    (m_dim_0),
			    eigenvalues_r,
			    eigenvalues_i,
			    NULL, 
			    (m_dim_0),
			    eigenvectors, 
			    (m_dim_0) );

		    destroyM(output);

		    // Error checking on the output
			if (success < 0)
			{
		    	fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_dgeev was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(eigenvectors);
		    	free(eigenvalues_r);
		    	free(eigenvalues_i);
				return(NULL);
			}
			if (success > 0)
			{
		    	fprintf(stderr, "%s: %s (%d): Failed to compute eigenvalues\n", __FILE__, __func__, __LINE__);
		    	free(eigenvectors);
		    	free(eigenvalues_r);
		    	free(eigenvalues_i);
				return(NULL);
			}

			// Check if any of the eigenvalues are complex or negative
			bool complex_eigenvalues_boolean = false;
			bool negative_eigenvalues_boolean = false;		
			for (register int i = 0 ; i < m_dim_0 ; i++)
			{
				if ( !almostEqual_id(0, ((double *)(eigenvalues_i))[i]) )
				{
					complex_eigenvalues_boolean = true;
					break;
				}
				if (((double *)(eigenvalues_r))[i] < 0)
				{
					negative_eigenvalues_boolean = true;
				}
			}

			// Property: m^n = P*(D^n)*inv(P), where:
			// P = a matrix whose columns are the eigenvectors of m,
			// D = a diagonal matrix whose entries are the corresponding eigenvalues

			// Construct the matrix of normalized eigenvectors (i.e., P)
			Matrix *norm_eigenvectors = NULL;

			// Construct the matrix of normalized eigenvectors times the scaled eigenvalues (i.e., P*D^n)
			Matrix *fast_first = NULL;

			// If there are complex eigenvalues
			if (complex_eigenvalues_boolean)
			{
				fast_first = createM(m->ndim, m->dim, COMPLEX);
				norm_eigenvectors = createM(m->ndim, m->dim, COMPLEX);

				double complex *fast_first_data_data = ((double complex *)(fast_first->data->data));
				double complex *norm_eigenvectors_data_data = ((double complex *)(norm_eigenvectors->data->data));

				// Iterate over the eigenvalues
				for (register int i = 0 ; i < m_dim_0 ; i++)
				{
					double complex raised_eigenvalue = 0;
					double complex raised_eigenvalue_conj = 0;
					switch(type)
					{
						case INTEGER:;
							raised_eigenvalue      = pow(eigenvalues_r[i] + eigenvalues_i[i]*I, (double complex)(*(int *)scalar));
							raised_eigenvalue_conj = pow(eigenvalues_r[i] - eigenvalues_i[i]*I, (double complex)(*(int *)scalar));
							break;

						case DOUBLE:;
							raised_eigenvalue      = pow(eigenvalues_r[i] + eigenvalues_i[i]*I, (double complex)(*(double *)scalar));
							raised_eigenvalue_conj = pow(eigenvalues_r[i] - eigenvalues_i[i]*I, (double complex)(*(double *)scalar));
							break;


						case COMPLEX:;
							raised_eigenvalue      = pow(eigenvalues_r[i] + eigenvalues_i[i]*I, (double complex)(*(double complex *)scalar));
							raised_eigenvalue_conj = pow(eigenvalues_r[i] - eigenvalues_i[i]*I, (double complex)(*(double complex *)scalar));
							break;

					}

					// Real eigenvalue
					if (almostEqual_ic(0, eigenvalues_i[i]))
					{
						for (register int j = 0 ; j < m_dim_1 ; j++)
						{
							norm_eigenvectors_data_data[i+j*(m_dim_0)] = (double complex)(eigenvectors[i+j*(m_dim_0)]);
							fast_first_data_data[i+j*(m_dim_0)]        = (double complex)(eigenvectors[i+j*(m_dim_0)]) * raised_eigenvalue;
						}
					}

					// Complex eigenvalue
					// Property: if a+bi is an eigenvalue, then so is a-bi
				    // If the i-th & (i+1)-th eigenvalues form a complex pair, 
				    // then the i-th column in the eigenvectors will be the real parts,  
				    // & the (i+1)-th column in the eigenvectors will be the imaginary parts
				    // Need to convert the i-th column to the complex eigenvector
				    // Need to convert the (i+1)-th column to the conjugate eigenvector
					else
					{
						for (register int j = 0 ; j < m_dim_1 ; j++)
						{
							norm_eigenvectors_data_data[i+j*(m_dim_0)]     =  eigenvectors[i+j*(m_dim_0)] + (eigenvectors[(i+1)+j*(m_dim_0)])*I;
							fast_first_data_data[i+j*(m_dim_0)]            = (eigenvectors[i+j*(m_dim_0)] + (eigenvectors[(i+1)+j*(m_dim_0)])*I) * raised_eigenvalue;

							norm_eigenvectors_data_data[(i+1)+j*(m_dim_0)] =  eigenvectors[i+j*(m_dim_0)] - (eigenvectors[(i+1)+j*(m_dim_0)])*I;
							fast_first_data_data[(i+1)+j*(m_dim_0)]        = (eigenvectors[i+j*(m_dim_0)] - (eigenvectors[(i+1)+j*(m_dim_0)])*I) * raised_eigenvalue_conj;
						}
						i++;
					}
				}
			}

			// All eigenvalues are real
			else
			{
				// Raising a negative double or negative int to a double or complex power will have a complex result
				if (type == COMPLEX || (negative_eigenvalues_boolean && type != INTEGER) )
				{
					fast_first = createM(m->ndim, m->dim, COMPLEX);
					double complex *fast_first_data_data = ((double complex *)(fast_first->data->data));
					switch(type)
					{
						case INTEGER: ;
							for (register int i = 0 ; i < m_dim_0 ; i++)
							{
								for (register int j = 0 ; j < m_dim_0 ; j++)
								{
									fast_first_data_data[i+j*(m_dim_0)] = (((double *)(eigenvectors))[i+j*(m_dim_0)]) * pow( (double complex)((((double *)(eigenvalues_r))[i]) + (((double *)(eigenvalues_i))[i])*I), *(int *)scalar);	
								}								
							}
							break;

						case DOUBLE: ;
							for (register int i = 0 ; i < m_dim_0 ; i++)
							{
								for (register int j = 0 ; j < m_dim_0 ; j++)
								{
									fast_first_data_data[i+j*(m_dim_0)] = (((double *)(eigenvectors))[i+j*(m_dim_0)]) * pow( (double complex)((((double *)(eigenvalues_r))[i]) + (((double *)(eigenvalues_i))[i])*I), *(double *)scalar);
								}
							}
							break;

						case COMPLEX: ;
							for (register int i = 0 ; i < m_dim_0 ; i++)
							{
								for (register int j = 0 ; j < m_dim_0 ; j++)
								{
									fast_first_data_data[i+j*(m_dim_0)] = (((double *)(eigenvectors))[i+j*(m_dim_0)]) * pow( (double complex)((((double *)(eigenvalues_r))[i]) + (((double *)(eigenvalues_i))[i])*I), *(double complex *)scalar);
								}
							}
							break;
					}

					
				}

				// All eigenvalues are real & positive, being raised to real power
				else
				{
					fast_first = createM(m->ndim, m->dim, DOUBLE);
					double *fast_first_data_data = ((double *)(fast_first->data->data));

					if (type == INTEGER)
					{
						for (register int i = 0 ; i < m_dim_0 ; i++)
						{
							for (register int j = 0 ; j < m_dim_0 ; j++)
							{
								fast_first_data_data[i+j*(m_dim_0)] = (((double *)(eigenvectors))[i+j*(m_dim_0)]) * pow( eigenvalues_r[i], *(int *)scalar);
							}
						}
					}
					else
					{
						for (register int i = 0 ; i < m_dim_0 ; i++)
						{
							for (register int j = 0 ; j < m_dim_0 ; j++)
							{
								fast_first_data_data[i+j*(m_dim_0)] = (((double *)(eigenvectors))[i+j*(m_dim_0)]) * pow( eigenvalues_r[i], *(double *)scalar);
							}
						}
					}					
				}

				// Eigenvectors
				norm_eigenvectors = createM(m->ndim, m->dim, DOUBLE);
				writeM(norm_eigenvectors, m->data->size, eigenvectors);
			}

			// Check if any of the eigenvalues are repeats
			bool multiple_same_eigenvalues = false;
			for (register int i = 0 ; i < m_dim_0 ; i++)
			{
				for (register int j = 0 ; j < i ; j++)
				{
					if ( almostEqual_dd( ((double *)(eigenvalues_r))[i], ((double *)(eigenvalues_r))[j] ) &&
						 almostEqual_dd( ((double *)(eigenvalues_i))[i], ((double *)(eigenvalues_i))[j] ))
					{
						multiple_same_eigenvalues = true;
						break;
					}
				}
				if (multiple_same_eigenvalues) {break;}
			}

			// Make sure the matrix m is diagonalizable
			if (multiple_same_eigenvalues)
			{
				double determinant_of_eigenvectors = 0;
				detM(norm_eigenvectors, &determinant_of_eigenvectors);
				if (almostEqual_id(0, determinant_of_eigenvectors))
				{
					fprintf(stderr, "%s: %s (%d): Matrix is non-diagonalizable\n", __FILE__, __func__, __LINE__);
					output = NULL;

					free(eigenvectors);
					free(eigenvalues_r);
					free(eigenvalues_i);
					if(!destroyM(norm_eigenvectors))
					{
						fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					}

					break;
				}
			}

			// Construct the matrix of inverted normalized eigenvectors (i.e., inv(P))
			Matrix *norm_eigenvectors_inv = invM(norm_eigenvectors);
			
			// Multiply the different components together
			// P*(m^n)*inv(P)
			output = mtimesM(fast_first, norm_eigenvectors_inv);

			// Clean up
			free(eigenvectors);
			free(eigenvalues_r);
			free(eigenvalues_i);
			if(!destroyM(norm_eigenvectors))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			if(!destroyM(norm_eigenvectors_inv))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			if(!destroyM(fast_first))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			break;

		case COMPLEX: ;
			output = full_duplicateM(m);

			double complex *output_data_data_complex = ((double complex *)(output->data->data));

			double complex *c_eigenvalues = NULL;
		    c_eigenvalues = malloc((m_dim_0)*sizeof(*c_eigenvalues));

		    double complex *c_eigenvectors = NULL;
		    c_eigenvectors = malloc((m->data->size)*sizeof(*c_eigenvectors));

		    success = LAPACKE_zgeev( 
		    	LAPACK_ROW_MAJOR,
		    	jobvl, 
		    	jobvr,
		    	m_dim_0,
		    	output_data_data_complex,
		    	m_dim_0,
		    	c_eigenvalues,
		    	NULL,
		    	m_dim_0,
		    	c_eigenvectors,
		    	m_dim_0 );

		    destroyM(output);

			if (success < 0)
			{
		    	fprintf(stderr, "%s: %s (%d): The %d-th argument to LAPACK_zgeev was invalid\n", __FILE__, __func__, __LINE__, (int) fabs(success));
				free(c_eigenvalues);
		    	free(c_eigenvectors);
				return(NULL);
			}
			if (success > 0)
			{
		    	fprintf(stderr, "%s: %s (%d): Failed to calculate eigenvalues\n", __FILE__, __func__, __LINE__);
				free(c_eigenvalues);
		    	free(c_eigenvectors);
				return(NULL);
			}

			// Eigenvectors
			norm_eigenvectors = createM(m->ndim, m->dim, COMPLEX);
			writeM(norm_eigenvectors, m->data->size, c_eigenvectors);

			// Check if any of the eigenvalues are repeats
			multiple_same_eigenvalues = false;
			for (register int i = 0 ; i < m_dim_0 ; i++)
			{
				for (register int j = 0 ; j < i ; j++)
				{
					if ( almostEqual_cc( ((double complex *)(c_eigenvalues))[i], ((double complex *)(c_eigenvalues))[j] ) )
					{
						multiple_same_eigenvalues = true;
					}
				}
			}

			// Make sure the matrix is diagonalizable
			if (multiple_same_eigenvalues)
			{
				double determinant_of_eigenvectors = 0;
				detM(norm_eigenvectors, &determinant_of_eigenvectors);
				if (almostEqual_id(0, determinant_of_eigenvectors))
				{
			    	fprintf(stderr, "%s: %s (%d): Matrix is non-diagonalizable\n", __FILE__, __func__, __LINE__);
					output = NULL;

					free(c_eigenvectors);
					free(c_eigenvalues);
					if(!destroyM(norm_eigenvectors))
					{
						fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
					}

					break;
				}
			}

			// Fast first (eigenvectors * raised eigenvalues)
			fast_first = createM(m->ndim, m->dim, COMPLEX);
			double complex *fast_first_data_data = ((double complex *)(fast_first->data->data));			
			switch(type)
			{
				case INTEGER: ;	
					for (register int i = 0 ; i < m_dim_0 ; i++)
					{
						for (register int j = 0 ; j < m_dim_0 ; j++)
						{						
							fast_first_data_data[i+j*(m_dim_0)] = (((double complex *)(c_eigenvectors))[i+j*(m_dim_0)]) * pow( (((double complex *)(c_eigenvalues))[i]), *(int *)scalar);
						}
					}
					break;

				case DOUBLE: ;
					for (register int i = 0 ; i < m_dim_0 ; i++)
					{
						for (register int j = 0 ; j < m_dim_0 ; j++)
						{						
							fast_first_data_data[i+j*(m_dim_0)] = (((double complex *)(c_eigenvectors))[i+j*(m_dim_0)]) * pow( (((double complex *)(c_eigenvalues))[i]), *(double *)scalar);
						}
					}
					break;

				case COMPLEX: ;
					for (register int i = 0 ; i < m_dim_0 ; i++)
					{
						for (register int j = 0 ; j < m_dim_0 ; j++)
						{						
							fast_first_data_data[i+j*(m_dim_0)] = (((double complex *)(c_eigenvectors))[i+j*(m_dim_0)]) * pow( (((double complex *)(c_eigenvalues))[i]), *(double complex *)scalar);
						}
					}
					break;
			}

			// Construct the matrix of inverted normalized eigenvectors
			norm_eigenvectors_inv = invM(norm_eigenvectors);
			
			// Multiply the different components together
			output = mtimesM(fast_first, norm_eigenvectors_inv);

			// Clean up
			free(c_eigenvectors);
			free(c_eigenvalues);
			if(!destroyM(norm_eigenvectors))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			if(!destroyM(norm_eigenvectors_inv))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			if(!destroyM(fast_first))
			{
				fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			}
			break;
	}

	return(output);
}


