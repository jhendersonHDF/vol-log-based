#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "H5VL_log.h"
#include "H5VL_log_dataset.hpp"
#include "H5VL_log_filei.hpp"
#include "H5VL_log_req.hpp"
#include "H5VL_logi.hpp"
#include "H5VL_logi_dataspace.hpp"
#include "H5VL_logi_filter.hpp"
#include "H5VL_logi_util.hpp"
#include "H5VL_logi_wrapper.hpp"
#include "H5VL_logi_zip.hpp"

/********************* */
/* Function prototypes */
/********************* */

const H5VL_dataset_class_t H5VL_log_dataset_g {
	H5VL_log_dataset_create,   /* create       */
	H5VL_log_dataset_open,	   /* open         */
	H5VL_log_dataset_read,	   /* read         */
	H5VL_log_dataset_write,	   /* write        */
	H5VL_log_dataset_get,	   /* get          */
	H5VL_log_dataset_specific, /* specific     */
	H5VL_log_dataset_optional, /* optional     */
	H5VL_log_dataset_close	   /* close        */
};

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_create
 *
 * Purpose:     Creates a dataset in a container
 *
 * Return:      Success:    Pointer to a dataset object
 *              Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
void *H5VL_log_dataset_create (void *obj,
							   const H5VL_loc_params_t *loc_params,
							   const char *name,
							   hid_t lcpl_id,
							   hid_t type_id,
							   hid_t space_id,
							   hid_t dcpl_id,
							   hid_t dapl_id,
							   hid_t dxpl_id,
							   void **req) {
	herr_t err = 0;
	int i;
	H5VL_log_obj_t *op	= (H5VL_log_obj_t *)obj;
	H5VL_log_dset_t *dp = NULL;
	H5VL_loc_params_t locp;
	hid_t sid = -1;
	void *ap;
	int ndim, nfilter;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	sid = H5Screate (H5S_SCALAR);
	CHECK_ID (sid);
	err = H5Pset_layout (dcpl_id, H5D_CONTIGUOUS);
	CHECK_ERR

	dp = new H5VL_log_dset_t (op, H5I_DATASET);

	if (req) {
		rp	  = new H5VL_log_req_t ();
		ureqp = &ureq;
	} else {
		ureqp = NULL;
	}

	TIMER_START;
	dp->uo = H5VLdataset_create (op->uo, loc_params, op->uvlid, name, lcpl_id, type_id, sid,
								 dcpl_id, dapl_id, dxpl_id, ureqp);
	TIMER_STOP (dp->fp, TIMER_H5VL_DATASET_CREATE);
	CHECK_PTR (dp->uo)
	if (req) { rp->append (ureq); }

	dp->dtype = H5Tcopy (type_id);
	CHECK_ID (dp->dtype)
	dp->esize = H5Tget_size (type_id);
	CHECK_ID (dp->esize)

	// NOTE: I don't know if it work for scalar dataset, can we create zero sized attr?
	ndim = H5Sget_simple_extent_dims (space_id, dp->dims, dp->mdims);
	CHECK_ID (ndim)
	dp->ndim = (hsize_t)ndim;

	dp->id = (dp->fp->ndset)++;
	/*
	if (dp->fp->mdc.size() < dp->id + 1){
		dp->fp->mdc.resize(dp->id + 1);
	}
	dp->fp->mdc[dp->id] = {ndim, dp->dtype, dp->esize};
	*/
	// Record metadata in fp
	dp->fp->idx.resize (dp->fp->ndset);
	dp->fp->ndim.resize (dp->fp->ndset);
	dp->fp->ndim[dp->id] = dp->ndim;
	dp->fp->dsizes.resize (dp->fp->ndset);
	for (i = 0; i < ndim; i++) { dp->fp->dsizes[dp->id][i] = dp->dims[i]; }

	// Dstep for encoding selection
	if (dp->fp->config & H5VL_FILEI_CONFIG_SEL_ENCODE) {
		dp->dsteps[dp->ndim - 1] = 1;
		for (i = dp->ndim - 2; i > -1; i--) { dp->dsteps[i] = dp->dsteps[i + 1] * dp->dims[i + 1]; }
	}

	// Atts
	err = H5VL_logi_add_att (dp, "_dims", H5T_STD_I64LE, H5T_NATIVE_INT64, dp->ndim, dp->dims,
							 dxpl_id, ureqp);
	CHECK_ERR
	if (req) { rp->append (ureq); }
	err = H5VL_logi_add_att (dp, "_mdims", H5T_STD_I64LE, H5T_NATIVE_INT64, dp->ndim, dp->mdims,
							 dxpl_id, ureqp);
	CHECK_ERR
	if (req) { rp->append (ureq); }
<<<<<<< HEAD
<<<<<<< HEAD
	err = H5VL_logi_add_att (dp, "_ID", H5T_STD_I32LE, H5T_NATIVE_INT32, 1, &(dp->id), dxpl_id);
=======
	err = H5VL_logi_add_att (dp, "_ID", H5T_STD_I32LE, H5T_NATIVE_INT32, 1, &(dp->id), dxpl_id,ureqp);
>>>>>>> req callbacks
=======
	err = H5VL_logi_add_att (dp, "_ID", H5T_STD_I32LE, H5T_NATIVE_INT32, 1, &(dp->id), dxpl_id,
							 ureqp);
>>>>>>> metadata format for varn;make metadata merging and filtering optional;encode metadata on dwrite
	CHECK_ERR
	if (req) {
		rp->append (ureq);
		*req = rp;
	}

	// Filters
	nfilter = H5Pget_nfilters (dcpl_id);
	CHECK_ID (nfilter);
	dp->filters.resize (nfilter);
	for (i = 0; i < nfilter; i++) {
		dp->filters[i].id = H5Pget_filter2 (dcpl_id, (unsigned int)i, &(dp->filters[i].flags),
											&(dp->filters[i].cd_nelmts), dp->filters[i].cd_values,
											LOGVOL_FILTER_NAME_MAX, dp->filters[i].name,
											&(dp->filters[i].filter_config));
		CHECK_ID (dp->filters[i].id);
	}

	TIMER_STOP (dp->fp, TIMER_DATASET_CREATE);

	goto fn_exit;
err_out:;
	if (dp) {
		delete dp;
		dp = NULL;
	}
fn_exit:;
	return (void *)dp;
} /* end H5VL_log_dataset_create() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_open
 *
 * Purpose:     Opens a dataset in a container
 *
 * Return:      Success:    Pointer to a dataset object
 *              Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
void *H5VL_log_dataset_open (void *obj,
							 const H5VL_loc_params_t *loc_params,
							 const char *name,
							 hid_t dapl_id,
							 hid_t dxpl_id,
							 void **req) {
	herr_t err = 0;
	int i;
	H5VL_log_obj_t *op	= (H5VL_log_obj_t *)obj;
	H5VL_log_dset_t *dp = NULL;
	H5VL_loc_params_t locp;
	va_list args;
	int nfilter;
	hid_t dcpl_id = -1;
	void *ap;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	dp = new H5VL_log_dset_t (op, H5I_DATASET);

	if (req) {
		rp	  = new H5VL_log_req_t ();
		ureqp = &ureq;
	} else {
		ureqp = NULL;
	}

	TIMER_START;
	dp->uo = H5VLdataset_open (op->uo, loc_params, op->uvlid, name, dapl_id, dxpl_id, NULL);
	CHECK_PTR (dp->uo);
	TIMER_STOP (dp->fp, TIMER_H5VL_DATASET_OPEN);

	TIMER_START;
	err = H5VL_logi_dataset_get_wrapper (dp->uo, dp->uvlid, H5VL_DATASET_GET_TYPE, dxpl_id, ureqp,
										 &(dp->dtype));
	CHECK_ERR
	TIMER_STOP (dp->fp, TIMER_H5VL_DATASET_GET);
	if (req) { rp->append (ureq); }

	dp->esize = H5Tget_size (dp->dtype);
	CHECK_ID (dp->esize)

	// Atts
	err = H5VL_logi_get_att_ex (dp, "_dims", H5T_NATIVE_INT64, &(dp->ndim), dp->dims, dxpl_id);
	CHECK_ERR
	if (req) { rp->append (ureq); }
	err = H5VL_logi_get_att (dp, "_mdims", H5T_NATIVE_INT64, dp->mdims, dxpl_id);
	CHECK_ERR
	if (req) { rp->append (ureq); }
	err = H5VL_logi_get_att (dp, "_ID", H5T_NATIVE_INT32, &(dp->id), dxpl_id);
	CHECK_ERR
	if (req) {
		rp->append (ureq);
		*req = rp;
	}

	// Dstep for encoding selection
	if (dp->fp->config & H5VL_FILEI_CONFIG_SEL_ENCODE) {
		dp->dsteps[dp->ndim - 1] = 1;
		for (i = dp->ndim - 2; i > -1; i--) { dp->dsteps[i] = dp->dsteps[i + 1] * dp->dims[i + 1]; }
	}

	// Record metadata in fp
	dp->fp->idx.resize (dp->fp->ndset);
	dp->fp->ndim.resize (dp->fp->ndset);
	dp->fp->ndim[dp->id] = dp->ndim;
	dp->fp->dsizes.resize (dp->fp->ndset);
	for (i = 0; i < dp->ndim; i++) { dp->fp->dsizes[dp->id][i] = dp->dims[i]; }

	// Filters
	err = H5VL_logi_dataset_get_wrapper (dp->uo, dp->uvlid, H5VL_DATASET_GET_DCPL, dxpl_id, NULL,
										 &(dcpl_id));
	CHECK_ERR
	nfilter = H5Pget_nfilters (dcpl_id);
	CHECK_ID (nfilter);
	dp->filters.resize (nfilter);
	for (i = 0; i < nfilter; i++) {
		dp->filters[i].id = H5Pget_filter2 (dcpl_id, (unsigned int)i, &(dp->filters[i].flags),
											&(dp->filters[i].cd_nelmts), dp->filters[i].cd_values,
											LOGVOL_FILTER_NAME_MAX, dp->filters[i].name,
											&(dp->filters[i].filter_config));
		CHECK_ID (dp->filters[i].id);
	}

	TIMER_STOP (dp->fp, TIMER_DATASET_OPEN);

	goto fn_exit;
err_out:;
	if (dp) delete dp;
	dp = NULL;
fn_exit:;
	if (dcpl_id >= 0) { H5Pclose (dcpl_id); }
	return (void *)dp;
} /* end H5VL_log_dataset_open() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_read
 *
 * Purpose:     Reads data elements from a dataset into a buffer.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_read (void *dset,
							  hid_t mem_type_id,
							  hid_t mem_space_id,
							  hid_t file_space_id,
							  hid_t plist_id,
							  void *buf,
							  void **req) {
	herr_t err = 0;
	int i, j;
	int n;
	size_t esize;
	htri_t eqtype;
	char *bufp = (char *)buf;
	H5VL_log_rreq_t r;
	H5S_sel_type stype, mstype;
	H5VL_log_req_type_t rtype;
	H5VL_log_dset_t *dp = (H5VL_log_dset_t *)dset;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	// Check mem space selection
	if (file_space_id == H5S_ALL)
		stype = H5S_SEL_ALL;
	else
		stype = H5Sget_select_type (file_space_id);

	if (stype == H5S_SEL_NONE) goto err_out;

	// Update the index if it is out of date
	if (!(dp->fp->idxvalid)) {
		err = H5VL_log_filei_metaupdate (dp->fp);
		CHECK_ERR
	}

	// H5S_All means using file space
	if (mem_space_id == H5S_ALL) mem_space_id = file_space_id;

	// Check mem space selection
	if (mem_space_id == H5S_ALL)
		mstype = H5S_SEL_ALL;
	else
		mstype = H5Sget_select_type (mem_space_id);

	// Setting metadata;
	r.did	= dp->id;
	r.ndim	= dp->ndim;
	r.ubuf	= (char *)buf;
	r.ptype = MPI_DATATYPE_NULL;
	r.dtype = -1;
	r.mtype = -1;
	r.esize = dp->esize;
	r.rsize = 0;  // Nomber of elements in record

	// Gather starts and counts
	if (stype == H5S_SEL_ALL) {
		r.sels.resize (1);
		r.rsize = 1;
		for (j = 0; j < dp->ndim; j++) {
			r.sels[0].start[j] = 0;
			r.sels[0].count[j] = dp->dims[j];
			r.rsize *= r.sels[0].count[j];
		}
		r.sels[0].size *= dp->esize;
	} else {
		TIMER_START;
		err = H5VL_logi_get_dataspace_selection (file_space_id, r.sels);
		CHECK_ERR
		TIMER_STOP (dp->fp, TIMER_DATASPACEI_GET_SEL);

		for (i = 0; i < r.sels.size (); i++) {
			r.sels[i].size = 1;
			for (j = 0; j < dp->ndim; j++) r.sels[i].size *= r.sels[i].count[j];
			r.rsize += r.sels[i].size;
			r.sels[i].size *= dp->esize;
		}
	}

	// Non-blocking?
	err = H5Pget_nonblocking (plist_id, &rtype);
	CHECK_ERR

	// Need convert?
	eqtype = H5Tequal (dp->dtype, mem_type_id);
	CHECK_ID (eqtype);

	// Can reuse user buffer
	if (eqtype > 0 && mstype == H5S_SEL_ALL) {
		r.xbuf = r.ubuf;
	} else {  // Need internal buffer
		// Get element size
		esize = H5Tget_size (mem_type_id);
		CHECK_ID (esize)

		// HDF5 type conversion is in place, allocate for whatever larger
		err = H5VL_log_filei_balloc (dp->fp, r.rsize * std::max (esize, (size_t) (dp->esize)),
									 (void **)(&(r.xbuf)));
		CHECK_ERR

		// Need packing
		if (mstype != H5S_SEL_ALL) {
			TIMER_START;
			err = H5VL_logi_get_dataspace_sel_type (mem_space_id, esize, &(r.ptype));
			CHECK_ERR
			TIMER_STOP (dp->fp, TIMER_DATASPACEI_GET_SEL_TYPE);
		}

		// Need convert
		if (eqtype == 0) {
			r.dtype = H5Tcopy (dp->dtype);
			CHECK_ID (r.dtype)
			r.mtype = H5Tcopy (mem_type_id);
			CHECK_ID (r.mtype)
		}
	}

	// Flush it immediately if blocking, otherwise place into queue
	if (rtype != H5VL_LOG_REQ_NONBLOCKING) {
		err = H5VL_log_nb_flush_read_reqs (dp->fp, std::vector<H5VL_log_rreq_t> (1, r), plist_id);
		CHECK_ERR
	} else {
		dp->fp->rreqs.push_back (r);
	}

	TIMER_STOP (dp->fp, TIMER_DATASET_READ);
err_out:;

	return err;
} /* end H5VL_log_dataset_read() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_write
 *
 * Purpose:     Writes data elements from a buffer into a dataset.
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_write (void *dset,
							   hid_t mem_type_id,
							   hid_t mem_space_id,
							   hid_t file_space_id,
							   hid_t plist_id,
							   const void *buf,
							   void **req) {
	herr_t err			= 0;
	H5VL_log_dset_t *dp = (H5VL_log_dset_t *)dset;
	int i, j, k, l;
	size_t esize, ssize;
	H5VL_log_wreq_t r;
	htri_t eqtype;
	H5S_sel_type stype, mstype;
	H5VL_log_req_type_t rtype;
	MPI_Datatype ptype = MPI_DATATYPE_NULL;
	H5VL_log_multisel_arg_t arg;
	H5VL_log_req_t *rp;
	std::vector<H5VL_log_selection> sels;
	void **ureqp, *ureq;
	char *mbuf, *zbuf;
	TIMER_START;

	TIMER_START;
	// Varn ?
	err = H5Pget_multisel (plist_id, &arg);
	CHECK_ERR

	if (arg.n) {
		stype  = H5S_SEL_ALL;
		mstype = H5S_SEL_ALL;
	} else {
		// Check file space selection
		if (file_space_id == H5S_ALL)
			stype = H5S_SEL_ALL;
		else
			stype = H5Sget_select_type (file_space_id);

		// H5S_All means using file space
		if (mem_space_id == H5S_ALL) mem_space_id = file_space_id;

		// Check mem space selection
		if (mem_space_id == H5S_ALL)
			mstype = H5S_SEL_ALL;
		else if (mem_space_id == H5S_CONTIG)
			mstype = H5S_SEL_ALL;
		else
			mstype = H5Sget_select_type (mem_space_id);
	}
	// Sanity check
	if (stype == H5S_SEL_NONE) goto err_out;
	if (!buf) ERR_OUT ("user buffer can't be NULL");
	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_INIT);

	TIMER_START;
	// Setting metadata;
	r.did = dp->id;
	// r.ndim	= dp->ndim;
	r.ldid	= -1;
	r.ldoff = 0;
	r.ubuf	= (char *)buf;
	r.rsize = 0;  // Nomber of elements in record
	if (arg.n) {
		r.nsel = arg.n;
	} else if (stype == H5S_SEL_ALL) {
		r.nsel = 1;
	} else {
		TIMER_START;
		err = H5VL_logi_get_dataspace_selection (file_space_id, sels);
		CHECK_ERR
		TIMER_STOP (dp->fp, TIMER_DATASPACEI_GET_SEL);
		r.nsel = sels.size ();
	}

	// Flags
	r.flag = 0;
	if (r.nsel > 1) {
		if ((dp->ndim > 1) && (dp->fp->config & H5VL_FILEI_CONFIG_SEL_ENCODE)) {
			r.flag |= H5VL_LOGI_META_FLAG_SEL_ENCODE;
		}
		r.flag |= H5VL_LOGI_META_FLAG_MUL_SEL;

		if (dp->fp->config & H5VL_FILEI_CONFIG_SEL_DEFLATE) {
			r.flag |= H5VL_LOGI_META_FLAG_SEL_DEFLATE;
		}
	}

	// Selection buffer
	r.ssize=sizeof(int) * 3 + sizeof(MPI_Offset) * 2;
	if (r.flag & H5VL_LOGI_META_FLAG_SEL_ENCODE) {
		r.ssize += sizeof (MPI_Offset) * (dp->ndim + r.nsel * 2);
	} else {
		r.ssize += sizeof (MPI_Offset) * (dp->ndim * r.nsel * 2);
	}
#ifdef ENABLE_ZLIB
	if (dp->fp->config & H5VL_LOGI_META_FLAG_SEL_DEFLATE) {
		r.esel = (char *)malloc (r.ssize * 2);
		zbuf   = r.esel + r.ssize;
	} else
#endif
	{
		r.esel = (char *)malloc (r.ssize);
	}
	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_DECODE);

	// Encode and pack selections
	TIMER_START
	mbuf = r.esel + sizeof(int)*3+sizeof(MPI_Offset)*2;
	if (r.flag & H5VL_LOGI_META_FLAG_SEL_ENCODE) {
		MPI_Offset soff, eoff;
		if (arg.n) {
			r.nsel	= arg.n;
			r.rsize = 0;
			for (i = 0; i < arg.n; i++) {
				ssize = 1;
				soff = eoff = 0;
				for (j = 0; j < dp->ndim; j++) {
					ssize *= arg.counts[i][j];	// Size of the selection
					soff +=
						arg.starts[i][j] * dp->dsteps[j];  // Starting offset of the bounding box
					eoff += (arg.starts[i][j] + arg.counts[i][j]) *
							dp->dsteps[j];	// Ending offset of the bounding box
				}
				*((MPI_Offset *)mbuf) = soff;
				mbuf += sizeof (MPI_Offset);
				*((MPI_Offset *)mbuf) = eoff;
				mbuf += sizeof (MPI_Offset);
				// Record overall size of the write req
				r.rsize += ssize;
			}
		} else {
			// We won't encode single H5S_ALL
			r.nsel	= sels.size ();
			r.rsize = 0;
			for (i = 0; i < sels.size (); i++) {
				ssize = 1;
				soff = eoff = 0;
				for (j = 0; j < dp->ndim; j++) {
					ssize *= sels[i].count[j];	// Size of the selection
					soff +=
						sels[i].start[j] * dp->dsteps[j];  // Starting offset of the bounding box
					eoff += (sels[i].start[j] + sels[i].count[j]) *
							dp->dsteps[j];	// Ending offset of the bounding box
				}
				*((MPI_Offset *)mbuf) = soff;
				mbuf += sizeof (MPI_Offset);
				*((MPI_Offset *)mbuf) = eoff;
				mbuf += sizeof (MPI_Offset);
				// Record overall size of the write req
				r.rsize += ssize;
			}
		}
	} else {
		if (arg.n) {
			r.nsel = arg.n;
			for (i = 0; i < arg.n; i++) {
				ssize = 1;
				for (j = 0; j < dp->ndim; j++) {
					ssize *= arg.counts[i][j];	// Size of the selection
					*((MPI_Offset *)mbuf) = arg.starts[i][j];
					mbuf += sizeof (MPI_Offset);
					*((MPI_Offset *)mbuf) = arg.counts[i][j];
					mbuf += sizeof (MPI_Offset);
				}
				// Record overall size of the write req
				r.rsize += ssize;
			}
		} else {
			if (stype == H5S_SEL_ALL) {
				r.rsize = 1;
				for (j = 0; j < dp->ndim; j++) {
					*((MPI_Offset *)mbuf) = 0;
					mbuf += sizeof (MPI_Offset);
					*((MPI_Offset *)mbuf) = dp->dims[j];
					mbuf += sizeof (MPI_Offset);
					r.rsize *= dp->dims[j];	 // Size of the selection
				}
			} else {
				for (i = 0; i < sels.size (); i++) {
					ssize = 1;
					for (j = 0; j < dp->ndim; j++) {
						ssize *= sels[i].count[j];
						*((MPI_Offset *)mbuf) = sels[i].start[j];
						mbuf += sizeof (MPI_Offset);
						*((MPI_Offset *)mbuf) = sels[i].count[j];
						mbuf += sizeof (MPI_Offset);
					}
					r.rsize += ssize;
				}
			}
		}
	}
	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_ENCODE);

	// Non-blocking?
	err = H5Pget_nonblocking (plist_id, &rtype);
	CHECK_ERR

	// Need convert?
	eqtype = H5Tequal (dp->dtype, mem_type_id);

	// Can reuse user buffer
	if (rtype == H5VL_LOG_REQ_NONBLOCKING && eqtype > 0 && mstype == H5S_SEL_ALL) {
		r.xbuf = r.ubuf;
	} else {  // Need internal buffer
		TIMER_START;
		// Get element size
		esize = H5Tget_size (mem_type_id);
		CHECK_ID (esize)

		// HDF5 type conversion is in place, allocate for whatever larger
		err = H5VL_log_filei_balloc (dp->fp, r.rsize * std::max (esize, (size_t) (dp->esize)),
									 (void **)(&(r.xbuf)));
		// err = H5VL_log_filei_pool_alloc (&(dp->fp->data_buf),
		//								 r.rsize * std::max (esize, (size_t) (dp->esize)),
		//								 (void **)(&(r.xbuf)));
		// CHECK_ERR

		// Need packing
		if (mstype != H5S_SEL_ALL) {
			i = 0;

			TIMER_START
			err = H5VL_logi_get_dataspace_sel_type (mem_space_id, esize, &ptype);
			CHECK_ERR
			TIMER_STOP (dp->fp, TIMER_DATASPACEI_GET_SEL_TYPE);

			MPI_Pack (r.ubuf, 1, ptype, r.xbuf, r.rsize * esize, &i, dp->fp->comm);

			LOG_VOL_ASSERT (i == r.rsize * esize)
		} else {
			memcpy (r.xbuf, r.ubuf, r.rsize * esize);
		}
		TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_PACK);

		TIMER_START;
		// Need convert
		if (eqtype == 0) {
			void *bg = NULL;

			if (H5Tget_class (dp->dtype) == H5T_COMPOUND) bg = malloc (r.rsize * dp->esize);
			err = H5Tconvert (mem_type_id, dp->dtype, r.rsize, r.xbuf, bg, plist_id);
			free (bg);
		}
		TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_CONVERT);
	}

	// Convert request size to number of bytes to be used by later routines
	r.rsize *= dp->esize;

	// Filtering
	TIMER_START;
	if (dp->filters.size ()) {
		char *buf = NULL;
		int csize = 0;

		err = H5VL_logi_filter (dp->filters, r.xbuf, r.rsize, (void **)&buf, &csize);
		CHECK_ERR

		if (r.xbuf != r.ubuf) {
			err = H5VL_log_filei_bfree (dp->fp, &(r.xbuf));
			CHECK_ERR
		}

		r.xbuf	= buf;
		r.rsize = csize;
	}
	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_FILTER);

	TIMER_START;

	// Put request in queue
	dp->fp->wreqs.push_back (r);
	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE_FINALIZE);

	TIMER_STOP (dp->fp, TIMER_DATASET_WRITE);
err_out:;
	if (err) {
		// if (r.xbuf != r.ubuf) H5VL_log_filei_bfree (dp->fp, r.xbuf);
	}
	H5VL_log_type_free (ptype);

	return err;
} /* end H5VL_log_dataset_write() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_get
 *
 * Purpose:     Gets information about a dataset
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_get (
	void *dset, H5VL_dataset_get_t get_type, hid_t dxpl_id, void **req, va_list arguments) {
	H5VL_log_dset_t *dp = (H5VL_log_dset_t *)dset;
	herr_t err			= 0;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	switch (get_type) {
		/* H5Dget_space */
		case H5VL_DATASET_GET_SPACE: {
			hid_t *ret_id = va_arg (arguments, hid_t *);

			*ret_id = H5Screate_simple (dp->ndim, dp->dims, dp->mdims);

			break;
		}

		/* H5Dget_space_status */
		case H5VL_DATASET_GET_SPACE_STATUS: {
			break;
		}

		/* H5Dget_type */
		case H5VL_DATASET_GET_TYPE: {
			hid_t *ret_id = va_arg (arguments, hid_t *);

			*ret_id = H5Tcopy (dp->dtype);

			break;
		}

		/* H5Dget_create_plist */
		case H5VL_DATASET_GET_DCPL: {
			err = -1;
			ERR_OUT ("get_type not supported")
			break;
		}

		/* H5Dget_access_plist */
		case H5VL_DATASET_GET_DAPL: {
			err = -1;
			ERR_OUT ("get_type not supported")
			break;
		}

		/* H5Dget_storage_size */
		case H5VL_DATASET_GET_STORAGE_SIZE: {
			hsize_t *ret = va_arg (arguments, hsize_t *);
			err			 = -1;
			ERR_OUT ("get_type not supported")
			break;
		}
		default:
			ERR_OUT ("get_type not supported")
	} /* end switch */

	TIMER_STOP (dp->fp, TIMER_DATASET_GET);
