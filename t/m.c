#define macro0(arg0) 0+arg0
#define macro1(arg0,arg1) macro0(arg0)+macro0(arg1)
#define macro2(arg0,arg1,arg2) macro1(arg0,arg1)+macro1(arg1,arg2)
int a = macro2(1,2,3);

#define macro_name1(arg0) 1+arg0
#define call(fn,arg) macro_##fn

int b = call(name1,2)(2);

