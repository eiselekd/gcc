#ifndef _HT_DEF_H
#define _HT_DEF_H

#include "cpplib.h"
#include <sys/types.h>
#include "symtab.h"
#include "line-map.h"
#include "internal.h"

enum htmltag_type {
  HTMLTAG_TOKEN,
  HTMLTAG_CONTEXT,
  HTMLTAG_DIRECTIVE
};

struct htmltag_tree  {
  enum htmltag_type type;
  struct htmltag_tree *n, *childs;
};

struct htmltag_tree_token  {
  struct htmltag_tree h;
  cpp_token tok;
  unsigned int skipping : 1;
};

struct htmltag_tree_context  {
  struct htmltag_tree h;
  cpp_context_htmltag_info info;
  struct cpp_context *ctx;
};

struct htmltag_tree_directive  {
  struct htmltag_tree h;
  int dir_no;
};

extern struct htmltag_tree *htmltag;
extern void htmltag_revert(void) ;

#ifdef DEBUG
#define HTMLTAG_DEBUG(fmt,...) do { fprintf(stderr,  fmt,  ##__VA_ARGS__); fflush(stderr); } while(0)
#else
#define HTMLTAG_DEBUG(fmt,...) do {  } while(0)
#endif

#define HDT_DUMP_SKIPPING (1 << 0)

#endif
