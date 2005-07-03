/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef MCA_BTL_IB_PROC_H
#define MCA_BTL_IB_PROC_H

#include "mca/ns/ns.h"
#include "opal/class/opal_object.h"
#include "proc/proc.h"
#include "btl_mvapi.h"
#include "btl_mvapi_endpoint.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
OBJ_CLASS_DECLARATION(mca_btl_mvapi_proc_t);

/**
 * Represents the state of a remote process and the set of addresses
 * that it exports. Also cache an instance of mca_btl_base_endpoint_t for
 * each
 * BTL instance that attempts to open a connection to the process.
 */
struct mca_btl_mvapi_proc_t {
    ompi_list_item_t super;                  
    /**< allow proc to be placed on a list */

    ompi_proc_t *proc_ompi;                  
    /**< pointer to corresponding ompi_proc_t */

    orte_process_name_t proc_guid;           
    /**< globally unique identifier for the process */

    size_t proc_addr_count;                  
    /**< number of addresses published by endpoint */

    struct mca_btl_base_endpoint_t **proc_endpoints; 
    /**< array of endpoints that have been created to access this proc */    

    size_t proc_endpoint_count;                  
    /**< number of endpoints */

    ompi_mutex_t proc_lock;                  
    /**< lock to protect against concurrent access to proc state */
};
typedef struct mca_btl_mvapi_proc_t mca_btl_mvapi_proc_t;

mca_btl_mvapi_proc_t* mca_btl_mvapi_proc_create(ompi_proc_t* ompi_proc);
int mca_btl_mvapi_proc_insert(mca_btl_mvapi_proc_t*, mca_btl_base_endpoint_t*);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif
#endif
