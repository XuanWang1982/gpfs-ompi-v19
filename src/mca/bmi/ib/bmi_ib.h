
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004 The Ohio State University.
 *                    All rights reserved.
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
/**
 * @file
 */
#ifndef MCA_PTL_IB_H
#define MCA_PTL_IB_H

/* Standard system includes */
#include <sys/types.h>
#include <string.h>

/* Open MPI includes */
#include "class/ompi_free_list.h"
#include "class/ompi_bitmap.h"
#include "event/event.h"
#include "mca/pml/pml.h"
#include "mca/bmi/bmi.h"
#include "util/output.h"
#include "mca/mpool/mpool.h" 

/* InfiniBand VAPI includes */
#include "mca/bmi/bmi.h"
#include "bmi_ib_vapi.h"
#include "bmi_ib_addr.h"
#include "bmi_ib_proc.h"
#include "bmi_ib_endpoint.h"
#include "bmi_ib_priv.h"
#include "bmi_ib_frag.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif


#if 1
#define D_PRINT(fmt, args...) {                                     \
    ompi_output(0, "[%s:%d:%s] " fmt, __FILE__, __LINE__, __func__, \
        ##args);                                                    \
}
#else
#define D_PRINT(fmt, args...)
#endif


/**
 * Infiniband (IB) BMI component.
 */

struct mca_bmi_ib_component_t {
    mca_bmi_base_component_1_0_0_t          super;  /**< base BMI component */ 
    
    uint32_t                                ib_num_bmis;
    /**< number of hcas available to the IB component */

    struct mca_bmi_ib_module_t             *ib_bmis;
    /**< array of available PTLs */

    int ib_free_list_num;
    /**< initial size of free lists */

    int ib_free_list_max;
    /**< maximum size of free lists */

    int ib_free_list_inc;
    /**< number of elements to alloc when growing free lists */

    ompi_list_t                             ib_procs;
    /**< list of ib proc structures */

    ompi_event_t                            ib_send_event;
    /**< event structure for sends */

    ompi_event_t                            ib_recv_event;
    /**< event structure for recvs */

    ompi_mutex_t                            ib_lock;
    /**< lock for accessing module state */

    int                                     ib_mem_registry_hints_log_size;
    /**< log2 size of hints hash array used by memory registry */
    
    char* ib_mpool_name; 
    /**< name of ib memory pool */ 
    

    
}; typedef struct mca_bmi_ib_component_t mca_bmi_ib_component_t;

extern mca_bmi_ib_component_t mca_bmi_ib_component;

typedef mca_bmi_base_registration_t mca_bmi_ib_registration_t; 
    


/**
 * IB PTL Interface
 */
struct mca_bmi_ib_module_t {
    mca_bmi_base_module_t  super;  /**< base PTL interface */
    bool bmi_inited; 
    mca_bmi_ib_registration_t ib_reg[256]; 
    VAPI_hca_id_t   hca_id;        /**< ID of HCA */
    VAPI_hca_port_t port;          /**< IB port of this PTL */
    VAPI_hca_hndl_t nic;           /**< NIC handle */
    VAPI_pd_hndl_t  ptag;          /**< Protection Domain tag */
    VAPI_cq_hndl_t  cq_hndl;       /**< Completion Queue handle */
    
    EVAPI_async_handler_hndl_t async_handler;
    /**< Async event handler used to detect weird/unknown events */
    
    mca_bmi_ib_mem_registry_t mem_registry; /**< registry of memory regions */
    ompi_free_list_t send_free;    /**< free list of buffer descriptors */
    ompi_free_list_t recv_free;    /**< free list of buffer descriptors */
    ompi_list_t repost;            /**< list of buffers to repost */
    mca_mpool_base_module_t* ib_pool; 
    /**< ib memory pool */
   
}; typedef struct mca_bmi_ib_module_t mca_bmi_ib_module_t;
    
extern mca_bmi_ib_module_t mca_bmi_ib_module;

/**
 * Register IB component parameters with the MCA framework
 */
extern int mca_bmi_ib_component_open(void);

/**
 * Any final cleanup before being unloaded.
 */
extern int mca_bmi_ib_component_close(void);

/**
 * IB component initialization.
 * 
 * @param num_bmi_modules (OUT)                  Number of BMIs returned in BMI array.
 * @param allow_multi_user_threads (OUT)  Flag indicating wether BMI supports user threads (TRUE)
 * @param have_hidden_threads (OUT)       Flag indicating wether BMI uses threads (TRUE)
 *
 *  (1) read interface list from kernel and compare against component parameters
 *      then create a BMI instance for selected interfaces
 *  (2) setup IB listen socket for incoming connection attempts
 *  (3) publish BMI addressing info 
 *
 */
extern mca_bmi_base_module_t** mca_bmi_ib_component_init(
    int *num_bmi_modules, 
    bool allow_multi_user_threads,
    bool have_hidden_threads
);


/**
 * IB component progress.
 */
extern int mca_bmi_ib_component_progress(
                                         void
);



/**
 * Cleanup any resources held by the BMI.
 * 
 * @param bmi  BMI instance.
 * @return     OMPI_SUCCESS or error status on failure.
 */

extern int mca_bmi_ib_finalize(
    struct mca_bmi_base_module_t* bmi
);


