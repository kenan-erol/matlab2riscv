//
//  scale_vec_test.c
//  Octave-C
//
//  Created by Zach Taylor on 12/01/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../../src/tgmath.h"
#include "../../src/matrix.h"

#include "../common/utils.h"

bool scale_vec_test(int s1, int s2, bool accuracy_check)
{
    bool success = true;

    int ndim = 2;
    int dim[2] = {s1,s2};

    int size = 1;
    for (int i = 0 ; i < ndim ; i++)
    {
        size *= dim[i];
    }

    // Create a int matrix (of ones)
    Matrix *int_matrix = onesM(ndim, dim);
    // printM(int_matrix);

    // Create a double matrix
    Matrix *double_matrix = createM(ndim, dim, DOUBLE);
    if ( double_matrix == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double *double_input = NULL;
    double_input = malloc(size*sizeof(*double_input));
    for (int i = 0 ; i < size ; i++)
    {
        double_input[i] = (i+1);
    }
    writeM(double_matrix, size, double_input);
    free(double_input);
    // printM(double_matrix);


    // Create a complex matrix
    Matrix *complex_matrix = createM(ndim, dim, COMPLEX);
    if ( complex_matrix == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double complex *complex_input = NULL;
    complex_input = malloc(size*sizeof(*complex_input));
    for (int i = 0 ; i < size ; i++)
    {
        complex_input[i] = (i+1)+(i+1)*I;
    }
    writeM(complex_matrix, size, complex_input);
    free(complex_input);
    // printM(complex_matrix);

    // Scalars
    int scalar_int = 2;
    double scalar_double = 2.1;
    double complex scalar_complex = 2.14 + I; 

    char notes[] = "";
    char no_overflow[] = "(Non-Overflow)";
    

    if(!accuracy_check){print_table_size(s1, s2);}
    // Multiply the int matrix by different scalars
    // function_comparison_Mx("scale", scaleM_ref, scaleM_vec, 
    Matrix *mat = int_matrix;
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_int, INTEGER, no_overflow, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_double, DOUBLE, notes, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_complex, COMPLEX, notes, accuracy_check, &success);

    // Multiply the double matrix by different scalars
    mat = double_matrix;
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_int, INTEGER, notes, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_double, DOUBLE, notes, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_complex, COMPLEX, notes, accuracy_check, &success);

    // Multiply the complex matrix by different scalars
    mat = complex_matrix;
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_int, INTEGER, notes, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_double, DOUBLE, notes, accuracy_check, &success);
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, mat, &scalar_complex, COMPLEX, notes, accuracy_check, &success);
    if(!accuracy_check){print_table_newline();}

    // Cleanup
    if (!destroyM(int_matrix))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(double_matrix))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(complex_matrix))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    return(success);
}

bool scale_vec_test_overflow(int s1, int s2, bool accuracy_check)
{
    bool success = true;

    int ndim = 2;
    int dim[2] = {s1,s2};

    int size = 1;
    for (int i = 0 ; i < ndim ; i++)
    {
        size *= dim[i];
    }

    // Create a int matrix (of ones)
    Matrix *int_matrix = onesM(ndim, dim);
    // printM(int_matrix);

    // Scalars
    int scalar_int = 2;
    int max = INT_MAX;
    int min = INT_MIN;

    char overflow[] = "(Overflow)";
    
    // Overflow and underflow
    Matrix *overflow_input = scaleM_ref(int_matrix, &scalar_int, INTEGER);

    if(!accuracy_check){print_table_size(s1, s2);}
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, overflow_input, &max, INTEGER, overflow, accuracy_check, &success);
    if(!accuracy_check){print_table_newline();}

    if(!accuracy_check){print_table_size(s1, s2);}
    function_comparison_Mx("scale", scaleM_ref, scaleM_vec, overflow_input, &min, INTEGER, overflow, accuracy_check, &success);
    if(!accuracy_check){print_table_newline();}

    // Cleanup
    if (!destroyM(int_matrix))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(overflow_input))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(success);
}


int main(int argc, char *argv[]) 
{
    bool success = true;

    char *comment="Normal";
    if(!run_test(argc, argv, scale_vec_test,"scaleM", MATRIX_SCALAR, comment)){success = false;}

    comment = "Overflow";
    if(!run_test(argc, argv, scale_vec_test_overflow,"scaleM", MATRIX_SCALAR, comment)){success = false;}

    return(success);
}