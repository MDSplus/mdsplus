/* /harpodka400/sys0/syscommon/posix$bin/lex -lo /alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$lex.c /alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$lex.x */
#define YYNEWLINE 10
#define INITIAL 0
#define yy_endst 45
#define yy_nxtmax 1663
#define YY_LA_SIZE 6

static unsigned short yy_la_act[] = {
 0, 10, 1, 4, 10, 10, 5, 6, 10, 10, 10, 7, 10, 10, 10, 10,
 9, 8, 8, 8, 6, 6, 6, 6, 6, 6, 5, 6, 3, 6, 6, 3,
 4, 4, 4, 3, 4, 514, 3, 3,
};

static unsigned char yy_look[] = {
 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0
};

static short yy_final[] = {
 0, 0, 2, 3, 5, 6, 9, 10, 11, 13, 14, 15, 16, 17, 17, 18,
 19, 19, 20, 21, 22, 22, 23, 24, 25, 26, 26, 26, 28, 29, 30, 30,
 31, 31, 31, 32, 33, 33, 34, 35, 37, 38, 39, 39, 39, 40
};
#ifndef yy_state_t
#define yy_state_t unsigned char
#endif

static yy_state_t yy_begin[] = {
 0, 0, 0
};

static yy_state_t yy_next[] = {
 11, 11, 11, 11, 11, 11, 11, 11, 11, 1, 2, 11, 11, 1, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 1, 11, 8, 11, 5, 11, 11, 8, 11, 11, 11, 11, 11, 9, 4, 10,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 11, 11, 11, 11, 11,
 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 11, 6, 11, 11, 5,
 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 12, 13, 14, 17, 26, 31, 45, 45, 45, 45, 45, 31, 30, 45, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 20, 45, 45, 45, 45, 45, 45, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 45, 45, 45, 45, 14, 45, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 45, 45, 45, 45, 45, 45,
 45, 45, 45, 16, 45, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15,
 45, 45, 45, 45, 45, 45, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 45, 45, 45, 18, 14, 45, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 45, 45, 45, 45, 18, 45,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 45, 45, 45, 45, 45,
 45, 45, 45, 45, 20, 45, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 20, 45, 45, 45, 45, 45, 45, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 19, 45, 45, 45, 21, 19, 45, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
 19, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 45, 45, 45, 45, 21,
 45, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 45, 45, 45, 45,
 45, 45, 45, 45, 45, 20, 45, 22, 22, 22, 22, 22, 22, 22, 22, 22,
 22, 20, 45, 45, 45, 45, 45, 45, 22, 22, 22, 22, 22, 22, 22, 22,
 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
 22, 22, 45, 45, 45, 23, 22, 45, 22, 22, 22, 22, 22, 22, 22, 22,
 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 45, 45, 45, 45,
 23, 45, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 45, 45, 45,
 45, 45, 45, 45, 45, 45, 20, 45, 24, 24, 24, 24, 24, 24, 24, 24,
 24, 24, 25, 45, 45, 45, 45, 45, 45, 24, 24, 24, 24, 24, 24, 24,
 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
 24, 24, 24, 45, 45, 45, 45, 24, 45, 24, 24, 24, 24, 24, 24, 24,
 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
 24, 24, 24, 27, 45, 45, 45, 45, 45, 45, 45, 45, 45, 20, 45, 27,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 20, 45, 45, 45, 45, 45, 45,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 45, 45, 45, 45, 27, 45,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 18, 45, 45, 45, 45, 45,
 45, 45, 45, 29, 45, 45, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
 45, 45, 45, 45, 45, 45, 45, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 45, 45, 45, 45, 18, 45, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 45, 45, 45, 45, 45,
 45, 45, 45, 45, 45, 32, 32, 32, 32, 32, 45, 45, 45, 45, 45, 45,
 45, 33, 45, 33, 32, 32, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
 45, 45, 45, 45, 45, 32, 32, 32, 32, 32, 45, 45, 45, 45, 45, 45,
 45, 45, 45, 45, 32, 32, 36, 45, 35, 35, 35, 35, 35, 35, 35, 35,
 35, 35, 45, 45, 45, 45, 45, 45, 45, 38, 38, 38, 37, 37, 37, 37,
 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 37, 37, 38, 38, 38,
 38, 38, 38, 45, 45, 45, 45, 45, 45, 38, 38, 38, 37, 37, 37, 37,
 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 37, 37, 38, 38, 38,
 38, 38, 38, 45, 45, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 45,
 45, 45, 45, 45, 45, 45, 45, 45, 45, 38, 38, 38, 38, 38, 45, 45,
 45, 45, 45, 45, 45, 45, 45, 45, 38, 38, 33, 45, 33, 45, 45, 39,
 39, 39, 39, 39, 39, 39, 39, 39, 39, 38, 38, 38, 38, 38, 45, 45,
 45, 45, 45, 38, 38, 38, 38, 38, 38, 38, 45, 45, 45, 45, 45, 45,
 45, 45, 38, 38, 45, 45, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
 45, 45, 45, 38, 38, 38, 38, 38, 45, 45, 38, 38, 38, 38, 38, 45,
 45, 45, 38, 38, 45, 45, 43, 45, 43, 38, 38, 44, 44, 44, 44, 44,
 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 38, 38, 38, 38, 38, 45,
 45, 45, 45, 45, 45, 45, 45, 45, 45, 38, 38, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 45, 40, 41, 41, 41, 41, 41,
 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 42,
 42, 42, 42, 42, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 42, 42,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 42,
 42, 42, 42, 42, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 42, 42,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41,
 41, 41, 41, 41, 41, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 42,
 42, 42, 42, 42, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 42, 42,
 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 42,
 42, 42, 42, 42, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 42, 42,

};

