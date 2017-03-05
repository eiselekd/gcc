#include "htmltag_internal.h"

struct htmltag_tree *htmltag = 0;

/*******************************/

void htmltag_revert(void) {
  static int isreverted = 0;
  struct htmltag_tree *p = htmltag, *n;
  if (isreverted)
    return;
  htmltag = 0;
  while (p) {
    n = p->n;
    p->n = htmltag;
    htmltag = p;
    p = n;
  }
  isreverted = 1;
}


struct htmltag_tree *htmltag_alloc(enum htmltag_type typ) {

  struct htmltag_tree *h = 0;
  switch(typ) {
  case HTMLTAG_TOKEN: {
    struct htmltag_tree_token *tok;
    tok = (struct htmltag_tree_token *) XCNEW(struct htmltag_tree_token);
    h = (struct htmltag_tree *)tok;
    break;
  }
  case HTMLTAG_CONTEXT: {
    struct htmltag_tree_context *ctx;
    ctx = (struct htmltag_tree_context *) XCNEW(struct htmltag_tree_context);
    h = (struct htmltag_tree *)ctx;
    break;
  }
  case HTMLTAG_DIRECTIVE: {
    struct htmltag_tree_directive *ctx;
    ctx = (struct htmltag_tree_directive *) XCNEW(struct htmltag_tree_directive);
    h = (struct htmltag_tree *)ctx;
    break;
  }
  default:
    while(1) {};
    break;
  }

  h->type = typ;
  h->n = htmltag;
  htmltag = h;
  return h;
}
