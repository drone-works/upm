%module javaupm_bh1745nuc
%include "../upm.i"

%{
    #include "bh1745nuc.h"
%}
%include "bh1745nuc.h"

%ignore installISR(int gpio, void (*isr)(char*), void* arg);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_bh1745nuc");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
