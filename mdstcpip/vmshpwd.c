/*

Title:		LGIHPWD
Author:		Shawn A. Clifford		(sysop@robot.nuceng.ufl.edu)
Date:		19-FEB-1993
Purpose:	Portable C version of the last 3 encryption methods for DEC's
		password hashing algorithms.

Usage:		status = Lgihpwd(&out_buf, &pwd_buf, encrypt, salt, &unam_buf);

		int Lgihpwd(
		    string *output_hash,
		    string *password,
		    unsigned char *encrypt,
		    unsigned short *salt,
		    string *username)

		Where:
			output_hash = 8 byte output buffer descriptor
			   password = n character password string descriptor
			    encrypt = 1 byte; value determines algorithm to use
				      0 -> CRC algorithm   (not supported)
				      1 -> Purdy algorithm
				      2 -> Purdy_V
				      3 -> Purdy_S (Hickory algorithm)
			       salt = 2 byte (word) random number
			   username = up to 31 character username descriptor

			status is either SsNORMAL (1) or SsABORT (44)

Compilation:	VAXC compiler, or 'gcc -traditional -c' to get hpwd.o file
		on a Un*x machine.

Comments:	The overall speed of this routine is not great.  This is
		dictated by the performance of the EMULQ routine which emulates
		the VAX EMULQ instruction (see notes in the EMULQ routine).
		If anyone can improve performance, or finds bugs in this
		program, please send me e-mail at the above address.

*/

#include <stdio.h>
#include <string.h>
#ifndef BIG_ENDIAN
#	include "b_order.h"
#endif


/* Uncomment this #define if you want the output in string comparable format */
/* #define STRING_OUTPUT	*/			/* String format */


#ifndef VMS
#	include <memory.h>			/* for memset/memcpy */
#endif

#ifdef VMS
#	include <ssdef.h>
#	include <descrip.h>
#else
#	define SsABORT	44
#	define SsNORMAL	1
#	define __SSDEF_LOADED	1
#endif

/* Encryption method codes */
#define UaiAD_II   0	     /* AUTODIN-II 32 bit crc code       */
#define UaiPURDY   1	     /* Purdy polynomial over salted input */
#define UaiPURDY_V 2	     /* Purdy polynomial + variable length username */
#define UaiPURDY_S 3      /* PURDY_V + additional bit rotation */


/** Type definitions **/

#ifndef __DESCRIP_LOADED
#define __DESCRIP_LOADED 1

/*
 ***  This section lifted from VAX C's  descrip.h  ***
 *
 *	DESCRIP.H - V3.0-003 - Argument Descriptor Formats
 *	(Based on the VAX Procedure Calling and Condition Handling Standard,
 *	 Revision 9.4 [13 March 1984];
 *	 see the "Introduction to VMS System Routines" manual for further
 *	 information.)
 */

/*
 *	Fixed-Length Descriptor:
 */
struct	descriptor_s
{
	unsigned short	length;	/* length of data item in bytes */
	unsigned char	dtype;	/* data type code (ignore) */
	unsigned char	class;	/* descriptor class code (ignore) */
	char		*pointer;	/* address of first byte of data storage */
};
#endif


typedef struct descriptor_s string;


/*
 *	Create a quadword data type as successive longwords.
 */
typedef struct {
	unsigned int	l_low,	/* Low order longword in the quadword */
			l_high;	/* High order longword in the quadword */
	} QUAD;


/*
 *	The following table of coefficients is used by the Purdy polynmial
 *	algorithm.  They are prime, but the algorithm does not require this.
 */
struct {
	QUAD	C1,
		C2,
		C3,
		C4,
		C5;
	} C = { {-83,  -1}, {-179, -1}, {-257, -1}, {-323, -1}, {-363, -1} };



/** Function prototypes **/

static void COLLAPSE_R2 (string *r3, char *r4, char r7);
						/* r3 -> input descriptor
						   r4 -> output buffer
						   r7 : Method 3 flag */
void Purdy (QUAD *U);				/* U -> output buffer */
static void PQMOD_R0 (QUAD *U);			/* U MOD P       */
static void PQEXP_R3 (QUAD *U,
		      unsigned int n,
		      QUAD *result);		/* U^n MOD P     */
static void PQADD_R0 (QUAD *U,
		      QUAD *Y,
		      QUAD *result);		/* U + Y MOD P   */