static yy_state_t yy_check[] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 10, 9, 13, 16, 25, 30, 34, 33, 34, 33, ~0, 29, 29, ~0, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 29, 44, 43, 44, 43, ~0, ~0, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, ~0, ~0, ~0, ~0, 13, ~0, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, ~0, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, 14, ~0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 ~0, ~0, ~0, ~0, ~0, ~0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 ~0, ~0, ~0, 7, 14, ~0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, ~0, ~0, ~0, ~0, 7, ~0,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 18, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, ~0, 18, ~0, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, ~0, ~0, ~0, ~0, ~0, ~0, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, ~0, ~0, ~0, 20, 18, ~0, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
 18, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, ~0, ~0, ~0, ~0, 20,
 ~0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, ~0, ~0, 21, ~0, 21, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, ~0, ~0, ~0, ~0, ~0, ~0, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, ~0, ~0, ~0, 6, 21, ~0, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
 21, 21, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, ~0, ~0, ~0, ~0,
 6, ~0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 23, ~0, ~0, ~0,
 ~0, ~0, ~0, ~0, ~0, ~0, 23, ~0, 23, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, ~0, ~0, ~0, ~0, ~0, ~0, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, ~0, ~0, ~0, ~0, 23, ~0, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
 23, 23, 23, 5, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 5, ~0, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, ~0, ~0, ~0, ~0, ~0, ~0,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, ~0, ~0, ~0, ~0, 5, ~0,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, 4, ~0, ~0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 4, ~0, ~0, ~0, ~0, 4, ~0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 4, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, ~0, ~0, 28, 28, 28, 28, 28, ~0, ~0, ~0, ~0, ~0, ~0,
 ~0, 32, ~0, 32, 28, 28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
 ~0, ~0, ~0, ~0, ~0, 28, 28, 28, 28, 28, ~0, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, ~0, ~0, 28, 28, 3, ~0, 3, 3, 3, 3, 3, 3, 3, 3,
 3, 3, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 3, 3, 3, 3, 3, 3, 3,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
 3, 3, 3, ~0, ~0, ~0, ~0, ~0, ~0, 3, 3, 3, 3, 3, 3, 3,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
 3, 3, 3, 38, ~0, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, ~0,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 38, 38, 38, 38, 38, ~0, ~0,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 38, 38, 37, ~0, 37, 37, ~0, 37,
 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, ~0, ~0,
 ~0, ~0, ~0, 37, 37, 37, 37, 37, 38, 38, ~0, ~0, ~0, ~0, ~0, ~0,
 ~0, ~0, 37, 37, 39, ~0, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
 ~0, ~0, ~0, 37, 37, 37, 37, 37, ~0, ~0, 39, 39, 39, 39, 39, ~0,
 ~0, ~0, 37, 37, ~0, ~0, 42, ~0, 42, 39, 39, 42, 42, 42, 42, 42,
 42, 42, 42, 42, 42, ~0, ~0, ~0, ~0, ~0, 39, 39, 39, 39, 39, ~0,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 39, 39, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, ~0, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 41, 41, 41, 41, 41,
 41, 41, 41, 41, 41, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 41,
 41, 41, 41, 41, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 41, 41,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 41,
 41, 41, 41, 41, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 41, 41,

};

static yy_state_t yy_default[] = {
 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
 45, 45, 45, 18, 45, 45, 21, 45, 23, 20, 7, 5, 45, 45, 20, 29,
 45, 32, 32, 3, 45, 3, 3, 3, 45, 45, 45, 42, 42,
};

static short yy_base[] = {
 0, 1664, 1664, 1064, 918, 831, 657, 367, 1664, 195, 214, 1664, 1664, 222, 309, 1664,
 213, 1664, 454, 1664, 512, 599, 1664, 744, 1664, 202, 1664, 1664, 993, 222, 216, 1664,
 1030, 220, 219, 1664, 1339, 1183, 1141, 1222, 1664, 1547, 1259, 239, 238, 1664
};


/*
 * Copyright 1988, 1992 by Mortice Kern Systems Inc.  All rights reserved.
 * All rights reserved.
 *
 * $Header$
 *
 */
#include <stdlib.h>
#include <stdio.h>
#if	defined(__STDC__) || defined(__cplusplus)   
#define YY_ARGS(args)	args
#define YY_FNARG1(type, name) (type name)              
#else
#define YY_ARGS(args)	()
#define YY_FNARG1(type, name) (name) type name;        
#endif

/*
 * Define m_textmsg() to an appropriate function for internationalized messages
 * or custom processing.
 */
#ifndef I18N
#define	m_textmsg(id, str, cls)	(str)
#else /*I18N*/
extern	char* m_textmsg YY_ARGS((int id, const char* str, char* cls));
#endif/*I18N*/

/*
 * Include string.h to get definition of memmove() and size_t.
 * If you do not have string.h or it does not declare memmove
 * or size_t, you will have to declare them here.
 */
#include <string.h>
/* Uncomment next line if memmove() is not declared in string.h */
/*extern char * memmove();*/
/* Uncomment next line if size_t is not available in stdio.h or string.h */
/*typedef unsigned size_t;*/
/* Drop this when LATTICE provides memmove */
#ifdef LATTICE
#define memmove	memcopy
#endif

/*
 * YY_STATIC determines the scope of variables and functions
 * declared by the lex scanner. It must be set with a -DYY_STATIC
 * option to the compiler (it cannot be defined in the lex program).
 */
#ifdef	YY_STATIC
/* define all variables as static to allow more than one lex scanner */
#define	YY_DECL	static
#else
/* define all variables as global to allow other modules to access them */
#define	YY_DECL	
#endif

/*
 * You can redefine yygetc. For YACC Tracing, compile this code
 * with -DYYTRACE to get input from yt_getc
 */
#ifdef YYTRACE
extern int	yt_getc YY_ARGS((void));
#define yygetc()	yt_getc()
#else
#define	yygetc()	getc(yyin) 	/* yylex input source */
#endif

/*
 * the following can be redefined by the user.
 */
#define	ECHO		fputs(yytext, yyout)
#define	output(c)	putc((c), yyout) /* yylex sink for unmatched chars */
#define	YY_FATAL(msg)	{ fprintf(stderr, "yylex: %s\n", msg); exit(1); }
#define	YY_INTERACTIVE	1		/* save micro-seconds if 0 */
#define	YYLMAX		100		/* token and pushback buffer size */

/*
 * If %array is used (or defaulted), yytext[] contains the token.
 * If %pointer is used, yytext is a pointer to yy_tbuf[].
 */
