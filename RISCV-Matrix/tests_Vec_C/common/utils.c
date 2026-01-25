//
//  utils.c
//  Octave-C
//
//  Created by Zach Taylor on 12/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../../src/tgmath.h"
#include "../../src/matrix.h"

#include "../common/utils.h"

void print_table_size(int s1, int s2)
{
    fprintf(stdout,"%d;\t", s1*s2);
}

void print_table_field(double elapsed_vec, double elapsed_ref, double ratio)
{
    fprintf(stdout,"%.0f;\t%.0f;\t%.5f;\t", elapsed_vec, elapsed_ref, ratio);
}

void print_table_newline()
{
    fprintf(stdout,"\n");
}

void print_cycles_MM(char *flavor, char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double cycles)
{
    fprintf(stdout,"%sM_%s; matrix 1 type: %s; matrix 2 type: %s; size: (%d, %d); %s ; time: %.0f cycles\n", op_name, flavor, type1, type2, s1, s2, notes, cycles);
}

void print_ratio_MM(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio)
{
    fprintf(stdout,"%sM;     matrix 1 type: %s; matrix 2 type: %s; size: (%d, %d); %s ; vectorized time / reference time: %.20f\n\n", op_name, type1, type2, s1, s2, notes, ratio);
}

void print_error_MM(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes)
{
    fprintf(stdout,"ERROR; %s; matrix 1 type: %s; matrix 2 type: %s; size: (%d, %d); %s\n", op_name, type1, type2, s1, s2, notes);
}

double function_comparison_MM(char *op_name, 
                            Matrix* (*func_ref)(Matrix*, Matrix*), 
                            Matrix* (*func_vec)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag)
{
    int mat_type1 = gettypeM(x);
    int mat_type2 = gettypeM(y);
    int *dims = getdimsM(x);
    int s1 = dims[0];
    int s2 = dims[1];

    const char * type_names[] = {"integer", "double", "complex"};

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_ref = (*func_ref)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_vec = (*func_vec)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the ratio
    double ratio = elapsed_vec/elapsed_ref;

    if (accuracy_check)
    {
        // Print it out
        print_cycles_MM("vec", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_vec);
        print_cycles_MM("ref", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_ref);
        print_ratio_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, ratio);

        if(!isEqualM(result_ref, result_vec))
        {
            *success_flag = false;
            print_error_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes);
            Matrix *eq = equalM(result_ref, result_vec);

            int size = getsizeM(eq);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val = 0;
                indexM(eq, &eq_val, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val == 1)
                {
                    continue;
                }

                // Show the differences
                switch (gettypeM(x))
                {
                    case INTEGER: ;
                        int x_val_i = 0;
                        indexM(x, &x_val_i, 1, index);
                        fprintf(stdout,"x   val: %d\n", x_val_i);
                        break;

                    case DOUBLE: ;
                        double x_val_d = 0;
                        indexM(x, &x_val_d, 1, index);
                        fprintf(stdout,"x   val: %.20f\n", x_val_d);
                        break;

                    case COMPLEX: ;
                        double complex x_val_c = 0;
                        indexM(x, &x_val_c, 1, index);
                        fprintf(stdout,"x   val: %.20f + %.20f*I\n", creal(x_val_c), cimag(x_val_c));
                        break;
                }

                switch (gettypeM(y))
                {
                    case INTEGER: ;
                        int y_val_i = 0;
                        indexM(y, &y_val_i, 1, index);
                        fprintf(stdout,"y   val: %d\n", y_val_i);
                        break;

                    case DOUBLE: ;
                        double y_val_d = 0;
                        indexM(y, &y_val_d, 1, index);
                        fprintf(stdout,"y   val: %.20f\n", y_val_d);
                        break;

                    case COMPLEX: ;
                        double complex y_val_c = 0;
                        indexM(y, &y_val_c, 1, index);
                        fprintf(stdout,"y   val: %.20f + %.20f*I\n", creal(y_val_c), cimag(y_val_c));
                        break;
                }

                switch (gettypeM(result_ref))
                {
                    case INTEGER: ;
                        int ref_val_i = 0;
                        indexM(result_ref, &ref_val_i, 1, index);
                        fprintf(stdout,"Ref val: %d\n", ref_val_i);
                        break;

                    case DOUBLE: ;
                        double ref_val_d = 0;
                        indexM(result_ref, &ref_val_d, 1, index);
                        fprintf(stdout,"Ref val: %.20f\n", ref_val_d);
                        break;

                    case COMPLEX: ;
                        double complex ref_val_c = 0;
                        indexM(result_ref, &ref_val_c, 1, index);
                        fprintf(stdout,"Ref val: %.20f + %.20f*I\n", creal(ref_val_c), cimag(ref_val_c));
                        break;
                }

                switch (gettypeM(result_vec))
                {
                    case INTEGER: ;
                        int vec_val_i = 0;
                        indexM(result_vec, &vec_val_i, 1, index);
                        fprintf(stdout,"Vec val: %d\n", vec_val_i);
                        break;

                    case DOUBLE: ;
                        double vec_val_d = 0;
                        indexM(result_vec, &vec_val_d, 1, index);
                        fprintf(stdout,"Vec val: %.20f\n", vec_val_d);
                        break;

                    case COMPLEX: ;
                        double complex vec_val_c = 0;
                        indexM(result_vec, &vec_val_c, 1, index);
                        fprintf(stdout,"Vec val: %.20f + %.20f*I\n", creal(vec_val_c), cimag(vec_val_c));
                        break;
                }
                fprintf(stdout,"\n");
            }

            destroyM(eq);
        }
    }
    else
    {
        print_table_field(elapsed_vec, elapsed_ref, ratio);
    }

    if (!destroyM(result_ref))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_vec))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(ratio);
}

