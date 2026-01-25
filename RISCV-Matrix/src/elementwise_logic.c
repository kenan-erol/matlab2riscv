//
//  elementwise_logic.c
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

void trueM_vec_i(int * y, int * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
        
        // True means not equal to zero
		vbool4_t v_mask = vmsne_vx_i32m8_b4 (v_x, 0, trueVL);

        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

		vse32_v_i32m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void trueM_vec_d(int * y, double * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;    
    for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);
        
        // True means not equal to zero
		vbool8_t v_mask = vmfne_vf_f64m8_b8 (v_x, 0, trueVL);
        
        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);
 
        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void trueM_vec_c(int * y, double complex * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints.. not sure why
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Stride-load the real parts
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Stride-load the imaginary parts
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // True means not equal to zero (real part OR imaginary part not equal to zero)
		vbool8_t v_mask_r = vmfne_vf_f64m8_b8 (v_x_r, 0, trueVL);
		vbool8_t v_mask_i = vmfne_vf_f64m8_b8 (v_x_i, 0, trueVL);
		vbool8_t v_mask = vmor_mm_b8 (v_mask_r, v_mask_i, trueVL);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * trueM_vec(Matrix* restrict m)
// Element-wise logical 1 
// essentially, NOT (NOT m)
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = zerosM(m->ndim, m->dim);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
            trueM_vec_i( ((int *)(output_data_data)), ((int *)(m_data_data)), m_data_size);
			break;

		case DOUBLE: ;
            trueM_vec_d( ((int *)(output_data_data)), ((double *)(m_data_data)), m_data_size);
			break;

		case COMPLEX: ;
            trueM_vec_c( ((int *)(output_data_data)), ((double complex *)(m_data_data)), m_data_size);
			break;
	}

	return(output);
}

Matrix * trueM_ref(Matrix* restrict m)
// Element-wise logical 1 
// essentially, NOT (NOT m)
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = zerosM(m->ndim, m->dim);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i])
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i])
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;

		case COMPLEX: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if ((creal(((double complex *)(m_data_data))[i])) ||
					(cimag(((double complex *)(m_data_data))[i])))
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;
	}

	return(output);
}

Matrix * trueM(Matrix* restrict m)
// Element-wise logical 1 
// essentially, NOT (NOT m)
{
    if (MODE == VEC) {return trueM_vec(m);}
    else if (MODE == REF) {return trueM_ref(m);}

	// Default mode
    return trueM_vec(m);
}



void andM_internal_vec_i(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // AND
        vint32m8_t v_res = vand_vv_i32m8 (v_x, v_y, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * andM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{

	int m_data_size = m->data->size;

	Matrix *output = zerosM(m->ndim, m->dim);

	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

    int *m_true = NULL;
    m_true = malloc(m_data_size*sizeof(*m_true));

    int *n_true = NULL;
    n_true = malloc(m_data_size*sizeof(*n_true));		

	switch(m->data->datatype)
	{
		case INTEGER: ;
            trueM_vec_i(m_true, ((int *)(m_data_data)), m_data_size);
            break;

		case DOUBLE: ;
            trueM_vec_d(m_true, ((double *)(m_data_data)), m_data_size);
            break;

		case COMPLEX: ;
            trueM_vec_c(m_true, ((double complex *)(m_data_data)), m_data_size);
            break;
    }

	switch(n->data->datatype)
	{
		case INTEGER: ;
            trueM_vec_i(n_true, ((int *)(n_data_data)), m_data_size);
            break;

		case DOUBLE: ;
            trueM_vec_d(n_true, ((double *)(n_data_data)), m_data_size);
            break;

		case COMPLEX: ;
            trueM_vec_c(n_true, ((double complex *)(n_data_data)), m_data_size);
            break;
    }

    andM_internal_vec_i(((int *)(output_data_data)), m_true, n_true, m_data_size);
	return(output);
}

Matrix * andM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{

	int m_data_size = m->data->size;

	Matrix *output = zerosM(m->ndim, m->dim);

	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] && ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] && ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] && ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] && ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] && ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] && ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] && ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] && ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] && ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * andM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(trueM_vec(m));
	}
	return(andM_internal_vec(m, n));
}

Matrix * andM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(trueM_ref(m));
	}
	return(andM_internal_ref(m, n));
}

Matrix * andM(Matrix *m, Matrix *n)
// Element-wise logical AND
// m && n
{
    if (MODE == REF){return(andM_ref(m, n));}
    else if (MODE == VEC){return(andM_vec(m, n));}

    // Default case
	return(andM_ref(m, n));
}