static void PQMUL_R2 (QUAD *U,
		      QUAD *Y,
		      QUAD *result);		/* U * Y MOD P   */
static void EMULQ    (unsigned int a,
		      unsigned int b,
		      QUAD *result);		/*  (a * b)	 */
static void PQLSH_R0 (QUAD *U,
		      QUAD *result); 		/* 2^32*U MOD P  */
void reverse_buf (char *buf, int size);



/** RELATIVELY GLOBAL variables **/

unsigned int 		MAXINT = 4294967295;	/* Largest 32 bit number */
unsigned int 		a = 59;	/* 2^64 - 59 is the biggest quadword prime */
QUAD			P;	/* P = 2^64 - 59 */
                        	


/** LGIHPWD entry point **/

int Lgihpwd(
	string *output_hash,
	string *password,
	unsigned char encrypt,
	unsigned short salt,
	string *username)
{

char *U;			/* Points to start of output buffer */
unsigned int    r0;		/* Index register */
string		*r3;		/* Holds descriptors for COLLAPSE_R2 */
QUAD        	*r4;		/* Address of the output buffer */
unsigned int	r5;		/* Length of username */
char		*r6,		/* Username descriptor */
		r7 = 0,		/* Flag for encryption method # 3 */
		*bytptr;	/* Pointer for adding in random salt */
QUAD		qword;		/* Quadword form of the output buffer */
char		uname[13] = "            ";

/* ------------------------------------------------------------------------ */


/* Check for invalid parameters */
if (output_hash->length != 8)
   {
      printf("Internal coding error, output_hash is not 8 bytes long.\n");
      exit(SsABORT);
   }
if ((encrypt < 1) || (encrypt > 3))
   {
      printf("Encrypt error; only algorithms 1-3 are currently supported.\n");
      exit(SsABORT);
   }
if (username->length > 31)
   {
      printf("Internal coding error, username is more than 31 bytes long.\n");
      exit(SsABORT);
   }


/* Setup pointer references */
U = output_hash->pointer; /* eg. U[1]..U[8] equals obuf[1]..obuf[8] */
r3 = password;			/* 1st COLLAPSE uses the password desc.   */
r4 = (QUAD *)output_hash->pointer;	/* @r4..@r4+7 equals obuf */
r5 = username->length;
r6 = (char *)username;
r7 = (encrypt == 3);


/* Define P = 2^64 - a = MAXINT.MAXINT - a + 1	*/
/* since MAXINT.MAXINT = 2^64 - 1		*/
P.l_high = MAXINT;
P.l_low = MAXINT - a + 1;


/* Clear the output buffer (zero the quadword) */
memset(U, 0, 8);


/* Check for the null password */
if (password->length == 0)
   {
	exit(SsNORMAL);
   }

switch (encrypt) {

   case UaiAD_II:		/* CRC algorithm with Autodin II poly */
	/* As yet unsupported */
	break;

   case UaiPURDY:		/* Purdy algorithm */

 	/* Use a blank padded username */
	strncpy(uname, username->pointer, r5);
	username->pointer = (char *)uname;
	username->length = 12;
	break;

   case UaiPURDY_V:		/* Purdy with blanks stripped */
   case UaiPURDY_S:		/* Hickory algorithm; Purdy_V with rotation */

	/* Check padding.  Don't count blanks in the string length.
	* Remember:  r6->username_descriptor   the first word is length, then
	* 2 bytes of class information (4 bytes total), then the address of the
	* buffer.  Usernames can not be longer than 31 characters.
	*/
	for (r0=0; (strcmp(r6+4+r0," ")==0) || (r0==r5) || (r0==31);
					r0++,(unsigned short)*r6++);
	/* This part       ^^^^^^^     is the buffer holding the username */

	/* If Purdy_S:  Bytes 0-1 => plaintext length */
	if (r7) {
	   *(unsigned short *)(U) = password->length;
	   if (BIG_ENDIAN) reverse_buf (U, 2);	/* Fix for byte order */
	}

	break;
   }


/* Collapse the password to a quadword U; buffer pointed to by r4 */
COLLAPSE_R2 (r3, (char *)r4, r7);
				/* r3 already points to password descriptor */


/* Add random salt into the middle of U */
/* This has to be done byte-wise because the Sun will not allow you */
/* to add unaligned words, or it will give you a bus error and core dump :) */
bytptr = (char *)((char *)r4 + 3 + 1);
*bytptr += (char)(salt>>8);			/* Add the high byte */

/* Check for carry out of the low byte */
bytptr--;
if ( (short)((unsigned char)*bytptr + (unsigned char)(salt & 0xff)) > 255) {
   *(bytptr + 1) += 1;				/* Account for the carry */
   }
*bytptr += (char)(salt & 0xff);			/* Add the low byte */



/* Collapse the username into the quadword */
r3 = username;			/* Point r3 to the valid username desriptor */
COLLAPSE_R2 (r3, (char *)r4, r7);

/* U (qword) contains the 8 character output buffer in quadword format */
if (BIG_ENDIAN) {
   reverse_buf(U, 8);
   memcpy(&qword.l_high, U, 4);	/* Reverse high and low longwords */
   memcpy(&qword.l_low, U+4, 4);
   }
else {
   memcpy (&qword, U, 8);
}


/* Run U through the polynomial mod P */
Purdy (&qword);


#ifdef STRING_OUTPUT

/* We only need to fix up the buffer for BIG_ENDIAN machines */
if (BIG_ENDIAN) {
   memcpy(U, &qword.l_high, 4);
   memcpy(U+4, &qword.l_low, 4);
   reverse_buf(U, 8);
   }
else {
   memcpy (U, &qword, 8);
}

#else 				/* Numerically correct answer */

/* Write qword back into the output buffer */
memcpy (U, &qword, 8);

#endif


/* Normal exit */
return SsNORMAL;

} /* LGIHPWD */




