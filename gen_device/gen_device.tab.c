#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define yyparse gen_deviceparse
#define yylex gen_devicelex
#define yyerror gen_deviceerror
#define yychar gen_devicechar
#define yyval gen_deviceval
#define yylval gen_devicelval
#define yydebug gen_devicedebug
#define yynerrs gen_devicenerrs
#define yyerrflag gen_deviceerrflag
#define yyss gen_devicess
#define yyssp gen_devicessp
#define yyvs gen_devicevs
#define yyvsp gen_devicevsp
#define yylhs gen_devicelhs
#define yylen gen_devicelen
#define yydefred gen_devicedefred
#define yydgoto gen_devicedgoto
#define yysindex gen_devicesindex
#define yyrindex gen_devicerindex
#define yygindex gen_devicegindex
#define yytable gen_devicetable
#define yycheck gen_devicecheck
#define yyname gen_devicename
#define yyrule gen_devicerule
#define yysslim gen_devicesslim
#define yystacksize gen_devicestacksize
#define YYPREFIX "gen_device"
#line 2 "gen_device.y"
#include <stdio.h>
#include <usagedef.h>
#include "gen_device.h"
InDescr  * type;
extern void yyerror();
extern NodeDescr *bottom_node_list;
extern NodeDescr *DevConcat();
extern char      *DevConcatString();
extern void       DevFreeNodeList();
extern void       DevGenAdd();
extern void       DevGenH();
extern void       DevGenOperation();
extern void       DevGenPartName();
extern InDescr   *DevAddInSet();
extern InDescr   *DevInConcat();
extern InDescr   *DevNewAction();
extern InDescr   *DevNewConvert();
extern InDescr   *DevNewFlag();
extern InDescr   *DevNewInElement();
extern InDescr   *DevNewInElementNoConv();
extern InDescr   *DevNewInElementErr();
extern InDescr   *DevNewMember();
extern InDescr   *DevNewNode();
extern InDescr   *DevNewRange();
extern InDescr   *DevNewSet();
extern InDescr   *DevSetType();
extern InDescr   *DevSkipExpr();

#define DEV_ADD_IN_SET DevAddInSet
#define DEV_CONCAT DevConcat
#define DEV_CONCAT_STRING DevConcatString
#define DEV_FREE_NODE_LIST DevFreeNodeList
#define DEV_GEN_ADD DevGenAdd
#define DEV_GEN_H DevGenH
#define DEV_GEN_OPERATION DevGenOperation
#define DEV_GEN_PART_NAME DevGenPartName
#define DEV_IN_CONCAT DevInConcat
#define DEV_NEW_ACTION DevNewAction
#define DEV_NEW_CONVERT DevNewConvert
#define DEV_NEW_FLAG DevNewFlag
#define DEV_NEW_IN_ELEMENT DevNewInElement
#define DEV_NEW_IN_ELEMENT_NOCONV DevNewInElementNoConv
#define DEV_NEW_IN_ELEMENT_ERR DevNewInElementErr
#define DEV_NEW_MEMBER DevNewMember
#define DEV_NEW_NODE DevNewNode
#define DEV_NEW_RANGE DevNewRange
#define DEV_NEW_SET DevNewSet
#define DEV_SET_TYPE DevSetType
#define DEV_SKIP_EXPR DevSkipExpr

