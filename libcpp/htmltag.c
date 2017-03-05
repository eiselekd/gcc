#include "config.h"
#include "system.h"
#include "htmltag.h"

#define MAX_PATH 260

/***********************************/
/* options */

struct option_value_info
{
  const char *const name;	/* the name of the value */
  const int value;		/* the value of the name */
};

static const struct option_value_info dump_options[] =
{
  {"skipping", HDT_DUMP_SKIPPING}
};

/***********************************/
/* snapshot sha */
struct htmltag_snapshot_sha_gather_ {
  int a;
};

static int
htmltag_snapshot_sha_gather(cpp_reader *pfile ATTRIBUTE_UNUSED, cpp_hashnode *node, void *v ATTRIBUTE_UNUSED)
{
  struct htmltag_snapshot_sha_gather_ *d = (struct htmltag_snapshot_sha_gather_ *)v; (void)d;
  
  if (node->type == NT_MACRO && !(node->flags & NODE_BUILTIN))
    {
      //fputs ("#define ", print.outf);
      //fputs ((const char *) cpp_macro_definition (pfile, node),
      //print.outf);
      //putc ('\n', print.outf);
      //print.printed = false;
      //print.src_line++;
    }
  return 1;
}

void htmltag_snapshot_sha(cpp_reader *pfile)
{
  struct htmltag_snapshot_sha_gather_ d;
  ht_forall (pfile->hash_table, (ht_cb) htmltag_snapshot_sha_gather, &d);
}


/***********************************/
/* mem mgmt */

#include "htmltag_general.c"

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

/*******************************************/
/* registration routines */

void htmltag_register_tok(cpp_reader *pfile, const cpp_token *tok)
{
  struct htmltag_tree_token *h;
  (void) pfile;
  if (!HTMLTAG_IS_ENABLED(pfile))
    return;

  h = (struct htmltag_tree_token *)htmltag_alloc(HTMLTAG_TOKEN);
  h->tok = *tok;
  h->skipping = pfile->state.skipping;
}

void htmltag_register_token_context(cpp_reader *pfile, const cpp_context *ctx, cpp_context_htmltag_info *info)
{
  struct htmltag_tree_context *h;
  (void) pfile;

  if (!HTMLTAG_IS_ENABLED(pfile))
    return;

  h = (struct htmltag_tree_context *)htmltag_alloc(HTMLTAG_CONTEXT);
  h->ctx = htmltag_clone_context(pfile, ctx);
  h->info = *info;
}

void htmltag_register_directive(cpp_reader *pfile, int dir_no, const cpp_token *dname)
{
  struct htmltag_tree_directive *h;
  (void) pfile; (void)dname;

  if (!HTMLTAG_IS_ENABLED(pfile))
    return;

  h = (struct htmltag_tree_directive *)htmltag_alloc(HTMLTAG_DIRECTIVE);
  h->dir_no = dir_no;
}


/*******************************/


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
  "a_", /* traditional.c: replace_args_and_p */

  "_d" /* create_iso_definition() */
  
};

#define LOCATION_LOCUS(LOC) \
  ((IS_ADHOC_LOC (LOC)) ? get_location_from_adhoc_loc (pfile->line_table, LOC) \
   : (LOC))

/* A value which will never be used to represent a real location.  */
#define UNKNOWN_LOCATION ((source_location) 0)

/* The location for declarations in "<built-in>" */
#define BUILTINS_LOCATION ((source_location) 1)


static expanded_location
expand_location_1 (cpp_reader *pfile, source_location loc,
		   bool expansion_point_p)
{
  expanded_location xloc;
  const line_map_ordinary *map;
  enum location_resolution_kind lrk = LRK_MACRO_EXPANSION_POINT;

  if (IS_ADHOC_LOC (loc))
    {
      loc = LOCATION_LOCUS (loc);
    }

  memset (&xloc, 0, sizeof (xloc));

  if (loc >= RESERVED_LOCATION_COUNT)
    {
      if (!expansion_point_p)
	{
	  /* We want to resolve LOC to its spelling location.

	     But if that spelling location is a reserved location that
	     appears in the context of a macro expansion (like for a
	     location for a built-in token), let's consider the first
	     location (toward the expansion point) that is not reserved;
	     that is, the first location that is in real source code.  */
	  loc = linemap_unwind_to_first_non_reserved_loc (pfile->line_table,
							  loc, NULL);
	  lrk = LRK_SPELLING_LOCATION;
	}
      loc = linemap_resolve_location (pfile->line_table, loc,
				      lrk, &map);
      xloc = linemap_expand_location (pfile->line_table, map, loc);
    }

  if (loc <= BUILTINS_LOCATION)
    xloc.file = loc == UNKNOWN_LOCATION ? NULL : _("<built-in>");

  return xloc;
}

struct dump_ctx {
  char p[MAX_PATH];

};

void htmltag_dump_tok(struct dump_ctx *ctx, cpp_reader *pfile, FILE *f, const cpp_token *tok) {
  const char *paste = tok->flags & PASTE_LEFT ? ":p" : "";
  (void) ctx;

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


void htmltag_dump_rec(struct dump_ctx *dctx, cpp_reader *pfile, FILE *f, struct htmltag_tree *p, int cnt)
{
  while (p) {
    int i;
    for (i = 0; i < cnt; i++) {
      fprintf(f, "  ");
    }
    switch(p->type) {
    case HTMLTAG_TOKEN: {
      struct htmltag_tree_token *tok = (struct htmltag_tree_token *)p;
      const cpp_token *token = &(((struct htmltag_tree_token *)p)->tok);
      expanded_location e = expand_location_1(pfile, token->src_loc, true);
      if (strcmp(dctx->p, e.file)) {
	fprintf(f, "L.%s\n", e.file);
	strncpy(dctx->p, e.file, MAX_PATH-1);
      }
      fprintf(f, "%c.%d:%d:%d:", tok->skipping ? 't' : 'T', token->htmltag_tokid,e.line,e.column);
      htmltag_dump_tok(dctx, pfile, f, token);
      fprintf(f,"\n");
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
      case CPP_CONTEXT_HTMLTAG_DEFINE:
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
	if ((it->htmltag_tokid != it->htmltag_origid)) {
	  fprintf(f, "%d<-%d ", it->htmltag_tokid, it->htmltag_origid);
	} else {
	  fprintf(f, "%d", it->htmltag_tokid);
	}
	htmltag_dump_tok(dctx, pfile, f, it);
	fprintf(f, " ");
	
      }
      fprintf(f, "]\n");
    }
      break;
    case HTMLTAG_DIRECTIVE: {
      struct htmltag_tree_directive *h = (struct htmltag_tree_directive *)p;
      const char *n = _cpp_directive_name (h->dir_no);
      fprintf(f, "D.%s\n", n);
    }
      break;
      
    default:
      break;
    }
    if (p->childs) {
      htmltag_dump_rec(dctx, pfile, f, p->childs, cnt+1);
    }
    p = p->n;
  }
}

void htmltag_dump(cpp_reader *pfile, FILE *f)
{
  struct htmltag_tree *p;
  struct dump_ctx ctx;
  memset(&ctx, 0, sizeof(ctx));
  htmltag_revert();
  p = htmltag;
  fprintf(f, "Trace:\n");
  htmltag_dump_rec(&ctx, pfile, f, p, 0);
}
