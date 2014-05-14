# OMPI_CHECK_SIOX(prefix, [action-if-found], [action-if-not-found])
# --------------------------------------------------------
# check if SIOX support can be found. Sets prefix_{CPPFLAGS, 
# LDFLAGS, LIBS} as needed and runs action-if-found if there is
# support, otherwise executes action-if-not-found

AC_DEFUN([OMPI_CHECK_SIOX],[

    check_siox_CPPFLAGS=
    check_siox_LDFLAGS=
    check_siox_LIBS=

    check_siox_save_LIBS="$LIBS" 
    check_siox_save_LDFLAGS="$LDFLAGS"
    check_siox_save_CPPFLAGS="$CPPFLAGS"

    check_siox_configuration="none"
    ompi_check_siox_happy="yes"


    # Get some configuration information
    AC_ARG_WITH([siox],
        [AC_HELP_STRING([--with-siox(=DIR)],
             [Build SIOX support, optionally adding DIR/include, DIR/lib, and DIR/lib64 to the search path for headers and libraries])])
    OPAL_CHECK_WITHDIR([siox], [$with_siox], [include/C/siox.h])

    AS_IF([test -z "$with_siox"],
          [ompi_check_siox_dir="/usr"],
          [ompi_check_siox_dir="$with_siox"])

    if test -e "$ompi_check_siox_dir/lib64" ; then
        ompi_check_siox_libdir="$ompi_check_siox_dir/lib64"
    else
        ompi_check_siox_libdir="$ompi_check_siox_dir/lib"
    fi

    # Add correct -I and -L flags
    OPAL_CHECK_PACKAGE([$1], [C/siox.h], [siox-monitoring-siox-ll], [siox_initialize_monitoring], [],
                       [$ompi_check_siox_dir], [$ompi_check_siox_libdir], [ompi_check_siox_happy="yes"],
                       [ompi_check_siox_happy="no"],[#include <C/siox.h>])

    AS_IF([test "$ompi_check_siox_happy" = "yes"],
          [$2],
          [AS_IF([test ! -z "$with_siox" -a "$with_siox" != "no"],
                  [echo SIOX support not found])
              $3])
])
