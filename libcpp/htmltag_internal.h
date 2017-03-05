#ifndef _HT_DEF_INTERNAL_H
#define _HT_DEF_INTERNAL_H

#include "htmltag_internal_ctx.h"
#include "md5.h"

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

#ifdef DEBUG
#define HTMLTAG_DEBUG(fmt,...) do { fprintf(stderr,  fmt,  ##__VA_ARGS__); fflush(stderr); } while(0)
#else
#define HTMLTAG_DEBUG(fmt,...) do {  } while(0)
#endif

#define HDT_DUMP_SKIPPING (1 << 0)

extern void htmltag_revert(void);
extern struct htmltag_tree *htmltag_alloc(enum htmltag_type typ);
extern long htmltag_new_tokid();

#define MAX_PATH 260*2

struct htmltag_files  {
  struct htmltag_files *n;
  int fileid;
  long tokid;
  char fn[MAX_PATH];
  unsigned char md5sum[16];
};

struct htmltag_tokid {
  struct htmltag_files *file;
  int tokid;
};

#endif
