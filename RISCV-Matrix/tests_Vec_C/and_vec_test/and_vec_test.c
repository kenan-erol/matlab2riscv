//
//  and_vec_test.c
//  Octave-C
//
//  Created by Zach Taylor on 12/12/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../../src/tgmath.h"
#include "../../src/matrix.h"

#include "../common/utils.h"


bool and_vec_test(int s1, int s2, bool accuracy_check)
{
    bool success = true;

    int ndim = 2;
    int dim[2] = {s1,s2};

    int size = 1;
    for (int i = 0 ; i < ndim ; i++)
    {
        size *= dim[i];
    }

    double one_d = 1.0;
    double complex one_c = 1.0 + I;

    // Create matrices of zeros & ones
    Matrix *int_mask = createM(ndim, dim, INTEGER);
    if ( int_mask == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    int *mask_input = NULL;
    mask_input = malloc(size*sizeof(*mask_input));
    for (int i = 0 ; i < size ; i++)
    {
        mask_input[i] = (i+1)%2;
    }
    writeM(int_mask, size, mask_input);
    free(mask_input);
    // printM(int_mask);

    Matrix *double_mask = scaleM(int_mask, &one_d, DOUBLE);
    Matrix *complex_mask = scaleM(int_mask, &one_c, COMPLEX);


    
    // Create an int matrix
    Matrix *int_matrix = createM(ndim, dim, INTEGER);
    if ( int_matrix == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    int *int_input = NULL;
    int_input = malloc(size*sizeof(*int_input));
    for (int i = 0 ; i < size ; i++)
    {
        int_input[i] = ((int) pow(-1, i))*((i+1)%3);
    }
    writeM(int_matrix, size, int_input);
    free(int_input);
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
        double_input[i] = (i+1)%3;
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
        complex_input[i] = ((i+1)%3)+((i+1)%3)*I;
    }
    writeM(complex_matrix, size, complex_input);
    free(complex_input);
    // printM(complex_matrix);

    char notes[] = "";

    Matrix *mat = int_matrix;
    if(!accuracy_check){print_table_size(s1, s2);}
    function_comparison_MM("and", andM_ref, andM_vec, mat, int_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, double_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, complex_mask, notes, accuracy_check, &success);

    mat = double_matrix;
    function_comparison_MM("and", andM_ref, andM_vec, mat, int_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, double_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, complex_mask, notes, accuracy_check, &success);

    mat = complex_matrix;
    function_comparison_MM("and", andM_ref, andM_vec, mat, int_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, double_mask, notes, accuracy_check, &success);
    function_comparison_MM("and", andM_ref, andM_vec, mat, complex_mask, notes, accuracy_check, &success);
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
    if (!destroyM(int_mask))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(double_mask))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(complex_mask))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    return(success);
}

int main(int argc, char *argv[]) 
{
    char *comment="Normal";
    return(run_test(argc, argv, and_vec_test,"andM", MATRIX_MATRIX, comment));
}