err_out:;
	return err;
} /* end H5VL_log_dataset_get() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_specific
 *
 * Purpose:     Specific operation on a dataset
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_specific (void *obj,
								  H5VL_dataset_specific_t specific_type,
								  hid_t dxpl_id,
								  void **req,
								  va_list arguments) {
	H5VL_log_dset_t *dp = (H5VL_log_dset_t *)obj;
	herr_t err			= 0;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	switch (specific_type) {
		case H5VL_DATASET_SET_EXTENT: { /* H5Dset_extent */
			int i;
			const hsize_t *new_sizes = va_arg (arguments, const hsize_t *);

			for (i = 0; i < dp->ndim; i++) {
				if (new_sizes[i] < 0) {
					err = -1;
					ERR_OUT ("size cannot be negative")
				}
				if (dp->mdims[i] != H5S_UNLIMITED && new_sizes[i] > dp->mdims[i]) {
					err = -1;
					ERR_OUT ("size cannot exceed max size")
				}
				dp->dims[i] = dp->fp->dsizes[dp->id][i] = new_sizes[i];
			}
			break;
		}
		default:
			TIMER_START;
			err = H5VLdataset_specific (dp->uo, dp->uvlid, specific_type, dxpl_id, req, arguments);
			TIMER_STOP (dp->fp, TIMER_H5VL_DATASET_SPECIFIC);
	}

	TIMER_STOP (dp->fp, TIMER_DATASET_SPECIFIC);
