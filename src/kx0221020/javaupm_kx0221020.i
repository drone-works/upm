%module javaupm_kx0221020
%include "../upm.i"

%{
    #include "kx0221020.h"
%}
%include "kx0221020.h"

%ignore installISR(int gpio, void (*isr)(char*), void* arg);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_kx0221020");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
