/*
 *	Codes for dsc$b_class:
 *
 *	Reserved descriptor class codes:
 *	codes 15-191 are reserved to DIGITAL;
 *	codes 160-191 are reserved to DIGITAL facilities for facility-specific
 *purposes; codes 192-255 are reserved for DIGITAL's Computer Special Systems
 *Group and for customers for their own use.
 */
DEFINE(S, 1)     /* fixed-length descriptor */
DEFINE(D, 2)     /* dynamic string descriptor */
DEFINE(V, 3)     /* variable buffer descriptor;  reserved for use by DIGITAL */
DEFINE(A, 4)     /* array descriptor */
DEFINE(P, 5)     /* procedure descriptor */
DEFINE(PI, 6)    /* procedure incarnation descriptor;  obsolete */
DEFINE(J, 7)     /* label descriptor;  reserved for use by the VMS Debugger */
DEFINE(JI, 8)    /* label incarnation descriptor;  obsolete */
DEFINE(SD, 9)    /* decimal string descriptor */
DEFINE(NCA, 10)  /* noncontiguous array descriptor */
DEFINE(VS, 11)   /* varying string descriptor */
DEFINE(VSA, 12)  /* varying string array descriptor */
DEFINE(UBS, 13)  /* unaligned bit string descriptor */
DEFINE(UBA, 14)  /* unaligned bit array descriptor */
DEFINE(SB, 15)   /* string with bounds descriptor */
DEFINE(UBSB, 16) /* unaligned bit string with bounds descriptor */
DEFINE(XD, 192)
DEFINE(XS, 193)
DEFINE(R, 194)
DEFINE(CA, 195)
DEFINE(APD, 196)
