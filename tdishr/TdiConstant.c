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
/*      Tdi1Constant.C
	Get descriptor of a constant value.
	Note $NARG (number of arguments) is in Tdi1Var.
	Note $VALUE (raw of signal, data of param) is in tdi_get_data.
	They are not constants, but context values.

	Ken Klare, LANL P-4     (c)1989,1990,1992
*/
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <mdsshr.h>
#include <math.h>
#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
    _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"")
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int Tdi1Constant(opcode_t opcode, int narg __attribute__ ((unused)),
		 mdsdsc_t *list[] __attribute__ ((unused)), mdsdsc_xd_t *out_ptr)
{

  return MdsCopyDxXd(((mdsdsc_t * (*)())*TdiRefFunction[opcode].f3) (), out_ptr);
}

/*------------------------------------------------
	Descriptor definitions of constants, MKS.

	Valid units (*=basic +=supplementary):
	m       meter*
	kg      kilogram*
	s       second*
	A       ampere*
	K       kelvin*
	mol     mole*
	cd      candela*
	rad     radian+
	sr      steradian+
	Hz      hertz
	J       joule
	N       newton
	Pa      pascal
	W       watt
	C       coulomb
	V       volt
	(omega) ohm
	S       siemens
	F       farad
	Wb      weber
	H       henry
	T       tesla
	lm      lumen
	lx      lux
	Bq      becquerel
	Gy      gray

Major sources:
"The 1986 Adjustment of the Fundamental Physical Constants..." by Cohen and Taylor
"1980 Revised NRL Plasma Formulary" by Book.
	NEED to remove / ** / when ANSI C permits sharp-sharp
*/
typedef void* MISSING;
typedef uint8_t BU;
typedef float FS;
typedef double FT;
typedef struct {
  float x, y;
} FSC;
typedef unsigned int FROP;

#define DTYPE_FROP      DTYPE_F

#define DATUM(type, x, data) \
	static const type     d##x = data;\
	static const mdsdsc_t Tdi##x##Constant = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
	mdsdsc_t *Tdi3##x(){return (mdsdsc_t *)&Tdi##x##Constant;}

#define DERR(type, x, data, error) \
	static const type     d##x = data;\
	static const type     e##x = error;\
	static const mdsdsc_t dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
	static const mdsdsc_t de##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&e##x};\
	static const DESCRIPTOR_WITH_ERROR(Tdi##x##Constant,&dd##x,&de##x);\
	mdsdsc_t *Tdi3##x(){return (mdsdsc_t *)&Tdi##x##Constant;}

#define UNITS(type, x, data, units) \
	static const type     d##x = data;\
	static const mdsdsc_t dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
	static const DESCRIPTOR(du##x, units);\
	static const DESCRIPTOR_WITH_UNITS(Tdi##x##Constant,&dd##x,&du##x);\
	mdsdsc_t *Tdi3##x(){return (mdsdsc_t *)&Tdi##x##Constant;}

#define UERR(type, x, data, error, units) \
	static const type     d##x = data;\
	static const type     e##x = error;\
	static const mdsdsc_t dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
	static const mdsdsc_t de##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&e##x};\
	static const DESCRIPTOR(du##x, units);\
	static const DESCRIPTOR_WITH_ERROR(dwe##x,&dd##x,&de##x);\
	static const DESCRIPTOR_WITH_UNITS(Tdi##x##Constant,&dwe##x,&du##x);\
	mdsdsc_t *Tdi3##x(){return (mdsdsc_t *)&Tdi##x##Constant;}

#define I_DATA {0., 1.}
DATUM(FT,	2Pi,		2*M_PI					)/*	circumference/radius	*/
UERR (FS,	A0,		5.29177249e-11,	0.00000024e-11,	"m"	)/*a0	Bohr radius		*/
DERR (FS,	Alpha,		7.29735308e-3,	0.00000033e-3		)/*	fine-structure constant	*/
UERR (FS,	Amu,		1.6605402e-27,	0.0000010e-27,	"kg"	)/*u	unified atomic mass unit*/
UNITS(FS,	C,		299792458.,			"m/s"	)/*c	speed of light		*/
UNITS(FS,	Cal,		4.1868,				"J"	)/*	calorie			*/
DATUM(FS,	Degree,		M_PI/180				)/*	pi/180			*/
UNITS(FS,	Epsilon0,	8.854187817e-12,		"F/m"	)/*eps0 permitivity of vacuum	*/
UERR (FS,	Ev,	 	1.60217733e-19,	0.00000049e-19,	"J/eV"	)/*eV	electron volt		*/
DATUM(BU,	False,		0					)/*	logically false		*/
UERR (FS,	Faraday,	9.6485309e4,	0.0000029e4,	"C/mol"	)/*F	Faraday constant	*/
UERR (FS,	G,		6.67259e-11,	0.00085,    "m^3/s^2/kg")/*G gravitational constant	*/
UERR (FS,	Gas,		8.314510,	0.000070,      "J/K/mol")/*R	gas constant		*/
UNITS(FS,	Gn,		9.80665,			"m/s^2"	)/*gn	acceleration of gravity	*/
UERR (FS,	H,		6.6260755e-34,	0.0000040,	"J*s"	)/*h	Planck constant		*/
UERR (FS,	Hbar,		1.05457266e-34,	0.00000063,	"J*s"	)/*hbar h/(2pi)			*/
DATUM(FSC,	I,		I_DATA					)/*i	imaginary		*/
UERR (FS,	K,		1.380658e-23,	0.000012e-23,	"J/K"	)/*k	Boltzmann constant	*/
UERR (FS,	Me,		9.1093897e-31,	0.0000054e-31,	"kg"	)/*me	mass of electron	*/
DATUM(MISSING,	Missing,	0					)/*	missing argument	*/
UERR (FS,	Mp,		1.6726231e-27,	0.0000010e-27,	"kg"	)/*mp	mass of proton		*/
UNITS(FS,	Mu0,		12.566370614e-7,		"N/A^2"	)/*mu0	permeability of vacuum	*/
UERR (FS,	N0,		2.686763e25,	0.000023e25, "/m^3"	)/*n0	Loschmidt's number (STP)*/
UERR (FS,	Na,		6.0221367e23,	0.0000036e23, "/mol"	)/*NA	Avogadro number		*/
UNITS(FS,	P0,		1.01325e5,			"Pa"	)/*atm	atmospheric pressure	*/
DATUM(FT,	Pi,		M_PI					)/*pi	circumference/diameter  */
UERR (FS,	Qe, 		1.60217733e-19, 0.000000493e-19,"C"	)/*e	charge on electron      */
UERR (FS,	Re, 		2.81794092e-15,	0.00000038e-15,	"m"	)/*re	class. electron radius	*/
DATUM(FROP, 	Roprand,	0x8000					)/*	reserved operand        */
UERR (FS,	Rydberg,	1.0973731534e7, 0.0000000013e7,	"/m"	)/*Rinf	Rydberg constant	*/
UNITS(FS,	T0,		273.16,				"K"	)/*	0 degC in Kelvin	*/
UNITS(FS,	Torr,		1.3332e2,			"Pa"	)/*	torr 1mm Hg pressure    */
DATUM(BU,	True,		1					)/*	logically true		*/
