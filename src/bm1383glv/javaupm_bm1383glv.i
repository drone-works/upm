%module javaupm_bm1383glv
%include "../upm.i"

%{
    #include "bm1383glv.h"
%}
%include "bm1383glv.h"

%ignore installISR(int gpio, void (*isr)(char*), void* arg);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_bm1383glv");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