/**
 * PML->BMI notification of change in the process list.
 * 
 * @param bmi (IN)
 * @param nprocs (IN)     Number of processes
 * @param procs (IN)      Set of processes
 * @param peers (OUT)     Set of (optional) peer addressing info.
 * @param peers (IN/OUT)  Set of processes that are reachable via this BMI.
 * @return     OMPI_SUCCESS or error status on failure.
 * 
 */

extern int mca_bmi_ib_add_procs(
    struct mca_bmi_base_module_t* bmi,
    size_t nprocs,
    struct ompi_proc_t **procs,
    struct mca_bmi_base_endpoint_t** peers,
    ompi_bitmap_t* reachable
);

/**
 * PML->BMI notification of change in the process list.
 *
 * @param bmi (IN)     BMI instance
 * @param nproc (IN)   Number of processes.
 * @param procs (IN)   Set of processes.
 * @param peers (IN)   Set of peer data structures.
 * @return             Status indicating if cleanup was successful
 *
 */
extern int mca_bmi_ib_del_procs(
    struct mca_bmi_base_module_t* bmi,
    size_t nprocs,
    struct ompi_proc_t **procs,
    struct mca_bmi_base_endpoint_t** peers
);


/**
 * PML->BMI Initiate a send of the specified size.
 *
 * @param bmi (IN)               BMI instance
 * @param bmi_base_peer (IN)     BMI peer addressing
 * @param send_request (IN/OUT)  Send request (allocated by PML via mca_bmi_base_request_alloc_fn_t)
 * @param size (IN)              Number of bytes PML is requesting BMI to deliver
 * @param flags (IN)             Flags that should be passed to the peer via the message header.
 * @param request (OUT)          OMPI_SUCCESS if the BMI was able to queue one or more fragments
 */
extern int mca_bmi_ib_send(
    struct mca_bmi_base_module_t* bmi,
    struct mca_bmi_base_endpoint_t* bmi_peer,
    struct mca_bmi_base_descriptor_t* descriptor, 
    mca_bmi_base_tag_t tag
);

/**
 * PML->BMI Initiate a put of the specified size.
 *
 * @param bmi (IN)               BMI instance
 * @param bmi_base_peer (IN)     BMI peer addressing
 * @param send_request (IN/OUT)  Send request (allocated by PML via mca_bmi_base_request_alloc_fn_t)
 * @param size (IN)              Number of bytes PML is requesting BMI to deliver
 * @param flags (IN)             Flags that should be passed to the peer via the message header.
 * @param request (OUT)          OMPI_SUCCESS if the BMI was able to queue one or more fragments
 */
extern int mca_bmi_ib_put(
    struct mca_bmi_base_module_t* bmi,
    struct mca_bmi_base_endpoint_t* bmi_peer,
    struct mca_bmi_base_descriptor_t* decriptor
);

extern int mca_bmi_ib_register(
                        struct mca_bmi_base_module_t* bmi, 
                        mca_bmi_base_tag_t tag, 
                        mca_bmi_base_module_recv_cb_fn_t cbfunc, 
                        void* cbdata); 
    
/**
 * Return a recv fragment to the modules free list.
 *
 * @param bmi (IN)   BMI instance
 * @param frag (IN)  IB receive fragment
 *
 */
extern void mca_bmi_ib_recv_frag_return(
    struct mca_bmi_base_module_t* bmi,
    struct mca_bmi_ib_frag_t* frag
);

/**
 * Allocate a segment.
 *
 * @param bmi (IN)      BMI module
 * @param size (IN)     Request segment size.
 */
extern mca_bmi_base_descriptor_t* mca_bmi_ib_alloc(
                                                   struct mca_bmi_base_module_t* bmi, 
                                                   size_t size); 


/**
 * Return a segment allocated by this BMI.
 *
 * @param bmi (IN)      BMI module
 * @param segment (IN)  Allocated segment.
 */
extern int mca_bmi_ib_free(
                           struct mca_bmi_base_module_t* bmi, 
                           mca_bmi_base_descriptor_t* des); 
    
   


/**
 * Pack data and return a descriptor that can be
 * used for send/put.
 *
 * @param bmi (IN)      BMI module
 * @param peer (IN)     BMI peer addressing
 */
mca_bmi_base_descriptor_t* mca_bmi_ib_prepare_src(
    struct mca_bmi_base_module_t* bmi,
    struct mca_bmi_base_endpoint_t* peer,
    struct ompi_convertor_t* convertor,
    size_t reserve,
    size_t* size
);

/**
 * Pack data and return a descriptor that can be
 * used for send/put.
 *
 * @param bmi (IN)      BMI module
 * @param peer (IN)     BMI peer addressing
 */
extern mca_bmi_base_descriptor_t* mca_bmi_ib_prepare_dst( 
                                                         struct mca_bmi_base_module_t* bmi, 
                                                         struct mca_bmi_base_endpoint_t* peer,
                                                         struct ompi_convertor_t* convertor,
                                                         size_t reserve,
                                                         size_t* size); 
    /**
 * Return a send fragment to the modules free list.
 *
 * @param bmi (IN)   BMI instance
 * @param frag (IN)  IB send fragment
 *
 */
extern void mca_bmi_ib_send_frag_return(
    struct mca_bmi_base_module_t* bmi,
    struct mca_bmi_ib_frag_t*
);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif
#endif
