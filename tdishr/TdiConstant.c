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
        Note $VALUE (raw of signal, data of param) is in TdiGetData.
        They are not constants, but context values.

        Ken Klare, LANL P-4     (c)1989,1990,1992
*/
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <mdsshr.h>
#include <STATICdef.h>



int Tdi1Constant(int opcode, int narg __attribute__ ((unused)),
		 struct descriptor *list[] __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{

  return MdsCopyDxXd(((struct descriptor * (*)())*TdiRefFunction[opcode].f3) (), out_ptr);
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
typedef void (*MISSING) ();
typedef unsigned char BU;
typedef float FLOAT;
typedef struct {
  float x, y;
} FLOAT_COMPLEX;
typedef unsigned int FROP;

#define DTYPE_FROP      DTYPE_F

#define DATUM(type, x, data) \
        STATIC_CONSTANT type                    d##x = data;\
        STATIC_CONSTANT struct descriptor       Tdi##x##Constant = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
        struct descriptor *Tdi3##x(){return &Tdi##x##Constant;}

#define DERR(type, x, data, error) \
        STATIC_CONSTANT type                    d##x = data;\
        STATIC_CONSTANT type                    e##x = error;\
        STATIC_CONSTANT struct descriptor       dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
        STATIC_CONSTANT struct descriptor       de##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&e##x};\
        STATIC_CONSTANT DESCRIPTOR_WITH_ERROR(Tdi##x##Constant,&dd##x,&de##x);\
        struct descriptor *Tdi3##x(){return (struct descriptor *)&Tdi##x##Constant;}

#define UNITS(type, x, data, units) \
        STATIC_CONSTANT type                    d##x = data;\
        STATIC_CONSTANT struct descriptor       dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
        STATIC_CONSTANT DESCRIPTOR(             du##x, units);\
        STATIC_CONSTANT DESCRIPTOR_WITH_UNITS(Tdi##x##Constant,&dd##x,&du##x);\
        struct descriptor *Tdi3##x(){return (struct descriptor *)&Tdi##x##Constant;}

#define UERR(type, x, data, error, units) \
        STATIC_CONSTANT type                    d##x = data;\
        STATIC_CONSTANT type                    e##x = error;\
        STATIC_CONSTANT DESCRIPTOR(du##x, units);\
        STATIC_CONSTANT struct descriptor       dd##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&d##x};\
        STATIC_CONSTANT struct descriptor       de##x = {sizeof(type), DTYPE_##type, CLASS_S, (char *)&e##x};\
        STATIC_CONSTANT DESCRIPTOR_WITH_ERROR(dwe##x,&dd##x,&de##x);\
        STATIC_CONSTANT DESCRIPTOR_WITH_UNITS(Tdi##x##Constant,&dwe##x,&du##x);\
        struct descriptor *Tdi3##x(){return (struct descriptor *)&Tdi##x##Constant;}

#define II {(float)0., (float)1.}
#define RR 0x8000

DATUM(FLOAT, 2Pi, (float)6.2831853072)	/* circumference/radius    */
    UERR(FLOAT, A0, (float)5.29177249e-11, (float)0.00000024e-11, "m")	/*a0       Bohr radius             */
    DERR(FLOAT, Alpha, (float)7.29735308e-3, (float)0.00000033e-3)	/* fine-structure constant */
    UERR(FLOAT, Amu, (float)1.6605402e-27, (float)0.0000010e-27, "kg")	/* u atomic mass unit, unified */
    UNITS(FLOAT, C, (float)299792458., "m/s")	/* c speed of light(exact) */
    UNITS(FLOAT, Cal, (float)4.1868, "J")	/* calorie                 */
    DATUM(FLOAT, Degree, (float).01745329252)	/* pi/180                  */
    UNITS(FLOAT, Epsilon0, (float)8.854187817e-12, "F/m")	/* permitivity of vacuum(exact) */
    UERR(FLOAT, Ev, (float)1.60217733e-19, (float)0.00000049e-19, "J/eV")	/* eV electron volt        */
    DATUM(BU, False, 0)		/* logically false         */
    UERR(FLOAT, Faraday, (float)9.6485309e4, (float)0.0000029e4, "C/mol")	/*F        Faraday constant        */
    UERR(FLOAT, G, (float)6.67259e-11, (float)0.00085, "m^3/s^2/kg")	/*G gravitational constant */
    UERR(FLOAT, Gas, (float)8.314510, (float)0.000070, "J/K/mol")	/*R       gas constant            */
    UNITS(FLOAT, Gn, (float)9.80665, "m/s^2")	/*gn       acceleration of gravity(exact) */
    UERR(FLOAT, H, (float)6.6260755e-34, (float)0.0000040, "J*s")	/*h        Planck constant         */
    UERR(FLOAT, Hbar, (float)1.05457266e-34, (float)0.00000063, "J*s")	/*hbar h/(2pi)             */
    DATUM(FLOAT_COMPLEX, I, II)	/*i        imaginary               */
    UERR(FLOAT, K, (float)1.380658e-23, (float)0.000012e-23, "J/K")	/*k        Boltzmann constant      */
    UERR(FLOAT, Me, (float)9.1093897e-31, (float)0.0000054e-31, "kg")	/*me       mass of electron        */
    DATUM(MISSING, Missing, 0)	/* missing argument        */
    UERR(FLOAT, Mp, (float)1.6726231e-27, (float)0.0000010e-27, "kg")	/*mp       mass of proton          */
    UNITS(FLOAT, Mu0, (float)12.566370614e-7, "N/A^2")	/* permeability of vacuum(exact) */
    UERR(FLOAT, N0, (float)2.686763e25, (float)0.000023e25, "/m^3")	/*n0       Loschmidt's number (STP) */
    UERR(FLOAT, Na, (float)6.0221367e23, (float)0.0000036e23, "/mol")	/*NA or L Avogadro number  */
    UNITS(FLOAT, P0, (float)1.01325e5, "Pa")	/*atm      atmospheric pressure(exact) */
    DATUM(FLOAT, Pi, (float)3.1415926536)	/* circumference/diameter  */
    UERR(FLOAT, Qe, (float)1.60217733e-19, (float)0.000000493e-19, "C")	/*e        charge on electron      */
    UERR(FLOAT, Re, (float)2.81794092e-15, (float)0.00000038e-15, "m")	/*re       classical electron radius */
    DATUM(FROP, Roprand, RR)	/* reserved operand        */
    UERR(FLOAT, Rydberg, (float)1.0973731534e7, (float)0.0000000013e7, "/m")	/*Rinf Rydberg constant    */
    UNITS(FLOAT, T0, (float)273.16, "K")	/*?        standard temperature    */
    UNITS(FLOAT, Torr, (float)1.3332e2, "Pa")	/*?torr 1mm Hg pressure    */
    DATUM(BU, True, 1)
