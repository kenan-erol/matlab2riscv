//
//  pairwise_min_vec_test.c
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

bool pairwise_min_vec_test(int s1, int s2, bool accuracy_check)
{
    bool success = true;

    int ndim = 2;
    int dim[2] = {s1,s2};

    int size = 1;
    for (int i = 0 ; i < ndim ; i++)
    {
        size *= dim[i];
    }

    
    // Create an int matrix (of ones)
    Matrix *int_matrix1 = onesM(ndim, dim);
    // printM(int_matrix);

    // Create another int matrix
    Matrix *int_matrix2 = createM(ndim, dim, INTEGER);
    if ( int_matrix2 == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    int *int_input2 = NULL;
    int_input2 = malloc(size*sizeof(*int_input2));
    for (int i = 0 ; i < size ; i++)
    {
        int_input2[i] = ((int) pow(-1, i))*(i);
    }
    writeM(int_matrix2, size, int_input2);
    free(int_input2);
    // printM(int_matrix2);

    // Create a double matrix
    Matrix *double_matrix1 = createM(ndim, dim, DOUBLE);
    if ( double_matrix1 == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double *double_input1 = NULL;
    double_input1 = malloc(size*sizeof(*double_input1));
    for (int i = 0 ; i < size ; i++)
    {
        double_input1[i] = ((int) pow(-1, i-1))*(i);
    }
    writeM(double_matrix1, size, double_input1);
    free(double_input1);
    // printM(double_matrix1);


    // Create another double matrix
    Matrix *double_matrix2 = createM(ndim, dim, DOUBLE);
    if ( double_matrix2 == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double *double_input2 = NULL;
    double_input2 = malloc(size*sizeof(*double_input2));
    for (int i = 0 ; i < size ; i++)
    {
        double_input2[i] = sqrt(i+1);
    }
    writeM(double_matrix2, size, double_input2);
    free(double_input2);
    // printM(double_matrix2);

    // Create a complex matrix
    Matrix *complex_matrix1 = createM(ndim, dim, COMPLEX);
    if ( complex_matrix1 == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double complex *complex_input1 = NULL;
    complex_input1 = malloc(size*sizeof(*complex_input1));
    for (int i = 0 ; i < size ; i++)
    {
        complex_input1[i] = (i+1)+(i+1)*I;
    }
    writeM(complex_matrix1, size, complex_input1);
    free(complex_input1);
    // printM(complex_matrix1);


    // Create another complex matrix
    Matrix *complex_matrix2 = createM(ndim, dim, COMPLEX);
    if ( complex_matrix2 == NULL )
    {
        fprintf(stderr, "Could not create matrix\n");
        return(false);
    }
    double complex *complex_input2 = NULL;
    complex_input2 = malloc(size*sizeof(*complex_input2));
    for (int i = 0 ; i < size ; i++)
    {
        complex_input2[i] = (((double) pow(-1, i))*(i+1)/7) + (((double) pow(-1, i+1))*1/(i+1)/7)*I;
    }
    writeM(complex_matrix2, size, complex_input2);
    free(complex_input2);
    // printM(complex_matrix2);

    char notes[] = "";

    if(!accuracy_check){print_table_size(s1, s2);}
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, int_matrix1, int_matrix2, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, int_matrix1, double_matrix1, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, int_matrix1, complex_matrix1, notes, accuracy_check, &success);

    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, double_matrix1, int_matrix1, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, double_matrix1, double_matrix2, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, double_matrix1, complex_matrix1, notes, accuracy_check, &success);

    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, complex_matrix1, int_matrix1, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, complex_matrix1, double_matrix1, notes, accuracy_check, &success);
    function_comparison_MM("pairwise_min", pairwise_minM_ref, pairwise_minM_vec, complex_matrix1, complex_matrix2, notes, accuracy_check, &success);
    if(!accuracy_check){print_table_newline();}


    // Cleanup
    if (!destroyM(int_matrix1))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(int_matrix2))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(double_matrix1))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(double_matrix2))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(complex_matrix1))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(complex_matrix2))
    {
        success = false;
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    return(success);
}

int main(int argc, char *argv[]) 
{
    char *comment="Normal";
    return(run_test(argc, argv, pairwise_min_vec_test,"pairwise_minM", MATRIX_MATRIX, comment));
}
