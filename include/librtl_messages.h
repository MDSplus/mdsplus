/*  CMS REPLACEMENT HISTORY, Element LIBRTL_MESSAGES.H */
/*  *5    12-JUL-1996 16:00:25 JAS "addign messages for multiple precision integer arithmatic" */
/*  *4    11-JUL-1996 13:47:07 JAS "adding some more string messages" */
/*  *3    10-JUL-1996 16:11:07 JAS "adding str$_match and str$_nomatch" */
/*  *2    28-NOV-1994 13:29:46 TWF "Fix" */
/*  *1    16-NOV-1994 09:40:46 TWF "lib$_xxx messages used by MDSplus" */
/*  CMS REPLACEMENT HISTORY, Element LIBRTL_MESSAGES.H */
#ifdef __VMS
#pragma extern_model save
#pragma extern_model globalvalue
#define LibINSVIRMEM LIB$_INSVIRMEM
extern LibINSVIRMEM;
#define LibINVARG LIB$_INVARG
extern LibINVARG;
#define LibINVSTRDES LIB$_INVSTRDES
extern LibINVSTRDES;
#define LibKEYNOTFOU LIB$_KEYNOTFOU
extern LibKEYNOTFOU;
#define LibNOTFOU LIB$_NOTFOU
extern LibNOTFOU;
#define LibQUEWASEMP LIB$_QUEWASEMP
extern LibQUEWASEMP;
#define LibSTRTRU LIB$_STRTRU
extern LibSTRTRU;
#define StrMATCH STR$_MATCH
extern StrMATCH;
#define StrNOMATCH STR$_NOMATCH
extern StrNOMATCH;
#define StrNOELEM STR$_NOELEM
extern StrNOELEM;
#define StrINVDELIM STR$_INVDELIM
extern StrINVDELIM;
#define SsINTOVF SS$_INTOVF
extern SsINTOVF;
#pragma extern_model restore
#else
#define LibINSVIRMEM 0x158214
#define LibINVARG    0x158234
#define LibINVSTRDES 0x158224
#define LibKEYNOTFOU 0x1582fc
#define LibNOTFOU    0x158274
#define LibQUEWASEMP 0x1582ec
#define LibSTRTRU    0x158011
#define StrMATCH     0x248419
#define StrNOMATCH   0x248208
#define StrNOELEM    0x248218
#define StrINVDELIM  0x248210
#define SsINTOVF     0x00047C
#endif