err_out:;
	return err;
} /* end H5VL_log_dataset_specific() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_optional
 *
 * Purpose:     Perform a connector-specific operation on a dataset
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_optional (void *obj,
								  H5VL_dataset_optional_t optional_type,
								  hid_t dxpl_id,
								  void **req,
								  va_list arguments) {
	H5VL_log_obj_t *op = (H5VL_log_obj_t *)obj;
	herr_t err		   = 0;
	H5VL_log_req_t *rp;
	void **ureqp, *ureq;
	TIMER_START;

	if (req) {
		rp	  = new H5VL_log_req_t ();
		ureqp = &ureq;
	} else {
		ureqp = NULL;
	}

	TIMER_START;
	err = H5VLdataset_optional (op->uo, op->uvlid, optional_type, dxpl_id, req, arguments);
	TIMER_STOP (op->fp, TIMER_H5VL_DATASET_OPTIONAL);

	if (req) {
		rp->append (ureq);
		*req = rp;
	}

	TIMER_STOP (op->fp, TIMER_DATASET_OPTIONAL);
	return err;
} /* end H5VL_log_dataset_optional() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_dataset_close
 *
 * Purpose:     Closes a dataset.
 *
 * Return:      Success:    0
 *              Failure:    -1, dataset not closed.
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_dataset_close (void *dset, hid_t dxpl_id, void **req) {
	herr_t err			= 0;
	H5VL_log_dset_t *dp = (H5VL_log_dset_t *)dset;
	TIMER_START;

	TIMER_START;
	err = H5VLdataset_close (dp->uo, dp->uvlid, dxpl_id, NULL);
	CHECK_ERR
	TIMER_STOP (dp->fp, TIMER_H5VL_DATASET_CLOSE);

	H5Tclose (dp->dtype);

	TIMER_STOP (dp->fp, TIMER_DATASET_CLOSE);

	delete dp;

err_out:;

	return err;
} /* end H5VL_log_dataset_close() */