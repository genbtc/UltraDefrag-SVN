/*
 *  UltraDefrag - a powerful defragmentation tool for Windows NT.
 *  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**
 * @file udefrag.c
 * @brief Entry point.
 * @details Each of the disk processing algorithms
 * should comply with the following rules:
 * -# never try to move directories on FAT entirely
 * -# never try to move MFT on NTFS entirely
 * -# work on disks with low amount of free space
 * -# sort primarily small files, because big files sorting is useless
 * -# save more time than needed to complete the disk processing itself
 * -# terminate quickly on already processed disks
 * -# don't sort out all the files when just a few files
 *    have been changed since the last optimization
 * -# advance progress indication from 0 to 100%
 * -# cleanup as much space as possible before use of the cleaned up space;
 *    otherwise NTFS processing will be slow
 * -# don't optimize any file twice
 * -# never go into infinite loops
 * -# handle correctly either normal or compressed/sparse files
 * -# handle correctly locked files
 * -# distinguish between file blocks and file fragments
 * -# handle the case when a region assumed to
 *    be free turns out to be already in use
 * -# filter files properly
 * -# produce reports properly
 *
 * How the statistical data adjusts in all the volume processing routines:
 * -# we calculate the maximum amount of data which may be moved in process
 *    and assign this value to jp->pi.clusters_to_process counter
 * -# when we move something, we adjust jp->pi.processed_clusters
 * -# before each pass we adjust the jp->pi.clusters_to_process value
 *    to prevent the progress indicator overflow
 * @addtogroup Engine
 * @{
 */

/*
* Revised by Stefan Pendl, 2010, 2011
* <stefanpe@users.sourceforge.net>
*/

#include "udefrag-internals.h"

//Globals
//-----------------------------------------------
HANDLE hMutex = NULL;

//see functions below for reason/meanings:
BOOL gui_finished = FALSE;      //related to gui_fileslist_finished()
BOOL wait_delete_thread_finished = FALSE; //related to wait_delete_lists_thread()
//------------------------------------------------
/**
 * @brief Initializes udefrag library.
 * @details This routine needs to be called
 * once before any use of other routines.
 * @return Zero for success, negative value otherwise.
 * @note This routine initializes zenwinx library as well.
 */
int udefrag_init_library(void)
{
    if(winx_init_library() < 0)
        return (-1);

    /* deny installation/upgrade */
    if(winx_get_os_version() >= WINDOWS_VISTA){
        (void)winx_create_mutex(L"\\Sessions\\1"
            L"\\BaseNamedObjects\\ultradefrag_mutex",
            &hMutex);
    } else {
        (void)winx_create_mutex(L"\\BaseNamedObjects"
            L"\\ultradefrag_mutex",&hMutex);
    }
    return 0;
}

/**
 * @brief Releases resources 
 * acquired by udefrag library.
 * @note
 * - Releases zenwinx resources as well.
 * - Call it ONLY if you know what you're doing.
 */
void udefrag_unload_library(void)
{
    /* allow installation/upgrade */
    winx_destroy_mutex(hMutex);

    winx_unload_library();
}

/**
 * @internal
 * @brief Delivers progress information to the caller.
 * @note 
 * - This routine delivers the completion_status parameter
 * to the caller instead of the appropriate field of the
 * jp->pi structure.
 * - If a cell of the cluster map is occupied entirely by the
 * MFT zone it will be drawn in light magenta only when it's
 * completely free. Otherwise, it will be drawn using another
 * color (according to its contents) to show explicitly that
 * some files are is still there.
 */
