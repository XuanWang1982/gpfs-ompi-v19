/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011      Los Alamos National Security, LLC.
 *                         All rights reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "orte_config.h"
#include "orte/constants.h"

#if !ORTE_DISABLE_FULL_SUPPORT

#include "opal/util/output.h"
#include "opal/mca/mca.h"
#include "opal/mca/base/base.h"


#include "orte/mca/plm/plm.h"
#include "orte/mca/plm/base/plm_private.h"

#endif

#include "orte/mca/plm/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * module's public mca_base_module_t struct.
 */

#include "orte/mca/plm/base/static-components.h"

#if ORTE_DISABLE_FULL_SUPPORT
/* have to include a bogus function here so that
 * the build system sees at least one function
 * in the library
 */
int orte_plm_base_open(void)
{
    return ORTE_SUCCESS;
}

#else


/*
 * Global public variables
 */
orte_plm_base_t orte_plm_base;

/*
 * Global variables for use within PLM frameworks
 */
orte_plm_globals_t orte_plm_globals;

/*
 * The default module
 */
orte_plm_base_module_t orte_plm = {
    orte_plm_proxy_init,
    NULL,   /* cannot set hnp name in a proxy */
    orte_plm_proxy_spawn,
    NULL,   /* cannot remotely spawn by default */
    NULL,   /* cannot terminate job from a proxy */
    NULL,   /* cannot terminate orteds from a proxy */
    NULL,   /* cannot terminate procs from a proxy */
    NULL,   /* cannot signal job from a proxy */
    orte_plm_proxy_finalize
};


/**
 * Function for finding and opening either all MCA modules, or the one
 * that was specifically requested via a MCA parameter.
 */
int orte_plm_base_open(void)
{
    int value;

    /* Debugging / verbose output.  Always have stream open, with
       verbose set by the mca open system... */
    orte_plm_globals.output = opal_output_open(NULL);
    
    /* init selected to be false */
    orte_plm_base.selected = false;

    /* init the next jobid */
    orte_plm_globals.next_jobid = 1;
    
    /* default to assigning daemons to nodes at launch */
    orte_plm_globals.daemon_nodes_assigned_at_launch = true;

    mca_base_param_reg_int_name("plm", "base_strip_prefix_from_node_names",
                                "Whether to strip leading characters and zeroes from node names returned by daemons",
                                false, false, (int)false, &value);
    orte_plm_globals.strip_prefix_from_node_names = OPAL_INT_TO_BOOL(value);

    /* Open up all the components that we can find */

    if (ORTE_SUCCESS != 
        mca_base_components_open("plm", orte_plm_globals.output,
                                 mca_plm_base_static_components, 
                                 &orte_plm_base.available_components, true)) {
       return ORTE_ERROR;
    }
    
    /* All done */

    return ORTE_SUCCESS;
}

#endif
