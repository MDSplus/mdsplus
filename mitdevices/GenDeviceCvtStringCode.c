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
/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$CVT_STRING_CODE   

		Type:   C function

     		Author:	GABRIELE MANDUCHI
			Istituto Gas Ionizzati del CNR - Padova (Italy)

		Date:   22-AUG-1989

    		Purpose:  Convetr a string into module interenal code, 
		    given translation table.

------------------------------------------------------------------------------

	Call sequence: 

EXPORT int GEN_DEVICE$CVT_STRING_CODE( )

------------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <string.h>
#include <mdsshr_messages.h>
#include <treeshr.h>
#include <mds_gendevice.h>

struct _table {
  short code;
  char *string;
};
EXPORT int GenDeviceCvtStringCode(short *code_ptr, char *string_ptr, struct _table *table)
{

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

  register int current;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

  if (!string_ptr)
    return (DEV$_BADPARAM);
  for (current = 0; table[current].string && strcmp(table[current].string, string_ptr); current++) ;
  if (table[current].string) {
    *code_ptr = table[current].code;
    return TreeNORMAL;
  }
  return LibKEYNOTFOU;
}
