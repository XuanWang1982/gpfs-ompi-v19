/*
 * $HEADER$
 */

#include "lam_config.h"

#include "mpi.h"
#include "mpi/interface/c/bindings.h"

#if LAM_HAVE_WEAK_SYMBOLS && LAM_PROFILING_DEFINES
#pragma weak MPI_File_write_at_all = PMPI_File_write_at_all
#endif

int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, void *buf,
		                  int count, MPI_Datatype datatype, 
						  MPI_Status *status) {
    return MPI_SUCCESS;
}
