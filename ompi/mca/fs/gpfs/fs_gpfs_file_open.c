/*
 *  Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                          University Research and Technology
 *                          Corporation.  All rights reserved.
 *  Copyright (c) 2004-2005 The University of Tennessee and The University
 *                          of Tennessee Research Foundation.  All rights
 *                          reserved.
 *  Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                          University of Stuttgart.  All rights reserved.
 *  Copyright (c) 2004-2005 The Regents of the University of California.
 *                          All rights reserved.
 *  Copyright (c) 2008-2012 University of Houston. All rights reserved.
 *  $COPYRIGHT$
 *  
 *  Additional copyrights may follow
 *  
 *  $HEADER$
 */

#include "ompi_config.h"

#include "ompi/communicator/communicator.h"
#include "ompi/info/info.h"
#include "ompi/file/file.h"
#include "ompi/mca/fs/fs.h"
#include "ompi/mca/fs/base/base.h"
#include "ompi/mca/fcoll/fcoll.h"
#include "ompi/mca/fcoll/base/base.h"
#include "ompi/mca/fbtl/fbtl.h"
#include "ompi/mca/fbtl/base/base.h"
#include "fs_gpfs.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <gpfs.h>
#include <fcntl.h>
#include <errno.h>
#include <gpfs_fcntl.h>
//#include <C/siox.h>

int
mca_fs_gpfs_file_open (struct ompi_communicator_t *comm,
						char* filename,
                        int access_mode,
                        struct ompi_info_t *info,
                        mca_io_ompio_file_t *fh)
{
    int amode;
    int old_mask, perm;

    if (fh->f_perm == OMPIO_PERM_NULL)  {
        old_mask = umask(022);
        umask(old_mask);
        perm = old_mask ^ 0666;
    }
    else {
        perm = fh->f_perm;
    }

    amode = 0;

    if (access_mode & MPI_MODE_CREATE)
        amode = amode | O_CREAT;
    if (access_mode & MPI_MODE_RDONLY)
        amode = amode | O_RDONLY;
    if (access_mode & MPI_MODE_WRONLY)
        amode = amode | O_WRONLY;
    if (access_mode & MPI_MODE_RDWR)
        amode = amode | O_RDWR;
    if (access_mode & MPI_MODE_EXCL)
        amode = amode | O_EXCL;

    printf("Opening a file using Linux open() within fs_gpfs_file_open\n");
    fh->fd = open (filename, amode, perm);
    if (-1 == fh->fd) {
        return OMPI_ERROR;
    }

    fh->f_amode=access_mode;
    //Setting GPFS Hints
    mca_fs_gpfs_file_set_info(fh, info);

    return OMPI_SUCCESS;
}

/*int
mca_fs_gpfs_file_close (ompi_file_t *fh)
{
    int ret = OMPI_SUCCESS;
    mca_io_ompio_data_t *data;
    int delete_flag = 0;
    char name[256];

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;
    if ( NULL == data ) {*/
	/* structure has already been freed, this is an erroneous call to file_close */
/*	return ret;
    }

    if(mca_fs_gpfs_coll_timing_info){
      strcpy (name, "WRITE");
      if (!ompi_fs_gpfs_empty_print_queue(WRITE_PRINT_QUEUE)){
	ret = ompi_fs_gpfs_print_time_info(WRITE_PRINT_QUEUE,
					    name,
					    &data->ompio_fh);
	if (OMPI_SUCCESS != ret){
	  printf("Error in print_time_info ");
	}
	
      }
      strcpy (name, "READ");
      if (!ompi_fs_gpfs_empty_print_queue(READ_PRINT_QUEUE)){
	ret = ompi_fs_gpfs_print_time_info(READ_PRINT_QUEUE,
					    name,
					    &data->ompio_fh);
	if (OMPI_SUCCESS != ret){
	  printf("Error in print_time_info ");
	}
      }
    }
    if ( data->ompio_fh.f_amode & MPI_MODE_DELETE_ON_CLOSE ) {
	delete_flag = 1;
    }

    ret = data->ompio_fh.f_fs->fs_file_close (&data->ompio_fh);
    if ( delete_flag && 0 == data->ompio_fh.f_rank ) {
	mca_fs_gpfs_file_delete ( data->ompio_fh.f_filename, MPI_INFO_NULL );
    }



    mca_fs_base_file_unselect (&data->ompio_fh);
    mca_fbtl_base_file_unselect (&data->ompio_fh);
    mca_fcoll_base_file_unselect (&data->ompio_fh);

    if (NULL != data->ompio_fh.f_io_array) {
        free (data->ompio_fh.f_io_array);
        data->ompio_fh.f_io_array = NULL;
    }

    if (NULL != data->ompio_fh.f_procs_in_group) {
        free (data->ompio_fh.f_procs_in_group);
        data->ompio_fh.f_procs_in_group = NULL;
    }

    if (NULL != data->ompio_fh.f_decoded_iov) {
        free (data->ompio_fh.f_decoded_iov);
        data->ompio_fh.f_decoded_iov = NULL;
    }

    if (NULL != data->ompio_fh.f_convertor) {
        free (data->ompio_fh.f_convertor);
        data->ompio_fh.f_convertor = NULL;
    }

    if (NULL != data->ompio_fh.f_datarep) {
        free (data->ompio_fh.f_datarep);
        data->ompio_fh.f_datarep = NULL;
    }

    if (MPI_DATATYPE_NULL != data->ompio_fh.f_iov_type) {
        ompi_datatype_destroy (&data->ompio_fh.f_iov_type);
    }

    if (MPI_COMM_NULL != data->ompio_fh.f_comm)  {
        ompi_comm_free (&data->ompio_fh.f_comm);
    }*/


    /*
    if (MPI_INFO_NULL != data->ompio_fh.f_info)
    {
        ompi_info_free (&data->ompio_fh.f_info);
    }
    */