void orM_internal_vec_i(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // OR
        vint32m8_t v_res = vor_vv_i32m8 (v_x, v_y, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * orM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{

	int m_data_size = m->data->size;

	Matrix *output = zerosM(m->ndim, m->dim);

	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

    int *m_true = NULL;
    m_true = malloc(m_data_size*sizeof(*m_true));

    int *n_true = NULL;
    n_true = malloc(m_data_size*sizeof(*n_true));		

	switch(m->data->datatype)
	{
		case INTEGER: ;
            trueM_vec_i(m_true, ((int *)(m_data_data)), m_data_size);
            break;

		case DOUBLE: ;
            trueM_vec_d(m_true, ((double *)(m_data_data)), m_data_size);
            break;

		case COMPLEX: ;
            trueM_vec_c(m_true, ((double complex *)(m_data_data)), m_data_size);
            break;
    }

	switch(n->data->datatype)
	{
		case INTEGER: ;
            trueM_vec_i(n_true, ((int *)(n_data_data)), m_data_size);
            break;

		case DOUBLE: ;
            trueM_vec_d(n_true, ((double *)(n_data_data)), m_data_size);
            break;

		case COMPLEX: ;
            trueM_vec_c(n_true, ((double complex *)(n_data_data)), m_data_size);
            break;
    }

    orM_internal_vec_i(((int *)(output_data_data)), m_true, n_true, m_data_size);
	return(output);
}

Matrix * orM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] || ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] || ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] || ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] || ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] || ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] || ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] || ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] || ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double complex *)(m_data_data))[i] || ((double complex *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * orM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(trueM_vec(m));
	}
	return(orM_internal_vec(m, n));
}

Matrix * orM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(trueM_ref(m));
	}
	return(orM_internal_ref(m, n));
}

Matrix * orM(Matrix *m, Matrix *n)
// Element-wise logical OR
// m || n
{
    if (MODE == REF){return(orM_ref(m, n));}
    else if (MODE == VEC){return(orM_vec(m, n));}

    // Default case
	return(orM_ref(m, n));
}





void notM_vec_i(int * y, int * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
        
        // Not means equal to zero
		vbool4_t v_mask = vmseq_vx_i32m8_b4 (v_x, 0, trueVL);

        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

		vse32_v_i32m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void notM_vec_d(int * y, double * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints.. not sure why
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;    
    for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);
        
        // Not means equal to zero
		vbool8_t v_mask = vmfeq_vf_f64m8_b8 (v_x, 0, trueVL);
        
        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);
 
        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void notM_vec_c(int * y, double complex * x, int n)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints.. not sure why
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Stride-load the real parts
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Stride-load the imaginary parts
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Not means equal to zero (real part AND imaginary part)
		vbool8_t v_mask_r = vmfeq_vf_f64m8_b8 (v_x_r, 0, trueVL);
		vbool8_t v_mask_i = vmfeq_vf_f64m8_b8 (v_x_i, 0, trueVL);
		vbool8_t v_mask = vmand_mm_b8 (v_mask_r, v_mask_i, trueVL);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // If the mask is true, then the output should be one (and vice versa)
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * notM_vec(Matrix* restrict m)
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = zerosM(m->ndim, m->dim);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
            notM_vec_i( ((int *)(output_data_data)), ((int *)(m_data_data)), m_data_size);
			break;

		case DOUBLE: ;
            notM_vec_d( ((int *)(output_data_data)), ((double *)(m_data_data)), m_data_size);
			break;

		case COMPLEX: ;
            notM_vec_c( ((int *)(output_data_data)), ((double complex *)(m_data_data)), m_data_size);
			break;
	}

	return(output);
}

Matrix * notM_ref(Matrix* restrict m)
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = zerosM(m->ndim, m->dim);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((int *)(m_data_data))[i] == 0)
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if (((double *)(m_data_data))[i] == 0)
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;

		case COMPLEX: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				if ((creal(((double complex *)(m_data_data))[i])) == 0 &&
					(cimag(((double complex *)(m_data_data))[i])) == 0)
				{
					((int *)(output_data_data))[i] = 1;
				}
			}
			break;
	}

	return(output);
}

Matrix * notM(Matrix* restrict m)
// Element-wise logical NOT
// ~m
{
    if (MODE == REF){return(notM_ref(m));}
    else if (MODE == VEC){return(notM_vec(m));}

    // Default case
	return(notM_ref(m));
}







