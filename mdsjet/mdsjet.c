#include <jetapidefines.h>
#include <jetsimpleapi.h>
#include <jetadvancedapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static float *pData, *pX;
static double *pTime;
static unsigned int uData,uT,uX;
static char *pUnits, *pTunits, *pXunits;
static JETRequestHandle RequestHandle;

extern int mdsjet(char *url, int *nData, int *nt, int *nx);

int mdsjet_setidentity( char *name, char *password)
{
	return _JETSetIdentity(name, password);
}

int mdsjet_idl(int argc, void **argv)
{
  int nd, nt, nx;
  nd = 0;
  nt = 0;
  nx = 0;
  printf("Calling mdsget: %d\n",mdsjet((char *)argv[0], &nd, &nt, &nx));
  *(int *)argv[1] = nd;
  *(int *)argv[2] = nt;
  *(int *)argv[3] = nx;
  /*  printf("URL: %s\n",(char *)argv[0]);
   * *(int *)argv[1] = 1.;
   */
  return 0;
}

int mdsjet(char *url, int *nData, int *nt, int *nx)
{
        int iRet,iUnits;
        printf("URL IN C: -->%s<--\n",url);
	iRet=_JETReadData(url,&pData,&pTime,&pX,&uData,&uT,&uX,&RequestHandle);
	
	if (iRet==RDA_ERR_NOERROR)
	{
		const char *dUnits="";
		const char *tUnits="";
		const char *xUnits="";
		*nData = uData;
		*nt = uT;
		*nx = uX;
//		iUnits = JETStringValue(RequestHandle, dUnits, &pUnits);
	}
	else
	{
		const char *pszString;
		unsigned long ulError;

		_JETGetLastErrorDetail(&ulError,&pszString);
		printf("The following RDA error was reported: %d\n",iRet);
		printf("Details: %x\n%s\n",ulError, pszString);
	}
	

	return iRet;
}


int mdsjetdata(float *data, double *t, double *x)
{
  memcpy(data, pData, uData * sizeof(float));
  memcpy(t, pTime, uT * sizeof(double));
  if (uX > 1)  memcpy(x, pX, uX * sizeof(float));
  _JETFreeData(RequestHandle);
  return 1;
}
