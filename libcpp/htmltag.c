#include "config.h"
#include "system.h"
#include "htmltag.h"

static int isreverted = 0;
struct htmltag_tree *htmltag = 0;

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
  default:
    while(1) {};
    break;
  }

  h->type = typ;
  h->n = htmltag;
  htmltag = h;
  return h;
}

void htmltag_register_tok(cpp_reader *pfile, const cpp_token *tok)
{
  struct htmltag_tree_token *h;
  (void) pfile;
  h = (struct htmltag_tree_token *)htmltag_alloc(HTMLTAG_TOKEN);
  h->tok = *tok;
}

cpp_context *htmltag_clone_context(cpp_reader *pfile, const cpp_context *ctx)
{
  cpp_context *c; const cpp_token **pit, *it; cpp_token *cpy;
  int cnt, i;
  (void) pfile;
  c = XNEW (cpp_context);
  memset (c, 0, sizeof (cpp_context));
  c->tokens_kind = TOKENS_KIND_DIRECT;

  /* place for optimization (hash tokid->ptr) */
  switch(ctx->tokens_kind) {
  case TOKENS_KIND_EXTENDED:
  case TOKENS_KIND_INDIRECT:
    cnt = LAST (ctx).ptoken - FIRST (ctx).ptoken;
    cpy = XNEWVEC(cpp_token , cnt);
    for (i = 0, pit = FIRST (ctx).ptoken; pit < LAST (ctx).ptoken; pit++, i++) {
      cpy[i] = **pit;
    }
    break;
  case TOKENS_KIND_DIRECT:
    cnt = LAST (ctx).token - FIRST (ctx).token;
    cpy = XNEWVEC(cpp_token , cnt);
    for (i = 0, it = FIRST (ctx).token; it < LAST (ctx).token; it++, i++) {
      cpy[i] = *it;
    }
    break;
  default:
    break;
  }
  return c;
}

void htmltag_register_token_context(cpp_reader *pfile, const cpp_context *ctx, cpp_context_htmltag_info *info)
{
  struct htmltag_tree_context *h;
  (void) pfile;
  h = (struct htmltag_tree_context *)htmltag_alloc(HTMLTAG_CONTEXT);
  h->ctx = htmltag_clone_context(pfile, ctx);
  h->info = *info;
}

/*******************************/

void htmltag_revert(void) {
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

void htmltag_dump(cpp_reader *pfile, FILE *f)
{
  struct htmltag_tree *p;
  htmltag_revert();
  p = htmltag;
  fprintf(f, "Trace:\n");
  while (p) {
    switch(p->type) {
    case HTMLTAG_TOKEN: {
      const cpp_token *token = &(((struct htmltag_tree_token *)p)->tok);
      if (!cpp_is_spellable(token)) {
	fprintf(f, "<->\n");
      } else {
	fprintf(f, "%d:%s\n", token->htmltag_tokid, cpp_token_as_text (pfile, token));
      }
      break;
    }
    case HTMLTAG_CONTEXT:
      break;
    }
    p = p->n;
  }
}
