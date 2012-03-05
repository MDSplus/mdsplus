// -------------------------------------------------------
// JET Remote Data Access Software 
// -------------------------------------------------------
//
// (c) 1999 JET Joint Undertaking
//
// Software Development Section
// Data Analysis & Modelling Division 
//
// E-mail remote-access-support@jet.uk
//
// This software is only for the use of authorised users
// collaborating with the JET Joint Undertaking.
//
// Use for any other purpose is strictly forbidden.
//
// The source code may not be modified in anyway or redistributed.
//
//
// --------------------------------------------------------
#if !defined (JETSIMPLEAPI_H__INCLUDED)
#define JETSIMPLEAPI_H__INCLUDED
#else
#error "multiple include of file JETSimpleAPI.h"
#endif /* JETSIMPLEAPI_H__INCLUDED */

#if !defined _JETAPIDEFINES_H__INCLUDED
#error "JETAPIdefines.h must be included before JETSimpleAPI.h"
#endif

#if !defined RDAAPI_INTERNAL

typedef void *JETRequestHandle;
typedef void *JETDataHandle;

#endif /* RDAAPI_INTERNAL */

#if defined __cplusplus
extern "C"
{
#endif /*__cplusplus*/

/* Needed to compile on MSVC and __declspec(...) */
typedef const char *PCONSTCHAR;

int RDAAPI_EXPORT _JETReadData(const char * pszURL, float ** pData, double ** pT, float ** pX, unsigned int * pDataSize, unsigned int * pTimeSize, unsigned int * pXSize, JETRequestHandle * pHandle );
int RDAAPI_EXPORT _JETFreeData(JETRequestHandle RequestHandle);
int RDAAPI_EXPORT _JETSetIdentity(const char *szUserName, const char *szPassword);

void RDAAPI_EXPORT _JETGetLastErrorDetail(long *pulDetail,PCONSTCHAR *ppszString);


#if defined __cplusplus
}
#endif /*__cplusplus*/
