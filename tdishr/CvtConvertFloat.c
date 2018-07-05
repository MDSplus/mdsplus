/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsplus.h>
#include <inttypes.h>
#include <STATICdef.h>

/** Adapted from VMS V7.0 sources CvtConvertFloat.lis                      **/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1991                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/

/*
**=============================================================================
**
**  File:               CvtConvertFloat.c
**
**  Version:            1-004
**
**  Facility:           CVTRTL - VMS Conversion Run Time Library
**
**  Abstract:           CvtConvertFloat and supporting routines.
**
**  Author:             Jeffrey S. Young
**
**  Creation Date:      25-Jun-1991
**
**  Modification History:
**  ---------------------------------------------------------------------------
**  1-004 29-Jul-1993 JSY  Fix unpack_ieee_x to properly shuffle bytes to
**                         little endian format.
**
**  1-003 07-Jun-1993 JSY  Add IEEE X floating point type.
**
**  1-002 31-Oct-1991 JSY  Change CvtConvert to CvtConvertFloat,
**                         Add IBM and Cray floating point types.
**
**  1-001 25-Jun-1991 JSY  Original version.
**
**=============================================================================
*/

/*
**=============================================================================
** ROUTINE TABLE OF CONTENTS:
**-----------------------------------------------------------------------------
**
** CvtConvertFloat              - General purpose conversion routine between
**                                two floating point data types.
**
** pack_vax_f                   - Converts the standard intermediate data type
**                                to VAX F_Floating.
**
** pack_vax_d                   - Converts the standard intermediate data type
**                                to VAX D_Floating.
**
** pack_vax_g                   - Converts the standard intermediate data type
**                                to VAX G_Floating.
**
** pack_vax_h                   - Converts the standard intermediate data type
**                                to VAX H_Floating.
**
** pack_ieee_s                  - Converts the standard intermediate data type
**                                to IEEE S.
**
** pack_ieee_t                  - Converts the standard intermediate data type
**                                to IEEE T.
**
** pack_ieee_x                  - Converts the standard intermediate data type
**                                to IEEE X.
**
** pack_ibm_long                - Converts the standard intermediate data type
**                                to IBM Long.
**
** pack_ibm_short               - Converts the standard intermediate data type
**                                to IBM Short.
**
** pack_cray                    - Converts the standard intermediate data type
**                                to CRAY floating point.
**
** _round                       - Rounds the standard intermediate data type
**                                according to specified rounding rules.
**
** unpack_vax_f                 - Converts VAX F_Floating point to the
**                                standard intermediate data type
**
** unpack_vax_d                 - Converts VAX D_Floating point to the
**                                standard intermediate data type
**
** unpack_vax_g                 - Converts VAX G_Floating point to the
**                                standard intermediate data type
**
** unpack_vax_h                 - Converts VAX H_Floating point to the
**                                standard intermediate data type
**
** unpack_ieee_s                - Converts IEEE S point to the standard
**                                intermediate data type
**
** unpack_ieee_t                - Converts IEEE T point to the standard
**                                intermediate data type
**
** unpack_ieee_x                - Converts IEEE X point to the standard
**                                intermediate data type
**
** unpack_ibm_long              - Converts IBM Long to the standard
**                                intermediate data type
**
** unpack_ibm_short             - Converts IBM Short to the standard
**                                intermediate data type
**
** unpack_cray                  - Converts Cray floating point to the standard
**                                intermediate data type
**
** debug_indent_print           - idented debug information printing routine
**=============================================================================
*/

/*
**=============================================================================
**  DEFINITIONS
**=============================================================================
*/

#define VAX_F DTYPE_F		/* VAX F     Floating point data    */
#define VAX_D DTYPE_D		/* VAX D     Floating point data    */
#define VAX_G DTYPE_G		/* VAX G     Floating point data    */
#define VAX_H DTYPE_H		/* VAX H     Floating point data    */
#define IEEE_S DTYPE_FS		/* IEEE S    Floating point data    */
#define IEEE_T DTYPE_FT		/* IEEE T    Floating point data    */
#define IBM_LONG 6		/* IBM Long  Floating point data    */
#define IBM_SHORT 7		/* IBM Short Floating point data    */
#define CRAY 8			/* Cray      Floating point data    */
#define IEEE_X 9		/* IEEE X    Floating point data    */
#define CVT_M_ROUND_TO_NEAREST 0x1
#define CVT_M_TRUNCATE 0x2
#define CVT_M_ROUND_TO_POS 0x4
#define CVT_M_ROUND_TO_NEG 0x8
#define CVT_M_VAX_ROUNDING 0x10
#define CVT_M_BIG_ENDIAN 0x20
#define CVT_M_ERR_UNDERFLOW 0x40
#define CVT_M_SPARE2 0xFFFFFF80
struct cvt_r_conversion_options {
  unsigned cvt_v_round_to_nearest:1;
  unsigned cvt_v_truncate:1;
  unsigned cvt_v_round_to_pos:1;
  unsigned cvt_v_round_to_neg:1;
  unsigned cvt_v_vax_rounding:1;
  unsigned cvt_v_big_endian:1;
  unsigned cvt_v_err_underflow:1;
  unsigned cvt_v_spare2:25;
};

#define CvtFACILITY 1530
#define CvtNORMAL 100302857
#define CvtINPCONERR 100302866
#define CvtINVINPTYP 100302874
#define CvtINVOPT 100302882
#define CvtINVOUTTYP 100302890
#define CvtINVVAL 100302898
#define CvtNEGINF 100302906
#define CvtOUTCONERR 100302914
#define CvtOVERFLOW 100302922
#define CvtPOSINF 100302930
#define CvtUNDERFLOW 100302938
typedef uint8_t CVT_BYTE;
typedef CVT_BYTE *CVT_BYTE_PTR;
typedef CVT_BYTE CVT_VAX_F[4];
typedef CVT_BYTE CVT_VAX_D[8];
typedef CVT_BYTE CVT_VAX_G[8];
typedef CVT_BYTE CVT_VAX_H[16];
typedef CVT_BYTE CVT_IEEE_S[4];
typedef CVT_BYTE CVT_IEEE_T[8];
typedef CVT_BYTE CVT_IEEE_X[16];
typedef CVT_BYTE CVT_IBM_SHORT[4];
typedef CVT_BYTE CVT_IBM_LONG[8];
typedef CVT_BYTE CVT_CRAY[8];
typedef uint32_t CVT_STATUS;
/*
typedef struct {unsigned hi : 7; unsigned exp :  8; unsigned sign : 1; unsigned low : 16;} f_float;
typedef struct {unsigned hi : 7; unsigned exp :  8; unsigned sign : 1; unsigned low : 16; unsigned int low2;} d_float;
typedef struct {unsigned hi : 4; unsigned exp : 11; unsigned sign : 1; unsigned low : 16; unsigned int low2;} g_float;
typedef struct {                 unsigned exp : 15; unsigned sign : 1; unsigned low : 16; unsigned int low2[3];} h_float;
typedef struct {unsigned low:16; unsigned hi : 7;   unsigned exp:8;    unsigned sign :1;} s_float;
typedef struct {unsigned int low2; unsigned low:16; unsigned hi : 4; unsigned exp:11; unsigned sign :1;} t_float;
*/

#define f_float_exp(val) ((*(int *)val >> 7) & 0xff)
#define f_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandF(val) ((f_float_exp(val) == 0) && (f_float_sign(val) == 1))
#define s_float_exp(val) ((*(int *)val >> 23) & 0xff)
#define IsRoprandS(val) (s_float_exp(val) == 255)
#define IsRoprandD(val) IsRoprandF(val)
#define g_float_exp(val) ((*(int *)val >> 4) & 0x7ff)
#define g_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandG(val) ((g_float_exp(val) == 0) && (g_float_sign(val) == 1))
#define t_float_exp(val) ((((int *)val)[1] >> 20) & 0x7ff)
#define IsRoprandT(val) (t_float_exp(val) == 2047)

#define cvt_s_normal                    CvtNORMAL
#define cvt_s_input_conversion_error    CvtINPCONERR
#define cvt_s_invalid_input_type        CvtINVINPTYP
#define cvt_s_invalid_option            CvtINVOPT
#define cvt_s_invalid_output_type       CvtINVOUTTYP
#define cvt_s_invalid_value             CvtINVVAL
#define cvt_s_neg_infinity              CvtNEGINF
#define cvt_s_output_conversion_error   CvtOUTCONERR
#define cvt_s_overflow                  CvtOVERFLOW
#define cvt_s_pos_infinity              CvtPOSINF
#define cvt_s_underflow                 CvtUNDERFLOW
#define RAISE(COND)                     return(COND)

/*
**=============================================================================
**  UNPACKED REAL:
**
**  [0]: excess 2147483648 (2 ^ 31) binary exponent
**  [1]: mantissa: msb ------>
**  [2]: -------------------->
**  [3]: -------------------->
**  [4]: ----------------> lsb
**  [5]: 28 unused bits, invalid bit, infinity bit, zero bit, negative bit
**
**  All fraction bits are explicit and are normalized
**  s.t. 0.5 <= fraction < 1.0
**
**=============================================================================
*/

typedef uint32_t UNPACKED_REAL[6];
typedef UNPACKED_REAL *UNPACKED_REAL_PTR;

/*
** Flags and flag patterns for use with the UNPACKED_REAL type.
**-----------------------------------------------------------------------------
*/
#define U_R_EXP         0
#define U_R_FLAGS       5

#define U_R_NEGATIVE    1
#define U_R_ZERO        2
#define U_R_INFINITY    4
#define U_R_INVALID     8
#define U_R_UNUSUAL (U_R_ZERO | U_R_INFINITY | U_R_INVALID)

#define U_R_BIAS        2147483648LU

/*
** Special floating point constant definitions
**=============================================================================
*/
STATIC_CONSTANT uint32_t const vax_c[] = {

  0x00008000, 0x00000000, 0x00000000, 0x00000000,	/* ROPs */
  0x00000000, 0x00000000, 0x00000000, 0x00000000,	/* zeros */
  0xffff7fff, 0xffffffff, 0xffffffff, 0xffffffff,	/* +huge */
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,	/* -huge */
};

STATIC_CONSTANT uint32_t const ieee_s[] = {

  0x7fbfffff,			/* little endian ieee s nan */
  0xffffbf7f,			/* big endian ieee s nan */
  0x00000000,			/* le ieee s +zero */
  0x00000000,			/* be ieee s +zero */
  0x80000000,			/* le ieee s -zero */
  0x00000080,			/* be ieee s -zero */
  0x7f7fffff,			/* le ieee s +huge */
  0xffff7f7f,			/* be ieee s +huge */
  0xff7fffff,			/* le ieee s -huge */
  0xffff7fff,			/* be ieee s -huge */
  0x7f800000,			/* le ieee s +infinity */
  0x0000807f,			/* be ieee s +infinity */
  0xff800000,			/* le ieee s -infinity */
  0x000080ff,			/* be ieee s -infinity */
};

