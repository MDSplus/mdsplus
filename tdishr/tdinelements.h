/*	tdinelements.h
	Internal routine for number of elements.

	Ken Klare, LANL P-4	(c)1989,1991
*/
#define N_ELEMENTS(dsc_ptr, count) \
	if (dsc_ptr->dtype == DTYPE_MISSING) count = 0;\
	else switch (dsc_ptr->class) {\
	default : count = 0; status = TdiINVCLADSC; break;\
	case CLASS_A : count = (int)((struct descriptor_a *)dsc_ptr)->arsize / (int)dsc_ptr->length; break;\
	case CLASS_S : case CLASS_D : count = 1; break;\
	}
