// -------------------------------------------------------------------
// JET Remote Data Access Software
// -------------------------------------------------------------------
//
// (c) 1999 JET Joint Undertaking
//
// Software Development Section
// Data Analysis & Modelling Division
//
// Email:  remote-access-support@jet.uk
//
// This software is only for the use of authorised users
// collaborating with the JET Joint Undertaking.
//
// Use for any other purpose is strictly forbidden.
//
// The source code may not be modified in any way or re-distributed.
//
// -------------------------------------------------------------------
//
////////////////////////////////////////////////////////////
//
// Main support public header for JETAPI modules
//
////////////////////////////////////////////////////////////

#if !defined (_JETAPIDEFINES_H__INCLUDED)
#define _JETAPIDEFINES_H__INCLUDED
#else
#error "multiple include of file JETAPIDefines.h"
#endif /* _JETAPIDEFINES_H__INCLUDED */


enum JETdatatype
 {
	JET_type_int_scalar,
	JET_type_short_scalar,
	JET_type_float_scalar,
	JET_type_double_scalar,
	JET_type_char_scalar,
	JET_type_string_scalar,
	JET_type_time_t_scalar,
	JET_type_int_array,
	JET_type_short_array,
	JET_type_float_array,
	JET_type_double_array,
	JET_type_char_array,
	JET_type_string_array,
	JET_type_time_t_array,
	JET_type_unknown
};
	
#define RDA_ERR_NOERROR					 0	/* No error, it worked! */
#define RDA_ERR_NOTIMPLEMENTED			-1	/* The function called has not yet been implemented! */
#define RDA_ERR_INTERNAL				-2  /* An internal error was detected */
#define RDA_ERR_INVALID_REQUESTHANDLE	-3	/* The user passed an invalid request handle to the API */
#define RDA_ERR_INVALID_DATAHANDLE		-4	/* The user passed an invalid data handle to the API */
#define RDA_ERR_DATANOTPRESENT			-5	/* The data asked for was not present in the reply */
#define RDA_ERR_WRONGDATATYPE			-6  /* The data asked for was present, but the wrong data type */

#define RDA_ERR_BADLOGON					-10
#define RDA_ERR_QUOTA_OUTSTANDINGREQUESTS	-11
#define RDA_ERR_QUOTA_BYTESTRANSFERRED		-12
#define RDA_ERR_QUOTA_SIGNALSTRANSFERRED	-13
#define RDA_ERR_ACCESSDENIED_READ			-14
#define RDA_ERR_ACCESSDENIED_WRITE			-15

#define RDA_ERR_URL_SYNTAX					-20
#define RDA_ERR_URL_UNKNOWNDATATYPE			-21

#define RDA_ERR_NET_NOCONNECTION			-30
#define RDA_ERR_NET_TIMEOUT					-31
#define RDA_ERR_NET_PROXYAUTHFAILED			-32

#define RDA_ERR_CONTROL_SYSTEM_UNAVAILABLE	-33

// error return names
#define RDA_NAME_ERR_CODE			"ErrorCode"
#define RDA_NAME_ERR_DETAIL_CODE	"ErrorDetailCode"
#define RDA_NAME_ERR_DETAIL_TEXT	"ErrorDetailText"

#if !defined RDAAPI_EXPORT

	#if defined WIN32
		#define RDAAPI_EXPORT __declspec(dllimport) __stdcall
	#else
		#define RDAAPI_EXPORT
	#endif /* WIN32 */

#endif /* RDAAPI_EXPORT */