void deliver_progress_info(udefrag_job_parameters *jp,int completion_status)
{
    udefrag_progress_info pi;
    int i, k, index, p1, p2;
    int mft_zone_detected;
    int free_cell_detected;
    ULONGLONG maximum, n;
    
    if(jp->cb == NULL)
        return;

    /* make a copy of jp->pi */
    memcpy(&pi,&jp->pi,sizeof(udefrag_progress_info));
    
    /* replace completion status */
    pi.completion_status = completion_status;
    
    /* calculate progress percentage */
    pi.percentage = calc_percentage(jp->pi.processed_clusters,jp->pi.clusters_to_process);
//Note: After the analyze job is finished: 
//   processed_clusters seems to be 2 clusters shy of clusters_to_process.
//    dtrace("proc_clus: %I64u",jp->pi.processed_clusters);
//    dtrace("Clus_to_proc: %I64u",jp->pi.clusters_to_process);
//    dtrace("bad_frags: %I64u",jp->pi.bad_fragments);
//    dtrace("all_Frags: %I64u",jp->pi.fragments);
//    dtrace("bad_Clus: %I64u",jp->pi.bad_clusters);
//    dtrace("used_Clus: %I64u",jp->pi.used_clusters);
    
    /* calculate fragmentation percentage #1 (original) (bad frags / total frags) */
    //pi.fragmentation = calc_percentage(jp->pi.bad_fragments,jp->pi.fragments);
    /* calculate fragmentation percentage #2 (bad clusters / used clusters) */
    pi.fragmentation = calc_percentage(jp->pi.bad_clusters,jp->pi.used_clusters);
    
    /* refill cluster map */
    if(jp->pi.cluster_map && jp->cluster_map.array \
      && jp->pi.cluster_map_size == jp->cluster_map.map_size){
        for(i = 0; i < jp->cluster_map.map_size; i++){
            /* check for mft zone to apply special rules there */
            mft_zone_detected = free_cell_detected = 0;
            maximum = 1; /* for jp->cluster_map.opposite_order */
            if(!jp->cluster_map.opposite_order){
                if(i == jp->cluster_map.map_size - jp->cluster_map.unused_cells - 1)
                    maximum = jp->cluster_map.clusters_per_last_cell;
                else
                    maximum = jp->cluster_map.clusters_per_cell;
            }
            if(jp->cluster_map.array[i][MFT_ZONE_SPACE] >= maximum)
                mft_zone_detected = 1;
            if(jp->cluster_map.array[i][FREE_SPACE] >= maximum)
                free_cell_detected = 1;
            if(mft_zone_detected && free_cell_detected){
                jp->pi.cluster_map[i] = MFT_ZONE_SPACE;
            } else {
                maximum = jp->cluster_map.array[i][0];
                index = 0;
                for(k = 1; k < jp->cluster_map.n_colors; k++){
                    n = jp->cluster_map.array[i][k];
                    if(n >= maximum){ /* support of colors precedence  */
                        if((k != MFT_ZONE_SPACE && k != FREE_SPACE) || !mft_zone_detected){
                            maximum = n;
                            index = k;
                        }
                    }
                }
                if(maximum == 0)
                    jp->pi.cluster_map[i] = DEFAULT_COLOR;
                else
                    jp->pi.cluster_map[i] = (char)index;
            }
        }
    }
    
    /* deliver information to the caller */
    jp->cb(&pi,jp->p);
    /* where stuff happens ^ */
    jp->progress_refresh_time = winx_xtime();
    if(jp->udo.dbgprint_level >= DBG_PARANOID)
        winx_dbg_print_header(0x20,0,D"progress update");
        
    if(jp->udo.dbgprint_level >= DBG_DETAILED){
        p1 = (int)(__int64)(pi.percentage * 100.00);
        p2 = p1 % 100;
        p1 = p1 / 100;
        
        if(p1 >= jp->progress_trigger){
            winx_dbg_print_header('>',0,D"progress %3u.%02u%% completed, "
                "trigger %3u", p1, p2, jp->progress_trigger);
            jp->progress_trigger = (p1 / 10) * 10 + 10;
        }
    }
}

/**
 */
static int killer(void *p)
{
    winx_dbg_print_header(0,0,I"*");
    winx_dbg_print_header(0x20,0,I"termination requested by caller");
    winx_dbg_print_header(0,0,I"*");
    return 1;
}

/**
 */
static int terminator(void *p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int result;

    /* ask caller */
    if(jp->t){
        result = jp->t(jp->p);
        if(result)
            killer(jp->p);
        return result;
    }

    /* continue */
    return 0;
}
BOOL listsCleaned = FALSE;

/**
 * \brief Runs the start of the job. Repeat Main Function.
 * \param p is a pointer to the job.
 * \return status
 */
