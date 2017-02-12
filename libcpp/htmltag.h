#ifndef _HT_DEF_H
#define _HT_DEF_H

#include <sys/types.h>
#include "symtab.h"
#include "line-map.h"


struct GTY(()) htmltag_tree  {
  int type;
  struct htmltag_tree *up;
};

#endif