double function_comparison_MM_bb(char *op_name, 
                            bool (*func_ref)(Matrix*, Matrix*), 
                            bool (*func_vec)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag)
{
    int mat_type1 = gettypeM(x);
    int mat_type2 = gettypeM(y);
    int *dims = getdimsM(x);
    int s1 = dims[0];
    int s2 = dims[1];

    const char * type_names[] = {"integer", "double", "complex"};

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    bool result_ref = (*func_ref)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    bool result_vec = (*func_vec)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the ratio
    double ratio = elapsed_vec/elapsed_ref;

    if (accuracy_check)
    {
        // Print it out
        print_cycles_MM("vec", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_vec);
        print_cycles_MM("ref", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_ref);
        print_ratio_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, ratio);

        if(result_ref != result_vec)
        {
            *success_flag = false;
            print_error_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes);
            /*
            Matrix *eq = equalM(result_ref, result_vec);

            int size = getsizeM(eq);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val = 0;
                indexM(eq, &eq_val, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val == 1)
                {
                    continue;
                }

                // Show the differences
                switch (gettypeM(x))
                {
                    case INTEGER: ;
                        int x_val_i = 0;
                        indexM(x, &x_val_i, 1, index);
                        fprintf(stdout,"x   val: %d\n", x_val_i);
                        break;

                    case DOUBLE: ;
                        double x_val_d = 0;
                        indexM(x, &x_val_d, 1, index);
                        fprintf(stdout,"x   val: %.20f\n", x_val_d);
                        break;

                    case COMPLEX: ;
                        double complex x_val_c = 0;
                        indexM(x, &x_val_c, 1, index);
                        fprintf(stdout,"x   val: %.20f + %.20f*I\n", creal(x_val_c), cimag(x_val_c));
                        break;
                }

                switch (gettypeM(y))
                {
                    case INTEGER: ;
                        int y_val_i = 0;
                        indexM(y, &y_val_i, 1, index);
                        fprintf(stdout,"y   val: %d\n", y_val_i);
                        break;

                    case DOUBLE: ;
                        double y_val_d = 0;
                        indexM(y, &y_val_d, 1, index);
                        fprintf(stdout,"y   val: %.20f\n", y_val_d);
                        break;

                    case COMPLEX: ;
                        double complex y_val_c = 0;
                        indexM(y, &y_val_c, 1, index);
                        fprintf(stdout,"y   val: %.20f + %.20f*I\n", creal(y_val_c), cimag(y_val_c));
                        break;
                }

                switch (gettypeM(result_ref))
                {
                    case INTEGER: ;
                        int ref_val_i = 0;
                        indexM(result_ref, &ref_val_i, 1, index);
                        fprintf(stdout,"Ref val: %d\n", ref_val_i);
                        break;

                    case DOUBLE: ;
                        double ref_val_d = 0;
                        indexM(result_ref, &ref_val_d, 1, index);
                        fprintf(stdout,"Ref val: %.20f\n", ref_val_d);
                        break;

                    case COMPLEX: ;
                        double complex ref_val_c = 0;
                        indexM(result_ref, &ref_val_c, 1, index);
                        fprintf(stdout,"Ref val: %.20f + %.20f*I\n", creal(ref_val_c), cimag(ref_val_c));
                        break;
                }

                switch (gettypeM(result_vec))
                {
                    case INTEGER: ;
                        int vec_val_i = 0;
                        indexM(result_vec, &vec_val_i, 1, index);
                        fprintf(stdout,"Vec val: %d\n", vec_val_i);
                        break;

                    case DOUBLE: ;
                        double vec_val_d = 0;
                        indexM(result_vec, &vec_val_d, 1, index);
                        fprintf(stdout,"Vec val: %.20f\n", vec_val_d);
                        break;

                    case COMPLEX: ;
                        double complex vec_val_c = 0;
                        indexM(result_vec, &vec_val_c, 1, index);
                        fprintf(stdout,"Vec val: %.20f + %.20f*I\n", creal(vec_val_c), cimag(vec_val_c));
                        break;
                }
                fprintf(stdout,"\n");
            }

            destroyM(eq);
            */
        }
    }
    else
    {
        print_table_field(elapsed_vec, elapsed_ref, ratio);
    }
    return(ratio);
}

