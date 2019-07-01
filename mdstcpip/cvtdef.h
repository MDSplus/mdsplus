/********************************************************************************************************************************/
/* Created: 28-DEC-1995 08:04:14 by OpenVMS SDL EV1-33     */
/* Source:  28-DEC-1995 08:03:53 HARPO$DKA400:[SYS0.SYSUPD.CC052]CVTDEF.SDI;1 */
/********************************************************************************************************************************/
/*** MODULE cvtdef ***/
#ifndef __CVTDEF_LOADED
#define __CVTDEF_LOADED 1

//#ifndef _MSC_VER
//#pragma nostandard
//#endif

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

extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue, uint32_t outdtype, uint32_t options);

#ifdef __cplusplus
}
#endif

//#ifndef _MSC_VER
//#pragma standard
//#endif

#endif				/* __CVTDEF_LOADED */
