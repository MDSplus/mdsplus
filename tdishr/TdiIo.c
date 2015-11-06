/*      Tdi1IO.C
        The input/output interfaces.
        Unwritten: BACKSPACE ENDFILE READ

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/
#ifdef __VMS
#include <dvidef.h>
#endif

#include <STATICdef.h>
#include <stdlib.h>
#include <stdio.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <mdsshr.h>
#include <string.h>



STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };

STATIC_CONSTANT DESCRIPTOR(dNUL, "\0");
STATIC_CONSTANT DESCRIPTOR(dBAD, "/*bad*/");

/*----------------------------------------------
        A kludge to get stdout.
        In a shared library, cannot match extern stdout, etc.
*/
#define kprintf(unit, ctrl) (unit ? fprintf(unit, ctrl) : printf(ctrl));
#define kprintf2(unit, ctrl, a1, a2) (unit ? fprintf(unit, ctrl, a1, a2) : printf(ctrl, a1, a2));

extern int TdiData();
extern int TdiGetLong();
extern int TdiGetFloat();
extern int TdiText();
extern int TdiEvaluate();
extern int TdiDecompile();

/*----------------------------------------------
        Internal routine to output a long.
*/
int TdiPutLong(int *data, struct descriptor_xd *out_ptr)
{
  int status;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_L;
  STATIC_CONSTANT unsigned short len = sizeof(int);

  if (out_ptr == 0)
    return 1;
  status = MdsGet1DxS(&len, &dtype, out_ptr);
  if (status & 1)
    *(int *)out_ptr->pointer->pointer = *data;
  return status;
}

/*----------------------------------------------
        Internal routine to output a unit
*/
STATIC_ROUTINE int TdiPutUnit(FILE * unit, struct descriptor_xd *out_ptr)
{
  struct descriptor unit_d = { 0, DTYPE_T, CLASS_S, 0 };
  unit_d.length = unit != 0 ? sizeof(unit) : 0;
  unit_d.pointer = unit != 0 ? (char *)&unit : 0;
  return MdsCopyDxXd(&unit_d, out_ptr);
}

/*----------------------------------------------
        Internal routine to input a unit
*/
STATIC_ROUTINE int TdiGetOutUnit(struct descriptor *in_ptr, FILE ** unit)
{
  int status;
  struct descriptor_d unit_d = { 0, DTYPE_T, CLASS_D, 0 };
  status = TdiEvaluate(in_ptr, &unit_d MDS_END_ARG);
  if (unit_d.length != sizeof(*unit))
    *unit = stdout;
  else
    *unit = *(FILE **) unit_d.pointer;
  StrFree1Dx(&unit_d);
  return status;
}

/*----------------------------------------------
        Internal routine to input a unit
*/
STATIC_ROUTINE int TdiGetInUnit(struct descriptor *in_ptr, FILE ** unit)
{
  int status;
  struct descriptor_d unit_d = { 0, DTYPE_T, CLASS_D, 0 };
  status = TdiEvaluate(in_ptr, &unit_d MDS_END_ARG);
  if (unit_d.length != sizeof(*unit))
    *unit = stdin;
  else
    *unit = *(FILE **) unit_d.pointer;
  StrFree1Dx(&unit_d);
  return status;
}

/*----------------------------------------------
        System data and time as text string.
                string = DATE_TIME([quadword time])
*/
int Tdi1DateTime(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int time[2] = { 0, 0 }, *ptime;
  unsigned short len;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_T;
  STATIC_CONSTANT unsigned short length = 23;

  if (narg > 0 && list[0]) {
    struct descriptor dtime = { sizeof(time), DTYPE_Q, CLASS_S, 0 };
    dtime.pointer = (char *)time;
    status = TdiData(list[0], &dtime MDS_END_ARG);
    ptime = time;
  } else
    ptime = 0;
  if (status & 1)
    status = MdsGet1DxS(&length, &dtype, out_ptr);
  if (status & 1)
    status = LibSysAscTim(&len, out_ptr->pointer, ptime);
  if (status & 1)
    out_ptr->pointer->length = len;
  return status;
}

