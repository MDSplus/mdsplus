/*	tdirefzone.h
	References to zone used by Tdi1BUILD_FUNCTION, TdiYacc, and TdiLex_... .

	Ken Klare, LANL CTR-7	(c)1989,1990
*/
struct TdiZoneStruct {
		char	*a_begin;		/*beginning of text	*/
		char	*a_cur;			/*current character	*/
		char	*a_end;			/*end of buffer	+ 1	*/
		int	l_ok;			/*parsed correctly	*/
		int	l_status;		/*error code		*/
		void 	*l_zone;		/*virtual memory zone	*/
		struct descriptor_d *a_result; /*the answer		*/
		int	l_narg;			/*number of arguments	*/
		int	l_iarg;			/*current arguments	*/
		struct descriptor **a_list;	/*first argument	*/
		int     l_rel_path;		/*keep relative paths	*/
};
extern struct TdiZoneStruct TdiRefZone;

#include <libroutines.h>

	/*--------------------------------------------------
	Definitions needed by Lex and Yacc.
	--------------------------------------------------*/
struct marker {
		struct descriptor_r	*rptr;
		short			builtin;
		unsigned short		w_ok;
};

#define yylex()		TdiLex()

#define MAKE_S(dtype_in,bytes,out)\
	{int dsc_size = sizeof(struct descriptor_s);\
	int vm_size = dsc_size + (bytes);\
		LibGetVm(&vm_size,&(out),&TdiRefZone.l_zone);\
		((struct descriptor *)(out))->length = bytes;\
		((struct descriptor *)(out))->dtype = dtype_in;\
		((struct descriptor *)(out))->class = CLASS_S;\
		((struct descriptor *)(out))->pointer = (char *)(out) + dsc_size;}

#define MAKE_XD(dtype_in,bytes,out)\
	{int dsc_size = sizeof(struct descriptor_xd);\
	int vm_size = dsc_size + (bytes);\
		LibGetVm(&vm_size,&(out),&TdiRefZone.l_zone);\
		((struct descriptor_xd *)(out))->l_length = bytes;\
		((struct descriptor_xd *)(out))->length = 0;\
		((struct descriptor_xd *)(out))->dtype = dtype_in;\
		((struct descriptor_xd *)(out))->class = CLASS_XD;\
		((struct descriptor_xd *)(out))->pointer = (struct descriptor *)((char *)(out) + dsc_size);}

#define MAKE_R(ndesc,dtype_in,bytes,out)\
	{int dsc_size = sizeof($RECORD(ndesc));\
	int vm_size = dsc_size + (bytes);\
		LibGetVm(&vm_size,&(out),&TdiRefZone.l_zone);\
		((struct descriptor *)(out))->length = bytes;\
		((struct descriptor *)(out))->dtype = dtype_in;\
		((struct descriptor *)(out))->class = CLASS_R;\
		((struct descriptor *)(out))->pointer = (char *)(out) + dsc_size;}

	/*--------------------------------------------------
	Definitions needed by Lex and friends.
	--------------------------------------------------*/
#define LEX_M_TOKEN	1023
#define LEX_K_IMMED	2048
#define LEX_K_NAMED	4096
#define LEX_K_SYMBOL	8192
#define LEX_K_UNUSUAL	16384
	/**********************************************
	Give an extra semicolon. Must be able to unput.
	Caution: side effect--unput changes c pointer.
	**********************************************/

#define yyerror(s)	TdiRefZone.l_ok = yyval.mark.w_ok; return 1

#define yywrap()	1
#ifdef yygetc
#undef yygetc
#endif
#define yygetc()		(TdiRefZone.a_cur < TdiRefZone.a_end ? *TdiRefZone.a_cur++ : \
	        		(TdiRefZone.a_cur++ == TdiRefZone.a_end ? ';' : 0))
#ifdef output
#undef output
#endif
#define output(c)	(c)

#ifdef unput
#undef unput
#endif
#define unput(c)	(--TdiRefZone.a_cur, c)
#define pos()		(TdiYylvalPtr->w_ok = TdiRefZone.a_cur - TdiRefZone.a_begin)
#define nlpos()
extern int input();
