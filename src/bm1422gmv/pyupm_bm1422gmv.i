%module pyupm_bm1422gmv
%include "../upm.i"
%include "cpointer.i"

/* Send "float *" and "bool *" to python as floatp and boolp */
%pointer_functions(float, floatp);
%pointer_functions(bool, boolp);

%feature("autodoc", "3");

%include "bm1422gmv.h"
%{
    #include "bm1422gmv.h"
%}

%constant void (*DefaultISR)(void *) = upm::BM1422GMV::defaultISR;
