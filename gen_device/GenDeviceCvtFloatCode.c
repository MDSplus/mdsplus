/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$CVT_FLOAT_CODE   

		Type:   C function

     		Author:	GABRIELE MANDUCHI

		Date:   22-AUG-1989

    		Purpose: Convert floating point number to code, given 
		    translation table. 

------------------------------------------------------------------------------

	Call sequence: 

int GEN_DEVICE$CVT_FLOAT_CODE(code_ptr, value, table, dimension)
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <librtl_messages.h>
#include <treeshr.h>

struct  _table {
    short   code;
    float   value;
    };

int GenDeviceCvtFloatCode(code_ptr, value, table, dimension)
short *code_ptr;
float value;
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

    register int	current;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

    for(current = 0; (value != (float)table[current].value) && (current < dimension); current++)
	;
    if(current == dimension) return LibKEYNOTFOU;
    else *code_ptr = table[current].code;
    return TreeNORMAL;
}
