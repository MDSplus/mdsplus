#include <string.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#define MAXEXPR 16384
extern int TdiExecute();

static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

static void tdiputs(char *line);
#ifdef HAVE_VXWORKS_H
int tditest(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
  FILE *in = stdin;
  int status;
/*
  static char expr[MAXEXPR] = "WRITE(_OUTPUT_UNIT,`DECOMPILE(`";
*/
  static char expr[MAXEXPR] = "";
  static struct descriptor expr_dsc = {0, DTYPE_T, CLASS_S, (char *)expr};
  static EMPTYXD(ans);
  static EMPTYXD(output_unit);
  static DESCRIPTOR(out_unit_stdout,"PUBLIC _OUTPUT_UNIT=*");
  static DESCRIPTOR(out_unit_other,"PUBLIC _OUTPUT_UNIT=FOPEN($,'w')");
  static DESCRIPTOR(reset_output_unit,"PUBLIC _OUTPUT_UNIT=$");
  int prefixlen = strlen(expr);
  char line_in[MAXEXPR];
  if (argc > 1)
    {
	  in = fopen(argv[1],"r");
          if (in == (FILE *)0)
	    {
               printf("Error opening input file /%s/\n",argv[1]);
               return 0;
            }
    }
  if (argc > 2)
  {
    struct descriptor out_d = {0,DTYPE_T,CLASS_S,0};
    out_d.length = (unsigned short)strlen(argv[2]);
    out_d.pointer = argv[2];
    TdiExecute(&out_unit_other,&out_d,&output_unit MDS_END_ARG);
  }
  else
    TdiExecute(&out_unit_stdout, &output_unit MDS_END_ARG);
  while(fgets(line_in,MAXEXPR,in) != NULL) {
    int comment = line_in[0] == '!';
    int len = strlen(line_in);
    if (!comment) {
      TdiExecute(&reset_output_unit,&output_unit, &ans MDS_END_ARG);
      tdiputs(line_in);
    }
    while(line_in[len-2] == '\\') {
      fgets(&line_in[len-2],MAXEXPR-len+2,in);
      if (!comment) tdiputs(&line_in[len-2]);
      len = strlen(line_in);
    }
    if (!comment) {
      static DESCRIPTOR(error_out,"_MSG=DEBUG(0),DEBUG(4),WRITE($,_MSG)");
      static DESCRIPTOR(clear_errors,"WRITE($,DECOMPILE($)),DEBUG(4)");
      expr[prefixlen]=0;
      strcat(expr,line_in);
      strcat(line_in," = ");
      expr_dsc.length = strlen(expr);    
/*
      expr_dsc.length = strlen(expr)-1;    
      expr_dsc.length = strlen(expr)-1;    
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
*/
      status = TdiExecute(&expr_dsc, &ans MDS_END_ARG);
      if (status & 1)
        TdiExecute(&clear_errors , &output_unit, &ans, &ans MDS_END_ARG);
      else
        TdiExecute(&error_out, &output_unit, &ans MDS_END_ARG);
    }
  }
  return 1;
}

static void tdiputs(char *line)
{
  static EMPTYXD(ans);
  static DESCRIPTOR(write_it,"WRITE(_OUTPUT_UNIT,$)");
  struct descriptor line_d = {0, DTYPE_T,CLASS_S,0};
  line_d.length = (unsigned short)strlen(line);
  line_d.pointer = line;
  if (line[line_d.length-1]=='\n') line_d.length--;
  TdiExecute(&write_it,&line_d,&ans MDS_END_ARG);
}
