//Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "./unknownTypes.h"

void fillVal(struct generic_val *gv, int type, int *data) {
    gv->type = type;
    if (type == 0) {
        gv->data.ival = *data;
    } else if (type == 1) {
        gv->data.dval = *data;
    } else if (type == 2) {
        gv->data.cval = *data;
    }
    return;
}

// TO DO: SUPPORT FOR idx = ..., i.e., va_list
void unknown_indexM(struct generic_val *gv, int type, Matrix *m, int ndim, int idx) {
    gv->type = type;
    if (type == 0) {
        indexM(m, &gv->data.ival, ndim, idx);
    } else if (type == 1) {
        indexM(m, &gv->data.dval, ndim, idx);
    } else if (type == 2) {
        indexM(m, &gv->data.cval, ndim, idx);
    }
    
    return;
}

void unknown_printf(struct generic_val *gv) {
                
    if (gv->type == 0) {
        printf("\n%d\n", gv->data.ival);
    } else if (gv->type == 1) {
        printf("\n%.4f\n", gv->data.dval);
    } else if (gv->type == 2) {
        printf("\n%.4f + %.4fi\n", creal(gv->data.cval), cimag(gv->data.cval));
    }
    
    return;
}