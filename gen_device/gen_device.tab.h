#ifndef YYERRCODE
#define YYERRCODE 256
#endif

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
#define GEN_SIGNAL 273
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
#define GEN_ROUTINE 302
#define GEN_ARRAY 303
#define COMPRESSIBLE 304
#define NO_WRITE_MODEL 305
#define NO_WRITE_SHOT 306
#define INCLUDE_IN_PULSE 307
typedef union  {
  char *ptr;
  int  status;
  int  option;
  InDescr *dptr;
  NodeDescr *nptr;
} YYSTYPE;
extern YYSTYPE gen_devicelval;