/*    if ( NULL != data ) {
	free ( data );
    }

    return ret;
}
*/
/*int mca_fs_gpfs_file_delete (char *filename,
                              struct ompi_info_t *info) 
{
    int ret = OMPI_SUCCESS;

    ret = unlink(filename);

    if (0 > ret) {
        return OMPI_ERROR;
    }

    return OMPI_SUCCESS;
}*/

/*int
mca_fs_gpfs_file_preallocate (ompi_file_t *fh,
                               OMPI_MPI_OFFSET_TYPE diskspace)
{
    int ret = OMPI_SUCCESS, cycles, i;
    OMPI_MPI_OFFSET_TYPE tmp, current_size, size, written, len;
    mca_io_ompio_data_t *data;
    char *buf = NULL;
    ompi_status_public_t *status = NULL;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    tmp = diskspace;

    data->ompio_fh.f_comm->c_coll.coll_bcast (&tmp,
                                              1,
                                              MPI_LONG_LONG,
                                              OMPIO_ROOT,
                                              data->ompio_fh.f_comm,
                                              data->ompio_fh.f_comm->c_coll.coll_bcast_module);

    if (tmp != diskspace) {
        return OMPI_ERROR;
    }*/

    /* ROMIO explanation
       On file systems with no preallocation function, we have to 
       explicitly write to allocate space. Since there could be holes in the file, 
       we need to read up to the current file size, write it back, 
       and then write beyond that depending on how much 
       preallocation is needed.
    */
/*    if (OMPIO_ROOT == data->ompio_fh.f_rank) {
        ret = data->ompio_fh.f_fs->fs_file_get_size (&data->ompio_fh, 
                                                     &current_size);

        size = diskspace;
        if (size > current_size) {
            size = current_size;
        }

        cycles = (size + OMPIO_PREALLOC_MAX_BUF_SIZE - 1)/
            OMPIO_PREALLOC_MAX_BUF_SIZE;
        buf = (char *) malloc (OMPIO_PREALLOC_MAX_BUF_SIZE);
        if (NULL == buf) {
            opal_output(1, "OUT OF MEMORY\n");
            return OMPI_ERR_OUT_OF_RESOURCE;
        }
        written = 0;

        for (i=0; i<cycles; i++) {
            len = OMPIO_PREALLOC_MAX_BUF_SIZE;
            if (len > size-written) {
                len = size - written;
            }
            ret = mca_fs_gpfs_file_read (fh, buf, len, MPI_BYTE, status);
            if (ret != OMPI_SUCCESS) {
                return OMPI_ERROR;
            }
            ret = mca_fs_gpfs_file_write (fh, buf, len, MPI_BYTE, status);
            if (ret != OMPI_SUCCESS) {
                return OMPI_ERROR;
            }
            written += len;
        }

        if (diskspace > current_size) {
            memset(buf, 0, OMPIO_PREALLOC_MAX_BUF_SIZE);
            size = diskspace - current_size;
            cycles = (size + OMPIO_PREALLOC_MAX_BUF_SIZE - 1) /
                OMPIO_PREALLOC_MAX_BUF_SIZE;
            for (i=0; i<cycles; i++) {
                len = OMPIO_PREALLOC_MAX_BUF_SIZE;
                if (len > diskspace-written) {
                    len = diskspace - written;
                }
                ret = mca_fs_gpfs_file_write (fh, buf, len, MPI_BYTE, status);
                if (ret != OMPI_SUCCESS) {
                    return OMPI_ERROR;
                }
                written += len;
            }
        }
        if (NULL != buf) {
            free (buf);
            buf = NULL;
        }
    }
    fh->f_comm->c_coll.coll_barrier (fh->f_comm,
                                     fh->f_comm->c_coll.coll_barrier_module);
    return ret;
}*/

int
mca_fs_gpfs_file_get_amode (ompi_file_t *fh,
                             int *amode)
{
    mca_io_ompio_data_t *data;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    *amode = data->ompio_fh.f_amode;

    return OMPI_SUCCESS;
}


