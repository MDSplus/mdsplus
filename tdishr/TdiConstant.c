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
#include <math.h>
#include <mdsshr.h>
#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
_Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")
#endif

    int Tdi1Constant(opcode_t opcode, int narg __attribute__((unused)),
                     mdsdsc_t *list[] __attribute__((unused)),
                     mdsdsc_xd_t *out_ptr)
{

  return MdsCopyDxXd(((mdsdsc_t * (*)()) * TdiRefFunction[opcode].f3)(),
                     out_ptr);
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
"The 1986 Adjustment of the Fundamental Physical Constants..." by Cohen and
Taylor "1980 Revised NRL Plasma Formulary" by Book. NEED to remove / ** / when
ANSI C permits sharp-sharp
*/
typedef void *MISSING;
typedef uint8_t BU;
typedef float FS;
typedef double FT;
typedef struct
{
  float x, y;
} FSC;
typedef unsigned int FROP;

#define DTYPE_FROP DTYPE_F

#define DATUM(type, x, data)                                               \
  mdsdsc_t *Tdi3##x()                                                      \
  {                                                                        \
    static const type val = data;                                          \
    static const mdsdsc_t constant = {sizeof(type), DTYPE_##type, CLASS_S, \
                                      (char *)&val};                       \
    return (mdsdsc_t *)&constant;                                          \
  }

#define UNITS(type, x, data, units)                                     \
  mdsdsc_t *Tdi3##x()                                                   \
  {                                                                     \
    static const type val = data;                                       \
    static const mdsdsc_t val_d = {sizeof(type), DTYPE_##type, CLASS_S, \
                                   (char *)&val};                       \
    static const DESCRIPTOR(units_d, units);                            \
    static const DESCRIPTOR_WITH_UNITS(constant, &val_d, &units_d);     \
    return (mdsdsc_t *)&constant;                                       \
  }

#define CAST_ERROR(type, x)                                  \
  (((x) - (double)(type)(x)) < 0 ? ((double)(type)(x) - (x)) \
                                 : ((x) - (double)(type)(x)))

#define DERR(type, x, data, error)                                      \
  mdsdsc_t *Tdi3##x()                                                   \
  {                                                                     \
    static const type val = data;                                       \
    static const type err = CAST_ERROR(type, data) + error;             \
    static const mdsdsc_t val_d = {sizeof(type), DTYPE_##type, CLASS_S, \
                                   (char *)&val};                       \
    static const mdsdsc_t err_d = {sizeof(type), DTYPE_##type, CLASS_S, \
                                   (char *)&err};                       \
    static const DESCRIPTOR_WITH_ERROR(constant, &val_d, &err_d);       \
    return (mdsdsc_t *)&constant;                                       \
  }

#define UERR(type, x, data, error, units)                               \
  mdsdsc_t *Tdi3##x()                                                   \
  {                                                                     \
    static const type val = data;                                       \
    static const type err = CAST_ERROR(type, data) + error;             \
    static const mdsdsc_t val_d = {sizeof(type), DTYPE_##type, CLASS_S, \
                                   (char *)&val};                       \
    static const mdsdsc_t err_d = {sizeof(type), DTYPE_##type, CLASS_S, \
                                   (char *)&err};                       \
    static const DESCRIPTOR(units_d, units);                            \
    static const DESCRIPTOR_WITH_ERROR(werr_d, &val_d, &err_d);         \
    static const DESCRIPTOR_WITH_UNITS(constant, &werr_d, &units_d);    \
    return (mdsdsc_t *)&constant;                                       \
  }

#ifdef M_PI
#define PI_DATA M_PI
#else
#define PI_DATA 3.14159265358979323846
#endif

// values by definition
#define BY_DEFINITION 0
#define C_DATA 299792458.
#define E_DATA 1.602176634e-19
#define GAS_DATA 8.31446261815324
#define GN_DATA 9.80665
#define H_DATA 6.62607015e-34
#define I_DATA \
  {            \
    0., 1.     \
  }
#define K_DATA 1.3806505e-23
#define MU0_DATA (4e-7 * PI_DATA)
#define NA_DATA 6.02214076e23
#define P0_DATA 101325
#define T0_DATA 273.15
// values by relation
#define ALPHA_DATA ((MU0_DATA * C_DATA * E_DATA * E_DATA) / (2 * H_DATA))
#define EPS0_DATA (1. / (MU0_DATA * C_DATA * C_DATA))
#define HBAR_DATA (H_DATA / (2 * PI_DATA))
#define FARADAY_DATA (E_DATA * NA_DATA)
#define N0_DATA (P0_DATA / (K_DATA * T0_DATA))
#define TORR_DATA (P0_DATA / 760.)
// empirical
#define CAL_DATA 4.1868
#define G_DATA 6.67430e-11
#define G_ERROR 15e-16
#define MU_DATA 1.66053906660e-27
#define MU_ERROR 50e-38 /*negligible*/
#define ME_DATA 9.1093837015e-31
#define ME_ERROR 28e-41 /*negligible*/
#define MP_DATA 1.67262192369e-27
#define MP_ERROR 51e-38 /*negligible*/
// values by relation
#define NEGLIGIBLE 0
#define A0_DATA \
  ((EPS0_DATA * H_DATA * H_DATA) / (PI_DATA * E_DATA * E_DATA * ME_DATA))
#define RE_DATA                                     \
  ((MU0_DATA * C_DATA * C_DATA * E_DATA * E_DATA) / \
   (4 * PI_DATA * ME_DATA * C_DATA * C_DATA))
#define RYDBERG_DATA \
  ((ALPHA_DATA * ALPHA_DATA * ME_DATA * C_DATA) / (2 * H_DATA))

DATUM(BU, False, 0)          /*	logically false		*/
DATUM(MISSING, Missing, 0)   /*	missing argument	*/
DATUM(FROP, Roprand, 0x8000) /*	reserved operand        */
DATUM(BU, True, 1)           /*	logically true		*/

DATUM(FT, 2Pi, 2 * PI_DATA)                 /*2pi	circumference/radius	*/
UERR(FS, A0, A0_DATA, NEGLIGIBLE, "m")      /*a0	Bohr radius		*/
DERR(FS, Alpha, ALPHA_DATA, BY_DEFINITION)  /*	fine-structure constant	*/
UERR(FS, Amu, MU_DATA, MU_ERROR, "kg")      /*u	atomic mass unit, dalton*/
UNITS(FT, C, C_DATA, "m/s")                 /*c	speed of light		*/
UNITS(FS, Cal, CAL_DATA, "J")               /*calIT int. steam tab. calorie*/
DATUM(FT, Degree, PI_DATA / 180)            /*pi/180			*/
UNITS(FT, Epsilon0, EPS0_DATA, "F/m")       /*eps0 permitivity of vacuum	*/
UERR(FS, Ev, E_DATA, BY_DEFINITION, "J/eV") /*eV	electron volt
                                             */
UERR(FS, Faraday, FARADAY_DATA, BY_DEFINITION,
     "C/mol")                              /*F	Faraday constant	*/
UERR(FS, G, G_DATA, G_ERROR, "m^3/s^2/kg") /*G	gravitational constant	*/
UERR(FS, Gas, GAS_DATA, BY_DEFINITION,
     "J/K/mol")                                 /*R	gas constant		*/
UNITS(FS, Gn, GN_DATA, "m/s^2")                 /*gn	acceleration of gravity	*/
UERR(FS, H, H_DATA, BY_DEFINITION, "J*s")       /*h	Planck constant		*/
UERR(FS, Hbar, HBAR_DATA, BY_DEFINITION, "J*s") /*hbar =h/(2pi)		*/
DATUM(FSC, I, I_DATA)                           /*i	imaginary		*/
UERR(FS, K, K_DATA, BY_DEFINITION, "J/K")       /*k	Boltzmann constant	*/
UERR(FS, Me, ME_DATA, ME_ERROR, "kg")           /*me	mass of electron	*/
UERR(FS, Mp, MP_DATA, MP_ERROR, "kg")           /*mp	mass of proton		*/
UNITS(FT, Mu0, MU0_DATA, "N/A^2")               /*mu0	permeability of vacuum	*/
UERR(FS, N0, N0_DATA, BY_DEFINITION, "/m^3")    /*n0	Loschmidt's number
                                                   (STP)*/
UERR(FS, Na, NA_DATA, BY_DEFINITION, "/mol")    /*NA	Avogadro number
                                                 */
UNITS(FS, P0, P0_DATA, "Pa")                    /*atm	atmospheric pressure	*/
DATUM(FT, Pi, PI_DATA)                          /*pi	circumference/diameter  */
UERR(FS, Qe, E_DATA, BY_DEFINITION, "C")        /*e	charge on electron      */
UERR(FS, Re, RE_DATA, NEGLIGIBLE, "m")          /*re	class. electron radius  */
UERR(FS, Rydberg, RYDBERG_DATA, NEGLIGIBLE,
     "/m")                       /*Rinf	Rydberg constant	*/
UNITS(FS, T0, T0_DATA, "K")      /*	0 degC in Kelvin	*/
UNITS(FT, Torr, TORR_DATA, "Pa") /*	torr 1mm Hg pressure    */
