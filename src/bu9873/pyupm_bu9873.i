%module pyupm_bu9873
%include "../upm.i"

%feature("autodoc", "3");

%include "bu9873.h"
%{
    #include "bu9873.h"
%}
