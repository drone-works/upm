%module pyupm_bm1383glv
%include "../upm.i"
%include "cpointer.i"

/* Send "float *" and "bool *" to python as floatp and boolp */
%pointer_functions(float, floatp);
%pointer_functions(bool, boolp);

%feature("autodoc", "3");

%include "bm1383glv.h"
%{
    #include "bm1383glv.h"
%}

%constant void (*DefaultISR)(void *) = upm::BM1383GLV::defaultISR;