YY_DECL char	yytext[YYLMAX+1];

#define	BEGIN		yy_start =
#define	REJECT		goto yy_reject
#define	NLSTATE		(yy_lastc = YYNEWLINE)
#define	YY_INIT \
	(yy_start = yyleng = yy_end = 0, yy_lastc = YYNEWLINE)
#define	yymore()	goto yy_more
#define	yyless(n)	if ((n) < 0 || (n) > yy_end) ; \
			else { YY_SCANNER; yyleng = (n); YY_USER; }
#if defined (__cplusplus)
extern "C"  {
#endif /*cplusplus*/

YY_DECL	void	yy_reset YY_ARGS((void));
YY_DECL	int	input	YY_ARGS((void));
YY_DECL	int	unput	YY_ARGS((int c));

/* functions defined in libl.lib */
extern	int	yywrap	YY_ARGS((void));
extern	void	yyerror	YY_ARGS((char *fmt, ...));
extern	void	yycomment	YY_ARGS((char *term));
extern	int	yymapch	YY_ARGS((int delim, int escape));

#if defined (__cplusplus)
	}
#endif


#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdirefzone.h"
#include "tdireffunction.h"
#include <strroutines.h>
#include <string.h>
#include <tdimessages.h>
#include <treeshr.h>

#ifdef output
#undef output
#endif
#define output(c)		(c)

#define YYLEX			TdiLex

extern int TdiConvert();
extern struct marker *TdiYylvalPtr;

extern unsigned short Opcdollar, OpcZero,
	OpcAdd,	OpcAnd,	OpcConcat,	OpcDivide,	OpcEq,	
	OpcGe,		OpcGt,		OpcIand,	OpcInot,	OpcIor,
	OpcLe,		OpcLt,		OpcMultiply,	OpcNe,		OpcNot,	OpcOr,	
	OpcPower,	OpcPreDec,	OpcPreInc,	OpcPromote,	OpcDtypeRange,
	OpcShiftLeft,	OpcShiftRight,	OpcSubtract;

extern
	LEX_ERROR,	LEX_IDENT,	LEX_VBL,	LEX_TEXT,	LEX_VALUE,
	LEX_IN,		LEX_LAND,	LEX_LEQ,	LEX_LEQV,	LEX_LGE,	LEX_LOR,	LEX_MUL,
	LEX_ADD,	LEX_CONCAT,	LEX_IAND,	LEX_INC,	LEX_IOR,
	LEX_POINT,	LEX_POWER,	LEX_PROMO,	LEX_RANGE,	LEX_SHIFT,	LEX_BINEQ,
	LEX_LANDS,	LEX_LEQS,	LEX_LGES,	LEX_LORS,	LEX_MULS,	LEX_UNARYS;

#define _MOVC3(a,b,c) memcpy(c,b,a)
static int TdiLexBinEq(int token);

extern void TdiYyReset()
{
  yy_reset();
}

extern int TdiHash(  );

static void	upcase(char str[]) {
char	*pc;

	for (pc = str; *pc; ++pc) if (*pc >= 'a' && *pc <= 'z') *pc += 'A' - 'a';
}
/*--------------------------------------------------------
	Remove comment from the Lex input stream.
	Nested comments allowed. Len is not used.
	Limitation:	Not ANSI C standard use of delimiters.
*/
static int			TdiLexComment(
int			len,
char			str[],
struct marker		*mark_ptr)
{
char			c, c1;
int			count = 1;

	while (count) {
		if((c = input()) == '/') {
			if ((c1 = input()) == '*') ++count;
			else	unput(c1);
		}
		else if (c == '*') {
			if ((c1 = input()) == '/') --count;
			else	unput(c1);
		}
		else if (c == '\0') {
			TdiRefZone.l_status = TdiUNBALANCE;
			return 1;
		}
	}
	return 0;
}
/*--------------------------------------------------------
	Convert floating point values with the follow
ing
	syntax to internal representation via descriptors:
		[sign] decimal [fraction] exponent
		or [sign] [decimal] fraction [exponent]
	where:	decimal		[+|-] 0-9...
		fraction	. 0-9...
		exponent	[E|F|D|G|H|S|T] [+|-] 0-9...
	NEED to size based on exponent range and number of digits.
*/
static DESCRIPTOR(dfghst_dsc, "DFGHSTdfghst");
static DESCRIPTOR(e_dsc, "E");
static DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHST \t");

static int ConvertFloating(struct descriptor_s *str, struct descriptor_r *out_d)
{
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c,str->pointer,len);
  str_c[len]=0;
  if (out_d->length == sizeof(double))
  {
    double tmp;
    struct descriptor tmp_d = {sizeof(double),DTYPE_DOUBLE,CLASS_S,0};
    tmp_d.pointer = (char *)&tmp;
    tmp = atof(str_c);
    return TdiConvert(&tmp_d,out_d);
  }
  else
  {
    float tmp;
    struct descriptor tmp_d = {sizeof(float),DTYPE_FLOAT,CLASS_S,0};
    tmp_d.pointer = (char *)&tmp;
    sscanf(str_c,"%g",&tmp);
    return TdiConvert(&tmp_d,out_d);
  }
}

static int			TdiLexFloat(
int		str_len,
char		str[],
struct marker		*mark_ptr)
{
struct descriptor_s str_dsc = {0,DTYPE_T,CLASS_S,0};
int			bad, idx, status, tst, type;
static struct {
	unsigned short	length;
	unsigned char	dtype;
} table[] = {
		{4,	DTYPE_FLOAT},
		{8,	DTYPE_D},
		{8,	DTYPE_G},
		{16,	DTYPE_H},
		{4,	DTYPE_F},
		{4,	DTYPE_FS},
		{8,	DTYPE_FT}
	};
        str_dsc.length = str_len;
        str_dsc.pointer = str;
	upcase(str);
	/*******************
	Find final location.
	*******************/
	bad = StrFindFirstNotInSet(&str_dsc, &valid_dsc);
	if (bad > 0) str_dsc.length = bad - 1;

