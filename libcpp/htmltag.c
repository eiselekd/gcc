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
    FIRST (c).token = cpy;
    LAST (c).token = cpy + cnt;
    break;
  case TOKENS_KIND_DIRECT:
    cnt = LAST (ctx).token - FIRST (ctx).token;
    cpy = XNEWVEC(cpp_token , cnt);
    for (i = 0, it = FIRST (ctx).token; it < LAST (ctx).token; it++, i++) {
      cpy[i] = *it;
    }
    FIRST (c).token = cpy;
    LAST (c).token = cpy + cnt;
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

void htmltag_clone_context_prange(cpp_reader *pfile, const cpp_token **ptok , int cnt, cpp_context_htmltag_info *info)
{
  int i;
  cpp_context *c; const cpp_token **it = ptok;cpp_token *cpy; 
  struct htmltag_tree_context *h;
  (void) pfile;
  h = (struct htmltag_tree_context *)htmltag_alloc(HTMLTAG_CONTEXT);
  c = XNEW (cpp_context);
  memset (c, 0, sizeof (cpp_context));
  c->tokens_kind = TOKENS_KIND_DIRECT;
  cpy = XNEWVEC(cpp_token , cnt);
  
  for (i = 0; i < cnt; it++, i++) {
    cpy[i] = **it;
  }
  FIRST (c).token = cpy;
  LAST (c).token = cpy + cnt;
  
  h->ctx = c;
  h->info = *info;
}

void htmltag_clone_context_range(cpp_reader *pfile, const cpp_token *tok , int cnt, cpp_context_htmltag_info *info)
{
  int i;
  cpp_context *c; const cpp_token *it = tok;cpp_token *cpy; 
  struct htmltag_tree_context *h;
  (void) pfile;
  h = (struct htmltag_tree_context *)htmltag_alloc(HTMLTAG_CONTEXT);
  c = XNEW (cpp_context);
  memset (c, 0, sizeof (cpp_context));
  c->tokens_kind = TOKENS_KIND_DIRECT;
  cpy = XNEWVEC(cpp_token , cnt);
  
  for (i = 0; i < cnt; it++, i++) {
    cpy[i] = *it;
  }
  FIRST (c).token = cpy;
  LAST (c).token = cpy + cnt;
  
  h->ctx = c;
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

static const char *ctx_to_name[32] = {
  0,
  "AC", /*collect_args*/
  "AE", /* expand_arg() */
  "MF", /* replace_args() */
  
  /* argumentless macros:
     + replace macro identifier with definition
  */
  "MP", /* enter_macro_context.paramc==0 */
  "PR", /* enter_macro_context pragma */

  /* buildin macro result */
  "BR", /* builtin_macro() */
  
  "PP", /* paste_all_tokens() */
  "ws", /* cpp_get_token_1() */
  "pp", /* funlike_invocation_p() */
  "pr", /* do_pragma() */
  "st", /* destringize_and_run() */
  
  "b_", /* traditionsl.c:_cpp_scan_out_logical_line() */
  "r_", /* traditoinsl.c: push_replacement_text() */
  "a_" /* traditional.c: replace_args_and_p */
};

void htmltag_dump_tok(cpp_reader *pfile, FILE *f, const cpp_token *tok) {
  const char *paste = tok->flags & PASTE_LEFT ? ":p" : "";
  if (!cpp_is_spellable(tok)) {
    switch(tok->type) {
    case CPP_MACRO_ARG:
      fprintf(f, "(<arg-%d> %s)",tok->val.macro_arg.arg_no, paste);
      break;
    case CPP_PRAGMA:
      fprintf(f, "(<pragma> %s)", paste);
      break;
    case CPP_PRAGMA_EOL:
      fprintf(f, "(<pragmaeol %s>)", paste);
      break;
      
    case CPP_PADDING:
      fprintf(f, "(<pad %s>)", paste);
      break;
    case CPP_PARSEINFOSTACK:
    default:
      fprintf(f, "(<- %s>)", paste);
      break;
    }
  } else {
    fprintf(f, "(%s %s)", cpp_token_as_text (pfile, tok), paste);
  }
}


void htmltag_dump_rec(cpp_reader *pfile, FILE *f, struct htmltag_tree *p, int cnt)
{
  while (p) {
    int i;
    for (i = 0; i < cnt; i++) {
      fprintf(f, "  ");
    }
    switch(p->type) {
    case HTMLTAG_TOKEN: {
      const cpp_token *token = &(((struct htmltag_tree_token *)p)->tok);
      if (!cpp_is_spellable(token)) {
      } else {
	fprintf(f, "T.%d:%s\n", token->htmltag_tokid, cpp_token_as_text (pfile, token));
      }
      break;
    }
    case HTMLTAG_CONTEXT: {
      const cpp_token *it; 
      struct htmltag_tree_context *h = (struct htmltag_tree_context *)p;
      cpp_context *ctx = h->ctx;
      fprintf(f,"C.%s.%d", ctx_to_name[h->info.context_type], h->info.context_id);
      
      switch(h->info.context_type) {
      case CPP_CONTEXT_HTMLTAG_BUILDIN_RESULT:
	fprintf(f,",t:%d",h->info.u.macro.macroid);
	break;
      case CPP_CONTEXT_HTMLTAG_COLLECT_ARGS_SRC:
      case CPP_CONTEXT_HTMLTAG_EXPAND_ARG_RESULT:
	fprintf(f,",{%d}",h->info.u.macro.argidx);
	break;
      case CPP_CONTEXT_HTMLTAG_REPLACE_RESULT:
      case CPP_CONTEXT_HTMLTAG_REPLACE_ARGS_RESULT:
	fprintf(f,",t:%d,m:%d",h->info.u.macro.tokenid, h->info.u.macro.macroid);
	break;
      case CPP_CONTEXT_HTMLTAG_PASTE_RESULT:
	fprintf(f,",t:%d",h->info.u.macro.tokenid);
	break;
      }
      fprintf(f,":[");
      for (it = FIRST (ctx).token; it < LAST (ctx).token; it++) {
	if (h->info.iscopy) {
	  fprintf(f, "%d<-%d ", it->htmltag_tokid, it->htmltag_origid);
	} else {
	  fprintf(f, "%d", it->htmltag_tokid);
	  htmltag_dump_tok(pfile, f, it);
	  fprintf(f, " ");
	}
      }
      fprintf(f, "]\n");
    }
      break;
    }
    if (p->childs) {
      htmltag_dump_rec(pfile, f, p->childs, cnt+1);
    }
    p = p->n;
  }
}

void htmltag_dump(cpp_reader *pfile, FILE *f)
{
  struct htmltag_tree *p;
  htmltag_revert();
  p = htmltag;
  fprintf(f, "Trace:\n");
  htmltag_dump_rec(pfile, f, p, 0);
}
