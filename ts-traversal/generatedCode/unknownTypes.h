#ifndef UNKNOWNTYPES_H
#define UNKNOWNTYPES_H

// Structure for unknown types
struct generic_val {
    int type;
    union {
        int ival;
        double dval;
        complex double cval;
        char chval[50];
    } data;
};

// Function declarations
void fillVal(struct generic_val *, int, int *);
void unknown_indexM(struct generic_val *, int, Matrix *, int, int);
void unknown_printf(struct generic_val *);
#endif

