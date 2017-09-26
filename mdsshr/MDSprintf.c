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
/*
* MDSprintf.c --
*
* Functions to allow re-direction of
*    "printf(...)" and
*    "fprintf(stderr, ...)"
*
* Usage:
*   int MDSprintf( char *fmt , ... )
*     - Use like printf()
*
*   int MDSfprintf( FILE *fp , char *fmt , ... )
*     - Use like fprintf().  Affects only stderr and stdout
*
*   void  MdsSetOutputFunctions(
*       int (*NEWvprintf)()
*      ,int (*NEWvfprintf)()
*      )
*     - Set addr of functions called via MDSprintf and MDSfprintf.
*       Special values:
*          0 :  suppress output
*         -1 :  restore default (vprintf / vfprintf)
*
*       Functions, if specified, must handle variable-length arg lists.
*       Defaults are:
*         o vprintf( char *fmt , va_list ap )           and
*         o vfprintf( FILE *fp , char *fmt , va_list ap )
*             Note: called only for "fp" stderr and stdout.
*       Any user-written function should follow these prototypes.
*
*   void  MdsGetOutputFunctions(
*        void **CURvprintf
*       ,void **CURvfprintf
*       )
*      - Get addresses of current functions (see above).
*
* History:
*  14-Feb-2001  TRG  Create.
*
*********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <STATICdef.h>
#include <mdsshr.h>

		/*=====================================================
		 * Static variables ...
		 *====================================================*/
STATIC_THREADSAFE int (*MDSvprintf) () = vprintf;	/* not really threadsafe but ok */
STATIC_THREADSAFE int (*MDSvfprintf) () = vfprintf;	/* not really threadsafe but ok */

	/******************************************************************
	 * MDSprintf:
	 ******************************************************************/
EXPORT int MDSprintf(char const *fmt, ...)
{
  va_list ap;

  if (!MDSvprintf)
    return (0);
  va_start(ap, fmt);		/* initialize "ap"              */
  return ((*MDSvprintf) (fmt, ap));
}

	/******************************************************************
	 * MDSfprintf:
	 ******************************************************************/
int MDSfprintf(FILE * fp, char const *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);		/* initialize "ap"              */
  if (fp != stderr && fp != stdout)
    return (vfprintf(fp, fmt, ap));
  if (!MDSvfprintf)
    return (0);
  return ((*MDSvfprintf) (fp, fmt, ap));
}

	/***************************************************************
	 * MdsSetOutputFunctions:
	 * MdsGetOutputFunctions:
	 ***************************************************************/
void MdsSetOutputFunctions(int (*NEWvprintf) (const char *, void *)
			   , int (*NEWvfprintf) (FILE *, const char *, void *)
    )
{
  MDSvprintf = ((void *)NEWvprintf == (void *)-1) ? (int (*)())vprintf : (int (*)())NEWvprintf;
  MDSvfprintf = ((void *)NEWvfprintf == (void *)-1) ? (int (*)())vfprintf : (int (*)())NEWvfprintf;
  return;
}

void MdsGetOutputFunctions(void **CURvprintf, void **CURvfprintf)
{
  if (CURvprintf)
    *CURvprintf = (void *)MDSvprintf;
  if (CURvfprintf)
    *CURvfprintf = (void *)MDSvfprintf;
  return;
}

#ifdef Main
	/****************************************************************
	 * main:
	 ****************************************************************/

STATIC_ROUTINE int woof(char *fmt, va_list ap)
{
  char xxfmt[80];

  sprintf(xxfmt, "\nWOOF: %s", fmt);
  return (vprintf(xxfmt, ap));
}

STATIC_ROUTINE int tweet(FILE * fp, char *fmt, va_list ap)
{
  char xxfmt[80];

  sprintf(xxfmt, "\nTWEET: %s\n", fmt);
  return (vfprintf(fp, xxfmt, ap));
}

int main(int argc, void *argv[])
{
  void *save_vprintf;
  void *save_vfprintf;
  MDSprintf("woof %d %d %d\n", 1, 2, 3);
  MDSfprintf(stderr, "tweet %d %d %d\n", 1, 2, 3);

  MdsGetOutputFunctions(&save_vprintf, &save_vfprintf);
  MdsSetOutputFunctions(woof, tweet);
  MDSprintf("woof %d %d %d\n", 1, 2, 3);
  MDSfprintf(stderr, "tweet %d %d %d\n", 1, 2, 3);

  MdsSetOutputFunctions(save_vprintf, save_vfprintf);
  MDSprintf("woof %d %d %d\n", 1, 2, 3);
  MDSfprintf(stderr, "tweet %d %d %d\n", 1, 2, 3);

  MdsGetOutputFunctions(&save_vprintf, &save_vfprintf);
  MdsSetOutputFunctions(woof, tweet);
  MDSprintf("woof %d %d %d\n", 1, 2, 3);
  MDSfprintf(stderr, "tweet %d %d %d\n", 1, 2, 3);

  MdsSetOutputFunctions((void *)-1, (void *)-1);
  MDSprintf("woof %d %d %d\n", 1, 2, 3);
  MDSfprintf(stderr, "tweet %d %d %d\n", 1, 2, 3);
}

#endif
