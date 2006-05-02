/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$CVT_INT_CODE   

		Type:   C function

     		Author:	GABRIELE MANDUCHI
			Istituto Gas Ionizzati del CNR - Padova (Italy)

		Date:   22-AUG-1989

    		Purpose:  Convert integer to code, given translation table.

------------------------------------------------------------------------------

	Call sequence: 

int GEN_DEVICE$CVT_INT_CODE( )

---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <librtl_messages.h>
#include <treeshr.h>

struct _table {
    short   code;
    int	    value;
    };

int GenDeviceCvtIntCode(code_ptr, value, table, dimension)
short *code_ptr;
int value;
struct _table table[];
int dimension;
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
    for(current = 0;(current < dimension) && (table[current].value != value);current++)
	;
    if (current == dimension) return LibKEYNOTFOU;
    else *code_ptr = table[current].code;
    return TreeNORMAL;
}