/*
int
mca_fs_gpfs_file_get_type_extent (ompi_file_t *fh,
                                   struct ompi_datatype_t *datatype,
                                   MPI_Aint *extent)
{
    opal_datatype_type_extent (&datatype->super, extent);
    return OMPI_SUCCESS;
}


int
mca_fs_gpfs_file_set_atomicity (ompi_file_t *fh,
                                 int flag)
{
    int tmp;
    mca_io_ompio_data_t *data;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    if (flag) {
        flag = 1;
    }*/

    /* check if the atomicity flag is the same on all processes */
/*    tmp = flag;
    data->ompio_fh.f_comm->c_coll.coll_bcast (&tmp,
                                              1,
                                              MPI_INT,
                                              OMPIO_ROOT,
                                              data->ompio_fh.f_comm,
                                              data->ompio_fh.f_comm->c_coll.coll_bcast_module);

    if (tmp != flag) {
        return OMPI_ERROR;
    }

    data->ompio_fh.f_atomicity = flag;

    return OMPI_SUCCESS;
}

int
mca_fs_gpfs_file_get_atomicity (ompi_file_t *fh,
                                 int *flag)
{
    mca_io_ompio_data_t *data;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    *flag = data->ompio_fh.f_atomicity;

    return OMPI_SUCCESS;
}*/

/*int
mca_fs_gpfs_file_sync (ompi_file_t *fh)
{
    int ret = OMPI_SUCCESS;
    mca_io_ompio_data_t *data;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    ret = data->ompio_fh.f_fs->fs_file_sync (&data->ompio_fh);

    return ret;
}*/

int mca_fs_gpfs_file_seek(ompi_file_t *fh, OMPI_MPI_OFFSET_TYPE off, int whence) {
	printf("GPFS FILE SEEK!");
	int ret = OMPI_SUCCESS;
	mca_io_ompio_data_t *data;
	OMPI_MPI_OFFSET_TYPE offset, temp_offset;

	data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

	offset = off * data->ompio_fh.f_etype_size;

	switch (whence) {
	case MPI_SEEK_SET:
		if (offset < 0) {
			return OMPI_ERROR;
		}
		break;
	case MPI_SEEK_CUR:
		offset += data->ompio_fh.f_position_in_file_view;
		offset += data->ompio_fh.f_disp;
		if (offset < 0) {
			return OMPI_ERROR;
		}
		break;
	case MPI_SEEK_END:
		ret = data->ompio_fh.f_fs->fs_file_get_size(&data->ompio_fh,
				&temp_offset);
		offset += temp_offset;
		if (offset < 0 || OMPI_SUCCESS != ret) {
			return OMPI_ERROR;
		}
		break;
	default:
		return OMPI_ERROR;
	}

	ret = ompi_io_ompio_set_explicit_offset(&data->ompio_fh, offset
			/ data->ompio_fh.f_etype_size);
	return ret;
}

int mca_fs_gpfs_file_get_position(ompi_file_t *fh, OMPI_MPI_OFFSET_TYPE *offset) {
	mca_io_ompio_data_t *data;

	data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

	*offset = data->ompio_fh.f_position_in_file_view
			/ data->ompio_fh.f_etype_size;

	return OMPI_SUCCESS;
}

/*int
mca_fs_gpfs_file_get_byte_offset (ompi_file_t *fh,
                                   OMPI_MPI_OFFSET_TYPE offset,
                                   OMPI_MPI_OFFSET_TYPE *disp)
{
    mca_io_ompio_data_t *data;
    int i, k, index;
    size_t position;
    size_t total_bytes;
    size_t temp_offset;

    data = (mca_io_ompio_data_t *) fh->f_io_selected_data;

    temp_offset = data->ompio_fh.f_view_extent * 
        (offset*data->ompio_fh.f_etype_size / data->ompio_fh.f_view_size);

    position = 0;
    total_bytes = (offset*data->ompio_fh.f_etype_size) % data->ompio_fh.f_view_size;
    index = 0;
    i = total_bytes;
    k = 0;

    while (1) {
        k += data->ompio_fh.f_decoded_iov[index].iov_len;
        if (i >= k) {
            i = i - data->ompio_fh.f_decoded_iov[index].iov_len;
            position += data->ompio_fh.f_decoded_iov[index].iov_len;
            index = index+1;
        }
        else {
            break;
        }
    }

    *disp = data->ompio_fh.f_disp + temp_offset +
        (OMPI_MPI_OFFSET_TYPE)data->ompio_fh.f_decoded_iov[index].iov_base;

    return OMPI_SUCCESS;
}

int
mca_fs_gpfs_file_seek_shared (ompi_file_t *fh,
                               OMPI_MPI_OFFSET_TYPE offset,
                               int whence)
{
    int ret = MPI_ERR_OTHER;
    return ret;
}


int
mca_fs_gpfs_file_get_position_shared (ompi_file_t *fh,
                                       OMPI_MPI_OFFSET_TYPE * offset)
{
    int ret = MPI_ERR_OTHER;
    return ret;
}*/
