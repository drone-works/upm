%module pyupm_bh1745nuc
%include "../upm.i"
%include "cpointer.i"

/* Send "uint16_t *" and "bool *" to python as uint16p and boolp */
%pointer_functions(uint16_t, uint16p);
%pointer_functions(bool, boolp);

%feature("autodoc", "3");

%include "bh1745nuc.h"
%{
    #include "bh1745nuc.h"
%}

%constant void (*DefaultISR)(void *) = upm::BH1745NUC::defaultISR;
