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
/********************************************************************************************************************************/
/* Created: 28-DEC-1995 08:04:14 by OpenVMS SDL EV1-33     */
/* Source:  28-DEC-1995 08:03:53 HARPO$DKA400:[SYS0.SYSUPD.CC052]CVTDEF.SDI;1 */
/********************************************************************************************************************************/
/*** MODULE cvtdef ***/
#ifndef __CVTDEF_LOADED
#define __CVTDEF_LOADED 1

#ifndef _MSC_VER
#pragma nostandard
#endif

#ifdef __cplusplus
extern "C" {
#define __unknown_params ...
#else
#define __unknown_params
#endif

#if !defined(__VAXC) && !defined(VAXC)
#define __struct struct
#define __union union
#else
#define __struct variant_struct
#define __union variant_union
#endif

#define CvtVAX_F 10		/* VAX F     Floating point data    */
#define CvtVAX_D 11		/* VAX D     Floating point data    */
#define CvtVAX_G 27		/* VAX G     Floating point data    */
#define CvtVAX_H 28		/* VAX H     Floating point data    */
#define CvtIEEE_S 52		/* IEEE S    Floating point data    */
#define CvtIEEE_T 53		/* IEEE T    Floating point data    */
#define CvtIBM_LONG 6		/* IBM Long  Floating point data    */
#define CvtIBM_SHORT 7		/* IBM Short Floating point data    */
#define CvtCRAY 8		/* Cray      Floating point data    */
#define CvtIEEE_X 9		/* IEEE X    Floating point data    */

extern unsigned long CvtConvertFloat(void *input_v, unsigned long input_t,
				     void *output_v, unsigned long output_t, ...);

#ifdef __cplusplus
}
#endif

#ifndef _MSC_VER
#pragma standard
#endif

#endif				/* __CVTDEF_LOADED */
