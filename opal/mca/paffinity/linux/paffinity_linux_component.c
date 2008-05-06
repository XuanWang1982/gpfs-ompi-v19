/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Cisco, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 * These symbols are in a file by themselves to provide nice linker
 * semantics.  Since linkers generally pull in symbols by object
 * files, keeping these symbols as the only symbols in this file
 * prevents utility programs such as "ompi_info" from having to import
 * entire components just to query their version and parameters.
 */

#include "opal_config.h"

#include "opal/constants.h"
#include "opal/mca/paffinity/paffinity.h"
#include "paffinity_linux.h"

/*
 * Public string showing the paffinity ompi_linux component version number
 */
const char *opal_paffinity_linux_component_version_string =
    "OPAL linux paffinity MCA component version " OPAL_VERSION;

/*
 * Local function
 */
static int linux_open(void);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */

const opal_paffinity_base_component_1_1_0_t mca_paffinity_linux_component = {

    /* First, the mca_component_t struct containing meta information
       about the component itself */

    {
        /* Indicate that we are a paffinity v1.1.0 component (which also
           implies a specific MCA version) */
        
        OPAL_PAFFINITY_BASE_VERSION_1_1_0,

        /* Component name and version */

        "linux",
        OPAL_MAJOR_VERSION,
        OPAL_MINOR_VERSION,
        OPAL_RELEASE_VERSION,

        /* Component open and close functions */

        linux_open,
        NULL,
        opal_paffinity_linux_component_query
    },
    {
        /* The component is checkpoint ready */
        MCA_BASE_METADATA_PARAM_CHECKPOINT
    }
};


static int linux_open(void)
{
    mca_base_param_reg_int(&mca_paffinity_linux_component.base_version,
                           "priority",
                           "Priority of the linux paffinity component",
                           false, false, 10, NULL);

    return OPAL_SUCCESS;
}