static DWORD WINAPI start_job(LPVOID p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    char *action;
    int result = -1;
    listsCleaned = FALSE;

    /* check job flags */
    if(jp->udo.job_flags & UD_JOB_REPEAT)
        itrace("repeat action until nothing left to move");
    
    /* do the job */
    if(jp->job_type == DEFRAGMENTATION_JOB) action = "Defragmentation";
    else if(jp->job_type == FULL_OPTIMIZATION_JOB) action = "Full optimization";
    else if(jp->job_type == QUICK_OPTIMIZATION_JOB) action = "Quick optimization";
    else if(jp->job_type == MFT_OPTIMIZATION_JOB) action = "MFT optimization";
    else if(jp->job_type == SINGLE_FILE_MOVE_FRONT_JOB) action = "Single File Move to Front";
    else if(jp->job_type == SINGLE_FILE_MOVE_END_JOB) action = "Single File Move to End";
    else action = "Analysis";

    winx_dbg_print_header(0,0,I"%s of disk %c: started",action,jp->volume_letter);
    remove_fragmentation_report(jp);
    (void)winx_vflush(jp->volume_letter); /* flush all file buffers */
    
    /* speedup file searching in optimization */
    if(jp->job_type != ANALYSIS_JOB \
    || jp->job_type != DEFRAGMENTATION_JOB)
        create_file_blocks_tree(jp);

    switch(jp->job_type){
    case ANALYSIS_JOB:
        result = analyze(jp);
        break;
    case DEFRAGMENTATION_JOB:
        result = defragment(jp);
        break;
    case FULL_OPTIMIZATION_JOB:
    case QUICK_OPTIMIZATION_JOB:
        result = optimize(jp);
        break;
    case MFT_OPTIMIZATION_JOB:
        result = optimize_mft(jp);
        break;
    case SINGLE_FILE_MOVE_FRONT_JOB:
        result = movefile_to_start_or_end(jp,1);
        break;
    case SINGLE_FILE_MOVE_END_JOB:
        result = movefile_to_start_or_end(jp,0);
        break;
    default:
        break;
    }

    destroy_file_blocks_tree(jp);
    if(jp->job_type != ANALYSIS_JOB)
        release_temp_space_regions(jp);
    (void)save_fragmentation_report(jp);

    /* now it is safe to adjust the completion status */
    jp->pi.completion_status = result;
    if(jp->pi.completion_status == 0)
        jp->pi.completion_status ++; /* success */
    
    winx_exit_thread(0); /* 8k/12k memory leak here?   ???whocares... */
    return 0;
    //Goes back to udefrag_start_job@line475
}

/**
 * @internal
 * @brief Destroys the list of free regions, the
 * list of files and the list of fragmented files.
 */

void destroy_lists(udefrag_job_parameters *jp)
{
    ULONGLONG start,end;
    start = winx_xtime();
    if (jp->fragmented_files) {
        dtrace("Destroying PRB jp->fragmented_files for Drive: %c...",jp->volume_letter);
        prb_destroy(jp->fragmented_files, NULL);
        jp->fragmented_files = NULL;
    }
    if (jp->filelist) {
        dtrace("Releasing jp->filelist...");
        winx_ftw_release(jp->filelist);
    }
    if (jp->free_regions) {
        dtrace("Releasing jp->free_regions...");
        winx_release_free_volume_regions(jp->free_regions);
    }
    end = winx_xtime();
    dtrace("Cleanup Finished. The list-deletion took: %d msec.",end-start);
    listsCleaned = TRUE;
}

/**
 * @brief Sub-thread to handle the filelists deletion after GUI finishes.
 * Needs to wait until it gets a response. 
 * Signals that lists have been deleted, memory has been cleared 
 * and the parent thread can now exit safely.
 * @return Afterwards, code flows back to the end of udefrag_start_job()
 */ 
static DWORD WINAPI wait_delete_lists_thread(LPVOID p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int delwaitcount = 0;
    while(!gui_finished)
    {
        delwaitcount++;
        dtrace("Sleeping 250ms waiting for GUI to come back. Wait Count = %d", delwaitcount);
        winx_sleep(250);
    }
    if (jp == NULL)
    {
        etrace("Error. Trying to clear a list thats already gone. JP was null.");
        return 1;
    }
    destroy_lists(jp);
    wait_delete_thread_finished = TRUE;
    winx_exit_thread(0);
    return 0;
}


/**
 * @brief Passthrough so the GUI can destroy the lists.
 * @details Sentinel/Flag so the GUI can say when its done
 * so the job thread can cleanup and exit.
 * @note If this is not called, things will break. This has 
 * to be kept in mind in the terminator also, to reset variables.
 */