/***************	Functions Section	*******************/


/***************
   COLLAPSE_R2
 ***************/

static void COLLAPSE_R2 (string *r3, char *r4, char r7)
/*
   r3 :  input string descriptor
   r4 :  output buffer (quadword)
   r7 :  flag (1) if using Hickory method (encrypt = 3)

This routine takes a string of bytes (the descriptor for which is pointed
to by r3) and collapses them into a quadword (pointed to by r4).  It does
this by cycling around the bytes of the output buffer adding in the bytes of
the input string.  Additionally, after every 8 characters, each longword in
the resultant hash is rotated by one bit (PURDY_S only).

*/

{
unsigned short r0, r1;
unsigned int high, low, rotate;
char *r2, mask = -8;

/* ------------------------------------------------------------------------- */

r0 = r3->length;			/* Obtain the number of input bytes */

if (r0 != 0) {

   r2 = r3->pointer;		/* Obtain pointer to input string */

   for (; (r0 != 0); r0--) {		/* Loop until input string exhausted */

	r1 = (~mask & r0);		/* Obtain cyclic index into out buff */
	*(r4+r1) += *r2++;		/* Add in this character */

	if ((r7) && (r1 == 7))		/* If Purdy_S and last byte ... */
	{
	   if (BIG_ENDIAN) {
   		reverse_buf(r4, 8);
		memcpy ((unsigned int *)&low, r4+4, 4);
		memcpy ((unsigned int *)&high, r4, 4);
	      }
	   else {
		memcpy ((unsigned int *)&low, r4, 4);
		memcpy ((unsigned int *)&high, r4+4, 4);
	   }

	   /* Rotate first longword one bit */
	   rotate = low & 0x80000000;
	   low <<= 1;
	   rotate >>=31;
	   low |= rotate;

	   /* Rotate second longword one bit */
	   rotate = high & 0x80000000;
	   high <<= 1;
	   rotate >>=31;
	   high |= rotate;

	   /* Stick the rotated longwords back into the buffer */
	   if (BIG_ENDIAN) {
		memcpy (r4+4, &low, 4);
		memcpy (r4, &high, 4);
   		reverse_buf(r4, 8);
	      }
	   else {
		memcpy (r4, &low, 4);
		memcpy (r4+4, &high, 4);
	      }
	} /* if Purdy_S */

     } /* for loop */

   } /* if not 0 length buffer */

return;

} /* COLLAPSE_R2 */



/*********
   Purdy
 *********/

void Purdy (QUAD *U)

/*
U : output buffer (quadword)

This routine computes  f(U) = p(U) MOD P.  Where P is a prime of the form
P = 2^64 - a.  The function p is the following polynomial:

		X^n0 + X^n1*C1 + X^3*C2 + X^2*C3 + X*C4 + C5
                ^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^
		    part1		  part2

Note:  Part1 is calculated by its congruence  (X^(n0-n1) + C1)*X^n1
       finding X^n1, X^(n0-n1), X^(n0-n1)+C1, then 
       (X^(n0-n1) + C1)*X^n1  which equals  X^n0 + X^n1*C1

The input U is an unsigned quadword.
*/

