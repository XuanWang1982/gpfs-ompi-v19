dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
dnl                         University Research and Technology
dnl                         Corporation.  All rights reserved.
dnl Copyright (c) 2004-2005 The University of Tennessee and The University
dnl                         of Tennessee Research Foundation.  All rights
dnl                         reserved.
dnl Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2005 The Regents of the University of California.
dnl                         All rights reserved.
dnl Copyright (c) 2010-2013 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2014      Intel, Inc. All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

# OPAL_CHECK_FUNC_LIB(func, lib, [action-if-found], [action-if-not-found])
# ------------------------------
# Try to find function func, first with the present LIBS, second with
# lib added to LIBS.  If func is found with the libraries listed in
# LIBS, no modification to LIBS is made.  If func is in lib (but not
# in LIBS) then lib is added to LIBS.  If func is not in lib, then
# LIBS is not modified.
AC_DEFUN([OPAL_CHECK_FUNC_LIB],[
    OPAL_VAR_SCOPE_PUSH([LIBS_save])
    AS_VAR_PUSHDEF([opal_var], [opal_cv_func_lib_$1_$2])dnl
    AC_CACHE_CHECK([if we need -l$2 for $1],
        opal_var,
        [AC_LINK_IFELSE([AC_LANG_FUNC_LINK_TRY([$1])],
            [AS_VAR_SET(opal_var, "no")],
            [LIBS_save="$LIBS"
             LIBS="$LIBS -l$2"
             AC_LINK_IFELSE([AC_LANG_FUNC_LINK_TRY([$1])],
                 [AS_VAR_SET(opal_var, "yes")],
                 [AS_VAR_SET(opal_var, "not found")])
             LIBS="$LIBS_save"])])
    AS_VAR_IF(opal_var, [yes],
              [LIBS="$LIBS -l$2"])

    # see if we actually have $1.  Use AC_CHECK_FUNCS so that it
    # does the glibc "not implemented" check.  Will use the current LIBS,
    # so will check in -l$2 if we decided we needed it above
    AC_CHECK_FUNCS([$1], $3, $4)
    AS_VAR_POPDEF([opal_var])dnl
    OPAL_VAR_SCOPE_POP
])
