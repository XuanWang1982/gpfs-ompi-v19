/*
 * $HEADER$
 */

#include "ompi_config.h"

#include <stdio.h>

#include "mpi.h"
#include "mpi/f77/bindings.h"

#if OMPI_HAVE_WEAK_SYMBOLS && OMPI_PROFILE_LAYER
#pragma weak PMPI_TYPE_CREATE_F90_COMPLEX = mpi_type_create_f90_complex_f
#pragma weak pmpi_type_create_f90_complex = mpi_type_create_f90_complex_f
#pragma weak pmpi_type_create_f90_complex_ = mpi_type_create_f90_complex_f
#pragma weak pmpi_type_create_f90_complex__ = mpi_type_create_f90_complex_f
#elif OMPI_PROFILE_LAYER
OMPI_GENERATE_F77_BINDINGS (PMPI_TYPE_CREATE_F90_COMPLEX,
                           pmpi_type_create_f90_complex,
                           pmpi_type_create_f90_complex_,
                           pmpi_type_create_f90_complex__,
                           pmpi_type_create_f90_complex_f,
                           (MPI_Fint *p, MPI_Fint *r, MPI_Fint *newtype, MPI_Fint *ierr),
                           (p, r, newtype, ierr) )
#endif

#if OMPI_HAVE_WEAK_SYMBOLS
#pragma weak MPI_TYPE_CREATE_F90_COMPLEX = mpi_type_create_f90_complex_f
#pragma weak mpi_type_create_f90_complex = mpi_type_create_f90_complex_f
#pragma weak mpi_type_create_f90_complex_ = mpi_type_create_f90_complex_f
#pragma weak mpi_type_create_f90_complex__ = mpi_type_create_f90_complex_f
#endif

#if ! OMPI_HAVE_WEAK_SYMBOLS && ! OMPI_PROFILE_LAYER
OMPI_GENERATE_F77_BINDINGS (MPI_TYPE_CREATE_F90_COMPLEX,
                           mpi_type_create_f90_complex,
                           mpi_type_create_f90_complex_,
                           mpi_type_create_f90_complex__,
                           mpi_type_create_f90_complex_f,
                           (MPI_Fint *p, MPI_Fint *r, MPI_Fint *newtype, MPI_Fint *ierr),
                           (p, r, newtype, ierr) )
#endif


#if OMPI_PROFILE_LAYER && ! OMPI_HAVE_WEAK_SYMBOLS
#include "mpi/f77/profile/defines.h"
#endif

void mpi_type_create_f90_complex_f(MPI_Fint *p, MPI_Fint *r,
				   MPI_Fint *newtype, MPI_Fint *ierr)
{
    MPI_Datatype c_newtype = MPI_Type_f2c(*newtype);

    *ierr = OMPI_INT_2_FINT(MPI_Type_create_f90_complex(OMPI_FINT_2_INT(*p), 
							OMPI_FINT_2_INT(*r),
							&c_newtype));

    if (MPI_SUCCESS == *ierr) {
      *newtype = MPI_Type_c2f (c_newtype); 
    }
}
