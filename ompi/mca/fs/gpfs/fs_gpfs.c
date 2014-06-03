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
 * Copyright (c) 2011      Cisco Systems, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ompi_config.h"
#include "mpi.h"
#include "ompi/mca/fs/fs.h"
#include "ompi/mca/fs/base/base.h"
#include "ompi/mca/fs/gpfs/fs_gpfs.h"

//TODO ifdef SIOX blablabla
//#ifdef HAVE_SIOX_H
//#include "C/siox.h"
//#endif

#ifdef HAVE_SYS_STATFS_H
#include <sys/statfs.h> /* or <sys/vfs.h> */ 
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

/*
 * *******************************************************************
 * ************************ actions structure ************************
 * *******************************************************************
 */
static mca_fs_base_module_1_0_0_t gpfs = { mca_fs_gpfs_module_init, /* initialize after being selected */
mca_fs_gpfs_module_finalize, /* close a module on a communicator */
mca_fs_gpfs_file_open, mca_fs_gpfs_file_close, mca_fs_gpfs_file_delete,
		mca_fs_gpfs_file_set_size, mca_fs_gpfs_file_get_size,
		mca_fs_gpfs_file_set_info, mca_fs_gpfs_file_sync };
/*
 * *******************************************************************
 * ************************* structure ends **************************
 * *******************************************************************
 */

int mca_fs_gpfs_component_init_query(bool enable_progress_threads,
		bool enable_mpi_threads) {
	/* Nothing to do */

	return OMPI_SUCCESS;
}

struct mca_fs_base_module_1_0_0_t *
mca_fs_gpfs_component_file_query(mca_io_ompio_file_t *fh, int *priority) {
	int err;
	char *dir;
	struct statfs fsbuf;
	char *tmp;

	/* The code in this function is based on the ADIO FS selection in ROMIO
	 *   Copyright (C) 1997 University of Chicago.
	 *   See COPYRIGHT notice in top-level directory.
	 */
	*priority = mca_fs_gpfs_priority;

	tmp = strchr(fh->f_filename, ':');
	if (!tmp) {
		if (OMPIO_ROOT == fh->f_rank) {
			do {
				err = statfs(fh->f_filename, &fsbuf);
			} while (err && (errno == ESTALE));

			if (err && (errno == ENOENT)) {
				mca_fs_base_get_parent_dir(fh->f_filename, &dir);
				err = statfs(dir, &fsbuf);
				free(dir);
			}

#ifndef GPFS_SUPER_MAGIC
#define GPFS_SUPER_MAGIC  0x47504653    /* Thats GPFS in ASCII */
#endif
			if (fsbuf.f_type == GPFS_SUPER_MAGIC) {
				fh->f_fstype = GPFS;
			}
		}
		fh->f_comm->c_coll.coll_bcast(&(fh->f_fstype), 1,
		MPI_INT,
		OMPIO_ROOT, fh->f_comm, fh->f_comm->c_coll.coll_bcast_module);
	} else {
		if (!strncmp(fh->f_filename, "gpfs:", 5)
				|| !strncmp(fh->f_filename, "GPFS:", 5)) {
			fh->f_fstype = GPFS;
		}
	}

	if (GPFS == fh->f_fstype) {
		if (*priority < 50) {
			*priority = 50;
			return &gpfs;
		}
	}

	return NULL;
}

int mca_fs_gpfs_component_file_unquery(mca_io_ompio_file_t *file) {
	/* This function might be needed for some purposes later. for now it
	 * does not have anything to do since there are no steps which need
	 * to be undone if this module is not selected */

	return OMPI_SUCCESS;
}

int mca_fs_gpfs_module_init(mca_io_ompio_file_t *file) {

/*#ifdef SIOX_API_H
	//START SIOX initialization
	printf("Initializing the SIOX in mca_fs_gpfs_module_init()\n");
	siox_unique_interface *siox_gpfs_uiid = NULL;
	siox_component *siox_gpfs_component = NULL;
	siox_component_activity *siox_gpfs_component_activity = NULL;

	siox_gpfs_uiid = siox_system_information_lookup_interface_id("MPI",
			"OMPIO");
	siox_gpfs_component = siox_component_register(siox_gpfs_uiid, "");
	siox_gpfs_component_activity = siox_component_register_activity(
			siox_gpfs_uiid, "GPFS_hints");

	printf("Beginning the SIOX_activity in mca_fs_gpfs_module_init()\n");

	file->f_siox_component = siox_gpfs_component;
	file->f_siox_activity = siox_activity_begin(siox_gpfs_component,
			siox_gpfs_component_activity);
	//END SIOX initialization
#endif*/
	return OMPI_SUCCESS;
}

int mca_fs_gpfs_module_finalize(mca_io_ompio_file_t *file) {

/*#ifdef SIOX_API_H
	printf("Finalizing the SIOX in mca_fs_gpfs_module_finalize()\n");
	siox_activity_end(file->f_siox_activity);
	siox_component_unregister(file->f_siox_component);
#endif*/
	return OMPI_SUCCESS;
}