void gui_fileslist_finished(void){
    gui_finished = TRUE;
}

/**
 * @brief Starts disk analysis/defragmentation/optimization job.
 * @param[in] volume_letter the volume letter.
 * @param[in] job_type one of the xxx_JOB constants, defined in udefrag.h
 * @param[in] flags combination of UD_JOB_xxx and UD_PREVIEW_xxx flags defined in udefrag.h
 * @param[in] cluster_map_size size of the cluster map, in cells.
 * Zero value forces to avoid cluster map use.
 * @param[in] cb address of procedure to be called each time when
 * the progress information updates, but no more frequently than
 * specified in UD_REFRESH_INTERVAL environment variable.
 * @param[in] t address of procedure to be called each time
 * when the requested job would like to know whether it must be terminated or not.
 * Nonzero value, returned by the terminator, forces the job to be terminated.
 * @param[in] p pointer to a user defined data to be passed to both callbacks.
 * @return Zero for success, negative value otherwise.
 * @note The callback procedures should complete as quickly
 * as possible to avoid slowdown of the volume processing.
 */
int udefrag_start_job(char volume_letter,udefrag_job_type job_type,int flags,
        int cluster_map_size,udefrag_progress_callback cb,udefrag_terminator t,void *p)
{
    udefrag_job_parameters jp;
    ULONGLONG time = 0;
    int use_limit = 0;
    int result;
    int delwaitcount = 0;
    
    /* initialize the job */
    memset(&jp,0,sizeof(udefrag_job_parameters));
    jp.win_version = winx_get_os_version();

    /* print the header */
    dbg_print_header(&jp);
    
    jp.volume_letter = winx_toupper(volume_letter);    /* convert volume letter to uppercase */
    jp.filelist = NULL;
    jp.fragmented_files = NULL;
    jp.free_regions = NULL;
    jp.progress_refresh_time = 0;
    jp.job_type = job_type;
    jp.cb = cb;
    jp.t = t;
    jp.p = p;

    /*
    * We deliver the progress information from
    * the current thread as well as decide whether
    * to terminate the job or not here. This 
    * multi-threaded technique works quite smoothly.
    */
    jp.termination_router = terminator;

    jp.start_time = jp.p_counters.overall_time = winx_xtime();
    jp.pi.completion_status = 0;
    
    if(get_options(&jp) < 0)
        goto done;
    
    jp.udo.job_flags = flags;

    if(allocate_map(cluster_map_size,&jp) < 0){
        release_options(&jp);
        goto done;
    }
    
    /* set additional privileges for Vista and above */
    if(jp.win_version >= WINDOWS_VISTA)
        (void)winx_enable_privilege(SE_BACKUP_PRIVILEGE);
    if(jp.win_version >= WINDOWS_7)
        (void)winx_enable_privilege(SE_MANAGE_VOLUME_PRIVILEGE);
    
    /* run the job in a separate thread */
    //if <0 this must mean that the job creation failed, so gracefully go to done.
    if(winx_create_thread(start_job,(LPVOID)&jp) < 0){
        free_map(&jp);
        release_options(&jp);
        goto done;
    }

    /*
    * Call the specified callback every refresh_interval milliseconds.
    * http://sourceforge.net/tracker/index.php?func=
    * detail&aid=2886353&group_id=199532&atid=969873
    */
    if(jp.udo.time_limit){
        time = jp.udo.time_limit * 1000;
        if(time / 1000 == jp.udo.time_limit){
            /* no overflow occured */
            use_limit = 1;
        } else {
            /* Windows will die sooner */
        }
    }
    do {
        winx_sleep(jp.udo.refresh_interval);
        deliver_progress_info(&jp,0); /* status = running */
        if(use_limit){
            if(time <= jp.udo.refresh_interval){
                /* time limit exceeded */
                winx_dbg_print_header(0,0,I"*");
                winx_dbg_print_header(0x20,0,I"time limit exceeded");
                winx_dbg_print_header(0,0,I"*");
                jp.termination_router = killer;
            } else {
                if(jp.start_time){
                    if(winx_xtime() - jp.start_time > jp.udo.time_limit * 1000)
                        time = 0;
                } else {
                    /* this method gives not so fine results, but requires no winx_xtime calls */
                    time -= jp.udo.refresh_interval; 
                }
            }
        }
    } while(jp.pi.completion_status == 0);

    /* cleanup */
    deliver_progress_info(&jp,jp.pi.completion_status);
    free_map(&jp);
    release_options(&jp);
    
done:
    jp.p_counters.overall_time = winx_xtime() - jp.p_counters.overall_time;
    dbg_print_performance_counters(&jp);
    dbg_print_footer(&jp);

    /* cleanup */
    winx_flush_dbg_log(0);
    
    result = jp.pi.completion_status;

    //Code to handle the filelists deletion after GUI finishes.
    // Needs to wait until it gets a response. If this thread exits too soon
    // without clearing the memory, deletion of lists will fail/break/segfault.
    // This has to be kept in mind in the terminator also, to reset variables.
    if (result > 0){
        dtrace("Job Complete. Creating deletion Thread with 333ms timeout ->");
        winx_create_thread(wait_delete_lists_thread,(LPVOID)&jp);
        while(!gui_finished || !wait_delete_thread_finished){
            dtrace("Sleeping 333ms waiting for deletion to complete. Wait Count = %d", delwaitcount);            
            delwaitcount++;
            winx_sleep(333);
        }
        gui_finished = FALSE;
        wait_delete_thread_finished = FALSE;
    }
    else
        destroy_lists(&jp);

    if(result < 0) return result;
    return (result > 0) ? 0 : (-1);
}

