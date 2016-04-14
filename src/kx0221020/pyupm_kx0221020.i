%module pyupm_kx0221020
%include "../upm.i"
%include "cpointer.i"

/* Send "float *" and "bool *" to python as floatp and boolp */
%pointer_functions(float, floatp);
%pointer_functions(bool, boolp);
%feature("autodoc", "3");

%include "kx0221020.h"
%{
    #include "kx0221020.h"
%}

%constant void (*DefaultISR)(void *) = upm::KX0221020::defaultISR;
