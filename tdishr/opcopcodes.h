/*  	opcopcodes.h
	Definition of internal/intrinsic function opcodes

	Ken Klare, LANL CTR-10	(c)1989
	NEED to remove comment trick when VAXC supports ANSI-C sharp sharp
*/
#define COM
#define OPC(name,builtin,f1,f2,f3,i1,i2,o1,o2,m1,m2,token) Opc##name,

enum OpcOpcodes {
#include "opcbuiltins.h"
OpcTOO_BIG};
