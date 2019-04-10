/*	tdinelements.h
	Internal routine for number of elements.

	Ken Klare, LANL P-4	(c)1989,1991
*/
#define N_ELEMENTS(dsc_ptr, count) \
	if (dsc_ptr->dtype == DTYPE_MISSING) {\
		count = 0;\
	} \
	else {\
		switch (dsc_ptr->class) {\
		default : count = 0; status = TdiINVCLADSC; break;\
		case CLASS_A : {				  \
		  array_coeff *aptr = (array_coeff *)dsc_ptr; \
		  count = 0; \
		  if (aptr->length != 0) { \
		    count = aptr->arsize / aptr->length; \
		  } else if ( aptr->dtype == DTYPE_T && aptr->aflags.coeff == 1 ) { \
		    int i; \
		    for (i=0;i<aptr->dimct;i++) { \
		      if (count == 0) count=aptr->m[i]; else count=count*aptr->m[i]; \
		    }\
		  }\
		  break;\
		}\
		case CLASS_S : case CLASS_D : count = 1; break;\
		} \
	}