void print_ratio_MM_three_compare_ref_blas(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio)
{
    fprintf(stdout,"%sM;     matrix 1 type: %s; matrix 2 type: %s; size: (%d, %d); %s ; reference time / blas time: %.20f\n", op_name, type1, type2, s1, s2, notes, ratio);
}

void print_ratio_MM_three_compare_vec_blas(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio)
{
    fprintf(stdout,"%sM;     matrix 1 type: %s; matrix 2 type: %s; size: (%d, %d); %s ; vectorized time / blas time: %.20f\n", op_name, type1, type2, s1, s2, notes, ratio);
}

void print_table_field_three_compare(double elapsed_vec, double elapsed_ref, double elapsed_blas, double ref_blas_ratio, double vec_ref_ratio, double vec_blas_ratio)
{
    fprintf(stdout,"%.0f;\t%.0f;\t%.0f;\t%.5f;\t%.5f;\t%.5f;\t", elapsed_vec, elapsed_ref, elapsed_blas, ref_blas_ratio, vec_ref_ratio, vec_blas_ratio);
}

double function_comparison_MM_three_compare(char *op_name, 
                            Matrix* (*naive_func_ref)(Matrix*, Matrix*), 
                            Matrix* (*naive_func_vec)(Matrix*, Matrix*), 
                            Matrix* (*blas_func)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag)
{
    int mat_type1 = gettypeM(x);
    int mat_type2 = gettypeM(y);
    int *dims = getdimsM(x);
    int s1 = dims[0];
    int s2 = dims[1];

    const char * type_names[] = {"integer", "double", "complex"};

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_ref = (*naive_func_ref)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_vec = (*naive_func_vec)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the blas time (in cycles)
    unsigned long start_blas = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_blas = (*blas_func)(x, y);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_blas = get_cycles();
    double elapsed_blas = elapsed_cycles(start_blas, end_blas);

    // Get the ratio
    double vec_ref_ratio = elapsed_vec/elapsed_ref;

    // Get the BLAS ratio
    double ref_blas_ratio = elapsed_ref/elapsed_blas;
    double vec_blas_ratio = elapsed_vec/elapsed_blas;

    if (accuracy_check)
    {
        // Print it out
        print_cycles_MM("vec", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_vec);
        print_cycles_MM("ref", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_ref);
        print_cycles_MM("bls", op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, elapsed_blas);
        print_ratio_MM_three_compare_ref_blas(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, ref_blas_ratio);
        print_ratio_MM_three_compare_vec_blas(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, vec_blas_ratio);
        print_ratio_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes, vec_ref_ratio);

        if(!isEqualM(result_ref, result_vec) || !isEqualM(result_ref, result_blas))
        {
            *success_flag = false;
            print_error_MM(op_name, type_names[mat_type1], type_names[mat_type2], s1, s2, notes);
            
            // FILE *blas_fptr = fopen("mtimes_vec_test/blas_output.txt","a");
            // fprintM(blas_fptr, result_blas);
            // fclose(blas_fptr);

            // FILE *ref_fptr = fopen("mtimes_vec_test/ref_output.txt","a");
            // fprintM(ref_fptr, result_ref);
            // fclose(ref_fptr);

            // FILE *vec_fptr = fopen("mtimes_vec_test/vec_output.txt","a");
            // fprintM(vec_fptr, result_vec);
            // fclose(vec_fptr);
            
            Matrix *eq_ref_vec = equalM(result_ref, result_vec);
            Matrix *eq_ref_blas = equalM(result_ref, result_blas);

            int size = getsizeM(eq_ref_vec);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val_ref_vec = 0;
                indexM(eq_ref_vec, &eq_val_ref_vec, 1, index);

                int eq_val_ref_blas = 0;
                indexM(eq_ref_blas, &eq_val_ref_blas, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val_ref_vec == 1 && eq_val_ref_blas == 1)
                {
                    continue;
                }

                // Show the differences
                switch (gettypeM(result_ref))
                {
                    case INTEGER: ;
                        int ref_val_i = 0;
                        indexM(result_ref, &ref_val_i, 1, index);
                        fprintf(stdout,"Ref val: %d\n", ref_val_i);
                        break;

                    case DOUBLE: ;
                        double ref_val_d = 0;
                        indexM(result_ref, &ref_val_d, 1, index);
                        fprintf(stdout,"Ref val: %.20f\n", ref_val_d);
                        break;

                    case COMPLEX: ;
                        double complex ref_val_c = 0;
                        indexM(result_ref, &ref_val_c, 1, index);
                        fprintf(stdout,"Ref val: %.20f + %.20f*I\n", creal(ref_val_c), cimag(ref_val_c));
                        break;
                }

                switch (gettypeM(result_vec))
                {
                    case INTEGER: ;
                        int vec_val_i = 0;
                        indexM(result_vec, &vec_val_i, 1, index);
                        fprintf(stdout,"Vec val: %d\n", vec_val_i);
                        break;

                    case DOUBLE: ;
                        double vec_val_d = 0;
                        indexM(result_vec, &vec_val_d, 1, index);
                        fprintf(stdout,"Vec val: %.20f\n", vec_val_d);
                        break;

                    case COMPLEX: ;
                        double complex vec_val_c = 0;
                        indexM(result_vec, &vec_val_c, 1, index);
                        fprintf(stdout,"Vec val: %.20f + %.20f*I\n", creal(vec_val_c), cimag(vec_val_c));
                        break;
                }
                switch (gettypeM(result_blas))
                {
                    case INTEGER: ;
                        int blas_val_i = 0;
                        indexM(result_blas, &blas_val_i, 1, index);
                        fprintf(stdout,"Bls val: %d\n", blas_val_i);
                        break;

                    case DOUBLE: ;
                        double blas_val_d = 0;
                        indexM(result_blas, &blas_val_d, 1, index);
                        fprintf(stdout,"Bls val: %.20f\n", blas_val_d);
                        break;

                    case COMPLEX: ;
                        double complex blas_val_c = 0;
                        indexM(result_blas, &blas_val_c, 1, index);
                        fprintf(stdout,"Bls val: %.20f + %.20f*I\n", creal(blas_val_c), cimag(blas_val_c));
                        break;
                }
                fprintf(stdout,"\n");
            }

            destroyM(eq_ref_vec);
            destroyM(eq_ref_blas);
        }
    }
    else
    {
        print_table_field_three_compare(elapsed_vec, elapsed_ref, elapsed_blas, ref_blas_ratio, vec_ref_ratio, vec_blas_ratio);
    }

    if (!destroyM(result_ref))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_vec))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_blas))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(vec_ref_ratio);
}

