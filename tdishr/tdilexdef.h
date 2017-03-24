#define tdiyywrap()	1
#ifdef getc
#undef getc
#endif
#define getc(file)		(TdiRefZone.a_cur < TdiRefZone.a_end ? *TdiRefZone.a_cur++ : \
	        		(TdiRefZone.a_cur++ == TdiRefZone.a_end ? ';' : 0))
#ifdef output
#undef output
#endif
#define output(c)	(c)

#ifdef unput
#undef unput
#endif
#define unput(c)	--TdiRefZone.a_cur
#define pos()		(TdiYylvalPtr->w_ok = TdiRefZone.a_cur - TdiRefZone.a_begin)
#define nlpos()
