#include "MdsLib.h"


void main(int argc, char *argv[]);

void main(int argc, char *argv[])
{
  long status;
  char *string[40000];
  float result[10], result1;
  int dsc,dsc1,dsc2,dscr,dscrsize,dscrstring,i,sizeresult;
  struct descrip ans;
  struct descrip darg1,darg2;
  struct descrip *jeff1,*jeff2;
  float arg1 = 1.234567;
  float arg2 = 2.345678;
  int null = NULL;
  int dtype_float = DTYPE_F;
  int dtype_short = DTYPE_SHORT;
  int dtype_cstring = DTYPE_CSTRING;
  int sx = 2;
  int sy = 13;
  int sresult = 10;
  float testmulti[2][13] = {
    {0., 31., 28., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.},
    {0., 31., 29., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.}};


  for (i=0;i<10;i++) result[i]=0;

  dsc = descr(&dtype_float,result,&sresult,&null);
  dsc1 = descr(&dtype_float,&arg1,&null);
  dsc2 = descr(&dtype_float,&arg2,&null);
  dscr = descr(&dtype_float,&result1,&null);
  dscrsize = descr(&dtype_short,&sizeresult,&null); 
  dscrstring = descr(&dtype_cstring,string,&null);

  if (argc > 1) MdsConnect(argv[1]); 

  status = MdsOpen("EFIT01",96021);
  printf("Status opening EFIT01, 96021: %d\n",status);
  status = MdsOpen("FOOFOOFOO",12345);
  printf("Status opening BOGUS Tree: %d\n",status);
  status = MdsOpen("ELECTRONS",96333);
  printf("Status opening ELECTRONS Tree: %d\n",status);

  status = MdsClose("FOOFOOFOO",12345);
  printf("Status closing BOGUS Tree: %d\n",status);
  status = MdsValue("$EXPT",&dscrstring,&null);
  printf("Current experiment: %s\n", string);
  status = MdsClose("EFIT01",96021);
  printf("Status closing EFIT01, 96021: %d\n",status);
  status = MdsValue("$EXPT",&dscrstring,&null);
  printf("Current experiment: %s\n", string);
  status = MdsClose("ELECTRONS",96333);
  printf("Status closing ELECTRONS Tree: %d\n",status);
  status = MdsValue("$EXPT",&dscrstring,&null);
  printf("Current experiment: %s\n", string);

  printf("=================== TEST 1 ======================\n");
  status = MdsValue("2. : 20. : 2.",&dsc,&null);
  printf("MdsValue status: %d\n",status);
  for (i=0; i<10; i++) 
  {
    printf("i: %d   result: %f \n",i,result[i]);
  }


  printf("=================== TEST 2 ======================\n");
  status = MdsValue("$ * $",&dsc1,&dsc2,&dscr,&null);
  printf("MdsValue status: %d\n",status);
  printf("result: %f\n",result1);


  printf("=================== TEST 3 ======================\n");
  printf("Status opening EFIT01, shot=10: %d\n",MdsOpen("EFIT01",10));
  status = MdsValue("$EXPT",&dscrstring,&null);
  printf("MdsValue status: %d\n",status);
  printf("experiment: %s\n",string);

  status = MdsValue("SIZE(\\ATIME)",&dscrsize,&null);
  printf("MdsValue status: %d\n",status);
  printf("result: %d\n",sizeresult);

  

  printf("=================== TEST 4 ======================\n");

  
  /*  dsc = descr(&dtype_float,result,&sresult,&null); */
  dsc = descr(&dtype_float, &testmulti, &sx, &sy, &null); 

  status = MdsPut("\\TOP:ONE_NUMBER","$",&dsc,&null);
  printf("Putting: %f\n",testmulti[0][2]);
  printf("Status putting \\TOP:ONE_NUMBER: %d\n",status);


  /*
  if (status && sizeresult) 
  {
    float *data = malloc(sizeresult * sizeof(float));
    dsc = descr(&dtype_float, data,0);
    status = MdsValue("FLOAT(\\ATIME)",&dsc,&null);
    printf("status: %d\n",status);
    free(data);
  }
  */

  printf("=================== TEST 5 ======================\n");

  printf("Status setting default: %d\n",MdsSetDefault("\\TOP.RESULTS.AEQDSK"));

  status = MdsValue("$DEFAULT",&dscrstring,&null);
  printf("MdsValue status: %d\n",status);
  printf("default: %s\n",string);

}