void print_cycles_M(char *flavor, char *op_name, const char *type1, int s1, int s2, char *notes, double cycles)
{
    fprintf(stdout,"%sM_%s; matrix type: %s; size: (%d, %d); %s ; time: %.0f cycles\n", op_name, flavor, type1, s1, s2, notes, cycles);
}

void print_ratio_M(char *op_name, const char *type1, int s1, int s2, char *notes, double ratio)
{
    fprintf(stdout,"%sM;     matrix type: %s; size: (%d, %d); %s ; vectorized time / reference time: %.20f\n\n", op_name, type1, s1, s2, notes, ratio);
}

void print_error_M(char *op_name, const char *type1, int s1, int s2, char *notes)
{
    fprintf(stdout,"ERROR; %s; matrix type: %s; size: (%d, %d); %s\n", op_name, type1, s1, s2, notes);
}

double function_comparison_M(char *op_name, 
                            Matrix* (*func_ref)(Matrix*), 
                            Matrix* (*func_vec)(Matrix*), 
                            Matrix *x, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag)
{
    int mat_type1 = gettypeM(x);
    int *dims = getdimsM(x);
    int s1 = dims[0];
    int s2 = dims[1];

    const char * type_names[] = {"integer", "double", "complex"};

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_ref = (*func_ref)(x);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_vec = (*func_vec)(x);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the ratio
    double ratio = elapsed_vec/elapsed_ref;

    if (accuracy_check)
    {
        // Print it out
        print_cycles_M("vec", op_name, type_names[mat_type1], s1, s2, notes, elapsed_vec);
        print_cycles_M("ref", op_name, type_names[mat_type1], s1, s2, notes, elapsed_ref);
        print_ratio_M(op_name, type_names[mat_type1], s1, s2, notes, ratio);

        if(!isEqualM(result_ref, result_vec))
        {
            *success_flag = false;
            print_error_M(op_name, type_names[mat_type1], s1, s2, notes);
            Matrix *eq = equalM(result_ref, result_vec);

            int size = getsizeM(eq);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val = 0;
                indexM(eq, &eq_val, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val == 1)
                {
                    continue;
                }

                // Show the differences
                switch (gettypeM(x))
                {
                    case INTEGER: ;
                        int x_val_i = 0;
                        indexM(x, &x_val_i, 1, index);
                        fprintf(stdout,"x   val: %d\n", x_val_i);
                        break;

                    case DOUBLE: ;
                        double x_val_d = 0;
                        indexM(x, &x_val_d, 1, index);
                        fprintf(stdout,"x   val: %.20f\n", x_val_d);
                        break;

                    case COMPLEX: ;
                        double complex x_val_c = 0;
                        indexM(x, &x_val_c, 1, index);
                        fprintf(stdout,"x   val: %.20f + %.20f*I\n", creal(x_val_c), cimag(x_val_c));
                        break;
                }

                switch (gettypeM(result_ref))
                {
                    case INTEGER: ;
                        int ref_val_i = 0;
                        indexM(result_ref, &ref_val_i, 1, index);
                        fprintf(stdout,"Ref val: %d\n", ref_val_i);
                        break;

                    case DOUBLE: ;
                        double ref_val_d = 0;
                        indexM(result_ref, &ref_val_d, 1, index);
                        fprintf(stdout,"Ref val: %.20f\n", ref_val_d);
                        break;

                    case COMPLEX: ;
                        double complex ref_val_c = 0;
                        indexM(result_ref, &ref_val_c, 1, index);
                        fprintf(stdout,"Ref val: %.20f + %.20f*I\n", creal(ref_val_c), cimag(ref_val_c));
                        break;
                }

                switch (gettypeM(result_vec))
                {
                    case INTEGER: ;
                        int vec_val_i = 0;
                        indexM(result_vec, &vec_val_i, 1, index);
                        fprintf(stdout,"Vec val: %d\n", vec_val_i);
                        break;

                    case DOUBLE: ;
                        double vec_val_d = 0;
                        indexM(result_vec, &vec_val_d, 1, index);
                        fprintf(stdout,"Vec val: %.20f\n", vec_val_d);
                        break;

                    case COMPLEX: ;
                        double complex vec_val_c = 0;
                        indexM(result_vec, &vec_val_c, 1, index);
                        fprintf(stdout,"Vec val: %.20f + %.20f*I\n", creal(vec_val_c), cimag(vec_val_c));
                        break;
                }
                fprintf(stdout,"\n");
            }

            destroyM(eq);
        }
    }
    else
    {
        print_table_field(elapsed_vec, elapsed_ref, ratio);
    }

    if (!destroyM(result_ref))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_vec))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(ratio);
}




