#ifdef ERROR
#undef ERROR
#endif
#ifdef TEXT
#undef TEXT
#endif
#ifdef CONST
#undef CONST
#endif
#ifdef IN
#undef IN
#endif
typedef union {
  struct marker mark;
} YYSTYPE;
extern YYSTYPE yylval;
#define ERROR 257
#define IDENT 258
#define POINT 259
#define TEXT 260
#define VALUE 261
#define BREAK 262
#define CASE 263
#define COND 264
#define DEFAULT 265
#define DO 266
#define ELSE 267
#define ELSEW 268
#define FOR 269
#define GOTO 270
#define IF 271
#define LABEL 272
#define RETURN 273
#define SIZEOF 274
#define SWITCH 275
#define USING 276
#define WHERE 277
#define WHILE 278
#define ARG 279
#define CAST 280
#define CONST 281
#define INC 282
#define ADD 283
#define CONCAT 284
#define IAND 285
#define IN 286
#define IOR 287
#define IXOR 288
#define LEQV 289
#define POWER 290
#define PROMO 291
#define RANGE 292
#define SHIFT 293
#define BINEQ 294
#define LAND 295
#define LEQ 296
#define LGE 297
#define LOR 298
#define MUL 299
#define UNARY 300
#define LANDS 301
#define LEQS 302
#define LGES 303
#define LORS 304
#define MULS 305
#define UNARYS 306
#define FUN 307
#define MODIF 308
#define VBL 309