/**
 * @brief Retrieves default formatted
 * results of a disk defragmentation job.
 * @param[in] pi pointer to structure containing
 * the progress information of the job.
 * @return The default formatted results
 * of the job. NULL indicates failure.
 * @note This function is intended for use
 * in console and native applications.
 * @note MUST call udefrag_release_results() on the 
 * malloc'ed pointer that is returned to free it, after.
 */
char* udefrag_get_results(udefrag_progress_info *pi)
{
    #define MSG_LENGTH 4095
    char *msg;
    char total_space[68];
    char free_space[68];
    double p;
    unsigned int ip, ifr;
    
    /* allocate memory */
    msg = (char *)winx_malloc(MSG_LENGTH + 1);

    (void)winx_bytes_to_hr(pi->total_space,2,total_space,sizeof(total_space));
    (void)winx_bytes_to_hr(pi->free_space,2,free_space,sizeof(free_space));

    p = calc_percentage(pi->fragments,pi->files);
    ip = (unsigned int)(p);
    if(ip < 100) ip = 100; /* fix round off error */
    ifr = (unsigned int)(pi->fragmentation * 100.00);

    (void)_snprintf(msg,MSG_LENGTH,
              "Drive information:\n\n"
              "  Total space                  = %s\n"
              "  Free space                   = %s\n\n"
              "  Total number of files        = %u\n"
              "  Number of fragmented files   = %u\n"
              "  Fragments per file           = %u.%02u\n"
              "  Fragmentation                = %u.%02u%%\n\n",
              total_space,
              free_space,
              pi->files,
              pi->fragmented,
              ip  / 100, ip  % 100,
              ifr / 100, ifr % 100
             );
    msg[MSG_LENGTH] = 0;
    return msg;
}

/**
 * @brief Console. Releases memory allocated by udefrag_get_results.
 * @param[in] results the string to be released.
 */
void udefrag_release_results(char *results)
{
    winx_free(results);
}

/**
 * @brief Retrieves a human readable error
 * description for an ultradefrag error code.
 * @param[in] error_code the error code.
 * @return A human readable description.
 */
char* udefrag_get_error_description(int error_code)
{
    switch(error_code){
    case UDEFRAG_UNKNOWN_ERROR:
        return "Disk is missing or some error has been encountered.";
    case UDEFRAG_NO_MEM:
        return "Not enough memory.";
    case UDEFRAG_CDROM:
        return "It is impossible to defragment CDROM drives.";
    case UDEFRAG_REMOTE:
        return "It is impossible to defragment remote disks.";
    case UDEFRAG_ASSIGNED_BY_SUBST:
        return "It is impossible to defragment disks\n"
               "assigned by the \'subst\' command.";
    case UDEFRAG_REMOVABLE:
        return "You are trying to defragment a removable disk.\n"
               "If the disk type was wrongly identified, send\n"
               "a bug report to the author, thanks.";
    case UDEFRAG_UDF_DEFRAG:
        return "UDF disks can neither be defragmented nor optimized,\n"
               "because the file system driver does not support FSCTL_MOVE_FILE.";
    case UDEFRAG_DIRTY_VOLUME:
        return "Disk is dirty, run CHKDSK to repair it.";
    default: break;
    }
    return "";
}

