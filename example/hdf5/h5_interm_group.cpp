/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This program checks if group exists in a file and creates it including
 * all intermediate groups.
 */

#include "hdf5.h"
#include "logvol.h"
#include <cassert>

#define H5FILE_NAME    "interm_group.h5"
#define TRUE            1
#define FALSE           0

int
main(void)
{

    hid_t    file;
    	hid_t log_vlid, faplid;
    hid_t    g1_id, g2_id, g3_id;
    hid_t    grp_crt_plist;
    H5G_info_t g2_info;
    char     name[3];

    herr_t   status;
    int      i;

	MPI_Init(NULL, NULL);

	faplid = H5Pcreate(H5P_FILE_ACCESS);
	// MPI and collective metadata is required by LOG VOL
	H5Pset_fapl_mpio(faplid, MPI_COMM_WORLD, MPI_INFO_NULL);
	H5Pset_all_coll_metadata_ops(faplid, 1);
	H5Pset_vol(faplid, log_vlid, NULL);

    /*
     * Create a file using the default properties.
     */
    file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, faplid); assert(file >= 0);

    /*
     * Create a group in the file.
     */
    g1_id = H5Gcreate2(file, "/G1", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT); assert(g1_id >= 0);

    H5Gclose(g1_id);
    H5Fclose(file);

    /*
     * Now reopen the file and group in the file.
     */
    file = H5Fopen(H5FILE_NAME, H5F_ACC_RDWR, H5P_DEFAULT); assert(file >= 0);

    /*
     * Check if group /G1 exists in the file.
     */
    if(H5Lexists(file, "/G1", H5P_DEFAULT) !=FALSE)
    printf("Group /G1 exists in the file\n");

    /*
     * Check that group G2/G3 exists in /G1 and if not create it using
     * intermediate group creation property.
     */
    g1_id = H5Gopen2(file, "/G1", H5P_DEFAULT); assert(g1_id >= 0);
/* Next commented call causes error stack to be printed out; the next one
 * works fine; is it a bug or a feature? EIP 04-25-07
*/
/*  if (H5Lexists(g1_id, "G2/G3", H5P_DEFAULT) !=TRUE) { */
    if (H5Lexists(g1_id, "G2", H5P_DEFAULT) !=TRUE) {

    grp_crt_plist = H5Pcreate(H5P_LINK_CREATE);

    /* Set flag for intermediate group creation */
    status = H5Pset_create_intermediate_group(grp_crt_plist, TRUE); assert(status == 0);
    g3_id = H5Gcreate2(g1_id, "G2/G3", grp_crt_plist, H5P_DEFAULT, H5P_DEFAULT); assert(g3_id >= 0);
    H5Gclose(g3_id);
    }
    H5Gclose(g1_id);


    /* Now check if group /G1/G2 exists in the file, then open it and print
     * its members names
     */
    if (H5Lexists(file, "/G1/G2", H5P_DEFAULT)) {

    	g2_id = H5Gopen2(file, "/G1/G2", H5P_DEFAULT); assert(g2_id >= 0);
    	status = H5Gget_info(g2_id, &g2_info); assert(status == 0);
    	printf("Group /G1/G2 has %d member(s)\n", (int)g2_info.nlinks);

    	for (i=0; i < (int)g2_info.nlinks; i++) {
    	H5Lget_name_by_idx(g2_id, ".", H5_INDEX_NAME, H5_ITER_NATIVE, (hsize_t)i,
    	                   name, 3, H5P_DEFAULT);
    	printf("Object's name is %s\n", name);

    	}
    H5Gclose(g2_id);
    }
    H5Fclose(file);

	status = H5Pclose(faplid); assert(status == 0);

	MPI_Finalize();

    return 0;
}






