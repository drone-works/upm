%module pyupm_rpr0521rs
%include "../upm.i"
%include "cpointer.i"

/* Send "uint16_t *" and "bool *" to python as uint16p and boolp */
%pointer_functions(uint16_t, uint16p);
%pointer_functions(bool, boolp);

%feature("autodoc", "3");

%include "rpr0521rs.h"
%{
    #include "rpr0521rs.h"
%}

%constant void (*DefaultISR)(void *) = upm::RPR0521RS::defaultISR;
