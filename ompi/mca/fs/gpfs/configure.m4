# -*- shell-script -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2008-2012 University of Houston. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#


# MCA_fs_gpfs_CONFIG(action-if-can-compile, 
#                        [action-if-cant-compile])
# ------------------------------------------------
AC_DEFUN([MCA_ompi_fs_gpfs_CONFIG],[
    AC_CONFIG_FILES([ompi/mca/fs/gpfs/Makefile])

    OMPI_CHECK_GPFS([fs_gpfs],
                     [fs_gpfs_happy="yes"],
                     [fs_gpfs_happy="no"])

    AS_IF([test "$fs_gpfs_happy" = "yes"],
           [fs_gpfs_WRAPPER_EXTRA_LDFLAGS="$fs_gpfs_LDFLAGS"
           fs_gpfs_WRAPPER_EXTRA_LIBS="$fs_gpfs_LIBS"
           $1],
          [$2])

    # substitute in the things needed to build gpfs
    AC_SUBST([fs_gpfs_CPPFLAGS])
    AC_SUBST([fs_gpfs_LDFLAGS])
    AC_SUBST([fs_gpfs_LIBS])
    
	# if siox installation is not found but gpfs installation is found
	# the gpfs component should also be compiled
    OMPI_CHECK_SIOX([fs_siox],
                     [fs_siox_happy="yes"],
                     [fs_siox_happy="yes"])

    AS_IF([test "$fs_siox_happy" = "yes"],
           [fs_siox_WRAPPER_EXTRA_LDFLAGS="$fs_siox_LDFLAGS"
           fs_siox_WRAPPER_EXTRA_LIBS="$fs_siox_LIBS"
           $1],
          [$2])

    # substitute in the things needed to build siox
    AC_SUBST([fs_siox_CPPFLAGS])
    AC_SUBST([fs_siox_LDFLAGS])
    AC_SUBST([fs_siox_LIBS])

    AC_CHECK_HEADERS([C/siox.h])
    
])dnl
