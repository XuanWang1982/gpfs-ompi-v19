/*
 * Copyright (c) 2009-2010 The Trustees of Indiana University.
 *                         All rights reserved.
 *
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/**
 * @file
 * 
 * Automatic Recovery Errmgr component
 *
 */

#ifndef MCA_ERRMGR_AUTOR_EXPORT_H
#define MCA_ERRMGR_AUTOR_EXPORT_H

#include "orte_config.h"

#include "opal/mca/mca.h"
#include "opal/event/event.h"

#include "orte/mca/filem/filem.h"
#include "orte/mca/errmgr/errmgr.h"

BEGIN_C_DECLS

    /*
     * Local Component structures
     */
    struct orte_errmgr_autor_component_t {
        orte_errmgr_base_component_t super;  /** Base Errmgr component */
        bool autor_enabled;
        bool timing_enabled;
        int  recovery_delay;
        bool skip_oldnode;
    };
    typedef struct orte_errmgr_autor_component_t orte_errmgr_autor_component_t;
    OPAL_MODULE_DECLSPEC extern orte_errmgr_autor_component_t mca_errmgr_autor_component;

    int orte_errmgr_autor_component_query(mca_base_module_t **module, int *priority);

    /*
     * Module functions: Global
     */
    int orte_errmgr_autor_global_module_init(void);
    int orte_errmgr_autor_global_module_finalize(void);

    int orte_errmgr_autor_global_update_state(orte_jobid_t job,
                                              orte_job_state_t jobstate,
                                              orte_process_name_t *proc_name,
                                              orte_proc_state_t state,
                                              pid_t pid,
                                              orte_exit_code_t exit_code,
                                              orte_errmgr_stack_state_t *stack_state);
    int orte_errmgr_autor_global_process_fault(orte_job_t *jdata,
                                               orte_process_name_t *proc_name,
                                               orte_proc_state_t state,
                                               orte_errmgr_stack_state_t *stack_state);
    int orte_errmgr_autor_global_suggest_map_targets(orte_proc_t *proc,
                                                     orte_node_t *oldnode,
                                                     opal_list_t *node_list,
                                                     orte_errmgr_stack_state_t *stack_state);
    int orte_errmgr_autor_global_ft_event(int state);

    /*
     * Module functions: Local (Daemon)
     */
    int orte_errmgr_autor_local_module_init(void);
    int orte_errmgr_autor_local_module_finalize(void);

    int orte_errmgr_autor_local_update_state(orte_jobid_t job,
                                             orte_job_state_t jobstate,
                                             orte_process_name_t *proc_name,
                                             orte_proc_state_t state,
                                             pid_t pid,
                                             orte_exit_code_t exit_code,
                                             orte_errmgr_stack_state_t *stack_state);
    int orte_errmgr_autor_local_ft_event(int state);


END_C_DECLS

#endif /* MCA_ERRMGR_AUTOR_EXPORT_H */
