#
# SYNOPSIS
#
#   AX_FORTRAN_FLAGS([variable name])
#
# DESCRIPTION
#

#serial 1

# AX_FORTRAN_FLAGS([variable name])
# if compiler is gcc and version is greater than 10 we need compiler flags to fix error in argument mismatch
# WARNING: all this should be removed once the actual fortran code is fixed
#
AC_DEFUN([AX_FORTRAN_FLAGS],[
    AX_COMPILER_VERSION    

    # WARNING: gfortran 10 dangerous fixes ... actual fortran code must be reviewed asap
    #          see: https://gcc.gnu.org/gcc-10/porting_to.html
    #          added flags in m4/m4_as_fortran_flags.m4 
    #          -fallow-invalid-boz -fallow-argument-mismatch
    if [ test x"$GCC" = x"yes" ]; then
    AX_COMPARE_VERSION([${ax_cv_c_compiler_version}], [ge], [10.0.0],
                [AS_VAR_SET([_fortran_flags_fix],["-fallow-invalid-boz -fallow-argument-mismatch"])])
	AS_VAR_APPEND([$1],[" ${_fortran_flags_fix}"])
    fi
])

