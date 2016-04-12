%module pyupm_bh1745nuc
%include "../upm.i"

%feature("autodoc", "3");

%include "bh1745nuc.h"
%{
    #include "bh1745nuc.h"
%}