	/**********************
	Find special exponents.
	**********************/
	idx = StrFindFirstInSet(&str_dsc, &dfghst_dsc);
	if (idx) { switch (tst = str[idx-1]) {
  	  case 'D' :	type = 1;	break;
	  case 'G' :	type = 2;	break;
	  case 'H' :	type = 3;	break;
	  case 'F' :	type = 4;	break;
	  case 'S' :	type = 5;	break;
	  case 'T' : 	type = 6;	break;
	  }
          str[idx-1] = 'E';
        }
	else type = 0;

	MAKE_S(table[type].dtype, table[type].length, mark_ptr->rptr);

	status = ConvertFloating(&str_dsc, mark_ptr->rptr);
	if (status & 1 && bad > 0 && str[bad-1] != '\0') status = TdiEXTRANEOUS;

	mark_ptr->builtin = -1;
	if (status & 1) return(LEX_VALUE);
	TdiRefZone.l_status = status;
	return(LEX_ERROR);
}
/*--------------------------------------------------------
	Convert Lex input to identifier name or builtin.
	Clobbers string with upcase. IDENT token returns name.
	Note, Lex strings are NUL terminated.
*/
static int compare (
char				*s1,
struct TdiFunctionStruct	*s2)
{
	return strcmp(s1, s2->name);
}

static int			TdiLexIdent(
int 			len,
char			*str,
struct marker		*mark_ptr)
{
struct descriptor_s		sd = {0,DTYPE_T,CLASS_S,0};
int				j, token;

        sd.length = len;
        sd.pointer = str;
/*
	upcase(str);
*/
	mark_ptr->builtin = -1;
	MAKE_S(DTYPE_T, len, mark_ptr->rptr);
	_MOVC3(mark_ptr->rptr->length, str, (char *)mark_ptr->rptr->pointer);

	/*****************************
	$ marks next compile argument.
	$nnn marks nnn-th argument.
	*****************************/
	if (str[0] == '$') {
		for (j = len; --j > 0;) if (str[j] < '0' || str[j] > '9') break;
		if (j == 0) {
			mark_ptr->builtin = Opcdollar;
			return(LEX_IDENT);
		}
	}
	else if (str[0] == '_') return (LEX_VBL);

	/**********************
	Search of initial list.
	**********************/
	j = TdiHash(len, str);
	if (j < 0) {
		if (str[0] == '$') return(LEX_VBL);
		return(LEX_IDENT);
	}

	/**********************************************
	Note difference of pointers is divided by step.
	Standard function gives number. Token if named.
	**********************************************/
	mark_ptr->builtin = (short)j;
	token = TdiRefFunction[j].token;
	if ((token & LEX_K_NAMED) != 0) {
		token = token & LEX_M_TOKEN;
		if (token == LEX_IN
		|| token == LEX_LAND
		|| token == LEX_LEQ
		|| token == LEX_LEQV
		|| token == LEX_LGE
		|| token == LEX_LOR
		|| token == LEX_MUL) return TdiLexBinEq(token);
		return token;
	}
	return(LEX_IDENT);
}
/*--------------------------------------------------------
	Convert integer values with the following syntax
	to internal representation via descriptors:
		[space]...[sign][radix][digit]...[type]
	where:	sign	+	ignored
			-	negative
		radix	0B b	binary		digit	0-1
	(digit		0O o	octal			0-7
	required)	0D d	decimal(float)		0-9
			0X x	hexadecimal		0-9A-Fa-f
		type	SB b	signed byte
			SW w	signed word
			SL l	signed long
			SQ q	signed quadword
			SO o	signed octaword
			UB bu	unsigned byte
			UW wu	unsigned word
			UL lu	unsigned long
			UQ qu	unsigned quadword
			UO ou	unsigned octaword
	CAUTION must use unsigned char to avoid sign extend in hex.
	WARNING without following digit B and O radix become byte and octaword.
	WARNING hexadecimal strings must use SB or UB for bytes.

	Limitations:
	Depends on contiguous character set, 0-9 A-F a-f.
	Depends on right-to-left byte assignment.
	Depends on ones- or twos-complement.
	NEED size based on range, thus no overflow.
*/
#define len1 8		/*length of a word in bits*/
#define num1 16		/*number of words to accumulate, octaword*/

static int			TdiLexInteger(
int		str_len,
char			str[],
struct marker		*mark_ptr)
{
static struct {
	unsigned short	length;
	unsigned char	udtype, sdtype;
} table[] = {
	{1,	DTYPE_BU,DTYPE_B},
	{2,	DTYPE_WU,DTYPE_W},
	{4,	DTYPE_LU,DTYPE_L},
	{8,	DTYPE_QU,DTYPE_Q},
	{16,	DTYPE_OU,DTYPE_O},
};
char			sign, *now = &str[0], *end = &str[str_len];
unsigned char		*qptr, qq[num1], qtst;
int			carry = 0, radix;
int			length, is_signed, status = 1, tst, type;

	/******************************
	Remove leading blanks and tabs.
	******************************/
	while (now < end && (*now == ' ' || *now == '\t')) ++now;
	upcase(now);

	/*********
	Save sign.
	*********/
	if (now >= end) sign = '+';
	else if ((sign = *now) == '-' || sign == '+') ++now;

	/***************************************************
	Select radix. Must be followed by appropriate digit.
	Leading zero is required in our LEX.
	Watch, 0bu a proper unsigned byte is good and
	0bub a binary with no digits is bad. Reject 0b33.
	***************************************************/
	if (now < end && *now == '0') ++now;
	if (now+1 < end) {
		switch (tst = *now++) {
		case 'B' :	radix = 2;	break;
		case 'O' :	radix = 8;	break;
	/*	case 'D' :	radix = 10;	break;*/
		case 'X' :	radix = 16;	break;
		default :	--now;	radix = 0;	break;
		}
		if ((carry = radix) == 0) {radix = 10;}
		else {
			if	((tst = *now)		>= '0' && tst <= '9') carry = tst - '0';
			else if (tst >= 'A' && tst <= 'F') carry = tst - 'A' + 10;
			if (carry >= radix) --now;
		}
	}
	else radix = 10;

