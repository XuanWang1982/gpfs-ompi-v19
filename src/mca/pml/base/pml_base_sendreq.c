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
#include "ompi_config.h"
#include <string.h>
#include "mca/pml/pml.h"
#include "mca/pml/base/pml_base_sendreq.h"

static void mca_pml_base_send_request_construct(mca_pml_base_send_request_t* req);
static void mca_pml_base_send_request_destruct(mca_pml_base_send_request_t* req);


ompi_class_t mca_pml_base_send_request_t_class = { 
    "mca_pml_base_send_request_t", 
    OBJ_CLASS(mca_pml_base_request_t),
    (ompi_construct_t) mca_pml_base_send_request_construct, 
    (ompi_destruct_t) mca_pml_base_send_request_destruct 
};


static void mca_pml_base_send_request_construct(mca_pml_base_send_request_t* request)
{
    /* no need to reinit for every send -- never changes */
    request->req_base.req_type = MCA_PML_REQUEST_SEND;
    request->req_cached = false;
    OBJ_CONSTRUCT(&request->req_convertor, ompi_convertor_t);
}

static void mca_pml_base_send_request_destruct(mca_pml_base_send_request_t* req)
{
}

