#include <Python.h>
#include "pinfo.h"

typedef struct _cpp_token {
  int typ;
} cpp_token;
#define XCNEW(n) ((n*)malloc(sizeof(n)))

#include "htmltag_general.c"

void scanhtmltag_dump(char *fn) {
  char c; int id, lnr, cnr; const char *curfile = "<buildin>"; (void)curfile;
  FILE *f; long l, r; char *b, *line;
  f = fopen(fn, "r");
  if (!f)
    return;
  fseek(f, SEEK_END, 0);
  l = ftell(f);
  b = malloc(l+2);
  r = fread(b, 1, l, f);
  if (r != l) {
    free(b);
    return;
  }
  line = strtok (b,"\n");
  while (line != NULL) {
    switch(line[0]) {
    case 'D':
      break;
    case 't':
    case 'T':
      if (sscanf(line, "%c.%d:%d:%d", &c, &id, &lnr, &cnr) == 4) {
	
      }
      break;
    case 'L':
      
      break;
    case 'C':
      break;
    default:
      break;
    }
    
    line = strtok (NULL, "\n");
  }

  
  free(b);
}

static PyObject* readpinfo(PyObject* self)
{
  return Py_BuildValue("s", "Hello, Python extensions!!");
}

static char readpinfo_docs[] =
  "readpinfo( ): read pinfo file\n";

static PyMethodDef pinfo_funcs[] = {
  {"readpinfo", (PyCFunction)readpinfo,
   METH_NOARGS, readpinfo_docs},
  {NULL}
};

PyMODINIT_FUNC
initpinfo(void)
{
  Py_InitModule3("pinfo", pinfo_funcs,
		 "Extension module example!");
}