	/**************
	Convert number.
	**************/
	for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) *qptr = 0;
	for ( ; now < end; ++now) {
		if	((tst = *now) >= '0' && tst <= '9') carry = tst - '0';
		else if (tst >= 'A' && tst <= 'F') carry = tst - 'A' + 10;
		else break;
		if (carry >= radix) {carry = 0; break;}
		for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) {*qptr = (char)(carry += (int)*qptr * radix); carry >>= len1;}
	}

	/***********************************************************
	Negative numbers do negation until nonzero, then complement.
	Works for 1 or 2's complement, not sign and magnitude.
	Unsigned overflow: carry | sign, signed: carry | wrong sign.
	***********************************************************/
	if (sign == '-') {
		for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) if ((*qptr = (char)(- *qptr)) != 0) break;
		for ( ; ++qptr < &qq[num1]; ) *qptr = (char)(~ *qptr);
	}

	/*******************************
	Find output size and signedness.
	*******************************/
	is_signed = -1;
	type = 2;
	if (now < end) switch (tst = *now++) {
	case 'U' :	is_signed = 0;	break;
	case 'S' :	is_signed = 1;	break;
	case 'B' :	type = 0;	break;
	case 'W' :	type = 1;	break;
	case 'L' :	type = 2;	break;
	case 'Q' :	type = 3;	break;
	case 'O' :	type = 4;	break;
	default :	--now;		break;
	}

	if (now >= end) {}
	else if (is_signed < 0) switch (tst = *now++) {
	case 'U' :	is_signed = 0;	break;
	case 'S' :	is_signed = 1;	break;
	default :	--now;		break;
	}
	else switch (tst = *now++) {
	case 'B' :	type = 0;	break;
	case 'W' :	type = 1;	break;
	case 'L' :	type = 2;	break;
	case 'Q' :	type = 3;	break;
	case 'O' :	type = 4;	break;
	default :	--now;		break;
	}

	/*******************************
	Remove trailing blanks and tabs.
	*******************************/
	while (now < end && (*now == ' ' || *now == '\t')) ++now;

	length = table[type].length;
	MAKE_S((unsigned char)(is_signed ? table[type].sdtype : table[type].udtype), (unsigned short)length, mark_ptr->rptr);
	mark_ptr->builtin = -1;
	_MOVC3(length, (char *)qq, (char *)mark_ptr->rptr->pointer);

	/*************************
	Check the high order bits.
	*************************/
	if (now < end && *now != '\0') status = TdiEXTRANEOUS;

	qtst = (unsigned char)((is_signed && (char)qq[length-1] < 0) ? -1 : 0);
	if (carry != 0 || (is_signed && ((qtst != 0) ^ (sign == '-')))) status = TdiTOO_BIG;
	else for (qptr = &qq[length]; qptr < &qq[num1]; ++qptr) if (*qptr != qtst) status = TdiTOO_BIG;

	if (status & 1) return(LEX_VALUE);
	TdiRefZone.l_status = status;
	return(LEX_ERROR);
}
/*--------------------------------------------------------
	Convert Lex input to NID or absolute PATH.
*/
int			TdiLexPath(
int			len,
char			*str,
struct marker		*mark_ptr)
{
struct descriptor path_d = {0,DTYPE_T,CLASS_S,0};
int			nid, status, token = LEX_VALUE;
char *path;
        path_d.length = len;
        path_d.pointer = str;
	upcase(str);
	path=MdsDescrToCstring(&path_d);
	mark_ptr->builtin = -1;
	if (TdiRefZone.l_rel_path) {
	MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
		_MOVC3(len, str, (char *)mark_ptr->rptr->pointer);
	}
	else if (TreeFindNode(path, &nid) & 1) {
	MAKE_S(DTYPE_NID, (unsigned short)sizeof(nid), mark_ptr->rptr);
		*(int *)mark_ptr->rptr->pointer = nid;
	}
	else {
		struct descriptor	abs_dsc = {0,DTYPE_T,CLASS_D,0};
		char *apath = TreeAbsPath(path);
		if (apath != NULL)
		{
			unsigned short alen = (unsigned short)strlen(apath);
			StrCopyR(&abs_dsc,&alen,apath);
			TreeFree(apath);
			if (status & 1) {
			MAKE_S(DTYPE_PATH, abs_dsc.length, mark_ptr->rptr);
			_MOVC3(abs_dsc.length, abs_dsc.pointer, (char *)mark_ptr->rptr->pointer);
		}
		else	{
			TdiRefZone.l_status = status;
			token = LEX_ERROR;
		}
		StrFree1Dx(&abs_dsc);
	}
	MdsFree(path);
	}
	return token;
}
/*--------------------------------------------------------
	Remove arrow and trailing punctation.
*/
static int			TdiLexPoint(
int			len,
char			str[],
struct marker		*mark_ptr)
{
int		lenx = len - 2;
	while (str[lenx+1] == '.' || str[lenx+1] == ':') --lenx;
	mark_ptr->builtin = -1;
	MAKE_S(DTYPE_T, lenx, mark_ptr->rptr);
	_MOVC3((unsigned short)lenx, &str[2], (char *)mark_ptr->rptr->pointer);
	return LEX_POINT;
}
/*--------------------------------------------------------
	Recognize some graphic punctuation Lex symbols for YACC.
	Note must be acceptable in written form also: a<=b, a LE b, LE(a,b).
	Binary a<=(b,c) is OK, but unary <=(b,c) should not be.
*/
static int			TdiLexBinEq(
int			token)
{
char		cx;

	while ((cx = input()) == ' ' || cx == '\t') ;
	if (cx == '=') return (LEX_BINEQ);
	unput(cx);
	return token;
}

static int			TdiLexPunct(
int			len,
char			str[],
struct marker		*mark_ptr)
{
char		c0 = str[0], c1 = input();

	mark_ptr->rptr = 0;