STATIC_CONSTANT uint32_t const ieee_t[] = {

  0xffffffff, 0x7ff7ffff,	/* le ieee t nan */
  0xfffff77f, 0xffffffff,	/* be ieee t nan */
  0x00000000, 0x00000000,	/* le ieee t +zero */
  0x00000000, 0x00000000,	/* be ieee t +zero */
  0x00000000, 0x80000000,	/* le ieee t -zero */
  0x00000080, 0x00000000,	/* be ieee t -zero */
  0xffffffff, 0x7fefffff,	/* le ieee t +huge */
  0xffffef7f, 0xffffffff,	/* be ieee t +huge */
  0xffffffff, 0xffefffff,	/* le ieee t -huge */
  0xffffefff, 0xffffffff,	/* be ieee t -huge */
  0x00000000, 0x7ff00000,	/* le ieee t +infinity */
  0x0000f07f, 0x00000000,	/* be ieee t +infinity */
  0x00000000, 0xfff00000,	/* le ieee t -infinity */
  0x0000f0ff, 0x00000000,	/* be ieee t -infinity */
};

STATIC_CONSTANT uint32_t const ieee_x[] = {

  0xffffffff, 0xffffffff, 0xffffffff, 0x7fff7fff,	/* le ieee x nan */
  0xff7fff7f, 0xffffffff, 0xffffffff, 0xffffffff,	/* be ieee x nan */
  0x00000000, 0x00000000, 0x00000000, 0x00000000,	/* le ieee x +zero */
  0x00000000, 0x00000000, 0x00000000, 0x00000000,	/* be ieee x +zero */
  0x00000000, 0x00000000, 0x00000000, 0x80000000,	/* le ieee x -zero */
  0x00000080, 0x00000000, 0x00000000, 0x00000000,	/* be ieee x -zero */
  0xffffffff, 0xffffffff, 0xffffffff, 0x7ffeffff,	/* le ieee x +huge */
  0xfffffe7f, 0xffffffff, 0xffffffff, 0xffffffff,	/* be ieee x +huge */
  0xffffffff, 0xffffffff, 0xffffffff, 0xfffeffff,	/* le ieee x -huge */
  0xfffffeff, 0xffffffff, 0xffffffff, 0xffffffff,	/* be ieee x -huge */
  0x00000000, 0x00000000, 0x00000000, 0x7fff0000,	/* le ieee x +infinity */
  0x0000ff7f, 0x00000000, 0x00000000, 0x00000000,	/* be ieee x +infinity */
  0x00000000, 0x00000000, 0x00000000, 0xffff0000,	/* le ieee x -infinity */
  0x0000ffff, 0x00000000, 0x00000000, 0x00000000,	/* be ieee x -infinity */
};

STATIC_CONSTANT uint32_t const ibm_s[] = {

  0x000000ff,			/* ibm s invalid */
  0x00000000,			/* ibm s +zero */
  0x00000080,			/* ibm s -zero */
  0xffffff7f,			/* ibm s +huge */
  0xffffffff,			/* ibm s -huge */
  0xffffff7f,			/* ibm s +infinity */
  0xffffffff,			/* ibm s -infinity */

};

STATIC_CONSTANT uint32_t const ibm_l[] = {

  0x000000ff, 0x00000000,	/* ibm t invalid */
  0x00000000, 0x00000000,	/* ibm t +zero */
  0x00000080, 0x00000000,	/* ibm t -zero */
  0xffffff7f, 0xffffffff,	/* ibm t +huge */
  0xffffffff, 0xffffffff,	/* ibm t -huge */
  0xffffff7f, 0xffffffff,	/* ibm t +infinity */
  0xffffffff, 0xffffffff,	/* ibm t -infinity */

};

STATIC_CONSTANT uint32_t const cray[] = {

  0x00000060, 0x00000000,	/* cray invalid */
  0x00000000, 0x00000000,	/* cray +zero */
  0x00000080, 0x00000000,	/* cray -zero */
  0xffffff5f, 0xffffffff,	/* cray +huge */
  0xffffffdf, 0xffffffff,	/* cray -huge */
  0x00000060, 0x00000000,	/* cray +infinity */
  0x000000e0, 0x00000000,	/* cray -infinity */

};

/*
** Standard names for the special floating point constants.
**-----------------------------------------------------------------------------
*/
#define VAX_F_INVALID &vax_c[0]
#define VAX_D_INVALID &vax_c[0]
#define VAX_G_INVALID &vax_c[0]
#define VAX_H_INVALID &vax_c[0]

#define VAX_F_ZERO &vax_c[4]
#define VAX_D_ZERO &vax_c[4]
#define VAX_G_ZERO &vax_c[4]
#define VAX_H_ZERO &vax_c[4]

#define VAX_F_POS_HUGE &vax_c[8]
#define VAX_D_POS_HUGE &vax_c[8]
#define VAX_G_POS_HUGE &vax_c[8]
#define VAX_H_POS_HUGE &vax_c[8]

#define VAX_F_NEG_HUGE &vax_c[12]
#define VAX_D_NEG_HUGE &vax_c[12]
#define VAX_G_NEG_HUGE &vax_c[12]
#define VAX_H_NEG_HUGE &vax_c[12]

#define IEEE_S_INVALID ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[1] : &ieee_s[0])
#define IEEE_S_POS_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[3] : &ieee_s[2])
#define IEEE_S_NEG_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[5] : &ieee_s[4])
#define IEEE_S_POS_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[7] : &ieee_s[6])
#define IEEE_S_NEG_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[9] : &ieee_s[8])
#define IEEE_S_POS_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[11] : &ieee_s[10])
#define IEEE_S_NEG_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_s[13] : &ieee_s[12])

#define IEEE_T_INVALID ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[2] : &ieee_t[0])
#define IEEE_T_POS_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[6] : &ieee_t[4])
#define IEEE_T_NEG_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[10] : &ieee_t[8])
#define IEEE_T_POS_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[14] : &ieee_t[12])
#define IEEE_T_NEG_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[18] : &ieee_t[16])
#define IEEE_T_POS_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[22] : &ieee_t[20])
#define IEEE_T_NEG_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_t[26] : &ieee_t[24])

#define IEEE_X_INVALID ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[4] : &ieee_x[0])
#define IEEE_X_POS_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[12] : &ieee_x[8])
#define IEEE_X_NEG_ZERO ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[20] : &ieee_x[16])
#define IEEE_X_POS_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[28] : &ieee_x[24])
#define IEEE_X_NEG_HUGE ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[36] : &ieee_x[32])
#define IEEE_X_POS_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[44] : &ieee_x[40])
#define IEEE_X_NEG_INFINITY ((options & CVT_M_BIG_ENDIAN) ? \
        &ieee_x[52] : &ieee_x[48])

#define IBM_S_INVALID   &ibm_s[0]
#define IBM_S_POS_ZERO  &ibm_s[1]
#define IBM_S_NEG_ZERO  &ibm_s[2]
#define IBM_S_POS_HUGE  &ibm_s[3]
#define IBM_S_NEG_HUGE  &ibm_s[4]
#define IBM_S_POS_INFINITY  &ibm_s[5]
#define IBM_S_NEG_INFINITY  &ibm_s[6]

#define IBM_L_INVALID   &ibm_l[0]
#define IBM_L_POS_ZERO  &ibm_l[2]
#define IBM_L_NEG_ZERO  &ibm_l[4]
#define IBM_L_POS_HUGE  &ibm_l[6]
#define IBM_L_NEG_HUGE  &ibm_l[8]
#define IBM_L_POS_INFINITY  &ibm_l[10]
#define IBM_L_NEG_INFINITY  &ibm_l[12]

#define CRAY_INVALID    &cray[0]
#define CRAY_POS_ZERO   &cray[2]
#define CRAY_NEG_ZERO   &cray[4]
#define CRAY_POS_HUGE   &cray[6]
#define CRAY_NEG_HUGE   &cray[8]
#define CRAY_POS_INFINITY  &cray[10]
#define CRAY_NEG_INFINITY  &cray[12]

/*
** Constant definitions
**-----------------------------------------------------------------------------
*/
#define False                   0
#define True                    1

