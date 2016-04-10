%module javaupm_bm1422gmv
%include "../upm.i"

%{
    #include "bm1422gmv.h"
%}
%include "bm1422gmv.h"

%ignore installISR(int gpio, void (*isr)(char*), void* arg);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_bm1422gmv");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
