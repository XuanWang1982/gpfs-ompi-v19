/* -*- C -*-
 *
 * $HEADER$
 */

#include "ompi_config.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

#include "include/constants.h"

#include "event/event.h"
#include "util/proc_info.h"
#include "util/argv.h"
#include "util/cmd_line.h"
#include "util/sys_info.h"
#include "util/session_dir.h"
#include "util/output.h"
#include "util/os_path.h"
#include "util/universe_setup_file_io.h"

#include "mca/base/base.h"
#include "mca/ns/ns.h"
#include "mca/ns/base/base.h"
#include "mca/pcm/base/base.h"
#include "mca/oob/base/base.h"

#include "runtime/runtime.h"
#include "runtime/ompi_rte_wait.h"

extern char** environ;


int
main(int argc, char *argv[])
{
    bool multi_thread = false;
    bool hidden_thread = false;
    int ret;
    ompi_cmd_line_t *cmd_line = NULL;
    ompi_list_t *nodelist = NULL;
    ompi_list_t schedlist;
    mca_ns_base_jobid_t new_jobid, jobid;
    mca_ns_base_vpid_t vpid;
    int num_procs = 1, rc;
    ompi_rte_node_schedule_t *sched;
    char cwd[MAXPATHLEN];
    char *my_contact_info, *tmp, *jobid_str, *procid_str;
    char *contact_file, *filenm, *universe, *segment;
    pid_t pid;
    ompi_rte_spawn_handle_t *spawn_handle;

    /*
     * Intialize our Open MPI environment
     */
    cmd_line = OBJ_NEW(ompi_cmd_line_t);

    if (OMPI_SUCCESS != ompi_init(argc, argv)) {
        /* BWB show_help */
        printf("show_help: ompi_init failed\n");
        return ret;
    }

    /* setup to read common command line options that span all Open MPI programs */
    cmd_line = OBJ_NEW(ompi_cmd_line_t);

    ompi_cmd_line_make_opt(cmd_line, 'v', "version", 0,
			   "Show version of Open MPI and this program");

    ompi_cmd_line_make_opt(cmd_line, 'h', "help", 0,
			   "Show help for this function");


    /* setup rte command line arguments */
    ompi_rte_cmd_line_setup(cmd_line);

    /*
     * setup  mca command line arguments
     */
    if (OMPI_SUCCESS != (ret = mca_base_cmd_line_setup(cmd_line))) {
	/* BWB show_help */
	printf("show_help: mca_base_cmd_line_setup failed\n");
	return ret;
    }

    if (OMPI_SUCCESS != mca_base_cmd_line_process_args(cmd_line)) {
	/* BWB show_help */
	printf("show_help: mca_base_cmd_line_process_args\n");
	return ret;
    }

    /* parse the local commands */
    if (OMPI_SUCCESS != ompi_cmd_line_parse(cmd_line, true, argc, argv)) {
	exit(ret);
    }

    if (ompi_cmd_line_is_taken(cmd_line, "help") || 
        ompi_cmd_line_is_taken(cmd_line, "h")) {
        printf("...showing ompi_info help message...\n");
        exit(1);
    }

    if (ompi_cmd_line_is_taken(cmd_line, "version") ||
	ompi_cmd_line_is_taken(cmd_line, "v")) {
	printf("...showing off my version!\n");
	exit(1);
    }

    /*
     * Setup mpirun-specific command line arguments
     */
    ompi_cmd_line_make_opt3(cmd_line, 'n', "np", "np", 1,
                            "Number of processes to start");
    ompi_cmd_line_make_opt3(cmd_line, '\0', "hostfile", "hostfile", 1,
			    "Host description file");

    if (OMPI_SUCCESS != ompi_cmd_line_parse(cmd_line, true, argc, argv) ||
        ompi_cmd_line_is_taken(cmd_line, "help") || 
        ompi_cmd_line_is_taken(cmd_line, "h")) {
        printf("...showing ompi_info help message...\n");
        exit(1);
    }

    if (OMPI_SUCCESS != mca_base_cmd_line_process_args(cmd_line)) {
        /* BWB show_help */
        printf("show_help: mca_base_cmd_line_process_args\n");
        return ret;
    }

    /* get our numprocs */
    if (ompi_cmd_line_is_taken(cmd_line, "np")) {
        num_procs = atoi(ompi_cmd_line_get_param(cmd_line, "np", 0, 0));
    }

    /*
     * Start the Open MPI Run Time Environment
     */
    if (OMPI_SUCCESS != (ret = mca_base_open())) {
        /* JMS show_help */
        printf("show_help: mca_base_open failed\n");
        return ret;
    }

    multi_thread = true;
    hidden_thread=false;
    if (OMPI_SUCCESS != ompi_rte_init_stage1(&multi_thread, &hidden_thread)) {
        /* JMS show_help */
        printf("show_help: mpirun failed in ompi_rte_init\n");
	return ret;
    }

    /* parse environmental variables and fill corresponding info structures
     * need the oob to be open so we can pass the contact info we extract
     */
    ompi_rte_parse_environ();

    /* parse the cmd_line for rte options - override settings from enviro, where necessary
     * copy everything into enviro variables for passing later on
     */
    ompi_rte_parse_cmd_line(cmd_line);

    /* parse the cmd_line for daemon options - gets all the options relating
     * specifically to seed behavior, in case i'm a seed, but also gets
     * options about scripts and hostfiles that might be of use to me
     * overrride enviro variables where necessary
     */
    ompi_rte_parse_daemon_cmd_line(cmd_line);

    /* check for existing universe to join */
    if (OMPI_SUCCESS != (ret = ompi_rte_universe_exists())) {
	if (ompi_rte_debug_flag) {
	    ompi_output(0, "mpirun: could not join existing universe");
	}
	if (OMPI_ERR_NOT_FOUND != ret) {
	    /* if it exists but no contact could be established,
	     * define unique name based on current one.
	     * and start new universe with me as seed
	     */
	    universe = strdup(ompi_universe_info.name);
	    free(ompi_universe_info.name);
	    ompi_universe_info.name = NULL;
	    pid = getpid();
	    if (0 > asprintf(&ompi_universe_info.name, "%s-%d", universe, pid) && ompi_rte_debug_flag) {
		ompi_output(0, "mpi_init: error creating unique universe name");
	    }
	}

	ompi_process_info.my_universe = strdup(ompi_universe_info.name);
	ompi_process_info.seed = true;
	if (NULL != ompi_universe_info.ns_replica) {
	    free(ompi_universe_info.ns_replica);
	    ompi_universe_info.ns_replica = NULL;
	}
	if (NULL != ompi_process_info.ns_replica) {
	    free(ompi_process_info.ns_replica);
	    ompi_process_info.ns_replica = NULL;
	}
	if (NULL != ompi_universe_info.gpr_replica) {
	    free(ompi_universe_info.gpr_replica);
	    ompi_universe_info.gpr_replica = NULL;
	}
	if (NULL != ompi_process_info.gpr_replica) {
	    free(ompi_process_info.gpr_replica);
	    ompi_process_info.gpr_replica = NULL;
	}
    }

    /* setup rest of rte */
    if (OMPI_SUCCESS != ompi_rte_init_stage2(&multi_thread, &hidden_thread)) {
	/* BWB show_help */
	printf("show_help: ompi_rte_init failed\n");
	return ret;
    }

    /*****    SET MY NAME   *****/
    if (NULL != ompi_process_info.name) { /* should NOT have been set yet */
	free(ompi_process_info.name);
	ompi_process_info.name = NULL;
    }

    if (ompi_process_info.seed) {
	ompi_process_info.name = ompi_name_server.create_process_name(0, 0, 0);
    } else { /* if not seed, then we joined universe - get jobid and name */
	jobid = ompi_name_server.create_jobid();
	vpid = ompi_name_server.reserve_range(jobid, 1);
	ompi_process_info.name = ompi_name_server.create_process_name(0, jobid, vpid);
    }

    /* setup my session directory */
    jobid_str = ompi_name_server.get_jobid_string(ompi_process_info.name);
    procid_str = ompi_name_server.get_vpid_string(ompi_process_info.name);
 
    if (ompi_rte_debug_flag) {
	ompi_output(0, "[%d,%d,%d] setting up session dir with", ompi_process_info.name->cellid, ompi_process_info.name->jobid, ompi_process_info.name->vpid);
	if (NULL != ompi_process_info.tmpdir_base) {
	    ompi_output(0, "\ttmpdir %s", ompi_process_info.tmpdir_base);
	}
	ompi_output(0, "\tuniverse %s", ompi_process_info.my_universe);
	ompi_output(0, "\tuser %s", ompi_system_info.user);
	ompi_output(0, "\thost %s", ompi_system_info.nodename);
	ompi_output(0, "\tjobid %s", jobid_str);
	ompi_output(0, "\tprocid %s", procid_str);
    }
    if (OMPI_ERROR == ompi_session_dir(true,
				       ompi_process_info.tmpdir_base,
				       ompi_system_info.user,
				       ompi_system_info.nodename, NULL, 
				       ompi_process_info.my_universe,
				       jobid_str, procid_str)) {
	if (jobid_str != NULL) free(jobid_str);
	if (procid_str != NULL) free(procid_str);
	exit(-1);
    }

    /* finalize the rte startup */
    if (OMPI_SUCCESS != (ret = ompi_rte_init_finalstage(&multi_thread,
							&hidden_thread))) {
	/* JMS show_help */
	printf("show_help: ompid failed in ompi_rte_init\n");
	return ret;
    }

    /* if i'm the seed, get my contact info and write my setup file for others to find */
    if (ompi_process_info.seed) {
	if (NULL != ompi_universe_info.seed_contact_info) {
	    free(ompi_universe_info.seed_contact_info);
	    ompi_universe_info.seed_contact_info = NULL;
	}
	ompi_universe_info.seed_contact_info = mca_oob_get_contact_info();
	contact_file = ompi_os_path(false, ompi_process_info.universe_session_dir,
				    "universe-setup.txt", NULL);

	if (OMPI_SUCCESS != (ret = ompi_write_universe_setup_file(contact_file))) {
	    if (ompi_rte_debug_flag) {
		ompi_output(0, "[%d,%d,%d] ompid: couldn't write setup file", ompi_process_info.name->cellid,
			    ompi_process_info.name->jobid, ompi_process_info.name->vpid);
	    }
	}
    }

    /*****    PREP TO START THE APPLICATION    *****/

    /* get the jobid for the application */
    new_jobid = ompi_name_server.create_jobid();

    /* get the spawn handle to start spawning stuff */
    spawn_handle = ompi_rte_get_spawn_handle(OMPI_RTE_SPAWN_HIGH_QOS, true);

    /* BWB - fix jobid, procs, and nodes */
    nodelist = ompi_rte_allocate_resources(spawn_handle, new_jobid, 0, num_procs);
    if (NULL == nodelist) {
	/* BWB show_help */
	printf("show_help: ompi_rte_allocate_resources failed\n");
	return -1;
    }

    /*
     * Process mapping
     */
    OBJ_CONSTRUCT(&schedlist,  ompi_list_t);
    sched = OBJ_NEW(ompi_rte_node_schedule_t);
    ompi_list_append(&schedlist, (ompi_list_item_t*) sched);
    ompi_cmd_line_get_tail(cmd_line, &(sched->argc), &(sched->argv));

    /*
     * build environment to be passed
     */
    mca_pcm_base_build_base_env(environ, &(sched->envc), &(sched->env));
    /* set initial contact info */
    if (ompi_process_info.seed) {  /* i'm the seed - direct them towards me */
	my_contact_info = mca_oob_get_contact_info();
    } else { /* i'm not the seed - direct them to it */
	my_contact_info = strdup(ompi_universe_info.ns_replica);
    }
    asprintf(&tmp, "OMPI_MCA_ns_base_replica=%s", my_contact_info);
    ompi_argv_append(&(sched->envc), &(sched->env), tmp);
    free(tmp);
    asprintf(&tmp, "OMPI_MCA_gpr_base_replica=%s", my_contact_info);
    ompi_argv_append(&(sched->envc), &(sched->env), tmp);
    free(tmp);
    if (NULL != ompi_universe_info.name) {
	asprintf(&tmp, "OMPI_universe_name=%s", ompi_universe_info.name);
	ompi_argv_append(&(sched->envc), &(sched->env), tmp);
	free(tmp);
    }
    if (ompi_cmd_line_is_taken(cmd_line, "tmpdir")) {  /* user specified the tmp dir base */
	asprintf(&tmp, "OMPI_tmpdir_base=%s", ompi_cmd_line_get_param(cmd_line, "tmpdir", 0, 0));
	ompi_argv_append(&(sched->envc), &(sched->env), tmp);
	free(tmp);
    }

    getcwd(cwd, MAXPATHLEN);
    sched->cwd = strdup(cwd);
    sched->nodelist = nodelist;

    if (sched->argc == 0) {
	printf("no app to start\n");
	return 1;
    }


    /*
     * register to monitor the startup and shutdown processes
     */
    /* setup segment for this job */
    asprintf(&segment, "ompi-job-%d", new_jobid);

    /* register a synchro on the segment so we get notified when everyone registers */
    rc = ompi_registry.synchro(
	 OMPI_REGISTRY_SYNCHRO_MODE_LEVEL|OMPI_REGISTRY_SYNCHRO_MODE_ONE_SHOT,
	 OMPI_REGISTRY_OR,
	 segment,
	 NULL,
	 num_procs,
	 ompi_rte_all_procs_registered, NULL);
    /* register a synchro on the segment so we get notified when everyone is gone
     */
    rc = ompi_registry.synchro(
         OMPI_REGISTRY_SYNCHRO_MODE_DESCENDING|OMPI_REGISTRY_SYNCHRO_MODE_ONE_SHOT,
         OMPI_REGISTRY_OR,
         segment,
         NULL,
         0,
         ompi_rte_all_procs_unregistered, NULL);



    /*
     * spawn procs
     */
    if (OMPI_SUCCESS != ompi_rte_spawn_procs(spawn_handle, new_jobid, &schedlist)) {
	printf("show_help: woops!  we didn't spawn :( \n");
	return -1;
    }
    
   
    if (OMPI_SUCCESS != ompi_rte_monitor_procs_registered()) {
	printf("procs didn't all register - aborting\n");
    } else {
	ompi_rte_monitor_procs_unregistered();
    }
    /*
     *   - ompi_rte_kill_job()
     */

    /*
     * Clean up
     */
    if (NULL != nodelist) ompi_rte_deallocate_resources(spawn_handle, new_jobid, nodelist);
    if (NULL != cmd_line) OBJ_RELEASE(cmd_line);
    if (NULL != spawn_handle) OBJ_RELEASE(spawn_handle);

    /* eventually, mpirun won't be the seed and so won't have to do this.
     * for now, though, remove the universe-setup.txt file so the directories
     * can cleanup
     */
    if (ompi_process_info.seed) {
	filenm = ompi_os_path(false, ompi_process_info.universe_session_dir, "universe-setup.txt", NULL);
	unlink(filenm);
    }

    /* finalize the system */
    /* BWB - fix me hack.  Close down the event library.  This has to
       happen now and not as part of ompi_rte_finalize (where it
       should be) because of a bug in the TCP PTL that exists only if
       the event library is open when the module is shut down.  So
       shut down everything that depends on the event library here
       (rather than the rte_finalize where it should be).

       Further note: we do this before rte_finalize even in MPIRUN so
       that we match pound for pound the shutdown sequence of
       MPI_Finalize*/
    ompi_rte_wait_finalize();
    ompi_event_fini();
  /* BWB - end fix me hack */
    ompi_rte_finalize();
    mca_base_close();
    ompi_finalize();

    OBJ_DESTRUCT(&schedlist);
    return 0;
}

