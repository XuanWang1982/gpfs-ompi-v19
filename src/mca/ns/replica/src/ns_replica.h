/* -*- C -*-
 * 
 * $HEADER$
 *
 */
#ifndef NS_REPLICA_H
#define NS_REPLICA_H


#include "ompi_config.h"
#include "include/types.h"
#include "include/constants.h"
#include "class/ompi_list.h"
#include "mca/oob/oob.h"
#include "mca/ns/ns.h"

/*
 * list class for tracking vpids/jobid
 * This structure is used to create a linked list of jobid-max vpid pairs. Basically, we
 * are tracking the max used vpid for each jobid that has been created.
 */
struct mca_ns_replica_name_tracker_t {
    ompi_list_item_t item;  /**< Allows this item to be placed on a list */
    mca_ns_base_jobid_t job;  /**< Job id */
    mca_ns_base_vpid_t last_used_vpid;      /**< Tracks the vpid last given out */
};
typedef struct mca_ns_replica_name_tracker_t mca_ns_replica_name_tracker_t;

OBJ_CLASS_DECLARATION(mca_ns_replica_name_tracker_t);

/*
 * globals needed within component
 */
extern mca_ns_base_cellid_t mca_ns_replica_last_used_cellid;
extern mca_ns_base_jobid_t mca_ns_replica_last_used_jobid;
extern ompi_list_t mca_ns_replica_name_tracker;

/*
 * Module open / close
 */
int mca_ns_replica_open(void);
int mca_ns_replica_close(void);


/*
 * Startup / Shutdown
 */
mca_ns_t* mca_ns_replica_init(bool *allow_multi_user_threads, bool *have_hidden_threads, int *priority);
int mca_ns_replica_finalize(void);

/*
 * oob interface
 */

mca_oob_callback_fn_t mca_ns_replica_recv(int status, const ompi_process_name_t *sender,
					  const struct iovec *msg, size_t count,
					  void *cbdata);

/*
 * Implementation of create_cellid().
 */
mca_ns_base_cellid_t ns_replica_create_cellid(void);

/*
 * Implementation of create_jobid().
 */
mca_ns_base_jobid_t ns_replica_create_jobid(void);

/*
 * Implementation of create_process_name()
 */
ompi_process_name_t* ns_replica_create_process_name(
			    mca_ns_base_cellid_t cell,
			    mca_ns_base_jobid_t job,
			    mca_ns_base_vpid_t vpid);

/*
 * Implementation of reserve_range()
 */
mca_ns_base_vpid_t ns_replica_reserve_range(
                                       mca_ns_base_jobid_t job,
                                       mca_ns_base_vpid_t range);

/*
 * Implementation of free_name()
 */
int ns_replica_free_name(ompi_process_name_t* name);

/*
 * Implementation of get_proc_name_string()
 */
char* ns_replica_get_proc_name_string(const ompi_process_name_t* name);

/*
 * Implementation of get_vpid_string()
 */
char* ns_replica_get_vpid_string(const ompi_process_name_t* name);

/*
 * Implementation of get_jobid_string()
 */
char* ns_replica_get_jobid_string(const ompi_process_name_t* name);

/*
 * Implementation of get_cellid_string()
 */
char* ns_replica_get_cellid_string(const ompi_process_name_t* name);

/*
 * Implementation of get_vpid()
 */
mca_ns_base_vpid_t ns_replica_get_vpid(const ompi_process_name_t* name);

/*
 * Implementation of get_jobid()
 */
mca_ns_base_jobid_t ns_replica_get_jobid(const ompi_process_name_t* name);

/*
 * Implementation of get_cellid()
 */
mca_ns_base_cellid_t ns_replica_get_cellid(const ompi_process_name_t* name);

/*
 * Implementation of compare()
 */
int ns_replica_compare(ompi_ns_cmp_bitmask_t fields,
		       const ompi_process_name_t* name1,
		       const ompi_process_name_t* name2);

#endif