	/********************
	Two graphic operator.
	********************/
	switch (c0) {
	case '!' :	if (c1 == '=') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '&' :	if (c1 == '&') {mark_ptr->builtin = OpcAnd;		return TdiLexBinEq	(LEX_LANDS);} break;
	case '*' :	if (c1 == '*') {mark_ptr->builtin = OpcPower;		return TdiLexBinEq	(LEX_POWER);} break;
	case '+' :	if (c1 == '+') {mark_ptr->builtin = OpcPreInc;	return			(LEX_INC);} break;
	case '-' :	if (c1 == '-') {mark_ptr->builtin = OpcPreDec;	return			(LEX_INC);} break;
/***			if (c1 == '>') {					return			(LEX_POINT);} break;***/
	case '.' :	if (c1 == '.') {mark_ptr->builtin = OpcDtypeRange;	return			(LEX_RANGE);} break;
	case '/' :	if (c1 == '/') {mark_ptr->builtin = OpcConcat;		return TdiLexBinEq	(LEX_CONCAT);} break;
/***                     else if (c1 == '*') return (TdiLexComment(len, str, mark_ptr) == 0) ? input() : 0; break; ***/
/***			if (c1 == '=') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);}*/
/***			if (c1 == ')') {					return 			']';} break;***/
/***	case '(' :	if (c1 == '/') {					return 			'[';} break;***/
	case '<' :	if (c1 == '<') {mark_ptr->builtin = OpcShiftLeft;	return TdiLexBinEq	(LEX_SHIFT);}
			if (c1 == '=') {mark_ptr->builtin = OpcLe;		return TdiLexBinEq	(LEX_LGES);}
			if (c1 == '>') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '=' :	if (c1 == '=') {mark_ptr->builtin = OpcEq;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '>' :	if (c1 == '=') {mark_ptr->builtin = OpcGe;		return TdiLexBinEq	(LEX_LGES);}
			if (c1 == '>') {mark_ptr->builtin = OpcShiftRight;	return TdiLexBinEq	(LEX_SHIFT);} break;
	case '|' :	if (c1 == '|') {mark_ptr->builtin = OpcOr;		return TdiLexBinEq	(LEX_LORS);} break;
	}
	unput(c1);

	/********************
	One graphic operator.
	********************/
	switch (c0) {
	case '!' :	mark_ptr->builtin = OpcNot;		return			(LEX_UNARYS);
/****	case '%' :	mark_ptr->builtin = OpcMOD;		return TdiLexBinEq	(LEX_MULS);****/
	case '&' :	mark_ptr->builtin = OpcIand;		return TdiLexBinEq	(LEX_IAND);
	case '*' :	mark_ptr->builtin = OpcMultiply;	return TdiLexBinEq	('*');
	case '+' :	mark_ptr->builtin = OpcAdd;		return TdiLexBinEq	(LEX_ADD);
	case '-' :	mark_ptr->builtin = OpcSubtract;	return TdiLexBinEq	(LEX_ADD);
	case '/' :	mark_ptr->builtin = OpcDivide;		return TdiLexBinEq	(LEX_MULS);
	case ':' :	mark_ptr->builtin = OpcDtypeRange;	return			(LEX_RANGE);
	case '<' :	mark_ptr->builtin = OpcLt;		return TdiLexBinEq	(LEX_LGES);
	case '>' :	mark_ptr->builtin = OpcGt;		return TdiLexBinEq	(LEX_LGES);
	case '@' :	mark_ptr->builtin = OpcPromote;	return			(LEX_PROMO);
	case '^' :	mark_ptr->builtin = OpcPower;		return TdiLexBinEq	(LEX_POWER);
	case '|' :	mark_ptr->builtin = OpcIor;		return TdiLexBinEq	(LEX_IOR);
	case '~' :	mark_ptr->builtin = OpcInot;		return			(LEX_UNARYS);
	}
	mark_ptr->builtin = -1;
	return(c0);
}
/*--------------------------------------------------------
	C-style text in matching quotes. Strict: must end in quote. Continuation: \ before newline.
	Limitation: Text is ASCII dependent in quotes.
	Limitation: No wide characters. L'\xabc'
	Code all ANSI C escapes.
	On \ followed by non-standard we remove \.
	NEED overflow check on octal and hex. Watch sign extend of char.
*/

int			TdiLexQuote(
int			len,
char			str[],
struct marker		*mark_ptr)
{
char		c, c1, *cptr = TdiRefZone.a_cur;
int		cur = 0, limit;

