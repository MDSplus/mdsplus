#include <mdsdescrip.h>
#include <mdstypes.h>


//Conversion to Float/Doublee to 64 bit int.

#ifdef HAVE_WINDOWS_H

int MdsFloatToTime(struct descriptor *floatD, _int64u *retInt64)
{
	double doubleVal;
	_int64 currInt64_1, currInt64_2;

	if(floatD->class != CLASS_S || (floatD->dtype != DTYPE_FLOAT && floatD->dtype != DTYPE_DOUBLE))
		return 0;
 //Invalid time format
 	
	if(floatD->dtype == DTYPE_FLOAT)
		doubleVal = *(float *)floatD->pointer;
	else
		doubleVal = *(double *)floatD->pointer;

	if(doubleVal >= 0)
	{
		//To preserve monotonicy it suffices to complement the sign bit (most significative)
		*retInt64 = *(_int64u *)&doubleVal;
		*retInt64 |= 0x8000000000000000;
	}
	else
	{
		//It is necessary to operate on exponent and mantissa
		currInt64_1 = *(_int64 *)&doubleVal;
		//Change Mantissa
		currInt64_1 &= 0x000fffffffffffff;
		currInt64_2 =  0x000fffffffffffff;
		*retInt64 = (_int64u)(currInt64_2 - currInt64_1);
		//Change exponent
		currInt64_1 = *(_int64 *)&doubleVal;
		currInt64_1 &= 0x7ff0000000000000;
		currInt64_2 =  0x7ff0000000000000;
	//???	currInt64_1 = currInt64_1 - currInt64_2;
		currInt64_1 = (_int64u)(currInt64_2 - currInt64_1);


		*retInt64 |= currInt64_1;
	}
	return 1;
}

#else
int MdsFloatToTime(struct descriptor *floatD, _int64u *retInt64)
{
	double doubleVal;
	_int64 currInt64_1, currInt64_2;

	if(floatD->class != CLASS_S || (floatD->dtype != DTYPE_FLOAT && floatD->dtype != DTYPE_DOUBLE))
		return 0;
 //Invalid time format
 	
	if(floatD->dtype == DTYPE_FLOAT)
		doubleVal = *(float *)floatD->pointer;
	else
		doubleVal = *(double *)floatD->pointer;

	if(doubleVal >= 0)
	{
		//To preserve monotonicy it suffices to complement the sign bit (most significative)
		*retInt64 = *(_int64u *)&doubleVal;
		*retInt64 |= 0x8000000000000000LL;
	}
	else
	{
		//It is necessary to operate on exponent and mantissa
		currInt64_1 = *(_int64 *)&doubleVal;
		//Change Mantissa
		currInt64_1 &= 0x000fffffffffffffLL;
		currInt64_2 =  0x000fffffffffffffLL;
		*retInt64 = (_int64u)(currInt64_2 - currInt64_1);
		//Change exponent
		currInt64_1 = *(_int64 *)&doubleVal;
		currInt64_1 &= 0x7ff0000000000000LL;
		currInt64_2 =  0x7ff0000000000000LL;
	//???	currInt64_1 = currInt64_1 - currInt64_2;
		currInt64_1 = (_int64u)(currInt64_2 - currInt64_1);


		*retInt64 |= currInt64_1;
	}
	return 1;
}
#endif