/**
 * @internal
 * @brief Writes a header to the log file.
 */
static void write_log_file_header(wchar_t *path)
{
    WINX_FILE *f;
    int os_version, mj, mn;
    const char bom[4] = {0xEF, 0xBB, 0xBF, 0x00};
    char *header;
    char format[] = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n"
                    "\r\n"
                    "  %s log file\r\n"
                    "  Windows NT %u.%u\r\n"
                    "\r\n"
                    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n"
                    "\r\n"
                    "  If you'd like to report a bug, attach this file to your bug report please:\r\n"
                    "\r\n"
                    "  http://sourceforge.net/tracker/?group_id=199532&atid=969870\r\n"
                    "\r\n"
                    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n"
                    "\r\n";

    f = winx_fopen(path,"a");
    if(f == NULL)
        return;

    os_version = winx_get_os_version();
    mj = os_version / 10;
    mn = os_version % 10;

    /*
    * UTF-8 encoded files need BOM to be written before the contents.
    */
    (void)winx_fwrite(bom,sizeof(char),3,f);

    header = winx_sprintf(format,VERSIONINTITLE,mj,mn);
    if(header == NULL){
        mtrace();
    } else {
        winx_fwrite(header,1,strlen(header),f);
        winx_free(header);
    }
    winx_fclose(f);
}

/**
 * @internal
 * @brief Creates target directory for a file.
 * @return Zero for success, negative value
 * otherwise.
 */
static int create_target_directory(wchar_t *path)
{
    wchar_t *path_copy;
    int result;

    path_copy = winx_wcsdup(path);
    if(path_copy == NULL){
        etrace("not enough memory");
        return (-1);
    }
    
    winx_path_remove_filename(path_copy);
    result = winx_create_path(path_copy);
    winx_free(path_copy);
    return result;
}

/** @brief convert to native path */
int convert_path_to_native(wchar_t *path, wchar_t **native_path)
{
    if(path == NULL){
        etrace("Abnormal Error. Could not obtain path from parameter!");
        return 0;
    }
    
    *native_path = winx_swprintf(L"\\??\\%ws",path);
    if(*native_path == NULL){
        etrace("Abnormal Error. Cannot build native path!");
        return 0;
    }
    dtrace("Inside native conversion function: Path was: %ws",native_path);
    return 1;
}

/**
 * @brief Turns logging to file on/off,
 * according to <b>\%UD_LOG_FILE_PATH\%</b>
 * environment variable.
 * @details If the library cannot create the
 * specified path, it uses the following path
 * instead: <b>\%SystemDrive\%\\UltraDefrag_Logs</b>.
 * @return Zero for success, negative value otherwise.
 * @note The environment variable mentioned above
 * must contain the full path of the log file.
 */
int udefrag_set_log_file_path(void)
{
    wchar_t *path, *native_path, *filename;
    
    path = winx_getenv(L"UD_LOG_FILE_PATH");
    if(path == NULL){
        /* empty variable forces to disable logging */
        winx_disable_dbg_log();
        return 0;
    }
    
    /* convert to native path */
    native_path = winx_swprintf(L"\\??\\%ws",path);
    winx_free(path);
    if(native_path == NULL){
        etrace("cannot build native path");
        return (-1);
    }
    
    /* delete old logfile */
    winx_delete_file(native_path);
    
    /* ensure that target path exists */
    if(create_target_directory(native_path) < 0){
        /* try %SystemDrive%\UltraDefrag_Logs */
        path = winx_getenv(L"SystemDrive");
        if(path == NULL){
            etrace("failed to query %%SystemDrive%%");
            goto fail;
        }
        filename = winx_wcsdup(native_path);
        if(filename == NULL){
            etrace("cannot allocate memory for filename");
                winx_free(path);
                goto fail;
        }
        winx_path_extract_filename(filename);
        winx_free(native_path);
        native_path = winx_swprintf(L"\\??\\%ws\\UltraDefrag_Logs\\%ws",path,filename);
        if(native_path == NULL){
                    etrace("cannot build %%SystemDrive%%\\UltraDefrag_Logs\\{filename}");
                    winx_free(filename); winx_free(path);
                    goto fail;
        }
        /* delete old logfile from the temporary folder */
        winx_delete_file(native_path);
        winx_free(filename);
        winx_free(path);
        /* ensure that target path exists */
        if(create_target_directory(native_path) < 0) goto fail;
    }
    
    /* write header to the log file */
    write_log_file_header(native_path);
    /* allow debugging output to be appended */
    winx_enable_dbg_log(native_path);
    winx_free(native_path);
    return 0;
    
fail:
    winx_free(native_path);
    return (-1);
}