void print_cycles_Mx(char *flavor, char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double cycles)
{
    fprintf(stdout,"%sM_%s; matrix type: %s; scalar type: %s; size: (%d, %d); %s ; time: %.0f cycles\n", op_name, flavor, type1, type2, s1, s2, notes, cycles);
}

void print_ratio_Mx(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio)
{
    fprintf(stdout,"%sM;     matrix type: %s; scalar type: %s; size: (%d, %d); %s ; vectorized time / reference time: %.20f\n\n", op_name, type1, type2, s1, s2, notes, ratio);
}

void print_error_Mx(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes)
{
    fprintf(stdout,"ERROR; %s; matrix type: %s; scalar type: %s; size: (%d, %d); %s\n", op_name, type1, type2, s1, s2, notes);
}

double function_comparison_Mx(char *op_name, 
                            Matrix* (*func_ref)(Matrix*, void*, int), 
                            Matrix* (*func_vec)(Matrix*, void*, int), 
                            Matrix *x, 
                            void *scalar,
                            int scalar_type, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag)
{
    int mat_type1 = gettypeM(x);
    int *dims = getdimsM(x);
    int s1 = dims[0];
    int s2 = dims[1];

    const char * type_names[] = {"integer", "double", "complex"};

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_ref = (*func_ref)(x, scalar, scalar_type);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_vec = (*func_vec)(x, scalar, scalar_type);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the ratio
    double ratio = elapsed_vec/elapsed_ref;

    if (accuracy_check)
    {
        // Print it out
        print_cycles_Mx("vec", op_name, type_names[mat_type1], type_names[scalar_type], s1, s2, notes, elapsed_vec);
        print_cycles_Mx("ref", op_name, type_names[mat_type1], type_names[scalar_type], s1, s2, notes, elapsed_ref);
        print_ratio_Mx(op_name, type_names[mat_type1], type_names[scalar_type], s1, s2, notes, ratio);

        if(!isEqualM(result_ref, result_vec))
        {
            *success_flag = false;
            print_error_Mx(op_name, type_names[mat_type1], type_names[scalar_type], s1, s2, notes);
            Matrix *eq = equalM(result_ref, result_vec);

            int size = getsizeM(eq);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val = 0;
                indexM(eq, &eq_val, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val == 1)
                {
                    continue;
                }

                // Show the differences
                switch (gettypeM(x))
                {
                    case INTEGER: ;
                        int x_val_i = 0;
                        indexM(x, &x_val_i, 1, index);
                        fprintf(stdout,"x   val: %d\n", x_val_i);
                        break;

                    case DOUBLE: ;
                        double x_val_d = 0;
                        indexM(x, &x_val_d, 1, index);
                        fprintf(stdout,"x   val: %.20f\n", x_val_d);
                        break;

                    case COMPLEX: ;
                        double complex x_val_c = 0;
                        indexM(x, &x_val_c, 1, index);
                        fprintf(stdout,"x   val: %.20f + %.20f*I\n", creal(x_val_c), cimag(x_val_c));
                        break;
                }

                switch (scalar_type)
                {
                    case INTEGER: ;
                        int y_val_i = *(int *)scalar;
                        fprintf(stdout,"y   val: %d\n", y_val_i);
                        break;

                    case DOUBLE: ;
                        double y_val_d = *(double *)scalar;
                        fprintf(stdout,"y   val: %.20f\n", y_val_d);
                        break;

                    case COMPLEX: ;
                        double complex y_val_c = *(double complex *)scalar;
                        fprintf(stdout,"y   val: %.20f + %.20f*I\n", creal(y_val_c), cimag(y_val_c));
                        break;
                }

                switch (gettypeM(result_ref))
                {
                    case INTEGER: ;
                        int ref_val_i = 0;
                        indexM(result_ref, &ref_val_i, 1, index);
                        fprintf(stdout,"Ref val: %d\n", ref_val_i);
                        break;

                    case DOUBLE: ;
                        double ref_val_d = 0;
                        indexM(result_ref, &ref_val_d, 1, index);
                        fprintf(stdout,"Ref val: %.20f\n", ref_val_d);
                        break;

                    case COMPLEX: ;
                        double complex ref_val_c = 0;
                        indexM(result_ref, &ref_val_c, 1, index);
                        fprintf(stdout,"Ref val: %.20f + %.20f*I\n", creal(ref_val_c), cimag(ref_val_c));
                        break;
                }

                switch (gettypeM(result_vec))
                {
                    case INTEGER: ;
                        int vec_val_i = 0;
                        indexM(result_vec, &vec_val_i, 1, index);
                        fprintf(stdout,"Vec val: %d\n", vec_val_i);
                        break;

                    case DOUBLE: ;
                        double vec_val_d = 0;
                        indexM(result_vec, &vec_val_d, 1, index);
                        fprintf(stdout,"Vec val: %.20f\n", vec_val_d);
                        break;

                    case COMPLEX: ;
                        double complex vec_val_c = 0;
                        indexM(result_vec, &vec_val_c, 1, index);
                        fprintf(stdout,"Vec val: %.20f + %.20f*I\n", creal(vec_val_c), cimag(vec_val_c));
                        break;
                }
                fprintf(stdout,"\n");
            }

            destroyM(eq);
        }
    }
    else
    {
        print_table_field(elapsed_vec, elapsed_ref, ratio);
    }

    if (!destroyM(result_ref))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_vec))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(ratio);
}