/*
** Function Prototypes
**-----------------------------------------------------------------------------
*/
STATIC_ROUTINE CVT_STATUS pack_vax_f(UNPACKED_REAL intermediate_value,
				     CVT_VAX_F output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_vax_d(UNPACKED_REAL intermediate_value,
				     CVT_VAX_D output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_vax_g(UNPACKED_REAL intermediate_value,
				     CVT_VAX_G output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_vax_h(UNPACKED_REAL intermediate_value,
				     CVT_VAX_H output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_ieee_s(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_S output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_ieee_t(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_T output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_ieee_x(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_X output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_ibm_l(UNPACKED_REAL intermediate_value,
				     CVT_IBM_LONG output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_ibm_s(UNPACKED_REAL intermediate_value,
				     CVT_IBM_SHORT output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE CVT_STATUS pack_cray(UNPACKED_REAL intermediate_value,
				    CVT_CRAY output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void _round(UNPACKED_REAL intermediate_value,
			   uint32_t round_bit_position, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_vax_f(CVT_VAX_F input_value,
				 UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_vax_d(CVT_VAX_D input_value,
				 UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_vax_g(CVT_VAX_G input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_vax_h(CVT_VAX_H input_value,
				 UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_ieee_s(CVT_IEEE_S input_value,
				  UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_ieee_t(CVT_IEEE_T input_value,
				  UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_ieee_x(CVT_IEEE_X input_value,
				  UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_ibm_l(CVT_IBM_LONG input_value,
				 UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_ibm_s(CVT_IBM_SHORT input_value,
				 UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

STATIC_ROUTINE void unpack_cray(CVT_CRAY input_value,
				UNPACKED_REAL intermediate_value, uint32_t options __attribute__ ((unused)));

extern EXPORT CVT_STATUS CvtConvertFloat(void *input_value,
			   uint32_t input_type, void *output_value, uint32_t output_type)

/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      CvtConvertFloat is a general purpose routine for converting between
**      any pair of the following data types:
**
**          VAX F_Floating
**          VAX D_Floating
**          VAX G_Floating
**          VAX H_Floating
**          IEEE Single
**          IEEE Double
**          IEEE Extended
**          IBM floating point
**          Cray floating point
**
**      The input value pointed to by *input_value will be interpreted as being
**      of type input_type (types are defined in CVTDEF.H).  CvtConvertFloat
**      will convert *input_value to output_type and store the result in
**      *output_value.  The conversion may be influenced by several options.
**      The options are specified by setting specific bits in the options
**      parameter (these bits are also defined in CVTDEF.H).
**
** INPUT PARAMETERS:
**
**      *input_value    - address of the value to be converted.
**      input_type      - data type of *input_value.
**      output_type     - data type of *output_value.
**      options         - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      *output_value   - address of where to store the converted value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_input_conversion_error
**      cvt_s_invalid_input_type
**      cvt_s_invalid_option
**      cvt_s_invalid_output_type
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_output_conversion_error
**      cvt_s_overflow
**      cvt_s_pos_infinity
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definitions.
   ** ==========================================================================
   */
  uint32_t options = 0;
  CVT_STATUS return_status;
  UNPACKED_REAL intermediate_value;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;
  /*
   ** Validate the options parameter.
   ** ==========================================================================
   */
  if (input_type == output_type) {
    switch (input_type) {
    case VAX_F:
      *(float *)output_value = *(float *)input_value;
      return !IsRoprandF(input_value);
    case VAX_D:
      *(double *)output_value = *(double *)input_value;
      return !IsRoprandD(input_value);
    case VAX_G:
      *(double *)output_value = *(double *)input_value;
      return !IsRoprandG(input_value);
    case IEEE_S:
      *(float *)output_value = *(float *)input_value;
      return !IsRoprandS(input_value);
    case IEEE_T:
      *(double *)output_value = *(double *)input_value;
      return !IsRoprandT(input_value);
    default:
      RAISE(cvt_s_invalid_input_type);
      break;
    }
  }

  switch (options & ~(CVT_M_BIG_ENDIAN | CVT_M_ERR_UNDERFLOW)) {
  case 0:
    switch (output_type) {
    case VAX_F:
    case VAX_D:
    case VAX_G:
    case VAX_H:
      options |= CVT_M_VAX_ROUNDING;
      break;
    case IEEE_S:
    case IEEE_T:
    case IEEE_X:
    case IBM_LONG:
    case IBM_SHORT:
    case CRAY:
      options |= CVT_M_ROUND_TO_NEAREST;
      break;
    default:
      RAISE(cvt_s_invalid_output_type);
      break;
    }
    break;

  case CVT_M_ROUND_TO_NEAREST:
  case CVT_M_TRUNCATE:
  case CVT_M_ROUND_TO_POS:
  case CVT_M_ROUND_TO_NEG:
  case CVT_M_VAX_ROUNDING:
    break;
  default:
    RAISE(cvt_s_invalid_option);
    break;
  }

  /*
   ** ==========================================================================
   **
   ** Unpack the input value based upon the input_type parameter.  Unpacking
   ** will convert the input value to an intermediate floating point type of the
   ** following form:
   **
   **  Unsigned 32 bit integer array:
   **
   **  [0]: excess 2147483648 (2 ^ 31) binary exponent
   **  [1]: mantissa: msb ------>
   **  [2]: -------------------->
   **  [3]: -------------------->
   **  [4]: ----------------> lsb
   **  [5]: 28 unused bits, invalid bit, infinity bit, zero bit, negative bit
   **
   **  All fraction bits are explicit and are normalized
   **  s.t. 0.5 <= fraction < 1.0
   **
   ** ==========================================================================
   */
  switch (input_type) {
  case VAX_F:
    unpack_vax_f(input_value, intermediate_value, options);
    break;

  case VAX_D:
    unpack_vax_d(input_value, intermediate_value, options);
    break;

  case VAX_G:
    unpack_vax_g(input_value, intermediate_value, options);
    break;

  case VAX_H:
    unpack_vax_h(input_value, intermediate_value, options);
    break;

  case IEEE_S:
    unpack_ieee_s(input_value, intermediate_value, options);
    break;

  case IEEE_T:
    unpack_ieee_t(input_value, intermediate_value, options);
    break;

  case IEEE_X:
    unpack_ieee_x(input_value, intermediate_value, options);
    break;

  case IBM_LONG:
    unpack_ibm_l(input_value, intermediate_value, options);
    break;

  case IBM_SHORT:
    unpack_ibm_s(input_value, intermediate_value, options);
    break;

  case CRAY:
    unpack_cray(input_value, intermediate_value, options);
    break;

  default:
    RAISE(cvt_s_invalid_input_type);
    break;
  }

  /*
   ** Pack the output value by converting the intermediate value to the type
   ** specified by the output_type parameter.
   ** ==========================================================================
   */
  switch (output_type) {
  case VAX_F:
    return_status = pack_vax_f(intermediate_value, output_value, options);
    break;

  case VAX_D:
    return_status = pack_vax_d(intermediate_value, output_value, options);
    break;

  case VAX_G:
    return_status = pack_vax_g(intermediate_value, output_value, options);
    break;

  case VAX_H:
    return_status = pack_vax_h(intermediate_value, output_value, options);
    break;

  case IEEE_S:
    return_status = pack_ieee_s(intermediate_value, output_value, options);
    break;

  case IEEE_T:
    return_status = pack_ieee_t(intermediate_value, output_value, options);
    break;

  case IEEE_X:
    return_status = pack_ieee_x(intermediate_value, output_value, options);
    break;

  case IBM_LONG:
    return_status = pack_ibm_l(intermediate_value, output_value, options);
    break;

  case IBM_SHORT:
    return_status = pack_ibm_s(intermediate_value, output_value, options);
    break;

  case CRAY:
    return_status = pack_cray(intermediate_value, output_value, options);
    break;

  default:
    RAISE(cvt_s_invalid_output_type);
    break;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;
}

STATIC_ROUTINE void FlipDouble(int *in)
{
  int tmp = in[0];
  in[0] = in[1];
  in[1] = tmp;
}

STATIC_ROUTINE CVT_STATUS pack_vax_f(UNPACKED_REAL intermediate_value,
				     CVT_VAX_F output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create a VAX f_floating number with those bits.
**
**      A VAX f_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 8 exp bits (bias 128), 7 fraction bits
**          [1]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the VAX F value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_overflow
**      cvt_s_pos_infinity
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO) {
      memcpy(output_value, VAX_F_ZERO, 4);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      memcpy(output_value, VAX_F_INVALID, 4);
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)
	RAISE(cvt_s_neg_infinity);
      else
	RAISE(cvt_s_pos_infinity);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, VAX_F_INVALID, 4);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Round the intermediate value at bit position 24.
   ** ==========================================================================
   */
  else {
    _round(intermediate_value, 24, options);

    /*
     ** Check for underflow.
     ** =======================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 127)) {
      memcpy(output_value, VAX_F_ZERO, 4);
      if (options & CVT_M_ERR_UNDERFLOW)
	RAISE(cvt_s_underflow);
    }

    /*
     ** Check for overflow.
     ** =======================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 127)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, VAX_F_NEG_HUGE, 4);
	} else {
	  memcpy(output_value, VAX_F_POS_HUGE, 4);
	}
      }

      else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_F_NEG_HUGE, 4);
      }

      else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_F_POS_HUGE, 4);
      }

      else {
	memcpy(output_value, VAX_F_INVALID, 4);
      }

      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** =======================================================================
     */
    else {
      /*
       ** Adjust bias of exponent.
       **-----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 128);

      /*
       ** Make room for exponent and sign bit.
       **-----------------------------------------------------------------------
       */
      intermediate_value[1] >>= 8;

      /*
       ** Clear implicit bit.
       **-----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x007FFFFFL;

      /*
       ** OR in exponent and sign bit.
       **-----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 23);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Adjust for VAX 16 bit floating format.
       **-----------------------------------------------------------------------
       */
      intermediate_value[1] = ((intermediate_value[1] << 16) | (intermediate_value[1] >> 16));

      memcpy(output_value, &intermediate_value[1], 4);

    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_vax_d(UNPACKED_REAL intermediate_value,
				     CVT_VAX_D output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create a VAX d_floating number with those bits.
**
**      A VAX d_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 8 exp bits (bias 128), 7 fraction bits
**          [1]: 16 more fraction bits
**          [2]: 16 more fraction bits
**          [3]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the VAX D value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_overflow
**      cvt_s_pos_infinity
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO) {
      memcpy(output_value, VAX_D_ZERO, 8);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      memcpy(output_value, VAX_D_INVALID, 8);
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)
	RAISE(cvt_s_neg_infinity);
      else
	RAISE(cvt_s_pos_infinity);
    }

    else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, VAX_D_INVALID, 8);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Round the intermediate value at bit position 56.
   ** ==========================================================================
   */
  else {
    _round(intermediate_value, 56, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 127)) {
      memcpy(output_value, VAX_D_ZERO, 8);
      if (options & CVT_M_ERR_UNDERFLOW)
	RAISE(cvt_s_underflow);
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 127)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, VAX_D_NEG_HUGE, 8);
	} else {
	  memcpy(output_value, VAX_D_POS_HUGE, 8);
	}
      }

      else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_D_NEG_HUGE, 8);
      }

      else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_D_POS_HUGE, 8);
      }

      else {
	memcpy(output_value, VAX_D_INVALID, 8);
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 128);

      /*
       ** Make room for the exponent and the sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[2] >>= 8;
      intermediate_value[2] |= (intermediate_value[1] << 24);
      intermediate_value[1] >>= 8;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x007FFFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 23);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Adjust for VAX 16 bit floating format.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] = ((intermediate_value[1] << 16) | (intermediate_value[1] >> 16));
      intermediate_value[2] = ((intermediate_value[2] << 16) | (intermediate_value[2] >> 16));

      memcpy(output_value, &intermediate_value[1], 8);
    }
  }
#ifdef WORDS_BIGENDIAN
  {
    CVT_VAX_D temp;
    memcpy(temp, output_value, 8);
    memcpy(output_value, &((char *)temp)[4], 4);
    memcpy(&((char *)output_value)[4], temp, 4);
  }
#endif
  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_vax_g(UNPACKED_REAL intermediate_value,
				     CVT_VAX_G output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create a VAX g_floating number with those bits.
**
**      A VAX g_floating number in (16 bit words) looks like:
**
**        [0]: Sign bit, 11 exp bits (bias 1024), 4 fraction bits
**        [1]: 16 more fraction bits
**        [2]: 16 more fraction bits
**        [3]: 16 more fraction bits
**
**        0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the VAX G value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_overflow
**      cvt_s_pos_infinity
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO) {
      memcpy(output_value, VAX_G_ZERO, 8);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      memcpy(output_value, VAX_G_INVALID, 8);
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)
	RAISE(cvt_s_neg_infinity);
      else
	RAISE(cvt_s_pos_infinity);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, VAX_G_INVALID, 8);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Round the intermediate value at bit position 53.
   ** ==========================================================================
   */
  else {
    _round(intermediate_value, 53, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 1023)) {
      memcpy(output_value, VAX_G_ZERO, 8);
      if (options & CVT_M_ERR_UNDERFLOW)
	RAISE(cvt_s_underflow);
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 1023)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, VAX_G_NEG_HUGE, 8);
	} else {
	  memcpy(output_value, VAX_G_POS_HUGE, 8);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_G_NEG_HUGE, 8);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_G_POS_HUGE, 8);
      } else {
	memcpy(output_value, VAX_G_INVALID, 8);
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 1024);

      /*
       ** Make room for the exponent and the sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[2] >>= 11;
      intermediate_value[2] |= (intermediate_value[1] << 21);
      intermediate_value[1] >>= 11;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x000FFFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 20);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Adjust for VAX 16 bit floating format.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] = ((intermediate_value[1] << 16) | (intermediate_value[1] >> 16));
      intermediate_value[2] = ((intermediate_value[2] << 16) | (intermediate_value[2] >> 16));

      memcpy(output_value, &intermediate_value[1], 8);
    }
  }

#ifdef WORDS_BIGENDIAN
  {
    CVT_VAX_G temp;
    memcpy(temp, output_value, 8);
    memcpy(output_value, &((char *)temp)[4], 4);
    memcpy(&((char *)output_value)[4], temp, 4);
  }
#endif
  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_vax_h(UNPACKED_REAL intermediate_value,
				     CVT_VAX_H output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create a VAX h_floating number with those bits.
**
**      A VAX h_floating number in (16 bit words) looks like:
**
**        [0]: Sign bit, 15 exp bits (bias 16384)
**        [1]: 16 fraction bits
**        [2]: 16 more fraction bits
**        [3]: 16 more fraction bits
**        [4]: 16 more fraction bits
**        [5]: 16 more fraction bits
**        [6]: 16 more fraction bits
**        [7]: 16 more fraction bits
**
**        0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the VAX H value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_overflow
**      cvt_s_pos_infinity
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO) {
      memcpy(output_value, VAX_H_ZERO, 16);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      memcpy(output_value, VAX_H_INVALID, 16);
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)
	RAISE(cvt_s_neg_infinity);
      else
	RAISE(cvt_s_pos_infinity);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, VAX_H_INVALID, 16);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Round the intermediate value at bit position 113.
   ** ==========================================================================
   */
  else {
    _round(intermediate_value, 113, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 16383)) {
      memcpy(output_value, VAX_H_ZERO, 16);
      if (options & CVT_M_ERR_UNDERFLOW)
	RAISE(cvt_s_underflow);
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 16383)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, VAX_H_NEG_HUGE, 16);
	} else {
	  memcpy(output_value, VAX_H_POS_HUGE, 16);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_H_NEG_HUGE, 16);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, VAX_H_POS_HUGE, 16);
      } else {
	memcpy(output_value, VAX_H_INVALID, 16);
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 16384);

      /*
       ** Make room for exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[4] >>= 15;
      intermediate_value[4] |= (intermediate_value[3] << 17);
      intermediate_value[3] >>= 15;
      intermediate_value[3] |= (intermediate_value[2] << 17);
      intermediate_value[2] >>= 15;
      intermediate_value[2] |= (intermediate_value[1] << 17);
      intermediate_value[1] >>= 15;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x0000FFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 16);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Adjust for VAX 16 bit floating format.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] = ((intermediate_value[1] << 16) | (intermediate_value[1] >> 16));
      intermediate_value[2] = ((intermediate_value[2] << 16) | (intermediate_value[2] >> 16));
      intermediate_value[3] = ((intermediate_value[3] << 16) | (intermediate_value[3] >> 16));
      intermediate_value[4] = ((intermediate_value[4] << 16) | (intermediate_value[4] >> 16));

      memcpy(output_value, &intermediate_value[1], 16);
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_ieee_s(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_S output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create an IEEE single number with those bits.
**
**      A normalized IEEE single precision floating number looks like:
**
**        Sign bit, 8 exp bits (bias 127), 23 fraction bits
**
**        1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see "Mips R2000 Risc Architecture" by Gerry Kane,
**      page 6-8 or ANSI/IEEE Std 754-1985.
**
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the IEEE S value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  int i;
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO)
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_S_NEG_ZERO, 4);
      } else {
	memcpy(output_value, IEEE_S_POS_ZERO, 4);
      }

    else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_S_NEG_INFINITY, 4);
      } else {
	memcpy(output_value, IEEE_S_POS_INFINITY, 4);
      }
    }

    else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, IEEE_S_INVALID, 4);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Precision varies if value will be a denorm.  So, figure out where to
   ** round (0 <= i <= 24).
   ** ==========================================================================
   */
  else {
    round_bit_position = intermediate_value[U_R_EXP] - ((U_R_BIAS - 126) - 23);
    if (round_bit_position < 0)
      round_bit_position = 0;
    else if (round_bit_position > 24)
      round_bit_position = 24;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for Denorm or underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 125)) {
      /*
       ** Value is too small for a denorm, so underflow.
       ** ----------------------------------------------------------------------
       */
      if (intermediate_value[U_R_EXP] < ((U_R_BIAS - 125) - 23)) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_S_NEG_ZERO, 4);
	} else {
	  memcpy(output_value, IEEE_S_POS_ZERO, 4);
	}

	if (options & CVT_M_ERR_UNDERFLOW)
	  RAISE(cvt_s_underflow);
      }

      /*
       ** Figure leading zeros for denorm and right-justify fraction
       ** ----------------------------------------------------------------------
       */
      else {
	i = 32 - (intermediate_value[U_R_EXP] - ((U_R_BIAS - 126) - 23));
	intermediate_value[1] >>= i;

	/*
	 ** Set sign bit.
	 ** --------------------------------------------------------------------
	 */
	intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

	if (options & CVT_M_BIG_ENDIAN) {
	  intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	  intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	  intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);
	  memcpy(output_value, intermediate_value, 4);
	} else {
	  memcpy(output_value, &intermediate_value[1], 4);
	}
      }
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 128)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_S_NEG_HUGE, 4);
	} else {
	  memcpy(output_value, IEEE_S_POS_HUGE, 4);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_S_NEG_HUGE, 4);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_S_POS_HUGE, 4);
      } else {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_S_NEG_INFINITY, 4);
	} else {
	  memcpy(output_value, IEEE_S_POS_INFINITY, 4);
	}
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 126);

      /*
       ** Make room for exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] >>= 8;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x007FFFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 23);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      if (options & CVT_M_BIG_ENDIAN) {
	intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);
	memcpy(output_value, intermediate_value, 4);
      } else {
	memcpy(output_value, &intermediate_value[1], 4);
      }
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_ieee_t(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_T output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create an IEEE double floating number with those bits.
**
**      A normalized IEEE double precision floating number looks like:
**
**          [0]: 32 low order fraction bits
**          [1]: Sign bit, 11 exp bits (bias 1023), 20 fraction bits
**
**          1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see "Mips R2000 Risc Architecture" by Gerry Kane,
**      page 6-8 or ANSI/IEEE Std 754-1985.
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the IEEE T value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  int i;
  CVT_STATUS return_status;
  STATIC_CONSTANT double fliptest = 42.;
  int flip = *(int *)&fliptest != 0;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO)
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_T_NEG_ZERO, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      } else {
	memcpy(output_value, IEEE_T_POS_ZERO, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      }

    else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_T_NEG_INFINITY, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      } else {
	memcpy(output_value, IEEE_T_POS_INFINITY, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      }
    }

    else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, IEEE_T_INVALID, 8);
      if (flip)
	FlipDouble((int *)output_value);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Precision varies if value will be a denorm.  So, figure out where to
   ** round (0 <= i <= 53).
   ** ==========================================================================
   */
  else {
    round_bit_position = intermediate_value[U_R_EXP] - ((U_R_BIAS - 1022) - 52);
    if (round_bit_position < 0)
      round_bit_position = 0;
    else if (round_bit_position > 53)
      round_bit_position = 53;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for Denorm or underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 1021)) {
      /*
       ** Value is too small for a denorm, so underflow.
       ** ----------------------------------------------------------------------
       */
      if (intermediate_value[U_R_EXP] < ((U_R_BIAS - 1021) - 52)) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_T_NEG_ZERO, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	} else {
	  memcpy(output_value, IEEE_T_POS_ZERO, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	}

	if (options & CVT_M_ERR_UNDERFLOW)
	  RAISE(cvt_s_underflow);
      }

      /*
       ** Figure leading zeros for denorm and right-justify fraction
       ** ----------------------------------------------------------------------
       */
      else {
	i = 64 - (intermediate_value[U_R_EXP] - ((U_R_BIAS - 1022) - 52));
	if (i > 31) {
	  i -= 32;
	  intermediate_value[2] = (intermediate_value[1] >> i);
	  intermediate_value[1] = 0;
	} else {
	  intermediate_value[2] >>= i;
	  intermediate_value[2] |= (intermediate_value[1] << (32 - i));
	  intermediate_value[1] >>= i;
	}

	/*
	 ** OR in sign bit.
	 ** --------------------------------------------------------------------
	 */
	intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);
	if (options & CVT_M_BIG_ENDIAN) {
	  intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	  intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	  intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);
	  intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
	  intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
	  intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);
	} else {
	  intermediate_value[0] = intermediate_value[2];
	}
	memcpy(output_value, intermediate_value, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      }
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 1024)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_T_NEG_HUGE, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	} else {
	  memcpy(output_value, IEEE_T_POS_HUGE, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	}
      }

      else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_T_NEG_HUGE, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      }

      else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_T_POS_HUGE, 8);
	if (flip)
	  FlipDouble((int *)output_value);
      }

      else {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_T_NEG_INFINITY, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	} else {
	  memcpy(output_value, IEEE_T_POS_INFINITY, 8);
	  if (flip)
	    FlipDouble((int *)output_value);
	}
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 1022);

      /*
       ** Make room for exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[2] >>= 11;
      intermediate_value[2] |= (intermediate_value[1] << 21);
      intermediate_value[1] >>= 11;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x000FFFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 20);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      if (options & CVT_M_BIG_ENDIAN) {
	intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);
	intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
	intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
	intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);
      } else {
	intermediate_value[0] = intermediate_value[2];
      }
      memcpy(output_value, &intermediate_value[0], 8);
      if (flip)
	FlipDouble((int *)output_value);
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_ieee_x(UNPACKED_REAL intermediate_value,
				      CVT_IEEE_X output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create an IEEE extended precision floating number with
**      those bits.
**
**      A normalized IEEE extended precision floating number looks like:
**
**          [0]: 32 low order fraction bits
**          [1]: 32 more fraction bits
**          [2]: 32 more fraction bits
**          [3]: Sign bit, 15 exp bits (bias 16383), 16 fraction bits
**
**          1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see ANSI/IEEE Std 754-1985.
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the IEEE X value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  int i;
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO)
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_X_NEG_ZERO, 16);
      } else {
	memcpy(output_value, IEEE_X_POS_ZERO, 16);
      }

    else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IEEE_X_NEG_INFINITY, 16);
      } else {
	memcpy(output_value, IEEE_X_POS_INFINITY, 16);
      }
    }

    else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, IEEE_X_INVALID, 16);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Precision varies if value will be a denorm.  So, figure out where to
   ** round (0 <= i <= 113).
   ** ==========================================================================
   */
  else {
    round_bit_position = intermediate_value[U_R_EXP] - ((U_R_BIAS - 16382) - 112);
    if (round_bit_position < 0)
      round_bit_position = 0;
    else if (round_bit_position > 113)
      round_bit_position = 113;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for Denorm or underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 16381)) {
      /*
       ** Value is too small for a denorm, so underflow.
       ** ----------------------------------------------------------------------
       */
      if (intermediate_value[U_R_EXP] < ((U_R_BIAS - 16381) - 112)) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_X_NEG_ZERO, 16);
	} else {
	  memcpy(output_value, IEEE_X_POS_ZERO, 16);
	}

	if (options & CVT_M_ERR_UNDERFLOW)
	  RAISE(cvt_s_underflow);
      }

      /*
       ** Figure leading zeros for denorm and right-justify fraction
       ** ----------------------------------------------------------------------
       */
      else {
	i = 128 - (intermediate_value[U_R_EXP] - ((U_R_BIAS - 16382) - 112));
	if (i > 95) {
	  i -= 96;
	  intermediate_value[4] = (intermediate_value[1] >> i);
	  intermediate_value[3] = 0;
	  intermediate_value[2] = 0;
	  intermediate_value[1] = 0;
	} else if (i > 63) {
	  i -= 64;
	  intermediate_value[4] = (intermediate_value[2] >> i);
	  intermediate_value[4] |= (intermediate_value[1] << (32 - i));
	  intermediate_value[3] = (intermediate_value[1] >> i);
	  intermediate_value[2] = 0;
	  intermediate_value[1] = 0;
	} else if (i > 31) {
	  i -= 32;
	  intermediate_value[4] = (intermediate_value[3] >> i);
	  intermediate_value[4] |= (intermediate_value[2] << (32 - i));
	  intermediate_value[3] = (intermediate_value[2] >> i);
	  intermediate_value[3] |= (intermediate_value[1] << (32 - i));
	  intermediate_value[2] = (intermediate_value[1] >> i);
	  intermediate_value[1] = 0;
	} else {
	  intermediate_value[4] >>= i;
	  intermediate_value[4] |= (intermediate_value[3] << (32 - i));
	  intermediate_value[3] >>= i;
	  intermediate_value[3] |= (intermediate_value[2] << (32 - i));
	  intermediate_value[2] >>= i;
	  intermediate_value[2] |= (intermediate_value[1] << (32 - i));
	  intermediate_value[1] >>= i;
	}

	/*
	 ** OR in sign bit.
	 ** --------------------------------------------------------------------
	 */
	intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);
	if (options & CVT_M_BIG_ENDIAN) {
	  intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	  intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	  intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);

	  intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
	  intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
	  intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);

	  intermediate_value[2] = ((intermediate_value[3] << 24) | (intermediate_value[3] >> 24));
	  intermediate_value[2] |= ((intermediate_value[3] << 8) & 0x00FF0000L);
	  intermediate_value[2] |= ((intermediate_value[3] >> 8) & 0x0000FF00L);

	  intermediate_value[3] = ((intermediate_value[4] << 24) | (intermediate_value[4] >> 24));
	  intermediate_value[3] |= ((intermediate_value[4] << 8) & 0x00FF0000L);
	  intermediate_value[3] |= ((intermediate_value[4] >> 8) & 0x0000FF00L);
	} else {
	  intermediate_value[0] = intermediate_value[3];
	  intermediate_value[3] = intermediate_value[1];
	  intermediate_value[1] = intermediate_value[0];
	  intermediate_value[0] = intermediate_value[4];
	}
	memcpy(output_value, intermediate_value, 16);
      }
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 16384)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_X_NEG_HUGE, 16);
	} else {
	  memcpy(output_value, IEEE_X_POS_HUGE, 16);
	}
      }

      else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_X_NEG_HUGE, 16);
      }

      else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IEEE_X_POS_HUGE, 16);
      }

      else {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IEEE_X_NEG_INFINITY, 16);
	} else {
	  memcpy(output_value, IEEE_X_POS_INFINITY, 16);
	}
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust the bias of the exponent.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 16382);

      /*
       ** Make room for exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[4] >>= 15;
      intermediate_value[4] |= (intermediate_value[3] << 17);
      intermediate_value[3] >>= 15;
      intermediate_value[3] |= (intermediate_value[2] << 17);
      intermediate_value[2] >>= 15;
      intermediate_value[2] |= (intermediate_value[1] << 17);
      intermediate_value[1] >>= 15;

      /*
       ** Clear implicit bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] &= 0x0000FFFFL;

      /*
       ** OR in exponent and sign bit.
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 16);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      if (options & CVT_M_BIG_ENDIAN) {
	intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
	intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
	intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);

	intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
	intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
	intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);

	intermediate_value[2] = ((intermediate_value[3] << 24) | (intermediate_value[3] >> 24));
	intermediate_value[2] |= ((intermediate_value[3] << 8) & 0x00FF0000L);
	intermediate_value[2] |= ((intermediate_value[3] >> 8) & 0x0000FF00L);

	intermediate_value[3] = ((intermediate_value[4] << 24) | (intermediate_value[4] >> 24));
	intermediate_value[3] |= ((intermediate_value[4] << 8) & 0x00FF0000L);
	intermediate_value[3] |= ((intermediate_value[4] >> 8) & 0x0000FF00L);
      } else {
	intermediate_value[0] = intermediate_value[3];
	intermediate_value[3] = intermediate_value[1];
	intermediate_value[1] = intermediate_value[0];
	intermediate_value[0] = intermediate_value[4];
      }
      memcpy(output_value, &intermediate_value[0], 16);
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_ibm_l(UNPACKED_REAL intermediate_value,
				     CVT_IBM_LONG output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create an IBM long floating number with those bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized IBM long floating number looks like:
**
**          [0]: Sign bit, 7 exp bits (bias 64), 24 fraction bits
**          [1]: 32 low order fraction bits
**
**          0.0625 <= fraction < 1.0, from 0 to 3 leading zeros to compensate
**          for the hexadecimal exponent.
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the IBM Long value.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  int i, j;
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO)
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_L_NEG_ZERO, 8);
      } else {
	memcpy(output_value, IBM_L_POS_ZERO, 8);
      }

    else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_L_NEG_INFINITY, 8);
      } else {
	memcpy(output_value, IBM_L_POS_INFINITY, 8);
      }
    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, IBM_L_INVALID, 8);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Precision varies because binary exp must be multiple of 4 (since we must
   ** convert it to a hexadecimal exponent).  So, figure out where to round
   ** (53 <= i <= 56).
   ** ==========================================================================
   */
  else {
    i = (intermediate_value[U_R_EXP] & 0x00000003L);
    if (i)
      round_bit_position = i + 52;
    else
      round_bit_position = 56;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 255)) {
      /*
       ** Value is too small, so underflow.
       ** ----------------------------------------------------------------------
       */
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_L_NEG_ZERO, 8);
      } else {
	memcpy(output_value, IBM_L_POS_ZERO, 8);
      }

      if (options & CVT_M_ERR_UNDERFLOW)
	RAISE(cvt_s_underflow);
    }
    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 252)) {
      /*
       ** Value is too large, so overflow.
       ** ----------------------------------------------------------------------
       */
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IBM_L_NEG_HUGE, 8);
	} else {
	  memcpy(output_value, IBM_L_POS_HUGE, 8);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IBM_L_NEG_HUGE, 8);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IBM_L_POS_HUGE, 8);
      } else {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IBM_L_NEG_INFINITY, 8);
	} else {
	  memcpy(output_value, IBM_L_POS_INFINITY, 8);
	}
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Figure leading zeros (i) and biased exponent (j)
       ** ----------------------------------------------------------------------
       */
      i = (intermediate_value[U_R_EXP] & 0x00000003L);
      j = ((int)(intermediate_value[U_R_EXP] - U_R_BIAS) / 4) + 64;

      if (i) {
	if (intermediate_value[U_R_EXP] > U_R_BIAS)
	  j += 1;
	i = 12 - i;
      } else {
	i = 8;
      }

      /*
       ** Make room for exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[2] >>= i;
      intermediate_value[2] |= (intermediate_value[1] << (32 - i));
      intermediate_value[1] >>= i;

      /*
       ** OR in exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (j << 24);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Shuffle bytes to big endian format
       ** ----------------------------------------------------------------------
       */
      intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
      intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
      intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);
      intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
      intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
      intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);

      memcpy(output_value, &intermediate_value[0], 8);
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_ibm_s(UNPACKED_REAL intermediate_value,
				     CVT_IBM_SHORT output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create an IBM short floating number with those bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized IBM short floating number looks like:
**
**          Sign bit, 7 exp bits (bias 64), 24 fraction bits
**
**          0.0625 <= fraction < 1.0, from 0 to 3 leading zeros
**          to compensate for the hexadecimal exponent.
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the IBM Short value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  int i, j;
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO)
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_S_NEG_ZERO, 4);
      } else {
	memcpy(output_value, IBM_S_POS_ZERO, 4);
    } else if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_S_NEG_INFINITY, 4);
      } else {
	memcpy(output_value, IBM_S_POS_INFINITY, 4);
      }

    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, IBM_S_INVALID, 4);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Precision varies because exp must be multiple of 4 (since we must convert
   ** it to a hexadecimal exponent).  So, figure out where to round
   ** (21 <= i <=24).
   ** ==========================================================================
   */
  else {
    i = (intermediate_value[U_R_EXP] & 0x00000003L);
    if (i)
      round_bit_position = i + 20;
    else
      round_bit_position = 24;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 255)) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, IBM_S_NEG_ZERO, 4);
      } else {
	memcpy(output_value, IBM_S_POS_ZERO, 4);
      }

      if (options & CVT_M_ERR_UNDERFLOW) {
	RAISE(cvt_s_underflow);
      }
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 252)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IBM_S_NEG_HUGE, 4);
	} else {
	  memcpy(output_value, IBM_S_POS_HUGE, 4);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IBM_S_NEG_HUGE, 4);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, IBM_S_POS_HUGE, 4);
      } else {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, IBM_S_NEG_INFINITY, 4);
	} else {
	  memcpy(output_value, IBM_S_POS_INFINITY, 4);
	}
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Figure leading zeros (i) and biased exponent (j)
       ** ----------------------------------------------------------------------
       */
      i = (intermediate_value[U_R_EXP] & 0x00000003L);
      j = ((int)(intermediate_value[U_R_EXP] - U_R_BIAS) / 4) + 64;

      if (i) {
	if (intermediate_value[U_R_EXP] > U_R_BIAS)
	  j += 1;
	i = 12 - i;
      } else {
	i = 8;
      }

      /*
       ** Make room for exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] >>= i;

      /*
       ** OR in exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (j << 24);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Shuffle bytes to big endian format
       ** ----------------------------------------------------------------------
       */
      intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
      intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
      intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);

      memcpy(output_value, &intermediate_value[0], 4);

    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE CVT_STATUS pack_cray(UNPACKED_REAL intermediate_value,
				    CVT_CRAY output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an UNPACKED_REAL
**      structure and to create a CRAY floating number with those bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized CRAY floating number looks like:
**
**          [0]: Sign bit, 15 exp bits (bias 16384), 16 fraction bits
**          [1]: 32 low order fraction bits
**
**          0.5 <= fraction < 1.0, MSB explicit
**          Since CRAY has no hidden bits the MSB must always be set.
**
**      Some of the CRAY exponent range is not used.
**      Exponents < 0x2000 and >= 0x6000 are invalid.
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to store the Cray floating
**                                point value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      cvt_s_normal
**      cvt_s_invalid_value
**      cvt_s_neg_infinity
**      cvt_s_pos_infinity
**      cvt_s_overflow
**      cvt_s_underflow
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int round_bit_position;
  CVT_STATUS return_status;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  return_status = cvt_s_normal;

  /*
   ** Check for unusual situations in the intermediate value.
   ** ie. zero, infinity or invalid numbers.
   ** ==========================================================================
   */
  if (intermediate_value[U_R_FLAGS] & U_R_UNUSUAL) {
    if (intermediate_value[U_R_FLAGS] & U_R_ZERO) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, CRAY_NEG_ZERO, 8);
      } else {
	memcpy(output_value, CRAY_POS_ZERO, 8);
      }
    }
    if (intermediate_value[U_R_FLAGS] & U_R_INFINITY) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, CRAY_NEG_INFINITY, 8);
	RAISE(cvt_s_neg_infinity);
      } else {
	memcpy(output_value, CRAY_POS_INFINITY, 8);
	RAISE(cvt_s_pos_infinity);
      }
    } else if (intermediate_value[U_R_FLAGS] & U_R_INVALID) {
      memcpy(output_value, CRAY_INVALID, 8);
      RAISE(cvt_s_invalid_value);
    }
  }

  /*
   ** Round at position 48.
   ** ==========================================================================
   */
  else {
    round_bit_position = 48;

    _round(intermediate_value, round_bit_position, options);

    /*
     ** Check for underflow.
     ** ========================================================================
     */
    if (intermediate_value[U_R_EXP] < (U_R_BIAS - 8192)) {
      if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	memcpy(output_value, CRAY_NEG_ZERO, 8);
      } else {
	memcpy(output_value, CRAY_POS_ZERO, 8);
      }

      if (options & CVT_M_ERR_UNDERFLOW) {
	RAISE(cvt_s_underflow);
      }
    }

    /*
     ** Check for overflow.
     ** ========================================================================
     */
    else if (intermediate_value[U_R_EXP] > (U_R_BIAS + 8191)) {
      if (options & CVT_M_TRUNCATE) {
	if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE) {
	  memcpy(output_value, CRAY_NEG_HUGE, 8);
	} else {
	  memcpy(output_value, CRAY_POS_HUGE, 8);
	}
      } else if ((options & CVT_M_ROUND_TO_POS) && (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, CRAY_NEG_HUGE, 8);
      } else if ((options & CVT_M_ROUND_TO_NEG) && !(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)) {
	memcpy(output_value, CRAY_POS_HUGE, 8);
      } else {
	memcpy(output_value, CRAY_INVALID, 8);
      }
      RAISE(cvt_s_overflow);
    }

    /*
     ** Pack up the output value and return it.
     ** ========================================================================
     */
    else {
      /*
       ** Adjust bias of exponent
       ** ----------------------------------------------------------------------
       */
      intermediate_value[U_R_EXP] -= (U_R_BIAS - 16384);

      /*
       ** Make room for exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[2] >>= 16;
      intermediate_value[2] |= (intermediate_value[1] << 16);
      intermediate_value[1] >>= 16;

      /*
       ** OR in exponent and sign bit
       ** ----------------------------------------------------------------------
       */
      intermediate_value[1] |= (intermediate_value[U_R_EXP] << 16);
      intermediate_value[1] |= (intermediate_value[U_R_FLAGS] << 31);

      /*
       ** Shuffle bytes to big endian format
       ** ----------------------------------------------------------------------
       */
      intermediate_value[0] = ((intermediate_value[1] << 24) | (intermediate_value[1] >> 24));
      intermediate_value[0] |= ((intermediate_value[1] << 8) & 0x00FF0000L);
      intermediate_value[0] |= ((intermediate_value[1] >> 8) & 0x0000FF00L);

      intermediate_value[1] = ((intermediate_value[2] << 24) | (intermediate_value[2] >> 24));
      intermediate_value[1] |= ((intermediate_value[2] << 8) & 0x00FF0000L);
      intermediate_value[1] |= ((intermediate_value[2] >> 8) & 0x0000FF00L);

      memcpy(output_value, &intermediate_value[0], 8);
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return return_status;

}