{
QUAD 	     *r5 = (QUAD *)&C;	/* Address of table (C) */
unsigned int n0 = 16777213,	/* These exponents are prime, but this is  */
	      n1 = 16777153;	/* not required by the algorithm 	   */
				/* n0 = 2^24 - 3;  n1 = 2^24 - 63;  n0-n1 = 60*/
QUAD	      X,		/* The variable X in the polynomial */
	      stack,		/* Returned quadword on stack */
	      part1,		/* Collect the polynomial ... */
	      part2;		/* ... in two parts */


/* ------------------------------------------------------------------------- */


/* Ensure U less than P by taking U MOD P  */
/* Save copy of result:  X = U MOD P       */
X.l_low = U->l_low;
X.l_high = U->l_high;

PQMOD_R0 (&X);				/* Make sure U is less than P   */

PQEXP_R3 (&X, n1, &stack);		/* Calculate X^n1		*/

PQEXP_R3 (&X, (n0-n1), &part1);		/* Calculate X^(n0-n1)		*/

PQADD_R0 (&part1, r5, &part2);		/* X^(n0-n1) + C1 		*/

PQMUL_R2 (&stack, &part2, &part1);	/* X^n0 + X^n1*C1		*/
/* Part 1 complete */


r5++;					/* Point to C2			*/

PQMUL_R2 (&X, r5, &stack);		/* X*C2				*/

r5++;					/* C3				*/

PQADD_R0 (&stack, r5, &part2);		/* X*C2 + C3			*/

PQMUL_R2 (&X, &part2, &stack);		/* X^2*C2 + X*C3		*/

r5++;					/* C4				*/

PQADD_R0 (&stack, r5, &part2);		/* X^2*C2 + X*C3 + C4		*/

PQMUL_R2 (&X, &part2, &stack);		/* X^3*C2 + X^2*C3 + X*C4	*/

r5++;					/* C5				*/

PQADD_R0 (&stack, r5, &part2);		/* X^3*C2 + X^2*C3 + X*C4 + C5	*/
/* Part 2 complete */


/* Add in the high order terms.  Replace U with f(x) */
PQADD_R0 (&part1, &part2, (QUAD *)U);


/* Outta here... */
return;

} /* Purdy */



/*********
   EMULQ
 *********/

static void EMULQ (unsigned int a, unsigned int b, QUAD *result)

/*
a, b   : longwords that we want to multiply (quadword result)
result : the quadword result

This routine knows how to multiply two unsigned longwords, returning the
unsigned quadword product.

Originally I wrote this using a much faster routine based on a
divide-and-conquer strategy put forth in Knuth's "The Art of Computer
Programming, Vol. 2, Seminumerical Algorithms" but I could not make the routine
reliable.  There is some sort of fixup for sign compensation, much like for the
VAX EMULQ instruction (where for each signed argument you must add the other
argument to the high longword).

If anyone can improve this routine, please send me source code.

The original idea behind this algorithm was to build a 2n-by-n matrix for the
n-bit arguments to fill, shifting over each row like you would do by hand.  I
found a simple way to account for the carries and then get the final result,
but the routine was about 4 to 5 times slower than it is now.  Then I realized
that if I made the variables global, that would save a little time on
allocating space for the vectors and matrices.  Last, I removed the matrix, and
added all the values to the 'temp' vector on the fly.  This greatly increased
the speed, but still nowhere near Knuth or calling LIB$EMULQ.
*/