#line 55 "gen_device.y"
typedef union  {
  char *ptr;
  int  status;
  int  option;
  InDescr *dptr;
  NodeDescr *nptr;
} YYSTYPE;
#line 102 "gen_device.tab.c"
#define YYERRCODE 256
#define DEVICE 257
#define MODEL 258
#define IDENTIFIER 259
#define LIBRARY 260
#define INTEGER 261
#define STATE 262
#define OPTIONS 263
#define WRITE_ONCE 264
#define ON 265
#define OFF 266
#define USAGE 267
#define STRUCTURE 268
#define ACTION 269
#define DISPATCH 270
#define FLOAT_NUMBER 271
#define NUMERIC 272
#define SIGNAL 273
#define TASK 274
#define TEXT 275
#define WINDOW 276
#define AXIS 277
#define SUBTREE 278
#define ANY 279
#define TAGS 280
#define WITH 281
#define UNDEFINED 282
#define DATUM 283
#define VALUE 284
#define STRING 285
#define FLOAT 286
#define INT_NUMBER 287
#define IN 288
#define OPERATION 289
#define TIME_OUT 290
#define SEQUENCE 291
#define PHASE 292
#define SERVER_CLASS 293
#define COMPLETION_EVENT 294
#define REQUIRES 295
#define VALID 296
#define IF 297
#define ERROR_MESSAGE 298
#define IS 299
#define STRING_WORD 300
#define PATH_ITEM 301
#define ROUTINE 302
#define ARRAY 303
#define COMPRESSIBLE 304
#define NO_WRITE_MODEL 305
#define NO_WRITE_SHOT 306
#define INCLUDE_IN_PULSE 307
const short gen_devicelhs[] = {                                        -1,
    0,    1,    3,    3,    3,    5,    5,    6,    7,    8,
    9,    9,   10,   10,   14,   14,   15,   15,   15,   15,
   15,   13,   13,   11,   11,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   12,   12,    4,
    4,   17,   17,   18,   20,   20,   20,   21,   22,   24,
   23,   25,   25,   26,   27,   28,   29,   29,    2,    2,
   30,   30,   31,   32,   32,   33,   33,   34,   34,   35,
   36,   36,   36,   36,   36,   37,   37,   40,   42,   43,
   43,   43,   44,   45,   45,   46,   46,   38,   38,   41,
   47,   48,   48,   48,   49,   50,   50,   51,   51,   39,
   39,   52,   52,   53,   53,   54,   54,   19,   19,   55,
   55,
};
const short gen_devicelen[] = {                                         2,
    2,    8,    1,    1,    2,    1,    2,    2,    3,    4,
    0,    3,    0,    3,    1,    3,    1,    1,    1,    1,
    1,    1,    1,    0,    3,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    0,    3,    0,
    4,    1,    2,    5,    1,    1,    1,    1,    4,    0,
    9,    0,    3,    3,    3,    3,    0,    5,    0,    2,
    3,    3,    4,    1,    2,    3,    3,    0,    3,    2,
    1,    1,    1,    1,    1,    2,    2,    3,    9,    1,
    1,    1,    3,    1,    3,    4,    6,    2,    2,    3,
    9,    1,    1,    1,    3,    1,    3,    4,    6,    2,
    2,    9,    9,    1,    3,    4,    6,    2,    2,    0,
    2,
};
const short gen_devicedefred[] = {                                      0,
    0,    0,   59,    0,    0,    0,    0,    0,   60,    0,
    0,    0,    0,    0,   61,   62,    0,    0,    0,  110,
  110,    0,    0,   64,    0,    0,    2,    0,    0,    6,
    0,    0,    0,    0,    0,    0,    0,   63,   65,    0,
    0,    0,    7,    8,  111,    0,    0,    0,    0,   70,
   71,   72,   73,   74,   75,   66,   67,    0,   42,    0,
    0,    9,    0,   69,    0,    0,   77,   76,    0,  101,
  100,    0,    0,   89,   88,   41,   43,    0,    0,    0,
    0,    0,   78,    0,    0,   90,    0,   48,    0,    0,
   45,   46,   47,   22,   23,   12,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   17,   18,   19,   20,   21,
    0,   15,    0,    0,   10,    0,    0,    0,    0,   50,
   44,    0,   28,   26,   27,   29,   30,   31,   32,   33,
   34,   35,   36,   37,   25,    0,    0,    0,   80,    0,
    0,    0,    0,    0,    0,    0,   92,    0,    0,    0,
   49,   16,   39,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   83,    0,
    0,   85,    0,    0,  105,    0,    0,    0,   95,    0,
    0,   97,    0,    0,    0,    0,   86,    0,    0,  106,
    0,    0,    0,   98,    0,    0,   53,    0,    0,    0,
   79,    0,  102,    0,  103,   91,    0,   54,    0,    0,
    0,   87,  107,   99,   55,    0,    0,   51,   56,    0,
    0,    0,   58,
};
const short gen_devicedgoto[] = {                                       2,
    3,    5,   27,   28,   29,   30,   31,   62,   63,   81,
   99,  115,   96,  111,  112,  135,   58,   59,   22,   90,
   91,   92,   93,  151,  168,  186,  200,  211,  218,    9,
   15,   23,   24,   36,   37,   50,   51,   52,   53,   54,
   55,   68,  138,  139,  140,  141,   75,  146,  147,  148,
  149,   71,  143,  144,   32,
};
const short gen_devicesindex[] = {                                   -226,
 -221,    0,    0,  -19, -254, -209, -199, -195,    0, -183,
 -225, -225,   17,  -43,    0,    0, -178, -232, -219,    0,
    0,  -58, -114,    0,  -41, -172,    0, -191, -191,    0,
 -182, -204, -204, -201, -233,   41,   42,    0,    0, -232,
 -160, -191,    0,    0,    0, -156, -258, -237, -257,    0,
    0,    0,    0,    0,    0,    0,    0, -113,    0,   46,
   44,    0, -157,    0, -189, -188,    0,    0, -186,    0,
    0, -185, -188,    0,    0,    0,    0, -239, -190,   48,
 -154, -181,    0, -174, -173,    0, -171,    0, -168, -160,
    0,    0,    0,    0,    0,    0, -250,   56, -161,   29,
   30,   31,   62,   63,   66,    0,    0,    0,    0,    0,
   82,    0, -184,   67,    0, -158, -170, -144, -128,    0,
    0, -250,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -127,   88,   43,    0,   90,
   91,   92,  -28,  -27,   93,   47,    0,   95,   97, -148,
    0,    0,    0,  -55, -155, -143, -140,   86, -151, -147,
 -150, -146,  -49, -145, -117, -116,   96, -135,    0, -129,
 -139,    0,  114, -126,    0, -137,  118, -134,    0, -123,
 -133,    0,  122,  -93,  108, -122,    0,  -88,  110,    0,
  -86,  112,  -84,    0,  -83,  115,    0, -109,  119, -112,
    0, -108,    0, -105,    0,    0, -104,    0,  -75,  124,
 -103,    0,    0,    0,    0,  -73,  128,    0,    0,  130,
  -67,  131,    0,
};
const short gen_devicerindex[] = {                                      0,
    0,    0,    0,    0,  194,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   19,    0,
    0,  136,    0,    0,    0,    0,    0,   21,   25,    0,
   10,  -56,  -53,    0,    0,    0,    0,    0,    0,    0,
    1,   26,    0,    0,    0,    0,  136,  136,  136,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    4,    0,    0,  136,    0,    0,    0,    0,
    0,    0,  136,    0,    0,    0,    0,    0,    0,    0,
   15,    0,    0,    0,    0,    0,    0,    0,    0,  -50,
    0,    0,    0,    0,    0,    0,    0,    0,   24,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   20,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -26,    0,    0,  103,
  104,  -22,    0,    0,  -21,    0,    0,  105,  106,  -91,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -51,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
const short gen_devicegindex[] = {                                      0,
    0,    0,    0,  170,  174,    7,    0,  113,    0,    0,
    0,    0,    0,    0,   83,    0,    0,  146,  -11,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  195,    0,  183,  -15,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  187,
};
#define YYTABLESIZE 328
const short gen_devicetable[] = {                                      35,
   11,  108,  108,   13,  109,  109,  170,   57,   11,   40,
   38,   76,  180,  106,   24,  159,  161,   84,   40,   14,
    3,  104,   96,   38,    4,    5,   20,   47,   60,   87,
    1,   67,   70,   74,    7,   43,    4,   65,   72,   34,
   34,    6,   88,   89,   66,   73,   60,    8,   43,   10,
   83,   48,   49,  107,  108,  109,  110,   86,   69,   11,
   34,   25,   13,   12,  160,  162,   84,   26,   21,   14,
  104,   96,  123,   24,   94,   95,   13,   17,   14,   18,
   19,   40,   38,  124,  125,  126,   41,  127,  128,  129,
  130,  131,  132,  133,  134,   26,   45,   46,   25,   56,
   57,   61,   64,   78,   79,   80,  100,   82,   97,   34,
   84,   85,   98,  101,  102,  104,  113,  103,  114,  116,
  117,  118,  119,  120,  121,  122,  145,  136,  137,  142,
  150,  153,  154,  156,  157,  155,  158,  163,  165,  164,
  166,  167,  171,  172,   20,   20,  173,  174,  175,  177,
  176,  178,  181,  182,  183,  185,  184,  187,  189,  188,
  190,  191,  192,  194,  193,  195,  196,  197,  198,  199,
  201,  202,  203,  204,  205,  206,  207,  208,  212,  209,
  210,  213,  214,  215,  216,  219,   21,   21,  220,  221,
  217,  222,  223,    1,   68,   81,   82,   93,   94,   52,
   44,   42,  105,   77,  152,   39,   16,   33,    0,    0,
   57,   57,   11,    0,    0,   57,   11,    0,    0,    0,
    0,  179,    0,    0,    0,    0,    0,    0,   57,   11,
    0,  169,    0,    0,    0,    0,    0,    0,    0,   34,
    0,  108,    0,    0,  109,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   11,    0,    0,    0,   11,    0,    0,
   13,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   11,   11,    0,   13,   13,    0,   14,   11,    0,   11,
   13,    0,   13,    0,   24,   24,   40,    0,   40,   14,
   14,   24,   11,   24,   38,   13,   14,   40,   14,    3,
   38,   40,   38,    4,    5,    0,   24,    0,    0,    0,
   40,   14,    3,    0,    0,   38,    4,    5,
};
const short gen_devicecheck[] = {                                      58,
    0,   58,   59,    0,   58,   59,   62,   59,   59,    0,
  125,  125,   62,  264,    0,   44,   44,   44,    0,    0,
    0,   44,   44,    0,    0,    0,  259,  261,   40,  269,
  257,   47,   48,   49,  289,   29,  258,  296,  296,  298,
  298,   61,  282,  283,  303,  303,   58,  302,   42,  259,
   66,  285,  286,  304,  305,  306,  307,   73,  296,  259,
  298,  281,   59,  259,   93,   93,   93,  287,  301,  295,
   93,   93,  257,   59,  265,  266,  260,   61,   59,  123,
  259,  123,   59,  268,  269,  270,  259,  272,  273,  274,
  275,  276,  277,  278,  279,  287,  301,  299,  281,   59,
   59,  262,  259,   58,   61,  263,  288,  297,   61,  298,
  297,  297,  267,  288,  288,  284,   61,  289,  280,   91,
   91,   91,   61,   61,   59,   44,  271,   61,  287,  300,
  259,  259,   45,   44,   44,   93,   45,   45,   44,   93,
   44,  290,  298,  287,  259,  259,  287,   62,  300,  300,
  298,  298,  298,  271,  271,  291,   61,  287,   45,  299,
  287,  299,   45,  287,  299,  299,   45,  261,   61,  292,
  259,   62,  259,   62,  259,  259,   62,  287,  287,   61,
  293,  287,  287,  259,   61,  259,  301,  301,   61,   60,
  294,  259,   62,    0,   59,   93,   93,   93,   93,  291,
   31,   28,   90,   58,  122,   23,   12,   21,   -1,   -1,
  262,  263,  263,   -1,   -1,  267,  267,   -1,   -1,   -1,
   -1,  271,   -1,   -1,   -1,   -1,   -1,   -1,  280,  280,
   -1,  287,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  298,
   -1,  298,   -1,   -1,  298,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  263,   -1,   -1,   -1,  267,   -1,   -1,
  267,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  280,  281,   -1,  280,  281,   -1,  267,  287,   -1,  289,
  287,   -1,  289,   -1,  280,  281,  287,   -1,  289,  280,
  281,  287,  302,  289,  281,  302,  287,  289,  289,  289,
  287,  302,  289,  289,  289,   -1,  302,   -1,   -1,   -1,
  302,  302,  302,   -1,   -1,  302,  302,  302,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 307
#if YYDEBUG
const char * const gen_devicename[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"','","'-'",0,0,0,0,0,0,0,0,0,0,0,0,"':'","';'","'<'","'='",
"'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"DEVICE","MODEL","IDENTIFIER","LIBRARY","INTEGER","STATE","OPTIONS",
"WRITE_ONCE","ON","OFF","USAGE","STRUCTURE","ACTION","DISPATCH","FLOAT_NUMBER",
"NUMERIC","SIGNAL","TASK","TEXT","WINDOW","AXIS","SUBTREE","ANY","TAGS","WITH",
"UNDEFINED","DATUM","VALUE","STRING","FLOAT","INT_NUMBER","IN","OPERATION",
"TIME_OUT","SEQUENCE","PHASE","SERVER_CLASS","COMPLETION_EVENT","REQUIRES",
"VALID","IF","ERROR_MESSAGE","IS","STRING_WORD","PATH_ITEM","ROUTINE","ARRAY",
"COMPRESSIBLE","NO_WRITE_MODEL","NO_WRITE_SHOT","INCLUDE_IN_PULSE",
};
const char * const gen_devicerule[] = {
"$accept : gen_device",
"gen_device : device_decl operations",
"device_decl : DEVICE MODEL '=' IDENTIFIER LIBRARY '=' IDENTIFIER body",
"body : members",
"body : nodes",
"body : members nodes",
"nodes : node",
"nodes : nodes node",
"node : node_def members",
"node_def : INT_NUMBER IDENTIFIER flags",
"flags : state options usage tags",
"state :",
"state : STATE '=' statedef",
"options :",
"options : OPTIONS '=' option_list",
"option_list : option",
"option_list : option_list ',' option",
"option : WRITE_ONCE",
"option : COMPRESSIBLE",
"option : NO_WRITE_MODEL",
"option : NO_WRITE_SHOT",
"option : INCLUDE_IN_PULSE",
"statedef : ON",
"statedef : OFF",
"usage :",
"usage : USAGE '=' usagedef",
"usagedef : STRUCTURE",
"usagedef : ACTION",
"usagedef : DEVICE",
"usagedef : DISPATCH",
"usagedef : NUMERIC",
"usagedef : SIGNAL",
"usagedef : TASK",
"usagedef : TEXT",
"usagedef : WINDOW",
"usagedef : AXIS",
"usagedef : SUBTREE",
"usagedef : ANY",
"tags :",
"tags : TAGS '=' IDENTIFIER",
"members :",
"members : WITH '{' member_list '}'",
"member_list : member",
"member_list : member_list member",
"member : path ':' spec flags ';'",
"spec : undefined",
"spec : datum",
"spec : method",
"undefined : UNDEFINED",
"datum : DATUM VALUE '=' expr",
"expr :",
"method : ACTION OPERATION '=' IDENTIFIER timout sequence phase class completion",
"timout :",
"timout : TIME_OUT '=' INTEGER",
"sequence : SEQUENCE '=' INT_NUMBER",
"phase : PHASE '=' IDENTIFIER",
"class : SERVER_CLASS '=' IDENTIFIER",
"completion :",
"completion : COMPLETION_EVENT '=' '<' IDENTIFIER '>'",
"operations :",
"operations : operations operation",
"operation : OPERATION IDENTIFIER require_list",
"operation : ROUTINE IDENTIFIER require_list",
"require_list : REQUIRES '{' in_elements '}'",
"in_elements : in_element",
"in_elements : in_elements in_element",
"in_element : path opt_error ';'",
"in_element : path conversion ';'",
"opt_error :",
"opt_error : ERROR_MESSAGE IS IDENTIFIER",
"conversion : ':' convert",
"convert : int_convert",
"convert : float_convert",
"convert : string_convert",
"convert : int_array_convert",
"convert : float_array_convert",
"int_convert : INTEGER opt_int_validity",
"int_convert : INTEGER opt_error",
"int_array_convert : INTEGER ARRAY opt_error",
"opt_int_validity : VALID IF IN '[' int_range_set ']' ERROR_MESSAGE IS IDENTIFIER",
"int_range_set : int_range",
"int_range_set : int_set",
"int_range_set : int_set_key",
"int_range : INT_NUMBER '-' INT_NUMBER",
"int_set : INT_NUMBER",
"int_set : int_set ',' INT_NUMBER",
"int_set_key : INT_NUMBER '-' '>' INT_NUMBER",
"int_set_key : int_set_key ',' INT_NUMBER '-' '>' INT_NUMBER",
"float_convert : FLOAT opt_float_validity",
"float_convert : FLOAT opt_error",
"float_array_convert : FLOAT ARRAY opt_error",
"opt_float_validity : VALID IF IN '[' float_range_set ']' ERROR_MESSAGE IS IDENTIFIER",
"float_range_set : float_range",
"float_range_set : float_set",
"float_range_set : float_set_key",
"float_range : FLOAT_NUMBER '-' FLOAT_NUMBER",
"float_set : FLOAT_NUMBER",
"float_set : float_set ',' FLOAT_NUMBER",
"float_set_key : FLOAT_NUMBER '-' '>' INT_NUMBER",
"float_set_key : float_set_key ',' FLOAT_NUMBER '-' '>' INT_NUMBER",
"string_convert : STRING opt_string_validity",
"string_convert : STRING opt_error",
"opt_string_validity : VALID IF IN '[' string_set ']' ERROR_MESSAGE IS IDENTIFIER",
"opt_string_validity : VALID IF IN '[' string_set_key ']' ERROR_MESSAGE IS IDENTIFIER",
"string_set : STRING_WORD",
"string_set : string_set ',' STRING_WORD",
"string_set_key : STRING_WORD '-' '>' INT_NUMBER",
"string_set_key : string_set_key ',' STRING_WORD '-' '>' INT_NUMBER",
"path : IDENTIFIER path_items",
"path : PATH_ITEM path_items",
"path_items :",
"path_items : path_items PATH_ITEM",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 68 "gen_device.y"
{ DEV_FREE_NODE_LIST(bottom_node_list); }
break;
case 2:
#line 73 "gen_device.y"
{ DEV_GEN_ADD(yyvsp[-4], yyvsp[-1], yyvsp[0]); 
								  DEV_GEN_H(yyvsp[-4], yyvsp[-1], yyvsp[0]);
								  DEV_GEN_PART_NAME(yyvsp[-4], yyvsp[-1], yyvsp[0]); 
								  printf("Parsing interface description\n");
								  bottom_node_list = (NodeDescr *)yyvsp[0].ptr;}
break;
case 3:
#line 80 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 4:
#line 81 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 5:
#line 82 "gen_device.y"
{ yyval.nptr = DEV_CONCAT(yyvsp[-1], yyvsp[0]); }
break;
case 6:
#line 86 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 7:
#line 87 "gen_device.y"
{ yyval.nptr = DEV_CONCAT(yyvsp[-1], yyvsp[0]); }
break;
case 8:
#line 90 "gen_device.y"
{ yyval.nptr = DEV_CONCAT(yyvsp[-1], yyvsp[0]); }
break;
case 9:
#line 94 "gen_device.y"
{ yyval.dptr = DEV_NEW_NODE(yyvsp[-2], yyvsp[-1], yyvsp[0]); }
break;
case 10:
#line 97 "gen_device.y"
{ yyval.dptr = DEV_NEW_FLAG(yyvsp[-3], yyvsp[-2], yyvsp[-1], yyvsp[0]); }
break;
case 11:
#line 100 "gen_device.y"
{ yyval.ptr = 0; }
break;
case 12:
#line 101 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 13:
#line 104 "gen_device.y"
{ yyval.option = 0; }
break;
case 14:
#line 105 "gen_device.y"
{ yyval.option = yyvsp[0].option; }
break;
case 15:
#line 109 "gen_device.y"
{ yyval.option = yyvsp[0].option; }
break;
case 16:
#line 110 "gen_device.y"
{ yyval.option = yyvsp[-2].option | yyvsp[0].option; }
break;
case 17:
#line 114 "gen_device.y"
{ yyval.option = OPT_WRITE_ONCE; }
break;
case 18:
#line 115 "gen_device.y"
{ yyval.option = OPT_COMPRESSIBLE; }
break;
case 19:
#line 116 "gen_device.y"
{ yyval.option = OPT_NO_WRITE_MODEL; }
break;
case 20:
#line 117 "gen_device.y"
{ yyval.option = OPT_NO_WRITE_SHOT; }
break;
case 21:
#line 118 "gen_device.y"
{ yyval.option = OPT_INCLUDE_IN_PULSE; }
break;
case 22:
#line 123 "gen_device.y"
{ yyval.option = 1; }
break;
case 23:
#line 124 "gen_device.y"
{ yyval.option = 0; }
break;
case 24:
#line 127 "gen_device.y"
{ yyval.ptr = 0; }
break;
case 25:
#line 128 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 26:
#line 132 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_STRUCTURE"; }
break;
case 27:
#line 133 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_ACTION"; }
break;
case 28:
#line 134 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_DEVICE"; }
break;
case 29:
#line 135 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_DISPATCH"; }
break;
case 30:
#line 136 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_NUMERIC"; }
break;
case 31:
#line 137 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_SIGNAL"; }
break;
case 32:
#line 138 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_TASK"; }
break;
case 33:
#line 139 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_TEXT"; }
break;
case 34:
#line 140 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_WINDOW"; }
break;
case 35:
#line 141 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_AXIS"; }
break;
case 36:
#line 142 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_SUBTREE"; }
break;
case 37:
#line 143 "gen_device.y"
{ yyval.ptr = "TreeUSAGE_ANY";  }
break;
case 38:
#line 146 "gen_device.y"
{ yyval.dptr = 0; }
break;
case 39:
#line 147 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 40:
#line 151 "gen_device.y"
{ yyval.dptr = 0; }
break;
case 41:
#line 152 "gen_device.y"
{ yyval = yyvsp[-1]; }
break;
case 42:
#line 156 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 43:
#line 157 "gen_device.y"
{ yyval.nptr = DEV_CONCAT(yyvsp[-1], yyvsp[0]);  }
break;
case 44:
#line 160 "gen_device.y"
{ yyval.dptr = DEV_NEW_MEMBER(yyvsp[-4], yyvsp[-2], yyvsp[-1], type); }
break;
case 45:
#line 163 "gen_device.y"
{ yyval = yyvsp[0]; type = (InDescr *)UND; }
break;
case 46:
#line 164 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 47:
#line 165 "gen_device.y"
{ yyval = yyvsp[0]; type = (InDescr *)ACT;}
break;
case 48:
#line 169 "gen_device.y"
{ type = (InDescr *)UND; }
break;
case 49:
#line 172 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 50:
#line 176 "gen_device.y"
{ yyval.dptr = DEV_SKIP_EXPR(&type); }
break;
case 51:
#line 180 "gen_device.y"
{ yyval.dptr = DEV_NEW_ACTION(yyvsp[-5], yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1], yyvsp[0]); }
break;
case 52:
#line 183 "gen_device.y"
{ yyval.status = 0; }
break;
case 53:
#line 184 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 54:
#line 189 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 55:
#line 192 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 56:
#line 195 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 57:
#line 199 "gen_device.y"
{ yyval.dptr = 0; }
break;
case 58:
#line 200 "gen_device.y"
{ yyval = yyvsp[-1]; }
break;
case 61:
#line 211 "gen_device.y"
{ DEV_GEN_OPERATION(yyvsp[-1], yyvsp[0], 1); }
break;
case 62:
#line 212 "gen_device.y"
{ DEV_GEN_OPERATION(yyvsp[-1], yyvsp[0], 0); }
break;
case 63:
#line 216 "gen_device.y"
{ yyval = yyvsp[-1]; }
break;
case 64:
#line 220 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 65:
#line 221 "gen_device.y"
{ yyval.dptr = DEV_IN_CONCAT(yyvsp[-1], yyvsp[0]); }
break;
case 66:
#line 225 "gen_device.y"
{ yyval.dptr = DEV_NEW_IN_ELEMENT_NOCONV(yyvsp[-2], yyvsp[-1]); }
break;
case 67:
#line 226 "gen_device.y"
{ yyval.dptr = DEV_NEW_IN_ELEMENT(yyvsp[-2], yyvsp[-1]); }
break;
case 68:
#line 230 "gen_device.y"
{ yyval.dptr = 0; }
break;
case 69:
#line 231 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 70:
#line 235 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 71:
#line 240 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 72:
#line 241 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 73:
#line 242 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 74:
#line 243 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 75:
#line 244 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 76:
#line 249 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(yyvsp[0], INT); }
break;
case 77:
#line 250 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR(yyvsp[0]), INT); }
break;
case 78:
#line 254 "gen_device.y"
{yyval.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR(yyvsp[0]), INT_ARRAY); }
break;
case 79:
#line 259 "gen_device.y"
{ yyval.dptr = DEV_NEW_CONVERT(yyvsp[-4], yyvsp[0]); }
break;
case 80:
#line 263 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 81:
#line 264 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 82:
#line 265 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 83:
#line 269 "gen_device.y"
{ yyval.dptr = DEV_NEW_RANGE(yyvsp[-2], yyvsp[0]); }
break;
case 84:
#line 273 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[0], 0, 0); }
break;
case 85:
#line 274 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-2], yyvsp[0], 0, 0); }
break;
case 86:
#line 278 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[-3], yyvsp[0], 1);  }
break;
case 87:
#line 279 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-5], yyvsp[-3], yyvsp[0], 1); }
break;
case 88:
#line 283 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(yyvsp[0], FLO); }
break;
case 89:
#line 284 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR(yyvsp[0]), FLO); }
break;
case 90:
#line 288 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR(yyvsp[0]), FLO_ARRAY); }
break;
case 91:
#line 294 "gen_device.y"
{ yyval.dptr = DEV_NEW_CONVERT(yyvsp[-4], yyvsp[0]); }
break;
case 92:
#line 299 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 93:
#line 300 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 94:
#line 301 "gen_device.y"
{ yyval = yyvsp[0]; }
break;
case 95:
#line 305 "gen_device.y"
{yyval.dptr = DEV_NEW_RANGE(yyvsp[-2], yyvsp[0]); }
break;
case 96:
#line 309 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[0], 0, 0); }
break;
case 97:
#line 310 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-2], yyvsp[0], 0, 0); }
break;
case 98:
#line 315 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[-3], yyvsp[0], 1);  }
break;
case 99:
#line 316 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-5], yyvsp[-3], yyvsp[0], 1); }
break;
case 100:
#line 321 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(yyvsp[0], STR); }
break;
case 101:
#line 322 "gen_device.y"
{ yyval.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR(yyvsp[0]), STR); }
break;
case 102:
#line 327 "gen_device.y"
{ yyval.dptr = DEV_NEW_CONVERT(yyvsp[-4], yyvsp[0], STR); }
break;
case 103:
#line 329 "gen_device.y"
{ yyval.dptr = DEV_NEW_CONVERT(yyvsp[-4], yyvsp[0], STR); }
break;
case 104:
#line 333 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[0], 0, 0); }
break;
case 105:
#line 334 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-2], yyvsp[0], 0, 0); }
break;
case 106:
#line 339 "gen_device.y"
{ yyval.dptr = DEV_NEW_SET(yyvsp[-3], yyvsp[0], 1);  }
break;
case 107:
#line 340 "gen_device.y"
{ yyval.dptr = DEV_ADD_IN_SET(yyvsp[-5], yyvsp[-3], yyvsp[0], 1); }
break;
case 108:
#line 345 "gen_device.y"
{ yyval.ptr = DEV_CONCAT_STRING(yyvsp[-1], yyvsp[0]); }
break;
case 109:
#line 346 "gen_device.y"
{ yyval.ptr = DEV_CONCAT_STRING(yyvsp[-1], yyvsp[0]); }
break;
case 110:
#line 351 "gen_device.y"
{yyval.ptr = 0; }
break;
case 111:
#line 352 "gen_device.y"
{yyval.ptr = DEV_CONCAT_STRING(yyvsp[-1], yyvsp[0]); }
break;
#line 1143 "gen_device.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
