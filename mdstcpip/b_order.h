/*
 * Find out what the byte order is on this machine.
 * Big endian is for machines that place the most significant byte first.
 * (eg. Sun SPARC)
 * Little endian is for machines that place the least significant byte first.
 * (eg. VAX)
 *
 * We figure out the byte order by stuffing a 2 byte string into a short and
 * examining the left byte.        '@' = 0x40  and  'P' = 0x50
 * If the left byte is the 'high' byte, then it is 'big endian'.  If the left
 * byte is the 'low' byte, then the machine is 'little endian'.
 *
 * 					-- Shawn A. Clifford (sac@eng.ufl.edu)
 */

#ifndef HAVE_VXWORKS_H
#define BIG_ENDIAN (((unsigned short)('@P')>>8) == 0x40)
#define LITTLE_ENDIAN (((unsigned short)('@P')>>8) == 0x50)
#endif