{
char bin_a[32], bin_b[32];
char temp[64], i, j;
char retbuf[8];


/* Initialize */
for (i=0; i<=63; i++) temp[i] = 0;
for (i=0; i<=31; i++) bin_a[i] = bin_b[i] = 0;
for (i=0; i<=7; retbuf[i]=0, i++);


/* Store the binary representation of a & b */
for (i=0; i<=31; i++) {
	bin_a[i] = a & 1;
	bin_b[i] = b & 1;
	a >>= 1;
	b >>= 1;
    }


/* Add in the shifted multiplicand */
for (i=0; i<=31; i++) {

        /* For each 1 in bin_b, add in bin_a, starting in the ith position */
	if (bin_b[i] == 1) {
		for (j=i; j<=i+31; j++)
			temp[j] += bin_a[j-i];
	}
    }


/* Carry into the next position and set the binary value */
for (j=0; j<=62; j++) {
	temp[j+1] += temp[j] / 2;
	temp[j] = temp[j] % 2;
    }
temp[63] = temp[63] % 2;


/* Convert binary bytes back into 8 packed bytes. */
/* LEAST SIGNIFICANT BYTE FIRST!!!  This is LITTLE ENDIAN format. */
for (i=0; i<=7; i++) {
	for (j=0; j<=7; j++) retbuf[i] += temp[i*8 + j] * (1<<j);
    }


/* Copy the 8 byte buffer into result */
if (BIG_ENDIAN) {
   reverse_buf(retbuf, sizeof(retbuf));

   /* Reverse high and low longwords */
   memcpy((char *)result, retbuf+4, 4);
   memcpy((char *)result+4, retbuf, 4);
   }
else {
   memcpy ((char *)result, retbuf, 8);
}

return;

} /* EMULQ */



/************
   PQMOD_R0
 ************/

static void PQMOD_R0 (QUAD *U)
/*
U : output buffer (quadword)

This routine replaces the quadword U with U MOD P, where P is of the form
P = 2^64 - a			(RELATIVELY GLOBAL a = 59)
  = FFFFFFFF.FFFFFFFF - 3B + 1	(MAXINT = FFFFFFFF = 4,294,967,295)
  = FFFFFFFF.FFFFFFC5

Method:  Since P is very nearly the maximum integer you can specify in a
quadword (ie. P = FFFFFFFFFFFFFFC5, there will be only 58 choices for
U that are larger than P (ie. U MOD P > 0).  So we check the high longword in
the quadword and see if all its bits are set (-1).  If not, then U can not
possibly be larger than P, and U MOD P = U.  If U is larger than MAXINT - 59,
then U MOD P is the differential between (MAXINT - 59) and U, else U MOD P = U.
If U equals P, then U MOD P = 0 = (P + 59).
*/

{

/* Check if U is larger/equal to P.  If not, then leave U alone. */

if (U->l_high == P.l_high && U->l_low >= P.l_low) {
	U->l_low += a;		/* This will have a carry out, and ...	*/
	U->l_high = 0;		/* the carry will make l_high go to 0   */
   }

return;

} /* PQMOD_R0 */



/************
   PQEXP_R3
 ************/

static void PQEXP_R3 (QUAD *U, unsigned int n, QUAD *result)
/*
U     : pointer to output buffer (quadword)
n     : unsigned longword (exponent for U)

The routine returns U^n MOD P where P is of the form P = 2^64-a.
U is a quadword, n is an unsigned longword, P is a RELATIVELY GLOBAL quad.

The method comes from Knuth, "The Art of Computer Programing, Vol. 2", section
4.6.3, "Evaluation of Powers."  This algorithm is for calculating U^n with
fewer than (n-1) multiplies.  The result is U^n MOD P only because the
multiplication routine is MOD P.  Knuth's example is from Pingala's Hindu
algorthim in the Chandah-sutra.
*/

{
QUAD Y, Z, Z1, Z2;		/* Intermediate factors for U */
unsigned int odd;		/* Test for n odd/even */


/* Initialize */
Y.l_low = 1;			/* Y = 1 */
Y.l_high = 0;
Z = *U;


/* Loop until n is zero */
while (n != 0) {

   /* Test n */
   odd = n % 2;

   /* Halve n */
   n /= 2;

   /* If n was odd, then we need an extra x (U) */
   if (odd) 	/* n was odd */	{
      PQMUL_R2 (&Y, &Z, result);
      if (n == 0)
	 break;			/* This is the stopping condition */
				/* Disregard the exponent, which  */
				/* would be the final Z^2.	  */
      Y = *result;		/* Copy for next pass */
      }

   /* Square Z; this squares the current power for Z */
   Z1 = Z2 = Z;
   PQMUL_R2 (&Z1, &Z2, &Z);

}

return;

} /* PQEXP_R3 */



/************
   PQMUL_R2
 ************/

static void PQMUL_R2 (QUAD *U, QUAD *Y, QUAD *result)

