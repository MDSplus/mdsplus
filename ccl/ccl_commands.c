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
#include        "clisysdef.h"

		/* Filename: ccl_commands.c
		 * created by cdu :  02-Jun-2014 15:59:12
		 ********************************************************/

extern int ccl_finish();

static struct cduValue v01p01value = {
  0x0020, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v01params[2] = {
  "P1", 0x1000, 0, 0, &v01p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v01q01value = {
  0x1000, "CCL_COMMANDS", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q02value = {
  0x0000, "CCLHELP", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q03value = {
  0x0000, "MDSDCL", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q04value = {
  0x0000, "CCL>", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q05value = {
  0x0000, "*.CCL", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q06value = {
  0x0000, "CCLKEYS", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q07value = {
  0x0000, "CCL$LIBRARY", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q08value = {
  0x0000, "CCLINIT", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q09value = {
  0x0000, "SYS$LOGIN:*.CCL", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v01q10value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v01qualifiers[13] = {
  "TABLES", 0x2001, 0, 0, &v01q01value, 0, "HELPLIB", 0x2001, 0, 0, &v01q02value, 0, "AUXHELPLIB", 0x2001, 0, 0, &v01q03value, 0, "PROMPT", 0x2001, 0, 0, &v01q04value, 0, "DEF_FILE", 0x2001, 0, 0, &v01q05value, 0, "KEY_DEFS", 0x2001, 0, 0, &v01q06value, 0, "DEF_LIBRARIES", 0x2001, 0, 0, &v01q07value, 0, "COMMAND", 0x2001, 0, 0, &v01q08value, 0, "INI_DEF_FILE", 0x2001, 0, 0, &v01q09value, 0, "LIBRARY", 0x2000, 0, 0, &v01q10value, 0, "DEBUG", 0x2000, 0, 0, 0, 0, "CLOSE", 0x2000, 0, 0, 0, 0, 0	/* null entry at end    */
};

extern int ccl_pio();

static struct cduValue v03p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v03params[2] = {
  "P1", 0x1000, "MODULE", 0, &v03p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v03q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v03q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v03q03value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v03q04value = {
  0x0000, "1", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v03q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v03qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v03q01value, 0, "FUNCTION", 0x2005, 0, 0, &v03q02value, 0, "DATA", 0x2004, 0, 0, &v03q03value, 0, "COUNT", 0x2005, 0, 0, &v03q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v03q09value, 0, 0	/* null entry at end    */
};

extern int ccl_qrep();

static struct cduValue v04p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v04params[2] = {
  "P1", 0x1000, "MODULE", 0, &v04p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v04q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v04q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v04q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v04q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v04q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v04qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v04q01value, 0, "FUNCTION", 0x2005, 0, 0, &v04q02value, 0, "COUNT", 0x2004, 0, 0, &v04q03value, 0, "DATA", 0x2004, 0, 0, &v04q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v04q09value, 0, 0	/* null entry at end    */
};

extern int ccl_fqrep();

static struct cduValue v05p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v05params[2] = {
  "P1", 0x1000, "MODULE", 0, &v05p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v05q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v05q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v05q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v05q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v05q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v05qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v05q01value, 0, "FUNCTION", 0x2005, 0, 0, &v05q02value, 0, "COUNT", 0x2004, 0, 0, &v05q03value, 0, "DATA", 0x2004, 0, 0, &v05q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v05q09value, 0, 0	/* null entry at end    */
};

extern int ccl_qscan();

static struct cduValue v06p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v06params[2] = {
  "P1", 0x1000, "MODULE", 0, &v06p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v06q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v06q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v06q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v06q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v06q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v06qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v06q01value, 0, "FUNCTION", 0x2005, 0, 0, &v06q02value, 0, "COUNT", 0x2004, 0, 0, &v06q03value, 0, "DATA", 0x2004, 0, 0, &v06q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v06q09value, 0, 0	/* null entry at end    */
};

extern int ccl_qstop();

static struct cduValue v07p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v07params[2] = {
  "P1", 0x1000, "MODULE", 0, &v07p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v07q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v07q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v07q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v07q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v07q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v07qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v07q01value, 0, "FUNCTION", 0x2005, 0, 0, &v07q02value, 0, "COUNT", 0x2004, 0, 0, &v07q03value, 0, "DATA", 0x2004, 0, 0, &v07q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v07q09value, 0, 0	/* null entry at end    */
};

extern int ccl_fqstop();

static struct cduValue v08p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v08params[2] = {
  "P1", 0x1000, "MODULE", 0, &v08p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v08q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v08q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v08q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v08q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v08q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v08qualifiers[10] = {
  "ADDRESS", 0x2005, 0, 0, &v08q01value, 0, "FUNCTION", 0x2005, 0, 0, &v08q02value, 0, "COUNT", 0x2004, 0, 0, &v08q03value, 0, "DATA", 0x2004, 0, 0, &v08q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v08q09value, 0, 0	/* null entry at end    */
};

extern int ccl_stop();

static struct cduValue v09p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v09params[2] = {
  "P1", 0x1000, "MODULE", 0, &v09p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v09q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v09q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v09q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v09q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v09q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v09qualifiers[11] = {
  "ADDRESS", 0x2005, 0, 0, &v09q01value, 0, "FUNCTION", 0x2005, 0, 0, &v09q02value, 0, "COUNT", 0x2004, 0, 0, &v09q03value, 0, "DATA", 0x2004, 0, 0, &v09q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v09q09value, 0, "ENHANCED", 0x2000, 0, 0, 0, 0, 0	/* null entry at end    */
};

extern int ccl_fstop();

static struct cduValue v10p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v10params[2] = {
  "P1", 0x1000, "MODULE", 0, &v10p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v10q01value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v10q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v10q03value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v10q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v10q09value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v10qualifiers[11] = {
  "ADDRESS", 0x2005, 0, 0, &v10q01value, 0, "FUNCTION", 0x2005, 0, 0, &v10q02value, 0, "COUNT", 0x2004, 0, 0, &v10q03value, 0, "DATA", 0x2004, 0, 0, &v10q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, &v10q09value, 0, "ENHANCED", 0x2001, 0, 0, 0, 0, 0	/* null entry at end    */
};

extern int ccl_dclst();
extern int ccl_lpio();

static struct cduValue v12p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v12params[2] = {
  "P1", 0x1000, "MODULE", 0, &v12p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v12q01value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v12q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v12q03value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v12q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v12q10value = {
  0x0000, "24", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v12qualifiers[11] = {
  "STEP", 0x2004, 0, 0, &v12q01value, 0, "ADDRESS", 0x2005, 0, 0, &v12q02value, 0, "FUNCTION", 0x2005, 0, 0, &v12q03value, 0, "DATA", 0x2004, 0, 0, &v12q04value, 0, "COUNT", 0x2005, 0, 0, 0, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2005, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2005, 0, 0, &v12q10value, 0, 0	/* null entry at end    */
};

extern int ccl_lqscan();

static struct cduValue v13p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v13params[2] = {
  "P1", 0x1000, "MODULE", 0, &v13p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v13q01value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v13q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v13q03value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v13q04value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v13q05value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v13q10value = {
  0x0000, "24", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v13qualifiers[11] = {
  "STEP", 0x2004, 0, 0, &v13q01value, 0, "ADDRESS", 0x2005, 0, 0, &v13q02value, 0, "FUNCTION", 0x2005, 0, 0, &v13q03value, 0, "COUNT", 0x2004, 0, 0, &v13q04value, 0, "DATA", 0x2004, 0, 0, &v13q05value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2005, 0, 0, &v13q10value, 0, 0	/* null entry at end    */
};

extern int ccl_lstop();

static struct cduValue v14p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v14params[2] = {
  "P1", 0x1000, "MODULE", 0, &v14p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v14q01value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v14q02value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v14q03value = {
  0x0000, "0", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v14q04value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v14q05value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v14q10value = {
  0x0000, "24", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v14qualifiers[11] = {
  "STEP", 0x2004, 0, 0, &v14q01value, 0, "ADDRESS", 0x2005, 0, 0, &v14q02value, 0, "FUNCTION", 0x2005, 0, 0, &v14q03value, 0, "COUNT", 0x2004, 0, 0, &v14q04value, 0, "DATA", 0x2004, 0, 0, &v14q05value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "MEMORY", 0x2005, 0, 0, &v14q10value, 0, 0	/* null entry at end    */
};

extern int ccl_execute();
extern int ccl_rdata();

static struct cduValue v16q01value = {
  0x0000, "1", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v16q02value = {
  0x0000, "16383", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v16q03value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v16qualifiers[5] = {
  "STEP", 0x2005, 0, 0, &v16q01value, 0, "COUNT", 0x2005, 0, 0, &v16q02value, 0, "MEMORY", 0x2004, 0, 0, &v16q03value, 0, "DATA", 0x2000, 0, 0, 0, 0, 0	/* null entry at end    */
};

extern int ccl_wdata();

static struct cduValue v17q01value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v17q02value = {
  0x0000, "16383", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v17q03value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v17q04value = {
  0x3000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v17qualifiers[9] = {
  "STEP", 0x2004, 0, 0, &v17q01value, 0, "COUNT", 0x2005, 0, 0, &v17q02value, 0, "MEMORY", 0x2004, 0, 0, &v17q03value, 0, "DATA", 0x2004, 0, 0, &v17q04value, 0, "HEX", 0x2004, 0, 0, 0, 0, "BINARY", 0x2004, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, 0	/* null entry at end    */
};

extern int ccl_lamwait();

static struct cduValue v18p01value = {
  0x0000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v18params[2] = {
  "P1", 0x1000, "MODULE", 0, &v18p01value, 0, 0	/* null entry at end    */
};

static struct cduValue v18q01value = {
  0x0000, "32767", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v18qualifiers[2] = {
  "TIMEOUT", 0x2005, 0, 0, &v18q01value, 0, 0	/* null entry at end    */
};

extern int ccl_set_xandq();
static struct cduKeyword SET_KEYWORDS[6];	/* Prototype only       */

static struct cduValue SET_XANDQ_p01value = {
  0x2001, 0, SET_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SET_XANDQ_params[2] = {
  "P1", 0x1000, 0, 0, &SET_XANDQ_p01value, 0, 0	/* null entry at end    */
};

static struct cduKeyword XANDQ_KEYWORDS[4] = {	/* "Type" def   */
  "ANY", 0x4000, 0, 0, 0, 0, "NO", 0x4000, 0, 0, 0, 0, "YES", 0x4000, 0, 0, 0, 0, 0	/* null entry at end    */
};

static struct cduValue SET_XANDQ_q01value = {
  0x0001, "ANY", XANDQ_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue SET_XANDQ_q02value = {
  0x0001, "ANY", XANDQ_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier SET_XANDQ_qualifiers[3] = {
  "X", 0x2001, 0, 0, &SET_XANDQ_q01value, 0, "Q", 0x2001, 0, 0, &SET_XANDQ_q02value, 0, 0	/* null entry at end    */
};

static struct cduVerb SET_XANDQ = {	/* Syntax def   */
  "SET_XANDQ", 0x0000, 0, 0, ccl_set_xandq, SET_XANDQ_params, SET_XANDQ_qualifiers
};

extern int ccl_set_memory();

static struct cduValue SET_MEMORY_p01value = {
  0x2001, 0, SET_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue SET_MEMORY_16_q01value = {
  0x0000, "16", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier SET_MEMORY_16_qualifiers[2] = {
  "MEMORY", 0x2005, 0, 0, &SET_MEMORY_16_q01value, 0, 0	/* null entry at end    */
};

static struct cduVerb SET_MEMORY_16 = {	/* Syntax def   */
  "SET_MEMORY_16", 0x0000, 0, 0, 0, 0, SET_MEMORY_16_qualifiers
};

static struct cduValue SET_MEMORY_24_q01value = {
  0x0000, "24", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier SET_MEMORY_24_qualifiers[2] = {
  "MEMORY", 0x2005, 0, 0, &SET_MEMORY_24_q01value, 0, 0	/* null entry at end    */
};

static struct cduVerb SET_MEMORY_24 = {	/* Syntax def   */
  "SET_MEMORY_24", 0x0000, 0, 0, 0, 0, SET_MEMORY_24_qualifiers
};

static struct cduKeyword MEMORY_KEYWORDS[3] = {	/* "Type" def   */
  "16", 0x4000, 0, &SET_MEMORY_16, 0, 0, "24", 0x4000, 0, &SET_MEMORY_24, 0, 0, 0	/* null entry at end    */
};

static struct cduValue SET_MEMORY_p02value = {
  0x0001, 0, MEMORY_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SET_MEMORY_params[3] = {
  "P1", 0x1000, 0, "What", &SET_MEMORY_p01value, 0, "P2", 0x1000, 0, 0, &SET_MEMORY_p02value, 0, 0	/* null entry at end    */
};

static struct cduVerb SET_MEMORY = {	/* Syntax def   */
  "SET_MEMORY", 0x0000, 0, 0, ccl_set_memory, SET_MEMORY_params, 0
};

extern int ccl_set_module();

static struct cduValue SET_MODULE_p01value = {
  0x2001, 0, SET_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue SET_MODULE_p02value = {
  0x2000, 0, 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SET_MODULE_params[3] = {
  "P1", 0x1000, 0, 0, &SET_MODULE_p01value, 0, "P2", 0x1000, "NAME", "Module", &SET_MODULE_p02value, 0, 0	/* null entry at end    */
};

static struct cduVerb SET_MODULE = {	/* Syntax def   */
  "SET_MODULE", 0x0000, 0, 0, ccl_set_module, SET_MODULE_params, 0
};

extern int ccl_set_noverbose();

static struct cduVerb SET_NOVERBOSE = {	/* Syntax def   */
  "SET_NOVERBOSE", 0x0000, 0, 0, ccl_set_noverbose, 0, 0
};

extern int ccl_set_verbose();

static struct cduVerb SET_VERBOSE = {	/* Syntax def   */
  "SET_VERBOSE", 0x0000, 0, 0, ccl_set_verbose, 0, 0
};

static struct cduKeyword SET_KEYWORDS[6] = {	/* "Type" def   */
  "XANDQ", 0x4000, 0, &SET_XANDQ, 0, 0, "MEMORY", 0x4000, 0, &SET_MEMORY, 0, 0, "MODULE", 0x4000, 0, &SET_MODULE, 0, 0, "NOVERBOSE", 0x4000, 0, &SET_NOVERBOSE, 0, 0, "VERBOSE", 0x4000, 0, &SET_VERBOSE, 0, 0, 0	/* null entry at end    */
};

static struct cduValue v19p01value = {
  0x2001, 0, SET_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v19params[2] = {
  "P1", 0x1000, 0, "What", &v19p01value, 0, 0	/* null entry at end    */
};

extern int ccl_show_data();
static struct cduKeyword SHOW_KEYWORDS[4];	/* Prototype only       */

static struct cduValue SHOW_DATA_p01value = {
  0x2001, 0, SHOW_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SHOW_DATA_params[2] = {
  "P1", 0x1000, 0, 0, &SHOW_DATA_p01value, 0, 0	/* null entry at end    */
};

static struct cduValue SHOW_DATA_q01value = {
  0x0000, "1", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue SHOW_DATA_q02value = {
  0x0000, "5", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier SHOW_DATA_qualifiers[8] = {
  "START", 0x2005, 0, 0, &SHOW_DATA_q01value, 0, "END", 0x2005, 0, 0, &SHOW_DATA_q02value, 0, "BINARY", 0x2004, 0, 0, 0, 0, "DECIMAL", 0x2005, 0, 0, 0, 0, "OCTAL", 0x2004, 0, 0, 0, 0, "HEX", 0x2004, 0, 0, 0, 0, "MEMORY", 0x2004, 0, 0, 0, 0, 0	/* null entry at end    */
};

static struct cduVerb SHOW_DATA = {	/* Syntax def   */
  "SHOW_DATA", 0x0000, 0, 0, ccl_show_data, SHOW_DATA_params, SHOW_DATA_qualifiers
};

extern int ccl_show_module();

static struct cduValue SHOW_MODULE_p01value = {
  0x2001, 0, SHOW_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SHOW_MODULE_params[2] = {
  "P1", 0x1000, 0, 0, &SHOW_MODULE_p01value, 0, 0	/* null entry at end    */
};

static struct cduVerb SHOW_MODULE = {	/* Syntax def   */
  "SHOW_MODULE", 0x0000, 0, 0, ccl_show_module, SHOW_MODULE_params, 0
};

extern int ccl_show_status();

static struct cduValue SHOW_STATUS_p01value = {
  0x2001, 0, SHOW_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam SHOW_STATUS_params[2] = {
  "P1", 0x1000, 0, 0, &SHOW_STATUS_p01value, 0, 0	/* null entry at end    */
};

static struct cduVerb SHOW_STATUS = {	/* Syntax def   */
  "SHOW_STATUS", 0x0000, 0, 0, ccl_show_status, SHOW_STATUS_params, 0
};

static struct cduKeyword SHOW_KEYWORDS[4] = {	/* "Type" def   */
  "DATA", 0x4000, 0, &SHOW_DATA, 0, 0, "MODULE", 0x4000, 0, &SHOW_MODULE, 0, 0, "STATUS", 0x4000, 0, &SHOW_STATUS, 0, 0, 0	/* null entry at end    */
};

static struct cduValue v20p01value = {
  0x2001, 0, SHOW_KEYWORDS, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduParam v20params[2] = {
  "P1", 0x1000, 0, "What", &v20p01value, 0, 0	/* null entry at end    */
};

extern int ccl_plot_data();

static struct cduValue v21q01value = {
  0x0000, "1", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v21q02value = {
  0x0000, "32767", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v21q03value = {
  0x0000, "TT:", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduValue v21q04value = {
  0x0000, "RETRO", 0, {0, DSC_K_DTYPE_T, DSC_K_CLASS_D, 0}, 0
};

static struct cduQualifier v21qualifiers[5] = {
  "START", 0x2005, 0, 0, &v21q01value, 0, "END", 0x2005, 0, 0, &v21q02value, 0, "DEVICE", 0x2005, 0, 0, &v21q03value, 0, "DEVTYPE", 0x2005, 0, 0, &v21q04value, 0, 0	/* null entry at end    */
};

struct cduVerb ccl_commands[22] = {
  "CCL", 0x0000, 0, 0, ccl_finish, v01params, v01qualifiers, "FINISH", 0x0000, 0, 0, ccl_finish, 0, 0, "PIO", 0x0000, 0, 0, ccl_pio, v03params, v03qualifiers, "QREP", 0x0000, 0, 0, ccl_qrep, v04params, v04qualifiers, "FQREP", 0x0000, 0, 0, ccl_fqrep, v05params, v05qualifiers, "QSCAN", 0x0000, 0, 0, ccl_qscan, v06params, v06qualifiers, "QSTOP", 0x0000, 0, 0, ccl_qstop, v07params, v07qualifiers, "FQSTOP", 0x0000, 0, 0, ccl_fqstop, v08params, v08qualifiers, "STOP", 0x0000, 0, 0, ccl_stop, v09params, v09qualifiers, "FSTOP", 0x0000, 0, 0, ccl_fstop, v10params, v10qualifiers, "DCLST", 0x0000, 0, 0, ccl_dclst, 0, 0, "LPIO", 0x0000, 0, 0, ccl_lpio, v12params, v12qualifiers, "LQSCAN", 0x0000, 0, 0, ccl_lqscan, v13params, v13qualifiers, "LSTOP", 0x0000, 0, 0, ccl_lstop, v14params, v14qualifiers, "EXECUTE", 0x0000, 0, 0, ccl_execute, 0, 0, "RDATA", 0x0000, 0, 0, ccl_rdata, 0, v16qualifiers, "WDATA", 0x0000, 0, 0, ccl_wdata, 0, v17qualifiers, "LAMWAIT", 0x0000, 0, 0, ccl_lamwait, v18params, v18qualifiers, "SET", 0x0000, 0, 0, 0, v19params, 0, "SHOW", 0x0000, 0, 0, 0, v20params, 0, "PLOT", 0x0000, 0, 0, ccl_plot_data, 0, v21qualifiers, 0	/* null entry at end    */
};
