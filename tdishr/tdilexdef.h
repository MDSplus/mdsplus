#define tdiyywrap()	1
#ifdef getc
#undef getc
#endif
#define getc(file)		(TDI_REFZONE.a_cur < TDI_REFZONE.a_end ? *TDI_REFZONE.a_cur++ : \
				(TDI_REFZONE.a_cur++ == TDI_REFZONE.a_end ? ';' : 0))
#ifdef output
#undef output
#endif
#define output(c)	(c)

#ifdef unput
#undef unput
#endif
#define unput(c)	--TDI_REFZONE.a_cur
#define pos()		(TdiYylvalPtr->w_ok = TDI_REFZONE.a_cur - TDI_REFZONE.a_begin)
#define nlpos()
