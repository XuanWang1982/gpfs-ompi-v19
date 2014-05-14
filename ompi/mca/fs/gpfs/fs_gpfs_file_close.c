/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2011 University of Houston. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "ompi_config.h"
#include "fs_gpfs.h"
#include "gpfs.h"

#include <fcntl.h>
#include <unistd.h>
#include "mpi.h"
#include "ompi/constants.h"
#include "ompi/mca/fs/fs.h"

/*
 *	file_close_lustre
 *
 *	Function:	- closes a new file
 *	Accepts:	- file handle
 *	Returns:	- Success if file closed
 */
int
mca_fs_gpfs_file_close (mca_io_ompio_file_t *fh)
{
	printf("Using mca_fs_gpfs_file_close to close a file.\n");

	/*const gpfs_iattr_t *iattrP;
    char *RootFsDirP, buf;
    int rc = 0, gpfs_ret = 0;
    gpfs_iscan_t *iscanP = NULL;
    gpfs_fssnap_handle_t *fsP = NULL;
    gpfs_ifile_t *dirxP = NULL;
    gpfs_ino_t maxInodeNum;
    gpfs_ino_t inode = 0;
    gpfs_off64_t offset;
    RootFsDirP = (char *) fh->f_filename;
    fsP = gpfs_get_fssnaphandle_by_path(RootFsDirP);
    if (fsP == NULL) {
    	rc = errno;
    	printf("gpfs_get_fssnaphandle_by_path(%s): %s\n", RootFsDirP, strerror(rc));
    	return OMPI_ERROR;
    }
    iscanP = gpfs_open_inodescan(fsP, NULL, &maxInodeNum);
    while (1){
    	rc = gpfs_next_inode(iscanP, maxInodeNum, &iattrP);
    	if (rc != 0){
    		rc = errno;
    		printf("gpfs_next_inode(%s, %d): %s\n", fsP, inode, strerror(rc));
    		return OMPI_ERROR;
    	}
    	if (iattrP == NULL){
    		break;
    	}
    	inode = iattrP->ia_inode;
    }
    printf("inode number is %u\n", inode);
    //gpfs_close_inodescan(iscanP);
    printf("Opening a file using gpfs_iopen() within fs_gpfs_file_close\n");
    dirxP = gpfs_iopen(fsP, inode, O_WRONLY, NULL, NULL);
    printf("inode number is %u and dirxP is %s\n", inode, &dirxP);
    rc = gpfs_iwrite(dirxP, "123456789", 10, &offset);
	if (rc != 0){
		rc = errno;
		printf("gpfs_iwrite(%s): %s\n", dirxP, strerror(rc));
		return OMPI_ERROR;
	}
    gpfs_iclose(dirxP);
    dirxP = gpfs_iopen(fsP, inode, O_RDONLY, NULL, NULL);
    rc = gpfs_iread(dirxP, &buf, 1, &offset);
	if (rc != 0){
		rc = errno;
		printf("gpfs_iread(%s, %s): %s\n", dirxP, &buf, strerror(rc));
		return OMPI_ERROR;
	}
    printf("output of the gpfs_iread() is %s \n", &buf);
    gpfs_iclose(dirxP);
    //gpfs_ret = gpfs_clone_copy("/gpfs/test1", "/gpfs/test-clone");
    printf("The return value of gpfs function is %d.\n", rc);*/


    fh->f_comm->c_coll.coll_barrier (fh->f_comm,
                                     fh->f_comm->c_coll.coll_barrier_module);
    /*    close (*(int *)fh->fd);*/
    close (fh->fd);
    /*    if (NULL != fh->fd)
    {
        free (fh->fd);
        fh->fd = NULL;
        }*/
    return OMPI_SUCCESS;
}