	while ((c = input()) && c != str[0]) if (c == '\\') input();
	limit = TdiRefZone.a_cur - cptr - 1;
	TdiRefZone.a_cur = cptr;
	MAKE_S(DTYPE_T, limit, mark_ptr->rptr);
	mark_ptr->builtin = -1;
	while (cur <= limit) {
		if ((c = input()) == str[0]) {
			mark_ptr->rptr->length = cur;
			return(LEX_TEXT);
		}
		else if (c == '\\') {
			c = input();
			switch (c) {
			default : break;		/*non-standard*/
			case '\n' : continue;		/*continuation ignores end of line*/
			case 'n' : c = '\n'; break;	/*NL/LF	newline*/
			case 't' : c = '\t'; break;	/*HT	horizontal tab*/
			case 'v' : c = '\v'; break;	/*VT	vertical tab*/
			case 'b' : c = '\b'; break;	/*BS	backspace*/
			case 'r' : c = '\r'; break;	/*CR	carriage return*/
			case 'f' : c = '\f'; break;	/*FF	formfeed*/
			case 'a' : c = '\007'; break;	/*BEL	audible alert*/
			case '\\' : break;		/*backslash*/
			case '\?' : break;		/*question mark*/
			case '\'' : break;		/*single quote*/
			case '\"' : break;		/*double quote*/
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
				c = c - '0';		/*octal number, 1-3 digits*/
				if ((c1 = input()) >= '0' && c1 <= '7') c = (c << 3) | (c1 - '0'); else unput(c1);
				if ((c1 = input()) >= '0' && c1 <= '7') c = (c << 3) | (c1 - '0'); else unput(c1);
				break;
			case 'x' :
				c = 0;			/*hex number, any number of digits*/
				while (c1 = input()) {
					if	(c1 >= '0' && c1 <= '9') c = (c << 4) | (c1 - '0');
					else if (c1 >= 'A' && c1 <= 'F') c = (c << 4) | (c1 - 'A' + 10);
					else if (c1 >= 'a' && c1 <= 'f') c = (c << 4) | (c1 - 'a' + 10);
					else break;
				}
				unput(c1);
			}
		}
		else if (c == '\0') break;
		mark_ptr->rptr->pointer[cur++] = c;
	}
	TdiRefZone.l_status = TdiUNBALANCE;
	return(LEX_ERROR);
}


#ifdef	YY_DEBUG
#undef	YY_DEBUG
#define	YY_DEBUG(fmt, a1, a2)	fprintf(stderr, fmt, a1, a2)
#else
#define	YY_DEBUG(fmt, a1, a2)
#endif

/*
 * The declaration for the lex scanner can be changed by
 * redefining YYLEX or YYDECL. This must be done if you have
 * more than one scanner in a program.
 */
#ifndef	YYLEX
#define	YYLEX yylex			/* name of lex scanner */
#endif

#ifndef YYDECL
#define	YYDECL	int YYLEX YY_ARGS((void))	/* declaration for lex scanner */
#endif

/*
 * stdin and stdout may not neccessarily be constants.
 * If stdin and stdout are constant, and you want to save a few cycles, then
 * #define YY_STATIC_STDIO 1.
 */
#define YY_STATIC_STDIO	0

#if YY_STATIC_STDIO
YY_DECL	FILE   *yyin = stdin;
YY_DECL	FILE   *yyout = stdout;
#else
YY_DECL	FILE   *yyin = (FILE *)0;
YY_DECL	FILE   *yyout = (FILE *)0;
#endif
YY_DECL	int	yylineno = 1;		/* line number */

/* yy_sbuf[0:yyleng-1] contains the states corresponding to yytext.
 * yytext[0:yyleng-1] contains the current token.
 * yytext[yyleng:yy_end-1] contains pushed-back characters.
 * When the user action routine is active,
 * yy_save contains yytext[yyleng], which is set to '\0'.
 * Things are different when YY_PRESERVE is defined. 
 */
static	yy_state_t yy_sbuf [YYLMAX+1];	/* state buffer */
static	int	yy_end = 0;		/* end of pushback */
static	int	yy_start = 0;		/* start state */
static	int	yy_lastc = YYNEWLINE;	/* previous char */
YY_DECL	int	yyleng = 0;		/* yytext token length */

#ifndef YY_PRESERVE	/* the efficient default push-back scheme */

static	char yy_save;	/* saved yytext[yyleng] */

#define	YY_USER	{ /* set up yytext for user */ \
		yy_save = yytext[yyleng]; \
		yytext[yyleng] = 0; \
	}
#define	YY_SCANNER { /* set up yytext for scanner */ \
		yytext[yyleng] = yy_save; \
	}

#else		/* not-so efficient push-back for yytext mungers */

static	char yy_save [YYLMAX];
static	char *yy_push = yy_save+YYLMAX;

#define	YY_USER { \
		size_t n = yy_end - yyleng; \
		yy_push = yy_save+YYLMAX - n; \
		if (n > 0) \
			memmove(yy_push, yytext+yyleng, n); \
		yytext[yyleng] = 0; \
	}
#define	YY_SCANNER { \
		size_t n = yy_save+YYLMAX - yy_push; \
		if (n > 0) \
			memmove(yytext+yyleng, yy_push, n); \
		yy_end = yyleng + n; \
	}

#endif

/*
 * The actual lex scanner (usually yylex(void)).
 * NOTE: you should invoke yy_init() if you are calling yylex()
 * with new input; otherwise old lookaside will get in your way
 * and yylex() will die horribly.
 */
