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
/*      Tdi3Char.C
        All the elemental character routines.

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/
#include <string.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdinelements.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <STATICdef.h>



extern int TdiHash();

STATIC_CONSTANT char false = 0;
STATIC_CONSTANT struct descriptor false_dsc = { 1, DTYPE_T, CLASS_S, (char *)&false };

/*------------------------------------------------------------------------------
        F8X elemental, adjust string left, if there are blanks at left, and add blanks at right.
                string = ADJUSTL(string)
*/
int Tdi3Adjustl(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *ps1 = in_ptr->pointer, *pe1 = ps1;
  char *ps2 = out_ptr->pointer, *pe2 = ps2;
  int step = out_ptr->length, n;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    for (; --n >= 0;) {
      pe1 += step;
      pe2 += step;
      while (ps1 < pe1 && (*ps1 == ' ' || *ps1 == '\t'))
	ps1++;
      while (ps1 < pe1)
	*ps2++ = *ps1++;
      while (ps2 < pe2)
	*ps2++ = ' ';
    }
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, adjust string right, if there are blanks at right and add blanks at left.
                string = ADJUSTR(string)
*/
int Tdi3Adjustr(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *ps1 = in_ptr->pointer, *pe1;
  char *ps2 = out_ptr->pointer, *pe2;
  int step = out_ptr->length, n, last;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    for (last = n * step - 1, pe1 = ps1 += last, pe2 = ps2 += last; --n >= 0;) {
      ps1 -= step;
      ps2 -= step;
      while (ps1 < pe1 && (*pe1 == ' ' || *pe1 == '\t'))
	pe1--;
      while (ps1 < pe1)
	*pe2-- = *pe1--;
      while (ps2 < pe2)
	*pe2-- = ' ';
    }
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, converts integer into ASCII (assumed) text byte.
                text = CHAR(integer)
*/
int Tdi3Char(struct descriptor *in_ptr, struct descriptor *kind_ptr __attribute__ ((unused)),
	     struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *p1 = in_ptr->pointer;
  char *p2 = out_ptr->pointer;
  int step = in_ptr->length, n;
#ifdef WORDS_BIGENDIAN
  p1 += step - 1;
#endif
  N_ELEMENTS(out_ptr, n);
  for (; --n >= 0; p1 += step)
    *p2++ = *(p1);
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, concatenate text strings pairwise.
                string = CONCAT(string, string ...)
                string = string // string ...
*/
int Tdi3Concat(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  char *p1 = in1_ptr->pointer, *pt1;
  char *p2 = in2_ptr->pointer, *pt2;
  char *p3 = out_ptr->pointer;
  int size1 = in1_ptr->length, step1 = (in1_ptr->class == CLASS_A ? size1 : 0), j1;
  int size2 = in2_ptr->length, step2 = (in2_ptr->class == CLASS_A ? size2 : 0), j2;
  int status = MDSplusSUCCESS, n;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    for (; --n >= 0; p1 += step1, p2 += step2) {
      for (j1 = size1, pt1 = p1; --j1 >= 0;)
	*p3++ = *pt1++;
      for (j2 = size2, pt2 = p2; --j2 >= 0;)
	*p3++ = *pt2++;
    }
  return status;
}

/*------------------------------------------------------------------------------
        DCL elemental, extract a delimited text string, DCL form.
                ELEMENT(number, delimiter, string)
        Limitation: generally only good for scalars if we were to trim.
        Need to trim the result.
*/
int Tdi3Element(struct descriptor *number_ptr,
		struct descriptor *delim_ptr,
		struct descriptor *source_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *pnumber = number_ptr->pointer;
  int number, n;
  struct descriptor source_dsc = *source_ptr;
  struct descriptor delim_dsc = *delim_ptr;
  struct descriptor out_dsc = *out_ptr;
  int number_step = (number_ptr->class == CLASS_A) ? number_ptr->length : 0;
  int delim_step = (delim_ptr->class == CLASS_A) ? delim_ptr->length : 0;
  int source_step = (source_dsc.class == CLASS_A) ? source_dsc.length : 0;

  N_ELEMENTS(out_ptr, n);
  source_dsc.class = CLASS_S;
  delim_dsc.class = CLASS_S;
  out_dsc.class = CLASS_S;
  for (; STATUS_OK && --n >= 0;) {
    STATIC_CONSTANT unsigned short zero = 0;
    number = *(int *)pnumber, pnumber += number_step;
    status = StrElement(&out_dsc, &number, &delim_dsc, &source_dsc);
    if STATUS_NOT_OK
      status = StrCopyR(&out_dsc, &zero, "");
    source_dsc.pointer += source_step;
    delim_dsc.pointer += delim_step;
    out_dsc.pointer += out_dsc.length;
  }
  return status;
}

/*------------------------------------------------------------------------------
        DCL elemental, extract a text string, DCL form.
                EXTRACT(start, length, string)
        Limitation: length must be a scalar, I think.
*/
int Tdi3Extract(struct descriptor *start_ptr,
		struct descriptor *length_ptr,
		struct descriptor *source_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *pstart = start_ptr->pointer, *plength = length_ptr->pointer;
  int start, length, n;
  struct descriptor source_dsc = *source_ptr;
  struct descriptor out_dsc = *out_ptr;
  int start_step = (start_ptr->class == CLASS_A) ? start_ptr->length : 0;
  int length_step = (length_ptr->class == CLASS_A) ? length_ptr->length : 0;
  int source_step = (source_dsc.class == CLASS_A) ? source_dsc.length : 0;

  N_ELEMENTS(out_ptr, n);
  source_dsc.class = CLASS_S;
  out_dsc.class = CLASS_S;
  for (; STATUS_OK && --n >= 0;) {
    start = *(int *)pstart + 1, pstart += start_step;
    length = *(int *)plength, plength += length_step;
    status = StrLenExtr(&out_dsc, &source_dsc, &start, &length);
    source_dsc.pointer += source_step;
    out_dsc.pointer += out_dsc.length;
  }
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, converts ASCII (assumed) text into integer byte.
                byte = IACHAR(ascii-text)
                byte = ICHAR(text)
*/
int Tdi3Ichar(struct descriptor *in_ptr, struct descriptor *out_ptr){
  return Tdi3Char(in_ptr,NULL,out_ptr);
}

/*------------------------------------------------------------------------------
        Modified F8X elemental, scan a string for first substring, gives offset.
                string = INDEX(char-string, char-set, [back])
        Examples:
                INDEX("FORTRAN", "R") is 2              offset of first R
                INDEX("FORTRAN", "R", $TRUE) is 5       offset from left of first R, left to right
*/
int Tdi3Index(struct descriptor *str_ptr,
	      struct descriptor *sub_ptr, struct descriptor *bac_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  struct descriptor str_dsc = *str_ptr;
  struct descriptor sub_dsc = *sub_ptr;
  struct descriptor bac_dsc = bac_ptr ? *bac_ptr : false_dsc;
  int str_step = (str_dsc.class == CLASS_A) ? str_dsc.length : 0;
  int sub_step = (sub_dsc.class == CLASS_A) ? sub_dsc.length : 0;
  int bac_step = (bac_dsc.class == CLASS_A) ? bac_dsc.length : 0;
  int n, j;
  int *out_point = (int *)out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  str_dsc.class = CLASS_S;
  sub_dsc.class = CLASS_S;
  while (--n >= 0) {
    if (*bac_dsc.pointer & 1) {
      int m, limit = str_dsc.length - sub_dsc.length;
      for (m = 0; j = m - 1, m++ <= limit;)
	if ((m = StrPosition(&str_dsc, &sub_dsc, &m)) <= 0)
	  break;
    } else
      j = StrPosition(&str_dsc, &sub_dsc, 0) - 1;
    *out_point++ = j;
    str_dsc.pointer += str_step;
    sub_dsc.pointer += sub_step;
    bac_dsc.pointer += bac_step;
  }
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, find length of string with trailing blanks and tabs removed.
        Example: LEN_TRIM([" A B "," "]) is [4,0]
*/
int Tdi3LenTrim(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *p1 = in_ptr->pointer, *pe1;
  int *p2 = (int *)out_ptr->pointer;
  int step = in_ptr->length, n;

  N_ELEMENTS(out_ptr, n);
  for (; --n >= 0; p1 += step) {
    pe1 = p1 + step;
    for (; p1 <= --pe1;)
      if (*pe1 != ' ' && *pe1 != '\t')
	break;
    *p2++ = pe1 - p1 + 1;
  }
  return status;
}

/*------------------------------------------------------------------------------
        F8X elemental, repeat a text string ncopies times.
                REPEAT(string, ncopies)
        Limitation: ncopies must be a scalar.
*/
int Tdi3Repeat(struct descriptor *in1_ptr, struct descriptor *in2_ptr __attribute__ ((unused)),
	       struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n, j, size = in1_ptr->length, ncopies = (int)out_ptr->length / size;
  char *p1 = in1_ptr->pointer, *p3 = out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  for (; --n >= 0; p1 += size)
    for (j = ncopies; --j >= 0; p3 += size)
      memcpy(p3, p1, size);
  return status;
}

/*------------------------------------------------------------------------------
        Modified F8X elemental, scan a string for a character in a set of characters, gives offset.
                string = SCAN(char-string, char-set, [back])
        Examples:
                SCAN("FORTRAN", "TR") is 2              offset of first R
                SCAN("FORTRAN", "TR", $TRUE) is 4       offset from left of first R, left to right
*/
int Tdi3Scan(struct descriptor *str_ptr,
	     struct descriptor *set_ptr, struct descriptor *bac_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  struct descriptor str_dsc = *str_ptr;
  struct descriptor set_dsc = *set_ptr;
  struct descriptor bac_dsc = bac_ptr ? *bac_ptr : false_dsc;
  struct descriptor tmp_dsc = false_dsc;
  int str_len = str_dsc.length;
  int str_step = (str_dsc.class == CLASS_A) ? str_dsc.length : 0;
  int set_step = (set_dsc.class == CLASS_A) ? set_dsc.length : 0;
  int bac_step = (bac_dsc.class == CLASS_A) ? bac_dsc.length : 0;
  int n, j;
  int *out_point = (int *)out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  str_dsc.class = CLASS_S;
  set_dsc.class = CLASS_S;
  while (--n >= 0) {
    if (*bac_dsc.pointer & 1) {
			/********************************************************************
                        FIND_LAST_IN_SET by checking INDEX(set, char) from right to left.
                        ********************************************************************/
      tmp_dsc.pointer = str_dsc.pointer + str_len;
      while (--tmp_dsc.pointer >= str_dsc.pointer)
	if (StrPosition(&set_dsc, &tmp_dsc, 0) != 0)
	  break;
      j = tmp_dsc.pointer - str_dsc.pointer;
    } else
      j = StrFindFirstInSet(&str_dsc, &set_dsc) - 1;
    *out_point++ = j;
    str_dsc.pointer += str_step;
    set_dsc.pointer += set_step;
    bac_dsc.pointer += bac_step;
  }
  return status;
}

/*------------------------------------------------------------------------------
        Elemental function to convert a string or builtin name to its opcode.
        Gives -1 if not found.
*/
int Tdi3StringOpcode(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int len, n;
  unsigned short step = in_ptr->length;
  char *str_ptr;
  short *code_ptr = (short *)out_ptr->pointer;
  struct descriptor_d tmp_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  struct descriptor_d one_dsc = { 0, DTYPE_T, CLASS_D, 0 };
  tmp_dsc.length = step;
  tmp_dsc.pointer = in_ptr->pointer;
  N_ELEMENTS(out_ptr, n);
  for (; --n >= 0; tmp_dsc.pointer += step) {
		/****************************************************
                Case insensitive and ignore trailing blanks and tabs.
                ****************************************************/
    status = StrUpcase((struct descriptor *)&one_dsc, (struct descriptor *)&tmp_dsc);
    if STATUS_OK
      status = StrTrim((struct descriptor *)&one_dsc, (struct descriptor *)&one_dsc, 0);
    if STATUS_NOT_OK
      break;
		/******************************
                Ignore leading Opc designator.
                ******************************/
    len = one_dsc.length;
    str_ptr = one_dsc.pointer;
    if (len > 4 && strncmp(str_ptr, "Opc", 4) == 0) {
      str_ptr += 4;
      len -= 4;
    }
    *code_ptr++ = (short)TdiHash(len, str_ptr);
  }
  StrFree1Dx(&one_dsc);
  return status;
}

/*------------------------------------------------------------------------------
        Translate character of a string.
                string = TRANSLATE(string, translation, match)
*/
int Tdi3Translate(struct descriptor *str_ptr,
		  struct descriptor *tra_ptr,
		  struct descriptor *mat_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  status = StrTranslate(out_ptr, str_ptr, tra_ptr, mat_ptr);
  return status;
}

/*------------------------------------------------------------------------------
        Convert a string to uppercase.
                string = UPCASE(string)
*/
int Tdi3Upcase(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  status = StrUpcase(out_ptr, in_ptr);
  return status;
}

/*------------------------------------------------------------------------------
        Modified F8X elemental, verifies that a set of characters contains all the characters in a string, gives offset.
                string = VERIFY(char-string, char-set, [back])
        Examples:
                VERIFY("ABBA", "AB") is -1      all are in set
                VERIFY("ABBA", "A") is 1        offset from left of first non-A
                VERIFY("ABBA", "A", $TRUE) is 2 offset from left of last non-A
*/
int Tdi3Verify(struct descriptor *str_ptr,
	       struct descriptor *set_ptr, struct descriptor *bac_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  struct descriptor str_dsc = *str_ptr;
  struct descriptor set_dsc = *set_ptr;
  struct descriptor bac_dsc = bac_ptr ? *bac_ptr : false_dsc;
  struct descriptor tmp_dsc = false_dsc;
  int str_len = str_dsc.length;
  int step_str = (str_dsc.class == CLASS_A) ? str_dsc.length : 0;
  int step_set = (set_dsc.class == CLASS_A) ? set_dsc.length : 0;
  int step_bac = (bac_dsc.class == CLASS_A) ? bac_dsc.length : 0;
  int n, j;
  int *out_point = (int *)out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  str_dsc.class = CLASS_S;
  set_dsc.class = CLASS_S;
  while (--n >= 0) {
    if (*bac_dsc.pointer & 1) {
			/********************************************************************
                        FIND_LAST_NOT_IN_SET by checking INDEX(set, char) from right to left.
                        ********************************************************************/
      tmp_dsc.pointer = str_dsc.pointer + str_len;
      while (--tmp_dsc.pointer >= str_dsc.pointer)
	if (StrPosition(&set_dsc, &tmp_dsc, 0) == 0)
	  break;
      j = tmp_dsc.pointer - str_dsc.pointer;
    } else
      j = StrFindFirstNotInSet(&str_dsc, &set_dsc) - 1;
    *out_point++ = j;
    str_dsc.pointer += step_str;
    set_dsc.pointer += step_set;
    bac_dsc.pointer += step_bac;
  }
  return status;
}
