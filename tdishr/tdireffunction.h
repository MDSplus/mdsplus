/*	tdireffunction.h
	Internal/intrinsic function table reference.
	For insertion into TdiEvaluate TDI$$COMPILE and others.

	Ken Klare, LANL CTR-10	(c)1989
*/
#define TdiFUNCTION_DEF
#include <mdstypes.h>
#include <mdsplus/mdsconfig.h>
struct TdiFunctionStruct {
  char *name;			/*string to compile/decompile   */
  int (*f1) ();			/*routine to check arguments    */
  int (*f2) ();			/*routine to set conversions    */
  int (*f3) ();			/*routine to do operation       */
  unsigned char i1;		/*input minimum cat             */
  unsigned char i2;		/*input maximum cat             */
  unsigned char o1;		/*output minimum cat            */
  unsigned char o2;		/*output maximum cat            */
  unsigned char m1;		/*minimum arguments             */
  unsigned char m2;		/*maximum arguments             */
  unsigned int token;		/*YACC-LEX token for this entry */
};

extern const int TdiFUNCTION_MAX;
extern const struct TdiFunctionStruct TdiRefFunction[];