//(copied code-flow starting from Optimize MFT @ optimize.c)
/**                    The Entry Point                       */
/** 
 * @brief Moves files to either the first free region or the last free region
 * @note Obtains list of files to act on from UD_CUT_FILTER
 * @param start_or_end int = 1 for start, and 0 for end
**/
int movefile_to_start_or_end(udefrag_job_parameters *jp,int start_or_end)
{
	int result, i, file_color;// , new_color;
    ULONGLONG time,jobruntime,filelength,filesize,totalfilesize,writeposition;
    double overall_speed;
    char buffer[32],*headerstring;
    winx_volume_region *region;
    winx_file_info *file;
    wchar_t *path, *native_path;
    winx_file_disposition oldfiledisp;
    winx_blockmap *block;

    /* perform volume analysis */
    result = analyze(jp); /* we need to call it once, here */
    if(result < 0){
        etrace("Abnormal Error. During Single file optimization, Analysis pass failed!");
        return result;
    }
    headerstring = start_or_end ? "Single File Move-to-Start Optimization" : "Single File Move-to-End Optimization";
    time = start_timing(headerstring,jp);
    jp->pi.current_operation = VOLUME_OPTIMIZATION;
    clear_currently_excluded_flag(jp);  //skipping something about clearing the excluded files flag.
    /* open volume handle in jp->fVolume */
    jp->fVolume = winx_vopen(winx_toupper(jp->volume_letter));
    if(jp->fVolume == NULL){
        etrace("Abnormal Error. Could not open volume handle!");
        return -1;
    }
    /* reset outer/job counters */
    jp->pi.total_moves = 0;
    jp->pi.pass_number = 0;
    totalfilesize = 0;
    for(i=0; i < jp->udo.cut_filter.count; i++){
        /* reset inner/file counters */
        //These clusters to process & processed clusters means the 
        // progress counter resets from 0-100% per every file in the loop.
        jp->pi.pass_number++;
        jp->pi.clusters_to_process = 0;
        jp->pi.processed_clusters = 0;
        jp->pi.moved_clusters = 0;

        path = jp->udo.cut_filter.array[i];
        /* convert to native path */
        if(!convert_path_to_native(path,&native_path))
            goto cleanup;
        /* iterate through the filelist (no other way) */
        for(file = jp->filelist; file; file = file->next){
            if(_wcsicmp(file->path,native_path) == 0) break;
            if(file->next == jp->filelist){
                etrace("Abnormal error. Could not match path to any scanned file...");
                result = -1; goto cleanup;
            }
        }
        /* at this point we should have the file's winx_file_info object in *file */
        //dtrace("The file's Native Path is: %ws",native_path);
        if(jp->udo.dbgprint_level >= DBG_DETAILED){
            dtrace("The File's path is: %ws",file->path);
            dtrace("Before: The File has %I64u fragments & resides @ LCN: %I64u",file->disp.fragments,file->disp.blockmap->lcn);
        }
        /* check whether we can move it or not */
        if(!can_move(file,jp->is_fat)){
            etrace("File cannot be moved because reasons."); //should have a better error message.
            result = -1; goto cleanup;
        }
        filelength = file->disp.clusters;
        jp->pi.clusters_to_process = filelength;        
        /* Find the first region that will fit the entire file (use conditional operators.) */
        region = start_or_end ? find_first_free_region(jp,0,filelength,NULL) 
    						  : find_last_free_region(jp, 0,filelength,NULL);
        writeposition = start_or_end ? region->lcn 
    							     :region->lcn + region->length - filelength;
        if (!region){
            etrace("No contiguous region could be found large enough to hold the selected file.");
            result = -1; goto cleanup;
        }

    	//color the file as in-progress. need to memcpy it to save the old-location to UN-color it after.
		memcpy(&oldfiledisp,&file->disp,sizeof(winx_file_disposition));
        file_color = get_file_color(jp,file);
        
/*		//mark the old file location as some color.
		for (block = file->disp.blockmap; block; block = block->next) {
			colorize_map_region(jp, block->lcn, block->length, IN_MOVE_PROGRESS_SPACE, file_color);
			if (block->next == file->disp.blockmap) break;
		}
*/
        /* Perform the move: */
        //purposefully fragmenting files. should only make fragments sized larger than jp->clusters_at_once
        if(move_file(file,0,filelength,writeposition,jp) >= 0){
            //update progress counters.
            file->user_defined_flags |= UD_FILE_MOVED_TO_FRONT;    //there is no existing MOVED_TO_END flag.
            jp->pi.total_moves++;
            jp->pi.moved_clusters = jp->pi.processed_clusters = filelength;
            jp->pi.clusters_to_process = 0;
/*			//undo the in-progress movement color and make the Old-file-Location the proper color.
			for(block = oldfiledisp.blockmap; block; block = block->next){
				colorize_map_region(jp,block->lcn,block->length,IN_MOVE_PROGRESS_SPACE,file_color);
				if(block->next == oldfiledisp.blockmap) break;
			}
*/
            //colorize the New-file-location a similar-but-different color. (so it persists on screen).
            for(block = file->disp.blockmap; block; block = block->next){
                colorize_map_region(jp,block->lcn,block->length,TEAL_BLUE_GREEN,file_color);
                if(block->next == file->disp.blockmap) break;
            }               
        }
        else {
            if (jp->pi.processed_clusters > 0)
                jp->pi.processed_clusters -= filelength;    //somehow needed, otherwise volume status is exactly +100% higher than it should be after a cancel.
            etrace("Moving failed for some reason."); //should have a better error message.
            result = -1;  goto endnicely;
        }
        /* Print status messages */
        filesize = jp->pi.moved_clusters * jp->v_info.bytes_per_cluster;
        totalfilesize += filesize;
        winx_bytes_to_hr(filesize,2,buffer,sizeof buffer);
        if(jp->udo.dbgprint_level >= DBG_DETAILED){
            dtrace("After: The File has %I64u fragments & resides @ LCN: %I64u",file->disp.fragments,file->disp.blockmap->lcn);
            dtrace("%I64u clusters (%s) moved",jp->pi.moved_clusters, buffer);
        }
    }
    result = 0;
    /* cleanup */
cleanup:
    jobruntime = stop_timing(headerstring,time,jp);
    overall_speed = totalfilesize / ((double)jobruntime / 1000);    
    winx_bytes_to_hr(totalfilesize,3,buffer,sizeof buffer);
    itrace("Finished. Total Files Moved: %I64u out of %d. (%s) ",jp->pi.total_moves,jp->udo.cut_filter.count,buffer);
    winx_bytes_to_hr((ULONGLONG)overall_speed,3,buffer,sizeof buffer);
    itrace("Avg. Speed = %s/s", buffer);
endnicely:    
    winx_flush_dbg_log(0);
    clear_currently_excluded_flag(jp); //again?
    winx_fclose(jp->fVolume);
    jp->fVolume = NULL;
    return result;
}

/** @todo 
    Enlarge Largest Free Space Region. (Also determine if anything tiny is blocking the region.)
    Find location with find_largest_free_region()
    Step1. Determine percentage area location of the MIDDLE of the chunk. (get start & end location divide by 2).
    Step2a. Starting backwards from the start, move files to the beginning of the drive.
    Step2b. Starting forwards from the end, move files to the end of the drive.
        This should probably only move FRAGMENTED files at first.
        this will free as much space as easily is possible. Will only move files that actually find a spot in Find_free_region.
        (if we just go sequentially, we will waste regions.)
        Ideally we should make a first-analysis pass so we can sort files to best-suited region 
        (I think this is why there was a find_matching_free_region that was never finished/commented out.)
    At a certain point, we will run out of suitable regions. At this point, the main algorithm should end, and there should be a 
        "Do you want to continue, even if not optimal?". Then it will move un-fragmented files into suitable regions.(fully fitting)
    The un-fragmented files will repeat the process above, filling suitable regions.
    At a certain point, we will run out of suitable regions. At this point, the un-fragmented algorithm should end, and ask
        "Do you want to continue, even if it will fragment files?"
    
*/
/** @} */
