/*  CMS REPLACEMENT HISTORY, Element TDITEST.C */
/*  *29   26-AUG-1996 13:56:23 TWF "" */
/*  *28   26-AUG-1996 13:55:30 TWF "No immediate" */
/*  *27   26-AUG-1996 13:50:25 TWF "Do not do immediate evaluation" */
/*  *26   31-JUL-1996 15:40:12 TWF "Clear out the message buffer" */
/*  *25   30-JUL-1996 11:47:42 TWF "Use TDI$WRITE for all output" */
/*  *24   30-JUL-1996 11:44:32 TWF "Use TDI$WRITE for all output" */
/*  *23   30-JUL-1996 10:34:09 TWF "Use TDI$WRITE for all output" */
/*  *22   30-JUL-1996 10:30:52 TWF "Use TDI$WRITE for all output" */
/*  *21   10-JUL-1996 14:23:44 TWF "Put unit in variable" */
/*  *20   10-JUL-1996 08:51:16 TWF "Format output of multi line" */
/*  *19   10-JUL-1996 08:47:00 TWF "Add multi line capability" */
/*  *18   10-JUL-1996 08:46:08 TWF "Add multi line capability" */
/*  *17    9-JUL-1996 17:19:14 TWF "error output" */
/*  *16    9-JUL-1996 17:11:09 TWF "Add error output" */
/*  *15    8-JUL-1996 15:59:54 TWF "Use decompile of immediate" */
/*  *14    8-JUL-1996 11:16:05 TWF "Write to same out" */
/*  *13    8-JUL-1996 11:11:33 TWF "put back lf" */
/*  *12    8-JUL-1996 11:08:19 TWF "Remove lf" */
/*  *11    8-JUL-1996 11:06:50 TWF "Remove lf" */
/*  *10    8-JUL-1996 11:03:57 TWF "" */
/*  *9     8-JUL-1996 11:00:01 TWF "Format output" */
/*  *8     8-JUL-1996 10:57:16 TWF "Format output" */
/*  *7     8-JUL-1996 10:05:44 TWF "Allow input file on command line" */
/*  *6     8-JUL-1996 10:01:33 TWF "Allow input file on command line" */
/*  *5    26-JUN-1996 09:58:54 TWF "Port to Unix/Windows" */
/*  *4    20-JUN-1996 15:29:04 JAS "" */
/*  *3    20-JUN-1996 15:26:45 JAS "" */
/*  *2    20-JUN-1996 15:22:21 JAS "echo the input" */
/*  *1    20-JUN-1996 13:04:20 JAS "test program for tdishr" */
/*  CMS REPLACEMENT HISTORY, Element TDITEST.C */
#include <string.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#define MAXEXPR 16384
extern int TdiExecute();

static void tdiputs(char *line);

int main(int argc, char **argv)
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
	  in = fopen(argv[1],"r");
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