/*----------------------------------------------
        Close a C file unit. (0=OK, EOF=bad)
*/
int Tdi1Fclose(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  FILE *unit;
  int err;

  TdiGetOutUnit(list[0], &unit);
  err = fclose(unit);
  return TdiPutLong((int *)&err, out_ptr);
}

/*----------------------------------------------
        Specify position of file pointer.
        err = FSEEK(unit, offset, origin)
        where offset is in bytes (to record for rec files.)
        and origin: 0=absolute 1=relative 2=relative to end.
*/
int Tdi1Fseek(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  FILE *unit;
  int offset = 0, origin = 0, err;

  status = TdiGetOutUnit(list[0], &unit);
  if (status & 1 && narg > 1)
    status = TdiGetLong(list[1], &offset);
  if (status & 1 && narg > 2)
    status = TdiGetLong(list[2], &origin);
  if (status & 1) {
    err = fseek(unit, offset, origin);
    status = TdiPutLong((int *)&err, out_ptr);
  }
  return status;
}

/*----------------------------------------------
        Find absolute position of file pointer.
        offset = FTELL(unit)
*/
int Tdi1Ftell(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  FILE *unit;
  int pos;

  TdiGetOutUnit(list[0], &unit);
  pos = ftell(unit);
  status = TdiPutLong((int *)&pos, out_ptr);
  return status;
}

/*----------------------------------------------
        Open a C file unit. (0=fail, else=file block pointer)
                unit = FOPEN(name, mode)
        Mode is a lowercase string and may include
        r=read w=write a=append r+/w+/a+=update b=binary
*/
int Tdi1Fopen(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  FILE *unit;
  struct descriptor_d dname = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_d dmode = { 0, DTYPE_T, CLASS_D, 0 };

  status = TdiData(list[0], &dname MDS_END_ARG);
  if (status & 1)
    status = TdiData(list[1], &dmode MDS_END_ARG);
  if (status & 1)
    status = StrAppend(&dname, (struct descriptor *)&dNUL);
  if (status & 1)
    status = StrAppend(&dmode, (struct descriptor *)&dNUL);
  if (status & 1) {
    unit = fopen(dname.pointer, dmode.pointer);
    status = TdiPutUnit(unit, out_ptr);
  }
  StrFree1Dx(&dname);
  StrFree1Dx(&dmode);
  return status;
}

/*----------------------------------------------
        Spawn a subprocess.
                status = SPAWN([command_string],[input_file],[output_file])
*/
int Tdi1Spawn(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int stat1;
  struct descriptor_d cmd = EMPTY_D;
  struct descriptor_d inp = EMPTY_D;
  struct descriptor_d out = EMPTY_D;

  if (narg > 0 && list[0])
    status = TdiText(list[0], &cmd MDS_END_ARG);
  if (narg > 1 && list[1] && status & 1)
    status = TdiText(list[1], &inp MDS_END_ARG);
  if (narg > 2 && list[2] && status & 1)
    status = TdiText(list[2], &out MDS_END_ARG);
  if (status & 1) {
    stat1 = LibSpawn((struct descriptor *)&cmd, 1, 0);
    status = TdiPutLong(&stat1, out_ptr);
  }
  return status;
}

/*----------------------------------------------
        WAIT a given number of seconds.
                float = WAIT(wait-time)
*/
int Tdi1Wait(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  float time;

  status = TdiGetFloat(list[0], &time);
  if (status & 1)
    status = LibWait(&time);
  return status;
}

