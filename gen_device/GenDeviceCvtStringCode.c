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

int GEN_DEVICE$CVT_STRING_CODE( )

------------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <string.h>
#include <librtl_messages.h>
#include <treeshr.h>
#include <mds_gendevice.h>

struct  _table {
    short   code;
    char    *string;
    };
int GenDeviceCvtStringCode(short *code_ptr, char *string_ptr, struct _table *table)
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

    register int	current;

/*------------------------------------------------------------------------------

 Executable:                                                                  */


    if(!string_ptr) return(DEV$_BADPARAM);
    for(current = 0;table[current].string && strcmp(table[current].string, string_ptr); current++);
    if(table[current].string)
    {
	*code_ptr = table[current].code;
	return TreeNORMAL;
    }
    return LibKEYNOTFOU;
}
