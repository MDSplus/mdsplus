#include "mdslib.h"


void main(int argc, char *argv[]);

void main(int argc, char *argv[])
{
  long status;
  int shot;
  char string[50];
  float result[10], result1;
  float thomsondata[1000];
  int dsc,dsc1,dsc2,dscr,dscrsize,dscrstring,i;
  short int sizeresult;
  struct descrip ans;
  struct descrip darg1,darg2;
  struct descrip *jeff1,*jeff2;
  float arg1 = 1.234567;
  float arg2 = 2.345678;
  int null = NULL;
  int dtype_float = DTYPE_F;
  int dtype_short = DTYPE_SHORT;
  int dtype_cstring = DTYPE_CSTRING;
  int dtype_long = DTYPE_LONG;
  int sx = 2;
  int sy = 13;
  int sresult = 10;
  int stringlength = 50;
  int returnlength=0;
  float testmulti[2][13] = {
    {0., 31., 28., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.},
    {0., 31., 29., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.}};

  for (i=0;i<10;i++) result[i]=0;

  for (i=0;i<1000;i++) thomsondata[i] = 12345.678;


  dsc = descr(&dtype_float,result,&sresult,&null);
  dsc1 = descr(&dtype_float,&arg1,&null);
  dsc2 = descr(&dtype_float,&arg2,&null);
  dscr = descr(&dtype_float,&result1,&null);
  dscrsize = descr(&dtype_short,&sizeresult,&null); 
  dscrstring = descr(&dtype_cstring,string,&null,&stringlength);


  if (argc > 1) 
    {
      printf("SOCKET: %d\n",MdsConnect(argv[1])); 
      printf("SOCKET: %d\n",MdsConnect(argv[1])); 
    }
  printf("doing mdsvalue(1)\n");
  status = MdsValue("1.",&dscr,&null,&returnlength);

  shot = 96021;
  status = MdsOpen("EFIT01",&shot);
  printf("Status opening EFIT01, %d: %d\n",shot,status);

  shot = 12345;
  status = MdsOpen("FOOFOOFOO",&shot);
  printf("Status opening BOGUS Tree: %d\n",status);

  shot = 96333;
  status = MdsOpen("ELECTRONS",&shot);
  printf("Status opening ELECTRONS Tree, %d: %d\n",shot, status);

  shot = 12345;
  status = MdsClose("FOOFOOFOO",&shot);
  printf("Status closing BOGUS Tree: %d\n",status);
  status = MdsValue("$EXPT",&dscrstring,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("Current experiment: %s \n", string);

  shot = 96021;
  status = MdsClose("EFIT01",&shot);
  printf("Status closing EFIT01, %d: %d\n",shot,status);
  status = MdsValue("$EXPT",&dscrstring,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("Current experiment: %s \n", string);

  shot = 96333;
  status = MdsClose("ELECTRONS",&shot);
  printf("Status closing ELECTRONS Tree, %d: %d\n",shot, status);
  status = MdsValue("$EXPT",&dscrstring,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("Current experiment: %s \n", string);

  printf("=================== TEST 1 ======================\n");
  status = MdsValue("2. : 20. : 2.",&dsc,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  for (i=0; i<10; i++) 
  {
    printf("i: %d   result: %f \n",i,result[i]);
  }


  printf("=================== TEST 2 ======================\n");
  status = MdsValue("$ * $",&dsc1,&dsc2,&dscr,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("result: %f\n",result1);


  printf("=================== TEST 3 ======================\n");

  shot = 10;
  printf("Status opening EFIT01, shot=%d: %d\n",shot,MdsOpen("EFIT01",&shot));
  status = MdsValue("$EXPT",&dscrstring,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("experiment: %s\n",string);

  status = MdsValue("SIZE(\\ATIME)",&dscrsize,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("result: %d\n",sizeresult);

  dsc = descr(&dtype_float,result,&sresult,&null);
  status = MdsValue("\\ATIME",&dsc,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  for (i=0; i<10; i++)
  {
    printf("i: %d   atime: %f \n",i,result[i]);
  }

  printf("=================== TEST 4 ======================\n");

  
  dsc = descr(&dtype_float, testmulti, &sx, &sy, &null); 

  status = MdsPut("\\TOP:NEMPROF","$",&dsc,&null);
  printf("Putting: %f\n",testmulti[0][2]);
  printf("Status putting \\TOP:NEMPROF: %d\n",status);


  if (status & 1) 
  {
    float *data = malloc(sx*sy * sizeof(float));
    dsc = descr(&dtype_float, data, &sx, &sy, &null);
    status = MdsValue("\\TOP:NEMPROF",&dsc,&null,&returnlength);
    printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
    for (i=0;i<sx;i++)
      {
	int j;
	for (j=0;j<sy;j++) printf("i: %d, j: %d, data: %f \n",i,j,(data+j)[i]);
      }
    free(data);
  }
  

  
  printf("=================== TEST 5 ======================\n");

  printf("Status setting default: %d\n",MdsSetDefault("\\TOP.RESULTS.AEQDSK"));
  status = MdsValue("$EXPT",&dscrstring,&null,&returnlength);
  printf("experiment: %s\n",string);

  dsc = descr(&dtype_long,&shot, &null);
  status = MdsValue("$SHOT",&dsc,&null,&returnlength);
  printf("SHOT: %d\n",shot);


  status = MdsValue("$DEFAULT",&dscrstring,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("default: %s\n",string);
  

  printf("=================== TEST 6 ======================\n");

  status = MdsValue("1.",&dsc1,&null,&returnlength);  /* DO NOT USE &null as last argument because then null!=0 !! */
  printf("MdsValue status: %d  (NO RETURN LENGTH IN ARGUMENT LIST)\n", status);
  printf("result: %f\n",arg1);

  printf("=================== TEST 7 ======================\n");
    
  dsc = descr(&dtype_cstring,string,&null,&stringlength);
  status = MdsValue("FINDSIG('TSTE_CORE')",&dsc,&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("FINDSIG(TSTE_CORE): %s\n",string);
  

  printf("=================== TEST 8 ======================\n");

  sresult = 1000;
  dsc = descr(&dtype_float, thomsondata, &sresult, &null);
  status = MdsOpen("EFIT01", &shot);
  if (status & 1)
    {
      status = MdsPut("\\TOP:ONE_NUMBER","$",&dsc,&null);
      printf("Putting: %f\n",thomsondata[0]);
      printf("Status putting \\TOP:ONE_NUMBER: %d\n",status);
    }
  

  printf("=================== TEST 9 ======================\n");

  sresult = 1000;
  dsc = descr(&dtype_float,thomsondata,&sresult,&null);
  shot = 100;
  status = MdsOpen("ELECTRONS",&shot);
  if (status & 1) 
    {
      status = MdsPut("\\TSTE_CORE","BUILD_SIGNAL($,*,*)",&dsc,&null);
    }
  printf("Status putting thomson data: %d\n",status);
  

  printf("=================== TIMING TEST ======================\n");


  for (i=0;i<500;i++) status = MdsValue("FINDSIG('\\TSTE_CORE')",&dscrstring,&null,&returnlength);
  
  /*  printf("=================== CDESCR TEST ======================\n");

  status = MdsValue("FINDSIG('TSTE_CORE')",cdescr(DTYPE_CSTRING,string,0,stringlength),&null,&returnlength);
  printf("MdsValue status: %d   Return length: %d\n",status,returnlength);
  printf("FINDSIG(TSTE_CORE): %s\n",string);
  */


  exit(0);

}
