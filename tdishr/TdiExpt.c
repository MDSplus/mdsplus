/*	Tdi3Expt.C
	Data base info.

	Ken Klare, LANL CTR-7	(c)1990
*/
#include <mdsdescrip.h>
#include <dbidef.h>
#include <libroutines.h>
#include <stdio.h>
#include <string.h>
#include <mdsshr.h>
#include <treeshr.h>
/*--------------------------------------------------------------
	Default path name.
*/
int				Tdi3MdsDefault(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
struct descriptor *dummy = in_ptr;
char	value[4096];
static unsigned char    dtype = (unsigned char)DTYPE_T;
int	retlen, status;
struct dbi_itm lst[] = {{sizeof(value),DbiDEFAULT,0,0},{0,DbiEND_OF_LIST,0}};
        lst[0].pointer = (unsigned char *)value;
        lst[0].return_length_address = &retlen;
	status = TreeGetDbi(lst);
	if (status & 1) status = MdsGet1DxS((unsigned short *)&retlen, &dtype, out_ptr);
	if (status & 1) memcpy(out_ptr->pointer->pointer, value, retlen);
	return status;
}
/*--------------------------------------------------------------
	Experiment name.
*/
int				Tdi3Expt(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
struct descriptor *dummy = in_ptr;
char	value[39-7];
int	retlen, status;
static unsigned char dtype = (unsigned char)DTYPE_T;
struct dbi_itm lst[] = {{sizeof(value),DbiNAME,0,0},{0,DbiEND_OF_LIST,0}};
        lst[0].pointer = (unsigned char *)value;
        lst[0].return_length_address = &retlen;
	status = TreeGetDbi(lst);
	if (status & 1) status = MdsGet1DxS((unsigned short *)&retlen, &dtype, out_ptr);
	if (status & 1) memcpy(out_ptr->pointer->pointer, value, retlen);
	return status;
}
/*--------------------------------------------------------------
	Shot number identifier.
*/
int				Tdi3Shot(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
struct descriptor *dummy = in_ptr;
int	value;
int	retlen, status;
static unsigned char dtype = (unsigned char)DTYPE_L;
struct dbi_itm lst[] = {{sizeof(value),DbiSHOTID,0,0},{0,DbiEND_OF_LIST,0}};
        lst[0].pointer = (unsigned char *)&value;
        lst[0].return_length_address = &retlen;
	status = TreeGetDbi(lst);
	if (status & 1) status = MdsGet1DxS((unsigned short *)&retlen, &dtype, out_ptr);
	if (status & 1) *(int *)out_ptr->pointer->pointer = value;
	return status;
}
/*--------------------------------------------------------------
	Shot number identifier converted to string.
*/
int				Tdi3Shotname(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
struct descriptor *dummy = in_ptr;
int	value;
int	retlen, status;
struct dbi_itm lst[] = {{sizeof(value),DbiSHOTID,0,0},{0,DbiEND_OF_LIST,0}};
DESCRIPTOR(dmodel, "MODEL");
char	string[15];
        lst[0].pointer = (unsigned char *)&value;
        lst[0].return_length_address = &retlen;
	status = TreeGetDbi(lst);
	if (value != -1) {
		sprintf(string, "%ld", value);
		dmodel.pointer = string;
		dmodel.length = (unsigned short)strlen(string);
	}
	if (status & 1) status = MdsCopyDxXd((struct descriptor *)&dmodel, out_ptr);
	return status;
}