int run_test(int argc, char *argv[], bool (*function)(int, int , bool), char *function_name, int table_type, char *comment) 
{
    bool success = true;
    bool accuracy_check = false;

    if (argc > 1)
    {
        if (!strcmp("-a", argv[1]) || !strcmp("--accuracy", argv[1]))
        {
            accuracy_check = true;
        }
    }
    
    fprintf(stdout,"\n\n%s\n\n", function_name);
    if (accuracy_check)
    {
        fprintf(stdout,"Accuracy checking: ENABLED\n");
        fprintf(stdout,"int size: %ld\n", sizeof(int));
        fprintf(stdout,"double size: %ld\n", sizeof(double));
        fprintf(stdout,"complex size: %ld\n", sizeof(double complex));
        fprintf(stdout,"Epsilon (equality threshold): %.20f\n\n", EPSILON);
    }
    else
    {
        fprintf(stdout,"Accuracy checking: DISABLED\n\n");

        // Begin table
        switch(table_type)
        {            
            case SINGLE_MATRIX:
                fprintf(stdout,"Context: %s\nSize;\tint_vec (cycles);\tint_ref (cycles);\tint_ratio (vec/ref);\tdouble_vec (cycles);\tdouble_ref (cycles);\tdouble_ratio (vec/ref);\tcomplex_vec (cycles);\tcomplex_ref (cycles);\tcomplex_ratio (vec/ref)\n", comment);
                break;

            case MATRIX_MATRIX:
                fprintf(stdout,"Context: %s\nSize;\tint_M_int_M_vec (cycles);\tint_M_int_M_ref (cycles);\tint_M_int_M_ratio (vec/ref);\tint_M_double_M_vec (cycles);\tint_M_double_M_ref (cycles);\tint_M_double_M_ratio (vec/ref);\tint_M_complex_M_vec (cycles);\tint_M_complex_M_ref (cycles);\tint_M_complex_M_ratio (vec/ref);\tdouble_M_int_M_vec (cycles);\tdouble_M_int_M_ref (cycles);\tdouble_M_int_M_ratio (vec/ref);\tdouble_M_double_M_vec (cycles);\tdouble_M_double_M_ref (cycles);\tdouble_M_double_M_ratio (vec/ref);\tdouble_M_complex_M_vec (cycles);\tdouble_M_complex_M_ref (cycles);\tdouble_M_complex_M_ratio (vec/ref);\tcomplex_M_int_M_vec (cycles);\tcomplex_M_int_M_ref (cycles);\tcomplex_M_int_M_ratio (vec/ref);\tcomplex_M_double_M_vec (cycles);\tcomplex_M_double_M_ref (cycles);\tcomplex_M_double_M_ratio (vec/ref);\tcomplex_M_complex_M_vec (cycles);\tcomplex_M_complex_M_ref (cycles);\tcomplex_M_complex_M_ratio (vec/ref);\t\n", comment);
                break;

            case MATRIX_SCALAR:
                fprintf(stdout,"Context: %s\nSize;\tint_M_int_S_vec (cycles);\tint_M_int_S_ref (cycles);\tint_M_int_S_ratio (vec/ref);\tint_M_double_S_vec (cycles);\tint_M_double_S_ref (cycles);\tint_M_double_S_ratio (vec/ref);\tint_M_complex_S_vec (cycles);\tint_M_complex_S_ref (cycles);\tint_M_complex_S_ratio (vec/ref);\tdouble_M_int_S_vec (cycles);\tdouble_M_int_S_ref (cycles);\tdouble_M_int_S_ratio (vec/ref);\tdouble_M_double_S_vec (cycles);\tdouble_M_double_S_ref (cycles);\tdouble_M_double_S_ratio (vec/ref);\tdouble_M_complex_S_vec (cycles);\tdouble_M_complex_S_ref (cycles);\tdouble_M_complex_S_ratio (vec/ref);\tcomplex_M_int_S_vec (cycles);\tcomplex_M_int_S_ref (cycles);\tcomplex_M_int_S_ratio (vec/ref);\tcomplex_M_double_S_vec (cycles);\tcomplex_M_double_S_ref (cycles);\tcomplex_M_double_S_ratio (vec/ref);\tcomplex_M_complex_S_vec (cycles);\tcomplex_M_complex_S_ref (cycles);\tcomplex_M_complex_S_ratio (vec/ref);\t\n", comment);
                break;

            case ONES:
                fprintf(stdout,"Context: %s\nSize;\tint_vec (cycles);\tint_ref (cycles);\tint_ratio (vec/ref)\n", comment);
                break;

            case MATRIX_MATRIX_REF:
                fprintf(stdout,"Context: %s\nSize;\tint_M_int_M_vec (cycles);\tint_M_int_M_ref (cycles);\tint_M_int_M_bls (cycles);\tint_M_int_M_ratio (ref/blas);\tint_M_int_M_ratio (vec/ref);\tint_M_int_M_ratio (vec/blas);\tint_M_double_M_vec (cycles);\tint_M_double_M_ref (cycles);\tint_M_double_M_bls (cycles);\tint_M_double_M_ratio (ref/blas);\tint_M_double_M_ratio (vec/ref);\tint_M_double_M_ratio (vec/blas);\tint_M_complex_M_vec (cycles);\tint_M_complex_M_ref (cycles);\tint_M_complex_M_bls (cycles);\tint_M_complex_M_ratio (ref/blas);\tint_M_complex_M_ratio (vec/ref);\tint_M_complex_M_ratio (vec/blas);\tdouble_M_int_M_vec (cycles);\tdouble_M_int_M_ref (cycles);\tdouble_M_int_M_bls (cycles);\tdouble_M_int_M_ratio (ref/blas);\tdouble_M_int_M_ratio (vec/ref);\tdouble_M_int_M_ratio (vec/blas);\tdouble_M_double_M_vec (cycles);\tdouble_M_double_M_ref (cycles);\tdouble_M_double_M_bls (cycles);\tdouble_M_double_M_ratio (ref/blas);\tdouble_M_double_M_ratio (vec/ref);\tdouble_M_double_M_ratio (vec/blas);\tdouble_M_complex_M_vec (cycles);\tdouble_M_complex_M_ref (cycles);\tdouble_M_complex_M_bls (cycles);\tdouble_M_complex_M_ratio (ref/blas);\tdouble_M_complex_M_ratio (vec/ref);\tdouble_M_complex_M_ratio (vec/blas);\tcomplex_M_int_M_vec (cycles);\tcomplex_M_int_M_ref (cycles);\tcomplex_M_int_M_bls (cycles);\tcomplex_M_int_M_ratio (ref/blas);\tcomplex_M_int_M_ratio (vec/ref);\tcomplex_M_int_M_ratio (vec/blas);\tcomplex_M_double_M_vec (cycles);\tcomplex_M_double_M_ref (cycles);\tcomplex_M_double_M_bls (cycles);\tcomplex_M_double_M_ratio (ref/blas);\tcomplex_M_double_M_ratio (vec/ref);\tcomplex_M_double_M_ratio (vec/blas);\tcomplex_M_complex_M_vec (cycles);\tcomplex_M_complex_M_ref (cycles);\tcomplex_M_complex_M_bls (cycles);\tcomplex_M_complex_M_ratio (ref/blas);\tcomplex_M_complex_M_ratio (vec/ref);\tcomplex_M_complex_M_ratio (vec/blas);\t\n", comment);
                break;
        }
    }

    int small_sizes[] = {1, 2, 3};
    int medium_sizes[] = {127, 128, 129};
    int large_sizes[] = {1023, 1024, 1025};
    size_t small_s = sizeof(small_sizes)/sizeof(small_sizes[0]);
    size_t medium_s = sizeof(medium_sizes)/sizeof(medium_sizes[0]);
    size_t large_s = sizeof(large_sizes)/sizeof(large_sizes[0]);

    unsigned long start_program = get_cycles();
    for(int i = 0 ; i < small_s ; i++)
    {
        for(int j = i ; j < small_s ; j++)
        { 
            if (accuracy_check)
            {
                fprintf(stdout,"\nSize: (%d,%d)\n", small_sizes[i], small_sizes[j]);
            }
            if(!(*function)(small_sizes[i], small_sizes[j], accuracy_check)){success = false; goto BREAK;}
        }
    }

    if (table_type == MATRIX_MATRIX_REF)
    {
        if (accuracy_check)
        {
            fprintf(stdout,"\nSize: (%d,%d)\n", 31, 31);
        }
        if(!(*function)(31, 31, accuracy_check)){success = false;}
        if (accuracy_check)
        {
            fprintf(stdout,"\nSize: (%d,%d)\n", 32, 32);
        }
        if(!(*function)(32, 32, accuracy_check)){success = false;}
        if (accuracy_check)
        {
            fprintf(stdout,"\nSize: (%d,%d)\n", 33, 33);
        }
        if(!(*function)(33, 33, accuracy_check)){success = false;}

        for(int i = 0 ; i < medium_s ; i++)
        {
            for(int j = i ; j < medium_s ; j++)
            { 
                if (accuracy_check)
                {
                    fprintf(stdout,"\nSize: (%d,%d)\n", medium_sizes[i], medium_sizes[j]);
                }
                if(!(*function)(medium_sizes[i], medium_sizes[j], accuracy_check)){success = false;}
            }
        }
        goto BREAK;
    }

    for(int i = 0 ; i < medium_s ; i++)
    {
        for(int j = i ; j < medium_s ; j++)
        { 
            if (accuracy_check)
            {
                fprintf(stdout,"\nSize: (%d,%d)\n", medium_sizes[i], medium_sizes[j]);
            }
            if(!(*function)(medium_sizes[i], medium_sizes[j], accuracy_check)){success = false; goto BREAK;}
        }
    }

    if (table_type == MATRIX_MATRIX_REF){goto BREAK;}

    for(int i = 0 ; i < large_s ; i++)
    {
        for(int j = i ; j < large_s ; j++)
        { 
            if (accuracy_check)
            {
                fprintf(stdout,"\nSize: (%d,%d)\n", large_sizes[i], large_sizes[j]);
            }
            if(!(*function)(large_sizes[i], large_sizes[j], accuracy_check)){success = false; goto BREAK;}
        }
    }
    

    BREAK: ;
    unsigned long end_program = get_cycles();
    double elapsed_program = elapsed_cycles(start_program, end_program);
    fprintf(stdout,"\n\n\nTotal program runtime: %.0f cycles\n", elapsed_program);

    if (accuracy_check)
    {
        if (success)
        {
            fprintf(stdout, "Accuracy test: PASSED\n");
        }
        else
        {
            fprintf(stdout, "Accuracy test: FAILED\n");
        }
    }
    return(success);
}