#include "h1.h"

#define macro0(arg0) 0+arg0
#define macro1(arg0,arg1) macro0(arg0)+macro0(arg1)
#define macro2(arg0,arg1,arg2) macro1(arg0,arg1)+macro1(arg1,arg2)
#define argexp(a) a+1
int a = macro2(argexp(1),2,3);

#define macro_name1(arg0) 1+arg0
#define call(fn,arg) macro_##fn

#include "h2.h"

int b = call(name1,2)(2);

#define a 1
#if a == 1
int c = 1;
#else
int c = 0;
#endif
