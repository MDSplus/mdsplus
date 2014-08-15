public fun jrg_tr1612_24_bit_data_read(in _name, in _chan_number, in _first_sample, in _last_sample)
{
/* $Id$
 *
 * Function to read Joerger Model TR (or "TR1612") data in 24 bit mode
 *
 * BAN/MBF 23-SEP-2003
 * BAN/MBF  4-AUG-2004
 *	Changed all CamPiow calls to 24 bit;
 *	Fixes bug with 18 bit data values in F17/A1
 * BAN      7-AUG-2004
 *	Changed CamStopw call to CamQstopw, which can read more than 
 *		64K-1 data points at a time.
 *	When reading 256KB for all 16 channels, it sped up from 
 *	22 s to 19 s for the full data read (~14% faster).
 */

/* set digitizer for readout at first memory location */	    
DevCamChk(_name,CamPiow(_name,1wu,17wu,_first_sample,24wu),1,1);

_samples_to_read = (_last_sample - _first_sample + 1)/2;

/* 24 bit data read */
DevCamChk(_name,CamQstopw(_name,_chan_number,2wu,_samples_to_read,_data,24wu),1,*);

/*
 * In 24 bit read mode, lower twelve bits are even index values,
 * and upper twelve bits are odd index values.
 * Make an array with lower twelve bits in first row and
 * upper twelve bits in second row.
 * Take the transpose, and the resulting array is in the
 * proper index order.  Concatenate with $MISSING to 
 * reshape the 2 X _samples/2 matrix to a vector.
 */
_data = [ $MISSING, 
	transpose([ _data & 0x000FFF, (_data & 0xFFF000) >> 12 ]) ];

/* if odd number of samples were requested, read the last one
* Add later
if ( mod( _samples_to_read, 2 ) )
  {
  DevCamChk(_name,CamStopw(_name,_chan_num,2wu,1,_last_value,24wu),1,*);
  _data = [ _data, _last_value & 0x000FFF ]
  }
*/

return( _data );
}
