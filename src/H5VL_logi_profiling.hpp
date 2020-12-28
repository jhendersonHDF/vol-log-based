
#pragma once






// NOTE: macro dependency can't be too long to overflow the stack
#define TIMER_FILE_CREATE 0
#define TIMER_FILE_OPEN (TIMER_FILE_CREATE + 1)
#define TIMER_FILE_GET (TIMER_FILE_OPEN + 1)
#define TIMER_FILE_SPECIFIC (TIMER_FILE_GET + 1)
#define TIMER_FILE_OPTIONAL (TIMER_FILE_SPECIFIC + 1)
#define TIMER_FILE_CLOSE (TIMER_FILE_OPTIONAL + 1)
#define TIMER_GROUP_CREATE 6
#define TIMER_GROUP_OPEN (TIMER_GROUP_CREATE + 1)
#define TIMER_GROUP_GET (TIMER_GROUP_OPEN + 1)
#define TIMER_GROUP_SPECIFIC (TIMER_GROUP_GET + 1)
#define TIMER_GROUP_OPTIONAL (TIMER_GROUP_SPECIFIC + 1)
#define TIMER_GROUP_CLOSE (TIMER_GROUP_OPTIONAL + 1)
#define TIMER_ATT_CREATE 12
#define TIMER_ATT_OPEN (TIMER_ATT_CREATE + 1)
#define TIMER_ATT_READ (TIMER_ATT_OPEN + 1)
#define TIMER_ATT_WRITE (TIMER_ATT_READ + 1)
#define TIMER_ATT_GET (TIMER_ATT_WRITE + 1)
#define TIMER_ATT_SPECIFIC (TIMER_ATT_GET + 1)
#define TIMER_ATT_OPTIONAL (TIMER_ATT_SPECIFIC + 1)
#define TIMER_ATT_CLOSE (TIMER_ATT_OPTIONAL + 1)
#define TIMER_DATASET_CREATE 20
#define TIMER_DATASET_OPEN (TIMER_DATASET_CREATE + 1)
#define TIMER_DATASET_READ (TIMER_DATASET_OPEN + 1)
#define TIMER_DATASET_WRITE (TIMER_DATASET_READ + 1)
#define TIMER_DATASET_WRITE_INIT (TIMER_DATASET_WRITE + 1)
#define TIMER_DATASET_WRITE_DECODE (TIMER_DATASET_WRITE_INIT + 1)
#define TIMER_DATASET_WRITE_ENCODE (TIMER_DATASET_WRITE_DECODE + 1)
#define TIMER_DATASET_WRITE_META_DEFLATE (TIMER_DATASET_WRITE_ENCODE + 1)
#define TIMER_DATASET_WRITE_PACK (TIMER_DATASET_WRITE_META_DEFLATE + 1)
#define TIMER_DATASET_WRITE_CONVERT (TIMER_DATASET_WRITE_PACK + 1)
#define TIMER_DATASET_WRITE_FILTER (TIMER_DATASET_WRITE_CONVERT + 1)
#define TIMER_DATASET_WRITE_FINALIZE (TIMER_DATASET_WRITE_FILTER + 1)
#define TIMER_DATASET_GET (TIMER_DATASET_WRITE_FINALIZE + 1)
#define TIMER_DATASET_SPECIFIC (TIMER_DATASET_GET + 1)
#define TIMER_DATASET_OPTIONAL (TIMER_DATASET_SPECIFIC + 1)
#define TIMER_DATASET_CLOSE (TIMER_DATASET_OPTIONAL + 1)
#define TIMER_H5VL_FILE_CREATE 33
#define TIMER_H5VL_FILE_OPEN (TIMER_H5VL_FILE_CREATE + 1)
#define TIMER_H5VL_FILE_GET (TIMER_H5VL_FILE_OPEN + 1)
#define TIMER_H5VL_FILE_SPECIFIC (TIMER_H5VL_FILE_GET + 1)
#define TIMER_H5VL_FILE_OPTIONAL (TIMER_H5VL_FILE_SPECIFIC + 1)
#define TIMER_H5VL_FILE_CLOSE (TIMER_H5VL_FILE_OPTIONAL + 1)
#define TIMER_H5VL_GROUP_CREATE 39
#define TIMER_H5VL_GROUP_OPEN (TIMER_H5VL_GROUP_CREATE + 1)
#define TIMER_H5VL_GROUP_GET (TIMER_H5VL_GROUP_OPEN + 1)
#define TIMER_H5VL_GROUP_SPECIFIC (TIMER_H5VL_GROUP_GET + 1)
#define TIMER_H5VL_GROUP_OPTIONAL (TIMER_H5VL_GROUP_SPECIFIC + 1)
#define TIMER_H5VL_GROUP_CLOSE (TIMER_H5VL_GROUP_OPTIONAL + 1)
#define TIMER_H5VL_ATT_CREATE 45
#define TIMER_H5VL_ATT_OPEN (TIMER_H5VL_ATT_CREATE + 1)
#define TIMER_H5VL_ATT_READ (TIMER_H5VL_ATT_OPEN + 1)
#define TIMER_H5VL_ATT_WRITE (TIMER_H5VL_ATT_READ + 1)
#define TIMER_H5VL_ATT_GET (TIMER_H5VL_ATT_WRITE + 1)
#define TIMER_H5VL_ATT_SPECIFIC (TIMER_H5VL_ATT_GET + 1)
#define TIMER_H5VL_ATT_OPTIONAL (TIMER_H5VL_ATT_SPECIFIC + 1)
#define TIMER_H5VL_ATT_CLOSE (TIMER_H5VL_ATT_OPTIONAL + 1)
#define TIMER_H5VL_DATASET_CREATE 53
#define TIMER_H5VL_DATASET_OPEN (TIMER_H5VL_DATASET_CREATE + 1)
#define TIMER_H5VL_DATASET_READ (TIMER_H5VL_DATASET_OPEN + 1)
#define TIMER_H5VL_DATASET_WRITE (TIMER_H5VL_DATASET_READ + 1)
#define TIMER_H5VL_DATASET_GET (TIMER_H5VL_DATASET_WRITE + 1)
#define TIMER_H5VL_DATASET_SPECIFIC (TIMER_H5VL_DATASET_GET + 1)
#define TIMER_H5VL_DATASET_OPTIONAL (TIMER_H5VL_DATASET_SPECIFIC + 1)
#define TIMER_H5VL_DATASET_CLOSE (TIMER_H5VL_DATASET_OPTIONAL + 1)
#define TIMER_FILEI_FLUSH 61
#define TIMER_FILEI_METAFLUSH (TIMER_FILEI_FLUSH + 1)
#define TIMER_FILEI_METAFLUSH_INIT (TIMER_FILEI_METAFLUSH + 1)
#define TIMER_FILEI_METAFLUSH_PACK (TIMER_FILEI_METAFLUSH_INIT + 1)
#define TIMER_FILEI_METAFLUSH_ZIP (TIMER_FILEI_METAFLUSH_PACK + 1)
#define TIMER_FILEI_METAFLUSH_SYNC (TIMER_FILEI_METAFLUSH_ZIP + 1)
#define TIMER_FILEI_METAFLUSH_CREATE (TIMER_FILEI_METAFLUSH_SYNC + 1)
#define TIMER_FILEI_METAFLUSH_WRITE (TIMER_FILEI_METAFLUSH_CREATE + 1)
#define TIMER_FILEI_METAFLUSH_CLOSE (TIMER_FILEI_METAFLUSH_WRITE + 1)
#define TIMER_FILEI_METAFLUSH_BARR (TIMER_FILEI_METAFLUSH_CLOSE + 1)
#define TIMER_FILEI_METAFLUSH_FINALIZE (TIMER_FILEI_METAFLUSH_BARR + 1)
#define TIMER_FILEI_METAFLUSH_SIZE (TIMER_FILEI_METAFLUSH_FINALIZE + 1)
#define TIMER_FILEI_METAFLUSH_SIZE_ZIP (TIMER_FILEI_METAFLUSH_SIZE + 1)
#define TIMER_FILEI_METAUPDATE (TIMER_FILEI_METAFLUSH_SIZE_ZIP + 1)
#define TIMER_DATASETI_GEN_RTYPE 75
#define TIMER_DATASETI_OPEN_UO (TIMER_DATASETI_GEN_RTYPE + 1)
#define TIMER_DATASETI_WRAP (TIMER_DATASETI_OPEN_UO + 1)
#define TIMER_DATASPACEI_GET_SEL_TYPE 78
#define TIMER_DATASPACEI_GET_SEL (TIMER_DATASPACEI_GET_SEL_TYPE + 1)
#define TIMER_NB_FLUSH_READ_REQ 80
#define TIMER_NB_FLUSH_WRITE_REQ (TIMER_NB_FLUSH_READ_REQ + 1)
#define TIMER_NB_WRITE_REQ_ALIGNED (TIMER_NB_FLUSH_WRITE_REQ + 1)
#define TIMER_NB_FLUSH_WRITE_REQ_SIZE (TIMER_NB_WRITE_REQ_ALIGNED + 1)

#define NTIMER (TIMER_NB_FLUSH_WRITE_REQ_SIZE + 1)


#ifdef LOGVOL_PROFILING
#include "H5VL_logi_profiling.hpp"
#define TIMER_START          \
	{                        \
		double tstart, tend; \
		tstart = MPI_Wtime ();
#define TIMER_STOP(A, B)                             \
	tend = MPI_Wtime ();                             \
	H5VL_log_profile_add_time (A, B, tend - tstart); \
	}
#else
#define TIMER_START \
	{}
#define TIMER_STOP(A, B) \
	{}
#endif

#ifdef LOGVOL_PROFILING
void H5VL_log_profile_add_time (void *file, int id, double t);
void H5VL_log_profile_sub_time (void *file, int id, double t);
void H5VL_log_profile_print (void *file);
void H5VL_log_profile_reset (void *file);
#endif