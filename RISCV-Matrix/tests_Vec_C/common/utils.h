//
//  utils.h
//  Octave-C
//
//  Created by Zach Taylor on 12/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../../src/tgmath.h"
#include "../../src/matrix.h"

#define SINGLE_MATRIX     0
#define MATRIX_MATRIX     1
#define MATRIX_SCALAR     2
#define ONES              3
#define MATRIX_MATRIX_REF 4

void print_table_size(int s1, int s2);
void print_table_field(double elapsed_vec, double elapsed_ref, double ratio);
void print_table_newline();

void print_cycles_MM(char *flavor, char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double cycles);
void print_ratio_MM(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio);
void print_error_MM(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes);

void print_cycles_M(char *flavor, char *op_name, const char *type1, int s1, int s2, char *notes, double cycles);
void print_ratio_M(char *op_name, const char *type1, int s1, int s2, char *notes, double ratio);
void print_error_M(char *op_name, const char *type1, int s1, int s2, char *notes);

void print_cycles_Mx(char *flavor, char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double cycles);
void print_ratio_Mx(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes, double ratio);
void print_error_Mx(char *op_name, const char *type1, const char *type2, int s1, int s2, char *notes);


int run_test(int argc, char *argv[], bool (*function)(int, int , bool), char *function_name, int table_type, char *comment);

double function_comparison_MM(char *op_name, 
                            Matrix* (*func_ref)(Matrix*, Matrix*), 
                            Matrix* (*func_vec)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag);

double function_comparison_MM_three_compare(char *op_name, 
                            Matrix* (*naive_func_ref)(Matrix*, Matrix*), 
                            Matrix* (*naive_func_vec)(Matrix*, Matrix*), 
                            Matrix* (*blas_func)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag);

double function_comparison_MM_bb(char *op_name, 
                            bool (*func_ref)(Matrix*, Matrix*), 
                            bool (*func_vec)(Matrix*, Matrix*), 
                            Matrix *x, 
                            Matrix *y, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag);

double function_comparison_M(char *op_name, 
                            Matrix* (*func_ref)(Matrix*), 
                            Matrix* (*func_vec)(Matrix*), 
                            Matrix *x, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag);

double function_comparison_Mx(char *op_name, 
                            Matrix* (*func_ref)(Matrix*, void*, int), 
                            Matrix* (*func_vec)(Matrix*, void*, int), 
                            Matrix *x, 
                            void *scalar,
                            int scalar_type, 
                            char *notes,
                            bool accuracy_check, 
                            bool *success_flag);