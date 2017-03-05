#ifndef LIBCPP_HTMLTAG_INTERNAL_CTX_H
#define LIBCPP_HTMLTAG_INTERNAL_CTX_H

/* in sync with ctx_to_name */
enum cpp_context_htmltag_type {
  /* fuction like macros:
     + first collect arguments
     + macro expand the arguments that are used
     + replace args in expanded macro
  */
  CPP_CONTEXT_HTMLTAG_COLLECT_ARGS_SRC = 1, /*collect_args*/
  CPP_CONTEXT_HTMLTAG_EXPAND_ARG_RESULT, /* expand_arg() */
  CPP_CONTEXT_HTMLTAG_REPLACE_ARGS_RESULT, /* replace_args() */

  /* argumentless macros:
     + replace macro identifier with definition
  */
  CPP_CONTEXT_HTMLTAG_REPLACE_RESULT, /* enter_macro_context.paramc==0 */
  CPP_CONTEXT_HTMLTAG_REPLACE_PRAGMA_RESULT, /* enter_macro_context pragma */
  
  /* result of buildin macro */
  CPP_CONTEXT_HTMLTAG_BUILDIN_RESULT, /* enter_macro_context.isbuildin, builtin_macro() */
  
  CPP_CONTEXT_HTMLTAG_PASTE_RESULT, /* paste_all_tokens() */
  CPP_CONTEXT_HTMLTAG_WHITESPACE, /* cpp_get_token_1() */
  CPP_CONTEXT_HTMLTAG_FUNLIKE_PADDING, /* funlike_invocation_p() */
  CPP_CONTEXT_HTMLTAG_PRAGMA_SRC, /* do_pragma() */
  CPP_CONTEXT_HTMLTAG_DESTRINGIZE_SRC, /* destringize_and_run() */
  
  CPP_CONTEXT_HTMLTAG_TRADITIONAL_BUILDIN, /* traditionsl.c:_cpp_scan_out_logical_line() */
  CPP_CONTEXT_HTMLTAG_TRADITIONAL_REPLACEMENT, /* traditoinsl.c: push_replacement_text() */
  CPP_CONTEXT_HTMLTAG_TRADITIONAL_ARGS_RESULT, /* traditional.c: replace_args_and_push() */

  CPP_CONTEXT_HTMLTAG_DEFINE, /* create_iso_definition() */
  
};

typedef struct cpp_context_htmltag_info
{
  int context_id;
  int context_type;
  int iscopy;
  union {
    struct {
      int macroid;
      int tokenid;
      int argidx;
    } macro;
  } u;
} cpp_context_htmltag_info;


#endif /* ! LIBCPP_HTMLTAG_INTERNAL_CTX_H */
