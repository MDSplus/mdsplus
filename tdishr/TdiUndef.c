/*	TDI$UNDEF.C unwritten routines

	Ken Klare, LANL P-4	(c)1989,1990,1992
*/

#include <tdimessages.h>

int TdiON_ERROR=0;

#define UNDEF(x) int x() {return TdiNO_OPC;}
/*** PERMANENT ***/
UNDEF(Tdi1Else)
UNDEF(Tdi2undef)
UNDEF(Tdi3undef)

/*** TEMPORARY ***/
UNDEF(Tdi1Matrix)
UNDEF(Tdi1Shift)

UNDEF(Tdi1Backspace)
UNDEF(Tdi1Convolve)
UNDEF(Tdi1DateAndTime)
UNDEF(Tdi1Decode)
UNDEF(Tdi1Element)
UNDEF(Tdi1Encode)
UNDEF(Tdi1Endfile)
UNDEF(Tdi1Fft)
UNDEF(Tdi1Fit)
UNDEF(Tdi1Inquire)
UNDEF(Tdi1Interpol)
UNDEF(Tdi1Intersect)
UNDEF(Tdi1Inverse)
UNDEF(Tdi1Laminate)
UNDEF(Tdi1Matmul)
UNDEF(Tdi1OnError)
UNDEF(Tdi1Project)
UNDEF(Tdi1Promote)
UNDEF(Tdi1RandomSeed)
UNDEF(Tdi1Read)
UNDEF(Tdi1Rebin)
UNDEF(Tdi1Reshape)
UNDEF(Tdi1Rewind)
UNDEF(Tdi1Seek)
UNDEF(Tdi1String)
UNDEF(Tdi1Transfer)
UNDEF(Tdi1Unpack)
UNDEF(Tdi1Where)

UNDEF(Tdi3Ibits)
UNDEF(Tdi3Cshift)
UNDEF(Tdi3Derivative)
UNDEF(Tdi3Eoshift)
UNDEF(Tdi3Integral)
UNDEF(Tdi3Inverse)
UNDEF(Tdi3Ishftc)
UNDEF(Tdi3Matmul)
UNDEF(Tdi3MatRot)
UNDEF(Tdi3MatRotInt)
UNDEF(Tdi3Median)
UNDEF(Tdi3Modulo)
UNDEF(Tdi3Nearest)
UNDEF(Tdi3RcDroop)
UNDEF(Tdi3Sign)
UNDEF(Tdi3Smooth)
UNDEF(Tdi3Solve)
UNDEF(Tdi3SystemClock)
UNDEF(Tdi3Transpose)
UNDEF(Tdi3TransposeMul)
UNDEF(Tdi3Rms)
UNDEF(Tdi3StdDev)
