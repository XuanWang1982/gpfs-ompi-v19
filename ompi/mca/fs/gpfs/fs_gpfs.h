/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2012 University of Houston. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef MCA_FS_GPFS_H
#define MCA_FS_GPFS_H

#include "ompi_config.h"
#include "opal/mca/mca.h"
#include "ompi/mca/fs/fs.h"
#include "ompi/mca/io/ompio/io_ompio.h"

//TODO ifdef SIOX blablabla
//#ifdef HAVE_C_SIOX_H
//#include "C/siox.h"
//#endif

#include <gpfs.h>

extern int mca_fs_gpfs_priority;

#ifdef SIOX_API_H
extern siox_unique_interface *siox_gpfs_uiid;
extern siox_component *siox_gpfs_component;
extern siox_component_activity *siox_gpfs_component_activity;
#endif

BEGIN_C_DECLS

int mca_fs_gpfs_component_init_query(bool enable_progress_threads,
		bool enable_mpi_threads);
struct mca_fs_base_module_1_0_0_t *
mca_fs_gpfs_component_file_query(mca_io_ompio_file_t *fh, int *priority);
int mca_fs_gpfs_component_file_unquery(mca_io_ompio_file_t *file);

int mca_fs_gpfs_module_init(mca_io_ompio_file_t *file);
int mca_fs_gpfs_module_finalize(mca_io_ompio_file_t *file);
OMPI_MODULE_DECLSPEC extern mca_fs_base_component_2_0_0_t mca_fs_gpfs_component;

/*
 * ******************************************************************
 * ********* functions which are implemented in this module *********
 * ******************************************************************
 */

int mca_fs_gpfs_file_open(struct ompi_communicator_t *comm, char *filename,
		int amode, struct ompi_info_t *info, struct mca_io_ompio_file_t *fh);
int mca_fs_gpfs_file_close(struct mca_io_ompio_file_t *fh);
int mca_fs_gpfs_file_delete(char *filename, struct ompi_info_t *info);
int mca_fs_gpfs_file_set_size(struct mca_io_ompio_file_t *fh,
		OMPI_MPI_OFFSET_TYPE size);
int mca_fs_gpfs_file_get_size(struct mca_io_ompio_file_t *fh,
		OMPI_MPI_OFFSET_TYPE * size);
int mca_fs_gpfs_file_get_amode(struct ompi_file_t *fh, int *amode);
int mca_fs_gpfs_file_set_info(struct mca_io_ompio_file_t *fh,
		struct ompi_info_t *info);
//int mca_fs_gpfs_file_get_info(struct ompi_file_t *fh,
				//struct ompi_info_t **info_used);
int mca_fs_gpfs_prefetch_hints_for_siox(int access_mode,
		mca_io_ompio_file_t *fh, struct ompi_info_t *info);
int mca_fs_gpfs_siox_io_selection(mca_io_ompio_file_t *fh,
		struct ompi_info_t *info, struct ompi_info_t *info_selected);
int mca_fs_gpfs_file_sync(struct ompi_file_t *fh);
int
mca_fs_gpfs_file_seek(struct ompi_file_t *fh, OMPI_MPI_OFFSET_TYPE off,
		int whence);
int
mca_fs_gpfs_file_get_position(struct ompi_file_t *fh,
		OMPI_MPI_OFFSET_TYPE *offset);

/*
 * ******************************************************************
 * ************ functions implemented in this module end ************
 * ******************************************************************
 */

END_C_DECLS

#endif /* MCA_FS_GPFS_H */
