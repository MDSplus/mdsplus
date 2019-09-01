#
# SYNOPSIS
#
#   AX_CHECK_JAVA_VERSION(min_version,ifok,ifnotok)
#
# DESCRIPTION
#
#   Checks for java with at least the given version.
#

AC_DEFUN([AX_CHECK_JAVA_VERSION], [
    AC_MSG_CHECKING([for version $1 or higher of java])
    version=`java -version 2>&1|head -n 1|grep -oe '[[0-9]]*\.[[0-9]]*\.[[0-9]]*'||echo 1.5`
    AX_COMPARE_VERSION([$version], [ge], [$1], [AC_MSG_RESULT([yes]);$2], [AC_MSG_RESULT([no]);$3])
])
