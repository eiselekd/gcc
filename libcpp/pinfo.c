#include <Python.h>
#include "pinfo.h"

static PyObject* helloworld(PyObject* self)
{
  return Py_BuildValue("s", "Hello, Python extensions!!");
}

static char helloworld_docs[] =
  "helloworld( ): Any message you want to put here!!\n";

static PyMethodDef helloworld_funcs[] = {
  {"helloworld", (PyCFunction)helloworld,
   METH_NOARGS, helloworld_docs},
  {NULL}
};

void scanhtmltag_dump(FILE *f) {
  
  
}

PyMODINIT_FUNC
initpinfo(void)
{
  Py_InitModule3("pinfo", helloworld_funcs,
		 "Extension module example!");
}
