#pragma once

#include "H5VL_log_file.hpp"
#include "H5VL_logi.hpp"

#define H5VL_FILEI_CONFIG_METADATA_MERGE 0x01
#define H5VL_FILEI_CONFIG_SEL_ENCODE	 0x02
#define H5VL_FILEI_CONFIG_SEL_DEFLATE	 0x04
#define H5VL_FILEI_CONFIG_METADATA_SHARE 0x08

#define H5VL_FILEI_CONFIG_DATA_ALIGN 0x100
#define H5VL_FILEI_CONFIG_SUBFILING	 0x200

#define H5VL_LOG_FILEI_GROUP_LOG "__LOG"
#define H5VL_LOG_FILEI_ATTR_INT	 "__int_att"
#define H5VL_LOG_FILEI_DSET_META "__md"
#define H5VL_LOG_FILEI_DSET_DATA "__ld"

// File internals

inline herr_t H5VL_log_filei_init_idx (H5VL_log_file_t *fp) {
	herr_t err = 0;
	switch (fp->index_type) {
		case list:
			fp->idx = new H5VL_logi_array_idx_t (fp);  // Array index
			break;
		case compact:
			fp->idx = new H5VL_logi_compact_idx_t (fp);	 // Compact index
			break;
		default:
			fp->idx = NULL;
			break;
	}
	CHECK_PTR (fp->idx)

err_out:;
	return err;
}

extern herr_t H5VL_log_filei_post_open (H5VL_log_file_t *fp);
extern herr_t H5VL_log_filei_dset_visit (hid_t o_id,
										 const char *name,
										 const H5O_info_t *object_info,
										 void *op_data);
extern herr_t H5VL_log_filei_flush (H5VL_log_file_t *fp, hid_t dxplid);
extern herr_t H5VL_log_filei_metaflush (H5VL_log_file_t *fp);
extern herr_t H5VL_log_filei_metaupdate (H5VL_log_file_t *fp);
extern herr_t H5VL_log_filei_metaupdate_part (H5VL_log_file_t *fp, int &md, int &sec);
extern herr_t H5VL_log_filei_balloc (H5VL_log_file_t *fp, size_t size, void **buf);
extern herr_t H5VL_log_filei_bfree (H5VL_log_file_t *fp, void *buf);

extern herr_t H5VL_log_filei_pool_alloc (H5VL_log_buffer_pool_t *p, ssize_t bsize, void **buf);
extern herr_t H5VL_log_filei_pool_init (H5VL_log_buffer_pool_t *p, ssize_t bsize);
extern herr_t H5VL_log_filei_pool_free (H5VL_log_buffer_pool_t *p);
extern herr_t H5VL_log_filei_pool_finalize (H5VL_log_buffer_pool_t *p);

extern herr_t H5VL_log_filei_parse_fapl (H5VL_log_file_t *fp, hid_t faplid);
extern herr_t H5VL_log_filei_parse_fcpl (H5VL_log_file_t *fp, hid_t fcplid);

extern herr_t H5VL_log_filei_contig_buffer_init (H5VL_log_contig_buffer_t *bp, size_t init_size);
extern void H5VL_log_filei_contig_buffer_free (H5VL_log_contig_buffer_t *bp);
extern void *H5VL_log_filei_contig_buffer_alloc (H5VL_log_contig_buffer_t *bp, size_t size);
extern herr_t H5VL_log_filei_create_subfile (H5VL_log_file_t *fp,
											 unsigned flags,
											 hid_t fapl_id,
											 hid_t dxpl_id);
extern herr_t H5VL_log_filei_open_subfile (H5VL_log_file_t *fp,
										   unsigned flags,
										   hid_t fapl_id,
										   hid_t dxpl_id);
extern herr_t H5VL_log_filei_parse_strip_info (H5VL_log_file_t *fp);
extern herr_t H5VL_log_filei_calc_node_rank (H5VL_log_file_t *fp);

extern herr_t H5VL_log_filei_close (H5VL_log_file_t *fp);
extern void H5VL_log_filei_inc_ref (H5VL_log_file_t *fp);
extern herr_t H5VL_log_filei_dec_ref (H5VL_log_file_t *fp);

extern void *H5VL_log_filei_wrap (void *uo, H5VL_log_obj_t *cp);
