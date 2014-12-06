set LIBS=%1%
%NACL_SDK_ROOT%/tools/sel_ldr_x86_64 -a -c -B %NACL_SDK_ROOT%/tools/irt_core_x86_64.nexe -- %LIBS%\runnable-ld.so --library-path %LIBS% %2%