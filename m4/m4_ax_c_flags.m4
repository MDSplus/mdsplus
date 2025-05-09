#
# SYNOPSIS
#
#   AX_C_FLAGS([C variable name])
#
# DESCRIPTION
#

# AX_C_FLAGS([C variable name])
#
AC_DEFUN([AX_C_FLAGS],[
    AX_COMPILER_VERSION    

    if [ test x"$GCC" = x"yes" ]; then
    # GCC 12+ triggers a lot of false-positives
    AX_COMPARE_VERSION([${ax_cv_c_compiler_version}], [ge], [12.0.0],
                [AS_VAR_SET([_c_cxx_flags_fix],["-Wno-strict-aliasing -Wno-uninitialized -Wno-clobbered -Wno-stringop-overflow -Wno-array-bounds -Wno-use-after-free -Wno-nonnull"])])
	AS_VAR_APPEND([$1],[" ${_c_cxx_flags_fix}"])
    fi
])