STATIC_ROUTINE void _round(UNPACKED_REAL intermediate_value,
			   uint32_t round_bit_position, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module rounds CVT floating point (UNPACKED_REAL) data to any
**      specified position.  Any of the following rounding modes can be
**      applied:
**
**      Note: None of the following implementations ever perform true truncation
**            on their values.  Whenever truncation becomes necessary - either
**            by being specified directly or by being required indirectly
**            through rounding - values are actually left untouched.  Users of
**            this routine must zero out fractional fields themselves if true
**            truncation is needed.
**
**         VAX ROUNDING
**
**              Input data are rounded such that the representable value
**              nearest the infinitely precise result is delivered; if two
**              representable values are equally near, the one greatest in
**              magnitude is delivered.
**
**         ROUND TO NEAREST
**
**              Input data are rounded such that the representable value
**              nearest the infinitely precise result is delivered; if two
**              representable values are equally near, the one with its least
**              significant bit zero is delivered.
**
**         ROUND TO POSITIVE INFINITY
**
**              Input data are rounded such that the representable value
**              closest to and no less than the infinitely precise result is
**              delivered.
**
**         ROUND TO NEGATIVE INFINITY
**
**              Input data are rounded such that the representable value
**              closest to and no greater than the infinitely precise result is
**              delivered.
**
**         TRUNCATION (ROUND TOWARDS ZERO)
**
**              True truncation is not implemented here.  Input values are
**              delivered in their original, untouched form.
**
**              A definition of "true" truncation follows:  Truncation, or
**              rounding towards zero, implies input data are rounded such
**              that the representable value closest to and no greater in
**              magnitude than the infinitely precise result is delivered.
**
**      Note: for efficiency this routine performs no explicit error checking.
**
**
** INPUT PARAMETERS:
**
**      intermediate_value      - address of the UNPACKED_REAL to be rounded.
**
**      round_bit_position      - An integer specifying the position to round
**                                to.  0 <= round_bit_position <= 127.
**
**                                Note: Valid CVT mantissa bits are addressed
**                                as 1 through 128.  Accordingly, specifying 0
**                                as a position to round to implies an exponent
**                                increase whenever rounding occurs.  As for
**                                truncation: truncation always leaves a CVT
**                                number untouched.
**
**      options                 - A valid CVT options bit mask in which at
**                                least one, and only one, CVT rounding mode is
**                                specified.  If no rounding mode is specified,
**                                results are unpredictable.  Rounding is
**                                performed in accordance with this mask.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      intermediate_value      - a rounded representation of the input.
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  uint32_t bit_mask;
  int i;
  int more_bits;
  int roundup;

  /*
   ** If we are not going to TRUNCATE the number.
   ** ==========================================================================
   */
  if (!(options & CVT_M_TRUNCATE)) {

    /*
     ** Determine which word the round bit resides in.
     ** ========================================================================
     */
    i = (round_bit_position >> 5) + 1;

    /*
     ** Create a mask isolating the round bit.
     ** ========================================================================
     */
    bit_mask = 0x1L << (31 - (round_bit_position & 0x1FL));

    /*
     ** If we are doing VAX ROUNDING.
     ** ========================================================================
     */
    if (options & CVT_M_VAX_ROUNDING)
      roundup = intermediate_value[i] & bit_mask;

    /*
     ** Else handle the other rounding options.
     ** ========================================================================
     */
    else {
      roundup = 0;
      switch (intermediate_value[i] & bit_mask) {

	/*
	 ** If round bit is clear, and ROUND TO NEAREST option is selected we
	 ** truncate.
	 ** --------------------------------------------------------------------
	 */
      case 0:
	if (options & CVT_M_ROUND_TO_NEAREST)
	  break;
	MDS_ATTR_FALLTHROUGH

	/*
	 ** Otherwise, make note of wheather there are any bits set after the
	 ** round bit, and then check the remaining cases.
	 ** --------------------------------------------------------------------
	 */
      default:
	if (!(more_bits = intermediate_value[i] & (bit_mask - 1)))
	  switch (i) {
	  case 1:
	    more_bits = intermediate_value[2];
	    MDS_ATTR_FALLTHROUGH
	  case 2:
	    more_bits |= intermediate_value[3];
	    MDS_ATTR_FALLTHROUGH
	  case 3:
	    more_bits |= intermediate_value[4];
	    break;
	  default:
	    break;
	  }

	/*
	 ** Re-check ROUND TO NEAREST option.  NOTE: if we've reached
	 ** this point and ROUND TO NEAREST has been selected, the
	 ** round bit is set.
	 ** ----------------------------------------------------------
	 */
	if (options & CVT_M_ROUND_TO_NEAREST) {
	  if (!(roundup = more_bits)) {
	    if (bit_mask << 1) {
	      roundup = intermediate_value[i] & (bit_mask << 1);
	    } else {
	      if (i != 1)
		roundup = intermediate_value[i - 1] & 1;
	    }
	  }
	}

	/*
	 ** Check ROUND TO POSITIVE INFINITY option.
	 ** ----------------------------------------------------------
	 */
	else if (options & CVT_M_ROUND_TO_POS) {
	  if (!(intermediate_value[U_R_FLAGS] & U_R_NEGATIVE))
	    roundup = (intermediate_value[i] & bit_mask) | more_bits;
	}

	/*
	 ** Check ROUND TO NEGATIVE INFINITY option.
	 ** ----------------------------------------------------------
	 */
	else if (intermediate_value[U_R_FLAGS] & U_R_NEGATIVE)
	  roundup = (intermediate_value[i] & bit_mask) | more_bits;
      }
    }

    /*
     ** Perform rounding if necessary.
     ** ========================================================================
     */
    if (roundup) {

      /*
       ** Add 1 at round position.
       ** ----------------------------------------------------------------------
       */
      bit_mask <<= 1;
      intermediate_value[i] = (intermediate_value[i] & ~(bit_mask - 1)) + bit_mask;

      /*
       ** Propagate any carry.
       ** ----------------------------------------------------------------------
       */
      while (!intermediate_value[i])
	intermediate_value[--i] += 1;

      /*
       ** If carry reaches exponent MSB gets zeroed and must be reset.
       ** ----------------------------------------------------------------------
       */
      if (i == 0)
	intermediate_value[1] = 0x80000000L;
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_vax_f(CVT_VAX_F input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a VAX f_floating
**      number and to initialize an UNPACKED_REAL structure with those bits.
**
**      A VAX f_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 8 exp bits (bias 128), 7 fraction bits
**          [1]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      input_value             - address of the VAX F value to be converted.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(&output_value[1], input_value, 4);

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 15) & U_R_NEGATIVE;

  /*
   ** Extract VAX biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 7) & 0x000000FFL;

  /*
   ** If the exponent is zero, determine if we have an invalid number or
   ** a zero value.  Set the flag bits accordingly.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    if (output_value[U_R_FLAGS])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] = U_R_ZERO;
  }

  /*
   ** Else the exponent is not zero and we will always have a valid number.
   ** ==========================================================================
   */
  else {
    output_value[1] = ((output_value[1] << 16) | (output_value[1] >> 16));

    /*
     ** Adjust for VAX 16 bit floating format.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 128);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00800000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 8;

    /*
     ** Clear uninitialized parts for unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[2] = 0;
    output_value[3] = 0;
    output_value[4] = 0;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_vax_d(CVT_VAX_D input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a VAX d_floating
**      number and to initialize an UNPACKED_REAL structure with those bits.
**
**      A VAX d_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 8 exp bits (bias 128), 7 fraction bits
**          [1]: 16 more fraction bits
**          [2]: 16 more fraction bits
**          [3]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      input_value             - address of the VAX D value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Initialization.
   ** ==========================================================================
   */
#ifdef WORDS_BIGENDIAN
  memcpy(&output_value[1], &((char *)input_value)[4], 4);
  memcpy(&output_value[2], input_value, 4);
#else
  memcpy(&output_value[1], input_value, 8);
#endif
  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 15) & U_R_NEGATIVE;

  /*
   ** Extract VAX biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 7) & 0x000000FFL;

  /*
   ** If the exponent is zero, determine if we have an invalid number or
   ** a zero value.  Set the flag bits accordingly.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    if (output_value[U_R_FLAGS])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] = U_R_ZERO;
  }

  /*
   ** Else the exponent is not zero and we will always have a valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust for VAX 16 bit floating format.
     ** ------------------------------------------------------------------------
     */
    output_value[1] = ((output_value[1] << 16) | (output_value[1] >> 16));
    output_value[2] = ((output_value[2] << 16) | (output_value[2] >> 16));

    /*
     ** Add unpacked real bias and subtract VAX bias.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 128);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00800000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 8;
    output_value[1] |= (output_value[2] >> 24);
    output_value[2] <<= 8;

    /*
     ** Clear uninitialized part of unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[3] = 0;
    output_value[4] = 0;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_vax_g(CVT_VAX_G input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a VAX g_floating
**      number and to initialize an UNPACKED_REAL structure with those bits.
**
**      A VAX g_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 11 exp bits (bias 1024), 4 fraction bits
**          [1]: 16 more fraction bits
**          [2]: 16 more fraction bits
**          [3]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      input_value             - address of the VAX G value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Initialization.
   ** ==========================================================================
   */
#ifdef WORDS_BIGENDIAN
  memcpy(&output_value[1], &((char *)input_value)[4], 4);
  memcpy(&output_value[2], input_value, 4);
#else
  memcpy(&output_value[1], input_value, 8);
#endif

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 15) & U_R_NEGATIVE;

  /*
   ** Extract VAX biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 4) & 0x000007FFL;

  /*
   ** If the exponent is zero, determine if we have an invalid number or
   ** a zero value.  Set the flag bits accordingly.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    if (output_value[U_R_FLAGS])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] = U_R_ZERO;
  }

  /*
   ** Else the exponent is not zero and we will always have a valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust for VAX 16 bit floating format.
     ** ------------------------------------------------------------------------
     */
    output_value[1] = ((output_value[1] << 16) | (output_value[1] >> 16));
    output_value[2] = ((output_value[2] << 16) | (output_value[2] >> 16));

    /*
     ** Add unpacked real bias and subtract VAX bias.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 1024);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00100000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 11;
    output_value[1] |= (output_value[2] >> 21);
    output_value[2] <<= 11;

    /*
     ** Clear uninitialized part of unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[3] = 0;
    output_value[4] = 0;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_vax_h(CVT_VAX_H input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a VAX h_floating
**      number and to initialize an UNPACKED_REAL structure with those bits.
**
**      A VAX h_floating number in (16 bit words) looks like:
**
**          [0]: Sign bit, 15 exp bits (bias 16384)
**          [1]: 16 fraction bits
**          [2]: 16 more fraction bits
**          [3]: 16 more fraction bits
**          [4]: 16 more fraction bits
**          [5]: 16 more fraction bits
**          [6]: 16 more fraction bits
**          [7]: 16 more fraction bits
**
**          0.5 <= fraction < 1.0, MSB implicit
**
** INPUT PARAMETERS:
**
**      input_value             - address of the VAX H value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(&output_value[1], input_value, 16);

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 15) & U_R_NEGATIVE;

  /*
   ** Extract VAX biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = output_value[1] & 0x00007FFFL;

  /*
   ** If the exponent is zero, determine if we have an invalid number or
   ** a zero value.  Set the flag bits accordingly.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    if (output_value[U_R_FLAGS])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] = U_R_ZERO;
  }

  /*
   ** Else the exponent is not zero and we will always have a valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust for VAX 16 bit floating format.
     ** ------------------------------------------------------------------------
     */
    output_value[1] = ((output_value[1] << 16) | (output_value[1] >> 16));
    output_value[2] = ((output_value[2] << 16) | (output_value[2] >> 16));
    output_value[3] = ((output_value[3] << 16) | (output_value[3] >> 16));
    output_value[4] = ((output_value[4] << 16) | (output_value[4] >> 16));

    /*
     ** Add unpacked real bias and subtract VAX bias.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 16384);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00010000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 15;
    output_value[1] |= (output_value[2] >> 17);
    output_value[2] <<= 15;
    output_value[2] |= (output_value[3] >> 17);
    output_value[3] <<= 15;
    output_value[3] |= (output_value[4] >> 17);
    output_value[4] <<= 15;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_ieee_s(CVT_IEEE_S input_value,
				  UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a IEEE single
**      floating number and to initialize an UNPACKED_REAL structure with those
**      bits.
**
**      A normalized IEEE single precision floating number looks like:
**
**          Sign bit, 8 exp bits (bias 127), 23 fraction bits
**
**          1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see "Mips R2000 Risc Architecture" by Gerry Kane,
**      page 6-8 or ANSI/IEEE Std 754-1985.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IEEE S value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int i;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  if (options & CVT_M_BIG_ENDIAN) {
    memcpy(output_value, input_value, 4);

    /*
     ** Shuffle bytes to little endian format.
     ** ------------------------------------------------------------------------
     */
    output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
    output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
    output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);
  } else {
    memcpy(&output_value[1], input_value, 4);
  }

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Extract biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 23) & 0x000000FFL;

  /*
   ** Check for denormalized values.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    /*
     ** Clear sign bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x7FFFFFFFL;

    /*
     ** If fraction is non-zero then normalize it.
     ** ------------------------------------------------------------------------
     */
    if (output_value[1] != 0) {
      /*
       ** Count leading zeros.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x00400000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent and normalize fraction.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 126 - i;
      output_value[1] <<= 9;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[2] = 0;
      output_value[3] = 0;
      output_value[4] = 0;
    }

    else {
      output_value[U_R_FLAGS] |= U_R_ZERO;
    }
  }

  /*
   ** Check for NANs and INFINITIES.
   ** ==========================================================================
   */
  else if (output_value[U_R_EXP] == 255) {
    /*
     ** Clear sign bit and exponent.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x007FFFFFL;
    if (output_value[1])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] |= U_R_INFINITY;
  }

  /*
   ** We have ourselves a genuine valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust exponent bias.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 126);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00800000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 8;

    /*
     ** Clear uninitialized part of unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[2] = 0;
    output_value[3] = 0;
    output_value[4] = 0;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_ieee_t(CVT_IEEE_T input_value,
				  UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a IEEE double
**      floating number and to initialize an UNPACKED_REAL structure with those
**      bits.
**
**      A normalized IEEE double precision floating number looks like:
**
**          [0]: 32 low order fraction bits
**          [1]: Sign bit, 11 exp bits (bias 1023), 20 fraction bits
**
**          1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see "Mips R2000 Risc Architecture" by Gerry Kane,
**      page 6-8 or ANSI/IEEE Std 754-1985.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IEEE T value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int i;
  STATIC_CONSTANT double fliptest = 42.;
  int flip = *(int *)&fliptest != 0;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(output_value, input_value, 8);
  if (flip)
    FlipDouble((int *)output_value);
  if (options & CVT_M_BIG_ENDIAN) {
    /*
     ** Shuffle bytes to little endian format.
     ** ------------------------------------------------------------------------
     */
    output_value[2] = ((output_value[1] << 24) | (output_value[1] >> 24));
    output_value[2] |= ((output_value[1] << 8) & 0x00FF0000L);
    output_value[2] |= ((output_value[1] >> 8) & 0x0000FF00L);
    output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
    output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
    output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);
  } else {
    output_value[2] = output_value[0];
  }

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Extract biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 20) & 0x000007FFL;

  /*
   ** Check for denormalized values.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    /*
     ** Clear sign bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x7FFFFFFFL;

    /*
     ** If fraction is non-zero then normalize it.
     ** ------------------------------------------------------------------------
     */
    if (output_value[1] != 0) {
      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x00080000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent and normalize fraction.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 1022 - i;
      output_value[1] <<= 12;
      i += 12;
      output_value[1] |= (output_value[2] >> (32 - i));
      output_value[2] <<= i;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[3] = 0;
      output_value[4] = 0;
    }

    else if (output_value[2]) {
      output_value[1] = output_value[2];

      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 20;
      while (!(output_value[1] & 0x80000000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 1022 - i;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[2] = 0;
      output_value[3] = 0;
      output_value[4] = 0;
    } else {
      output_value[U_R_FLAGS] |= U_R_ZERO;
    }
  }

  /*
   ** Check for NANs and INFINITIES.
   ** ==========================================================================
   */
  else if (output_value[U_R_EXP] == 2047) {
    /*
     ** Clear sign bit and exponent.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x000FFFFFL;

    if (output_value[1] | output_value[2])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] |= U_R_INFINITY;
  }

  /*
   ** We have ourselves a genuine valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust exponent.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 1022);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00100000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 11;
    output_value[1] |= (output_value[2] >> 21);
    output_value[2] <<= 11;

    /*
     ** Clear uninitialized part of unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[3] = 0;
    output_value[4] = 0;
  }
  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_ieee_x(CVT_IEEE_X input_value,
				  UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a IEEE extended
**      precision floating number and to initialize an UNPACKED_REAL structure
**      with those bits.
**
**      A normalized IEEE extended precision floating number looks like:
**
**          [0]: 32 low order fraction bits
**          [1]: 32 more fraction bits
**          [2]: 32 more fraction bits
**          [3]: Sign bit, 15 exp bits (bias 16383), 16 fraction bits
**
**          1.0 <= fraction < 2.0, MSB implicit
**
**      For more details see ANSI/IEEE Std 754-1985.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IEEE X value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int i;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(output_value, input_value, 16);

  if (options & CVT_M_BIG_ENDIAN) {
    /*
     ** Shuffle bytes to little endian format.
     ** ------------------------------------------------------------------------
     */
    output_value[4] = ((output_value[3] << 24) | (output_value[3] >> 24));
    output_value[4] |= ((output_value[3] << 8) & 0x00FF0000L);
    output_value[4] |= ((output_value[3] >> 8) & 0x0000FF00L);

    output_value[3] = ((output_value[2] << 24) | (output_value[2] >> 24));
    output_value[3] |= ((output_value[2] << 8) & 0x00FF0000L);
    output_value[3] |= ((output_value[2] >> 8) & 0x0000FF00L);

    output_value[2] = ((output_value[1] << 24) | (output_value[1] >> 24));
    output_value[2] |= ((output_value[1] << 8) & 0x00FF0000L);
    output_value[2] |= ((output_value[1] >> 8) & 0x0000FF00L);

    output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
    output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
    output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);
  } else {
    output_value[4] = output_value[0];
    output_value[0] = output_value[3];
    output_value[3] = output_value[1];
    output_value[1] = output_value[0];
  }

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Extract biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = (output_value[1] >> 16) & 0x00007FFFL;

  /*
   ** Check for denormalized values.
   ** ==========================================================================
   */
  if (output_value[U_R_EXP] == 0) {
    /*
     ** Clear sign bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x7FFFFFFFL;

    /*
     ** If fraction is non-zero then normalize it.
     ** ------------------------------------------------------------------------
     */
    if (output_value[1] != 0) {
      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x00008000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent and normalize fraction.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 16382 - i;
      output_value[1] <<= 16;
      i += 16;
      output_value[1] |= (output_value[2] >> (32 - i));
      output_value[2] <<= i;
      output_value[2] |= (output_value[3] >> (32 - i));
      output_value[3] <<= i;
      output_value[3] |= (output_value[4] >> (32 - i));
      output_value[4] <<= i;
    }

    /*
     ** If fraction in first 32 bits is zero then move each longword up one
     ** longword and then normalize it.
     ** ------------------------------------------------------------------------
     */
    else if (output_value[2]) {
      output_value[1] = output_value[2];
      output_value[2] = output_value[3];
      output_value[3] = output_value[4];

      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x80000000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 16382 - (i + 16);
      output_value[1] |= (output_value[2] >> (32 - i));
      output_value[2] <<= i;
      output_value[2] |= (output_value[3] >> (32 - i));
      output_value[3] <<= i;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[4] = 0;
    }

    /*
     ** If fraction in first 64 bits is zero then move each longword up two
     ** longwords and then normalize it.
     ** ------------------------------------------------------------------------
     */
    else if (output_value[3]) {
      output_value[1] = output_value[3];
      output_value[2] = output_value[4];

      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x80000000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 16382 - (i + 48);
      output_value[1] |= (output_value[2] >> (32 - i));
      output_value[2] <<= i;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[3] = 0;
      output_value[4] = 0;
    }

    /*
     ** If fraction in first 96 bits is zero then move each longword up three
     ** longwords and then normalize it.
     ** ------------------------------------------------------------------------
     */
    else if (output_value[4]) {
      output_value[1] = output_value[4];

      /*
       ** Count leading zeros in fraction.
       ** ----------------------------------------------------------------------
       */
      i = 0;
      while (!(output_value[1] & 0x80000000L)) {
	output_value[1] <<= 1;
	i += 1;
      }

      /*
       ** Adjust exponent.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] = U_R_BIAS - 16382 - (i + 80);

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[2] = 0;
      output_value[3] = 0;
      output_value[4] = 0;
    }

    /*
     ** Otherwise the fraction is completely zero, so set the zero flag.
     ** ------------------------------------------------------------------------
     */
    else {
      output_value[U_R_FLAGS] |= U_R_ZERO;
    }
  }

  /*
   ** Check for NANs and INFINITIES.
   ** ==========================================================================
   */
  else if (output_value[U_R_EXP] == 32767) {
    /*
     ** Clear sign bit and exponent.
     ** ------------------------------------------------------------------------
     */
    output_value[1] &= 0x0000FFFFL;

    if (output_value[1] | output_value[2] | output_value[3] | output_value[4])
      output_value[U_R_FLAGS] |= U_R_INVALID;
    else
      output_value[U_R_FLAGS] |= U_R_INFINITY;
  }

  /*
   ** We have ourselves a genuine valid number.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust exponent.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 16382);

    /*
     ** Set hidden bit.
     ** ------------------------------------------------------------------------
     */
    output_value[1] |= 0x00010000L;

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 15;
    output_value[1] |= (output_value[2] >> 17);
    output_value[2] <<= 15;
    output_value[2] |= (output_value[3] >> 17);
    output_value[3] <<= 15;
    output_value[3] |= (output_value[4] >> 17);
    output_value[4] <<= 15;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_ibm_l(CVT_IBM_LONG input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an IBM long
**      floating number and to initialize an UNPACKED_REAL structure with those
**      bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized IBM long floating number looks like:
**
**          [0]: Sign bit, 7 exp bits (bias 64), 24 fraction bits
**          [1]: 32 low order fraction bits
**
**          0.0625 <= fraction < 1.0, from 0 to 3 leading zeros to compensate
**          for the hexadecimal exponent.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IBM LONG value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int i;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(output_value, input_value, 8);

  /*
   ** Shuffle bytes to little endian format.
   ** ==========================================================================
   */
  output_value[2] = ((output_value[1] << 24) | (output_value[1] >> 24));
  output_value[2] |= ((output_value[1] << 8) & 0x00FF0000L);
  output_value[2] |= ((output_value[1] >> 8) & 0x0000FF00L);
  output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
  output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
  output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Clear sign bit.
   ** ==========================================================================
   */
  output_value[1] &= 0x7FFFFFFFL;

  /*
   ** Check if 0.
   ** ==========================================================================
   */
  if ((output_value[1] == 0) && (output_value[2] == 0)) {
    output_value[U_R_FLAGS] |= U_R_ZERO;
  }

  /*
   ** Ok, we know that the number is not zero.
   ** ==========================================================================
   */
  else {
    /*
     ** Get unbiased hexadecimal exponent and convert it to binary.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] = U_R_BIAS + (((output_value[1] >> 24) - 64) * 4);

    /*
     ** Count leading zeros.
     ** ------------------------------------------------------------------------
     */
    i = 0;
    while (!(output_value[1] & 0x00800000L)) {
      i += 1;
      if (i > 3)
	break;
      output_value[1] <<= 1;
    }

    /*
     ** If we have more than three leading zero bits then number is invalid.
     ** ------------------------------------------------------------------------
     */
    if (i > 3) {
      output_value[U_R_FLAGS] |= U_R_INVALID;
    }

    /*
     ** Number is valid so far...
     ** ------------------------------------------------------------------------
     */
    else {

      /*
       ** Adjust exponent to compensate for leading zeros.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] -= i;

      /*
       ** Left justify fraction bits.
       ** ----------------------------------------------------------------------
       */
      output_value[1] <<= 8;
      i += 8;
      output_value[1] |= (output_value[2] >> (32 - i));
      output_value[2] <<= i;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[3] = 0;
      output_value[4] = 0;
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_ibm_s(CVT_IBM_SHORT input_value,
				 UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from an IBM single
**      floating number and to initialize an UNPACKED_REAL structure with those
**      bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized IBM short floating number looks like:
**
**          Sign bit, 7 exp bits (bias 64), 24 fraction bits
**
**          0.0625 <= fraction < 1.0, from 0 to 3 leading zeros to compensate
**          for the hexadecimal exponent.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IBM LONG value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  int i;

  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(output_value, input_value, 4);

  /*
   ** Shuffle bytes to little endian format.
   ** ==========================================================================
   */
  output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
  output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
  output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Clear sign bit.
   ** ==========================================================================
   */
  output_value[1] &= 0x7FFFFFFFL;

  /*
   ** Check if 0.
   ** ==========================================================================
   */
  if (output_value[1] == 0) {
    output_value[U_R_FLAGS] |= U_R_ZERO;
  }

  /*
   ** Ok, we know that the number is not zero.
   ** ==========================================================================
   */
  else {
    /*
     ** Get unbiased hexadecimal exponent and convert it to binary.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] = U_R_BIAS + (((output_value[1] >> 24) - 64) * 4);

    /*
     ** Count leading zeros.
     ** ------------------------------------------------------------------------
     */
    i = 0;
    while (!(output_value[1] & 0x00800000L)) {
      i += 1;
      if (i > 3)
	break;
      output_value[1] <<= 1;
    }

    /*
     ** If we have more than three leading zero bits then number is invalid.
     ** ------------------------------------------------------------------------
     */
    if (i > 3) {
      output_value[U_R_FLAGS] |= U_R_INVALID;
    }

    /*
     ** Number is valid so far...
     ** ------------------------------------------------------------------------
     */
    else {

      /*
       ** Adjust exponent to compensate for leading zeros.
       ** ----------------------------------------------------------------------
       */
      output_value[U_R_EXP] -= i;

      /*
       ** Left justify fraction bits.
       ** ----------------------------------------------------------------------
       */
      output_value[1] <<= 8;

      /*
       ** Clear uninitialized part of unpacked real.
       ** ----------------------------------------------------------------------
       */
      output_value[2] = 0;
      output_value[3] = 0;
      output_value[4] = 0;
    }
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}

STATIC_ROUTINE void unpack_cray(CVT_CRAY input_value, UNPACKED_REAL output_value, uint32_t options __attribute__ ((unused)))
/*
**=============================================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**      This module contains code to extract information from a cray floating
**      number and to initialize an UNPACKED_REAL structure with those bits.
**
**      See the header files for a description of the UNPACKED_REAL structure.
**
**      A normalized CRAY floating number looks like:
**
**          [0]: Sign bit, 15 exp bits (bias 16384), 16 fraction bits
**          [1]: 32 low order fraction bits
**
**          0.5 <= fraction < 1.0, MSB explicit
**          Since CRAY has no hidden bits the MSB must always be set.
**
**      Some of the CRAY exponent range is not used.
**      Exponents < 0x2000 and >= 0x6000 are invalid.
**
** INPUT PARAMETERS:
**
**      input_value             - address of the IBM LONG value to be unpacked.
**      options                 - bits set specify various conversion options.
**
** IMPLICIT INPUTS:
**
**      NONE
**
** OUTPUT PARAMETERS:
**
**      output_value            - address of where to place the UNPACKED_REAL
**                                result value
**
** IMPLICIT OUTPUTS:
**
**      NONE
**
** FUNCTION VALUE:
**
**      NONE
**
** SIDE EFFECTS:
**
**      NONE
**
**=============================================================================
*/
{
  /*
   ** Local variable definition.
   ** ==========================================================================
   */
  /*
   ** Initialization.
   ** ==========================================================================
   */
  memcpy(output_value, input_value, 8);

  /*
   ** Shuffle bytes to little endian format.
   ** ==========================================================================
   */
  output_value[2] = ((output_value[1] << 24) | (output_value[1] >> 24));
  output_value[2] |= ((output_value[1] << 8) & 0x00FF0000L);
  output_value[2] |= ((output_value[1] >> 8) & 0x0000FF00L);
  output_value[1] = ((output_value[0] << 24) | (output_value[0] >> 24));
  output_value[1] |= ((output_value[0] << 8) & 0x00FF0000L);
  output_value[1] |= ((output_value[0] >> 8) & 0x0000FF00L);

  /*
   ** Initialize FLAGS and perhaps set NEGATIVE bit.
   ** ==========================================================================
   */
  output_value[U_R_FLAGS] = (output_value[1] >> 31);

  /*
   ** Clear sign bit.
   ** ==========================================================================
   */
  output_value[1] &= 0x7FFFFFFFL;

  /*
   ** Extract CRAY biased exponent.
   ** ==========================================================================
   */
  output_value[U_R_EXP] = output_value[1] >> 16;

  /*
   ** Check if 0 or an invalid exponent.
   ** ==========================================================================
   */
  if ((output_value[1] == 0) && (output_value[2] == 0)) {
    output_value[U_R_FLAGS] |= U_R_ZERO;
  } else if ((output_value[U_R_EXP] < 0x2000) ||
	     (output_value[U_R_EXP] >= 0x6000) || (!(output_value[1] & 0x00008000L))) {
    output_value[U_R_FLAGS] |= U_R_INVALID;
  }

  /*
   ** Ok, we know that the number is not zero.
   ** ==========================================================================
   */
  else {
    /*
     ** Adjust bias.
     ** ------------------------------------------------------------------------
     */
    output_value[U_R_EXP] += (U_R_BIAS - 16384);

    /*
     ** Left justify fraction bits.
     ** ------------------------------------------------------------------------
     */
    output_value[1] <<= 16;
    output_value[1] |= (output_value[2] >> 16);
    output_value[2] <<= 16;

    /*
     ** Clear uninitialized part of unpacked real.
     ** ------------------------------------------------------------------------
     */
    output_value[3] = 0;
    output_value[4] = 0;
  }

  /*
   ** Exit the routine.
   ** ==========================================================================
   */
  return;

}
