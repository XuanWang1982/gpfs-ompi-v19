/*
 * $HEADERS$
 */
#include "lam_config.h"
#include <stdio.h>

#include "mpi.h"
#include "mpi/c/bindings.h"
#include "attribute/attribute.h"

#if LAM_HAVE_WEAK_SYMBOLS && LAM_PROFILING_DEFINES
#pragma weak MPI_Win_set_attr = PMPI_Win_set_attr
#endif

#if LAM_PROFILING_DEFINES
#include "mpi/c/profile/defines.h"
#endif

static char FUNC_NAME[] = "MPI_Win_set_attr";

int MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val) {

    int ret;

    if (MPI_PARAM_CHECK) {
	if (MPI_WIN_NULL == win) {
	    return LAM_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_WIN, 
					 FUNC_NAME);
	}
    }

    ret = lam_attr_set(WIN_ATTR, win, win->w_keyhash, 
		       win_keyval, attribute_val, 0);

    LAM_ERRHANDLER_RETURN(ret, win, MPI_ERR_OTHER, FUNC_NAME);  
}
