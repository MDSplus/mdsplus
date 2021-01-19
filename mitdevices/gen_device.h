/*
#define yyact gen_device_yyact
#define yyback gen_device_yyback
#define yybgin gen_device_yybgin
#define yychar gen_device_yychar
#define yychk gen_device_yychk
#define yycrank gen_device_yycrank
#define yydebug gen_device_yydebug
#define yydef gen_device_yydef
#define yyerrflag gen_device_yyerrflag
#define yyestate gen_device_yyestate
#define yyexca gen_device_yyexca
#define yyextra gen_device_yyextra
#define yyfnd gen_device_yyfnd
#define yyin gen_device_yyin
#define yyinput gen_device_yyinput
#define yyleng gen_device_yyleng
#define yylineno gen_device_yylineno
#define yylook gen_device_yylook
#define yylsp gen_device_yylsp
#define yylstate gen_device_yylstate
#define yylval gen_device_yylval
#define yymatch gen_device_yymatch
#define yymaxdepth gen_device_yymaxdepth
#define yymorfg gen_device_yymorfg
#define yynerrs gen_device_yynerrs
#define yyolsp gen_device_yyolsp
#define yyout gen_device_yyout
#define yyoutput gen_device_yyoutput
#define yypact gen_device_yypact
#define yypgo gen_device_yypgo
#define yyprevious gen_device_yyprevious
#define yyps gen_device_yyps
#define yypv gen_device_yypv
#define yyr1 gen_device_yyr1
#define yyr2 gen_device_yyr2
#define yys gen_device_yys
#define yysbuf gen_device_yysbuf
#define yysptr gen_device_yysptr
#define yystate gen_device_yystate
#define yysvec gen_device_yysvec
#define yytchar gen_device_yytchar
#define yytext gen_device_yytext
#define yytmp gen_device_yytmp
#define yytop gen_device_yytop
#define yyunput gen_device_yyunput
#define yyv gen_device_yyv
#define yyval gen_device_yyval
#define yyvstop gen_device_yyvstop
*/

#define UND 0
#define INT 1
#define FLO 2
#define STR 3
#define EXP 4
#define ACT 5
#define TRE 6
#define INT_ARRAY 7
#define FLO_ARRAY 8

#define NONE 0
#define RANGE 1
#define SET 2
#define CONV_SET 3
#define MAX_DIM_SET 128
#define OPT_WRITE_ONCE 1
#define OPT_COMPRESSIBLE 2
#define OPT_NO_WRITE_MODEL 4
#define OPT_NO_WRITE_SHOT 8
#define OPT_INCLUDE_IN_PULSE 16

typedef struct nodeDescr {
  struct nodeDescr *nxt, *prv;
  char *name;
  char nci_flags, state, type, pad;
  char *tags, *usage, *value, *method, *phase, *sequence, *completion, *timout,
      *server;
} NodeDescr;

typedef struct {
  char state, nci_flags, *usage, *tags;
} FlagsDescr;

typedef struct {
  char *method, *phase, *sequence, *completion, *timout, *server;
} ActionDescr;

typedef struct inDescr {
  struct inDescr *nxt, *prv;
  int type, mode;
  char *path_name;
  char *set[MAX_DIM_SET];
  char *codes[MAX_DIM_SET];
  char *error_code;
} InDescr;

typedef struct {
  struct descriptor_xd *xds;
  int num_xds;
} CommonInStruct;
