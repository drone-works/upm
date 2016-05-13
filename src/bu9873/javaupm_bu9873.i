%module javaupm_bu9873
%include "../upm.i"
%include "arrays_java.i";
%include "../java_buffer.i"

%{
    #include "bu9873.h"
%}
%include "bu9873.h"


%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_bu9873");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