YYDECL {
	register int c, i, yybase;
	unsigned	yyst;	/* state */
	int yyfmin, yyfmax;	/* yy_la_act indices of final states */
	int yyoldi, yyoleng;	/* base i, yyleng before look-ahead */
	int yyeof;		/* 1 if eof has already been read */



#if !YY_STATIC_STDIO
	if (yyin == (FILE *)0)
		yyin = stdin;
	if (yyout == (FILE *)0)
		yyout = stdout;
#endif

	yyeof = 0;
	i = yyleng;
	YY_SCANNER;

  yy_again:
	yyleng = i;
	/* determine previous char. */
	if (i > 0)
		yy_lastc = yytext[i-1];
	/* scan previously accepted token adjusting yylineno */
	while (i > 0)
		if (yytext[--i] == YYNEWLINE)
			yylineno++;
	/* adjust pushback */
	if (yy_end >= yyleng)
		yy_end -= yyleng;
	else
		yy_end = 0;
	memmove(yytext, yytext+yyleng, (size_t) yy_end);
	i = 0;

  yy_contin:
	yyoldi = i;

	/* run the state machine until it jams */
	yyst = yy_begin[yy_start + (yy_lastc == YYNEWLINE)];
	yy_sbuf[i] = (yy_state_t) yyst;
	do {
		YY_DEBUG(m_textmsg(1547, "<state %d, i = %d>\n", "1"), yyst, i);
		if (i >= YYLMAX)
			YY_FATAL(m_textmsg(1548, "Token buffer overflow", "1"));

		/* get input char */
		if (i < yy_end)
			c = yytext[i];		/* get pushback char */
		else if (!yyeof && (c = yygetc()) != EOF) {
			yy_end = i+1;
			yytext[i] = (char) c;
		} else /* c == EOF */ {
			c = EOF;		/* just to make sure... */
			if (i == yyoldi) {	/* no token */
				yyeof = 0;
				if (yywrap())
					return 0;
				else
					goto yy_again;
			} else {
				yyeof = 1;	/* don't re-read EOF */
				break;
			}
		}
		YY_DEBUG(m_textmsg(1549, "<input %d = 0x%02x>\n", "1"), c, c);

		/* look up next state */
		while ((yybase = yy_base[yyst]+c) > yy_nxtmax
		    || yy_check[yybase] != (yy_state_t) yyst) {
			if (yyst == yy_endst)
				goto yy_jammed;
			yyst = yy_default[yyst];
		}
		yyst = yy_next[yybase];
	  yy_jammed: ;
	  yy_sbuf[++i] = (yy_state_t) yyst;
	} while (!(yyst == yy_endst || YY_INTERACTIVE && yy_base[yyst] > yy_nxtmax && yy_default[yyst] == yy_endst));
	YY_DEBUG(m_textmsg(1550, "<stopped %d, i = %d>\n", "1"), yyst, i);
	if (yyst != yy_endst)
		++i;

  yy_search:
	/* search backward for a final state */
	while (--i > yyoldi) {
		yyst = yy_sbuf[i];
		if ((yyfmin = yy_final[yyst]) < (yyfmax = yy_final[yyst+1]))
			goto yy_found;	/* found final state(s) */
	}
	/* no match, default action */
	i = yyoldi + 1;
	output(yytext[yyoldi]);
	goto yy_again;

  yy_found:
	YY_DEBUG(m_textmsg(1551, "<final state %d, i = %d>\n", "1"), yyst, i);
	yyoleng = i;		/* save length for REJECT */
	
	/* pushback look-ahead RHS */
	if ((c = (int)(yy_la_act[yyfmin]>>9) - 1) >= 0) { /* trailing context? */
		unsigned char *bv = yy_look + c*YY_LA_SIZE;
		static unsigned char bits [8] = {
			1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7
		};
		while (1) {
			if (--i < yyoldi) {	/* no / */
				i = yyoleng;
				break;
			}
			yyst = yy_sbuf[i];
			if (bv[(unsigned)yyst/8] & bits[(unsigned)yyst%8])
				break;
		}
	}

	/* perform action */
	yyleng = i;
	YY_USER;
	switch (yy_la_act[yyfmin] & 0777) {
	case 0:
	{;}
	break;
	case 1:
	{nlpos();}
	break;
	case 2:
	{pos();  return	(TdiLexFloat(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 3:
	{pos(); return	(TdiLexFloat(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 4:
	{pos();  return	(TdiLexInteger(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 5:
	{pos();  return	(TdiLexIdent(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 6:
	{pos();  return	(TdiLexPath(		yyleng, yytext, TdiYylvalPtr));}
	break;
	case 7:
	{pos();  return	(TdiLexQuote(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 8:
	{pos();  return	(TdiLexPoint(	yyleng, yytext, TdiYylvalPtr));}
	break;
	case 9:
	{pos(); if      (TdiLexComment(      yyleng, yytext, TdiYylvalPtr)) return(LEX_ERROR);}
	break;
	case 10:
	{pos(); return	(TdiLexPunct(	yyleng, yytext, TdiYylvalPtr));}
	break;


	}
	YY_SCANNER;
	i = yyleng;
	goto yy_again;			/* action fell though */

  yy_reject:
	YY_SCANNER;
	i = yyoleng;			/* restore original yytext */
	if (++yyfmin < yyfmax)
		goto yy_found;		/* another final state, same length */
	else
		goto yy_search;		/* try shorter yytext */

  yy_more:
	YY_SCANNER;
	i = yyleng;
	if (i > 0)
		yy_lastc = yytext[i-1];
	goto yy_contin;
}
/*
 * Safely switch input stream underneath LEX
 */
typedef struct yy_save_block_tag {
	FILE	* oldfp;
	int	oldline;
	int	oldend;
	int	oldstart;
	int	oldlastc;
	int	oldleng;
	char	savetext[YYLMAX+1];
	yy_state_t	savestate[YYLMAX+1];
} YY_SAVED;

YY_SAVED *
yySaveScan YY_FNARG1(FILE*, fp)
{
	YY_SAVED * p;

	if ((p = (YY_SAVED *) malloc(sizeof(*p))) == NULL)
		return p;

	p->oldfp = yyin;
	p->oldline = yylineno;
	p->oldend = yy_end;
	p->oldstart = yy_start;
	p->oldlastc = yy_lastc;
	p->oldleng = yyleng;
	(void) memcpy(p->savetext, yytext, sizeof yytext);
	(void) memcpy((char *) p->savestate, (char *) yy_sbuf,
		sizeof yy_sbuf);

	yyin = fp;
	yylineno = 1;
	YY_INIT;

	return p;
}
/*f
 * Restore previous LEX state
 */
void
yyRestoreScan YY_FNARG1(YY_SAVED*, p)
{
	if (p == NULL)
		return;
	yyin = p->oldfp;
	yylineno = p->oldline;
	yy_end = p->oldend;
	yy_start = p->oldstart;
	yy_lastc = p->oldlastc;
	yyleng = p->oldleng;

	(void) memcpy(yytext, p->savetext, sizeof yytext);
	(void) memcpy((char *) yy_sbuf, (char *) p->savestate,
		sizeof yy_sbuf);
	free(p);
}
/*
 * User-callable re-initialization of yylex()
 */
void
yy_reset()
{
	YY_INIT;
	yylineno = 1;		/* line number */
}
/* get input char with pushback */
YY_DECL int
input()
{
	int c;
#ifndef YY_PRESERVE
	if (yy_end > yyleng) {
		yy_end--;
		memmove(yytext+yyleng, yytext+yyleng+1,
			(size_t) (yy_end-yyleng));
		c = yy_save;
		YY_USER;
#else
	if (yy_push < yy_save+YYLMAX) {
		c = *yy_push++;
#endif
	} else
		c = yygetc();
	yy_lastc = c;
	if (c == YYNEWLINE)
		yylineno++;
	return c;
}

/*f
 * pushback char
 */
YY_DECL int
unput YY_FNARG1(int, c)
{
#ifndef YY_PRESERVE
	if (yy_end >= YYLMAX)
		YY_FATAL(m_textmsg(1552, "Push-back buffer overflow", "1"));
	if (yy_end > yyleng) {
		yytext[yyleng] = yy_save;
		memmove(yytext+yyleng+1, yytext+yyleng,
			(size_t) (yy_end-yyleng));
		yytext[yyleng] = 0;
	}
	yy_end++;
	yy_save = (char) c;
#else
	if (yy_push <= yy_save)
		YY_FATAL(m_textmsg(1552, "Push-back buffer overflow", "1"));
	*--yy_push = c;
#endif
	if (c == YYNEWLINE)
		yylineno--;
	return c;
}

