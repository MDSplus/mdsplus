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
