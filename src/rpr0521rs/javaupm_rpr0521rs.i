%module javaupm_rpr0521rs
%include "../upm.i"

%{
    #include "rpr0521rs.h"
%}
%include "rpr0521rs.h"

%ignore installISR(int gpio, void (*isr)(char*), void* arg);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_rpr0521rs");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
