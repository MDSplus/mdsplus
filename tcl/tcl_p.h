#ifndef TCL_P
#define TCL_P

typedef enum
{
  on_off,
  rename_node,
  delete,
  new,
  tree,
  set_def
} NodeTouchType;

extern void
TclSetCallbacks(                    /* Returns: void                        */
                void (*error_out)() /* <r> addr of error output routine */
                ,
                void (*text_out)() /* <r> addr of normal text output routine */
                ,
                void (*node_touched)() /* <r> addro of "node touched" routine */
);
extern void TclNodeTouched(        /* Returns: void                        */
                           int nid /* <r> node id                          */
                           ,
                           NodeTouchType type /* <r> type of "touch" */
);

extern char tclUsageToNumber(const char *usage, char **error);
extern int tclStringToShot(char *str, int *shot, char **error);
extern void tclAppend(char **output, char *string);

#endif