/*----------------------------------------------
        WRITE string to terminal or file unit.
                bytes = WRITE(unit,item,...)
        Unit * is standard output.

        Each null like WRITE(*,) sends a newline.
        Scalars are placed without spaces on a row until full.
        A long scalar at the left margin does not linefeed before.
        Arrays of numeric or text are printed on a new row, as fits.
        Other types (including signals and with units)
        are decompiled and start and end with a new row.
*/
int Tdi1Write(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int j, stat1, bytes = 0, col = 0, len;
  FILE *unit = 0;
  struct descriptor *pd, *ptmp;
  char *pt, *plim;
  EMPTYXD(tmp);

  STATIC_CONSTANT int width = 132;

  status = TdiGetOutUnit(list[0], &unit);
  if (status & 1)
    for (j = 1; j < narg; ++j) {
      stat1 = TdiEvaluate(list[j], &tmp MDS_END_ARG);
      if (!(stat1 & 1))
	pd = (struct descriptor *)&dBAD;
      else if ((ptmp = tmp.pointer) == 0)
	pd = (struct descriptor *)&dNUL;
      else
	switch (ptmp->dtype) {
	case DTYPE_BU:
	case DTYPE_B:
	case DTYPE_WU:
	case DTYPE_W:
	case DTYPE_LU:
	case DTYPE_L:
	case DTYPE_QU:
	case DTYPE_Q:
	case DTYPE_OU:
	case DTYPE_O:
	case DTYPE_FC:
	case DTYPE_F:
	case DTYPE_DC:
	case DTYPE_D:
	case DTYPE_GC:
	case DTYPE_G:
	case DTYPE_HC:
	case DTYPE_H:
	case DTYPE_FSC:
	case DTYPE_FS:
	case DTYPE_FTC:
	case DTYPE_FT:
	  stat1 = TdiText(&tmp, &tmp MDS_END_ARG);
	  if (!(stat1 & 1)) {
	    pd = (struct descriptor *)&dBAD;
	    break;
	  }
	case DTYPE_T:
	  ptmp = tmp.pointer;
	  len = ptmp->length;
	  pt = ptmp->pointer;
	  plim = pt + len;
	  switch (ptmp->class) {
	  case CLASS_S:
	  case CLASS_D:
	    if (col > 0 && col + len > width)
	      col = 0, bytes += kprintf(unit, "\n");
	    col += len;
	    bytes += kprintf2(unit, "%.*s", len, pt);
	    continue;
	  case CLASS_A:
	    plim = pt + ((struct descriptor_a *)ptmp)->arsize;
	    if (col > 0)
	      col = 0, bytes += kprintf(unit, "\n");
	    for (; pt < plim; pt += len) {
	      if (col > 0 && col + len > width)
		col = 0, bytes += kprintf(unit, "\n");
	      col += len;
	      bytes += kprintf2(unit, "%.*s", len, pt);
	    }
	    col = 0, bytes += kprintf(unit, "\n");
	    continue;
	  default:
	    goto none;
	  }
	default:
 none:	  stat1 = TdiDecompile(&tmp, &tmp MDS_END_ARG);
	  pd = (stat1 & 1) ? tmp.pointer : (struct descriptor *)&dBAD;
	  if (col > 0)
	    col = 0, bytes += kprintf(unit, "\n");
	  break;
	}
      bytes += kprintf2(unit, "%.*s\n", pd->length, pd->pointer);
    }
  if (col > 0)
    bytes += kprintf(unit, "\n");
  if (status & 1)
    status = TdiPutLong((int *)&bytes, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*----------------------------------------------
        Read string from terminal or file unit.
                string = READ(unit)
        Unit * is standard input.

*/
int Tdi1Read(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int bytes = 0;
  FILE *unit = 0;
  status = TdiGetInUnit(list[0], &unit);
  if (status & 1) {
    char line[4096];
    char *ans;
    struct descriptor line_d = { 0, DTYPE_T, CLASS_S, 0 };
    ans = fgets(line, sizeof(line), unit);
    if (ans) {
      line_d.length = strlen(line) - 1;
      line_d.pointer = line;
      status = MdsCopyDxXd(&line_d, out_ptr);
    } else
      status = 0;
  }
  return status;
}