/*
U, Y : quadwords we want to multiply

Computes the product U*Y MOD P where P is of the form P = 2^64 - a.
U, Y are quadwords less than P.  The product replaces U and Y on the stack.

The product may be formed as the sum of four longword multiplications
which are scaled by powers of 2^32 by evaluating:

	2^64*v*z + 2^32*(v*y + u*z) + u*y
        ^^^^^^^^   ^^^^^^^^^^^^^^^^   ^^^
        part1      part2 & part3      part4

The result is computed such that division by the modulus P is avoided.

u is the low longword of  U;	u = U.l_low
v is the high longword of U;	v = U.l_high
y is the low longword of  Y;	y = Y.l_low
z is the high longword of Y;	z = Y.l_high
*/

{
QUAD stack, part1, part2, part3, part4;


EMULQ(U->l_high, Y->l_high, &stack);		/* Multiply   v*z      */

PQMOD_R0(&stack);				/* Get   (v*z) MOD P   */


/*** 1st term ***/
PQLSH_R0(&stack, &part1);			/* Get   2^32*(v*z) MOD P  */


EMULQ(U->l_high, Y->l_low, &stack);		/* Multiply   v*y      */

PQMOD_R0(&stack);				/* Get   (v*y) MOD P   */

EMULQ(U->l_low, Y->l_high, &part2);		/* Multiply   u*z      */

PQMOD_R0(&part2);				/* Get   (u*z) MOD P   */

PQADD_R0 (&stack, &part2, &part3);		/* Get   (v*y + u*z)   */

PQADD_R0 (&part1, &part3, &stack);   /* Get   2^32*(v*z) + (v*y + u*z) */


/*** 1st & 2nd terms ***/
PQLSH_R0(&stack, &part1);	/* Get   2^64*(v*z) + 2^32*(v*y + u*z) */


EMULQ(U->l_low, Y->l_low, &stack);		/* Multiply   u*y      */


/*** Last term ***/
PQMOD_R0(&stack);				


PQADD_R0(&part1, &stack, result);		/* Whole thing */

return;

} /* PQMUL_R2 */



/************
   PQLSH_R0
 ************/

static void PQLSH_R0 (QUAD *U, QUAD *result)

/*
Computes the product 2^32*U MOD P where P is of the form P = 2^64 - a.
U is a quadword less than P.

This routine is used by PQMUL in the formation of quadword products in
such a way as to avoid division by modulus the P.
The product 2^64*v + 2^32*u is congruent a*v + 2^32*U MOD P.

u is the low longword in U
v is the high longword in U
*/

{
QUAD stack;


/* Get	a*v   */
EMULQ(U->l_high, a, &stack);

/* Form  Y = 2^32*u  */
U->l_high = U->l_low;
U->l_low = 0;


/* Get	U + Y MOD P  */
PQADD_R0 (U, &stack, result);

return;

} /* PQLSH_R0 */



/************
   PQADD_R0
 ************/

static void PQADD_R0 (QUAD *U, QUAD *Y, QUAD *result)

/*
U, Y : quadwords that we want to add


Computes the sum U + Y MOD P where P is of the form P = 2^64 - a.
U, Y are quadwords less than P.

Fixed with the help of the code written by Terence Lee (DEC/HKO).
*/

{
unsigned int carry = 0;


/* Add the low longwords, checking for carry out */
if ( (MAXINT - U->l_low) < Y->l_low ) carry = 1;
result->l_low = U->l_low + Y->l_low;

/* Add the high longwords */
result->l_high = U->l_high + Y->l_high + carry;

/* Test for carry out of high bit in the quadword */
if ( (MAXINT - U->l_high) < (Y->l_high + carry) )
	carry = 1;
else
	carry = 0;

/* Check if we have to MOD P the result */
if (!carry && Y->l_high != MAXINT) return;	/* Outta here? */
if ( Y->l_low > (MAXINT - a) )
	carry = 1;
else
	carry = 0;
result->l_low += a;				/* U + Y MOD P */
result->l_high += carry;


return;						/* Outta here! */
} /* PQADD_R0 */



void reverse_buf (char *buf, int size)

/*
This routine will take an array of bytes (char) and reverses their order in
the array.  Useful for correcting for byte order different machines.

Usage:	reverse_buf ((char *)&buffer, sizeof(buffer));
*/

{
char	*temp;
int	i;

temp = (char *)malloc(size);
if (temp != NULL) {
   for (i=0; i<=(size-1); i++) *(temp+i) = *(buf+(size-i-1));
   for (i=0; i<=(size-1); i++) *(buf+i) = *(temp+i);
   free(temp);
   }
else {
   fputs("Memory allocation error in 'reverse_buf'\n", stderr);
   exit(0);
   }

} /* reverse_buf */
