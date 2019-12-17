# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ac_git_revision.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_IS_BUSYBOX([shell_cmd],[action if true],[action if false])
#
# DESCRIPTION
#  Find if a command is provided by busybox
#  
# LICENSE
#
#   Copyright (c) 2019 Andrea Rigoni Garola <andrea.rigoni@igi.cnr.it>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 1

# //////////////////////////////////////////////////////////////////////////// #
# ////// IS BUSYBOX ////////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

AC_DEFUN([AX_PROG_BUSYBOX],[
    AC_CHECK_TOOL([BUSYBOX],[busybox])
])

AC_DEFUN([AX_IS_BUSYBOX],[
    AC_REQUIRE([AX_PROG_BUSYBOX])
    echo which ${BUSYBOX}
    if test -n "${BUSYBOX}"; then
    AS_IF([cmp $(which [$1]) $(which ${BUSYBOX})],[$2],[$3])
    else
    $3
    fi
])

