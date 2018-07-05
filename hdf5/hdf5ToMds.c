/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>
#include <hdf5.h>



static int d_status = EXIT_SUCCESS;

static const char *tree = 0;
static const char *shot = 0;

static void usage(const char *prog)
{
  fflush(stdout);
  fprintf(stdout, "usage: %s file tree shot\n", prog);
}

static void parse_command_line(int argc, const char *argv[])
{
  if (argc < 4) {
    usage(argv[0]);
    exit(1);
  }

  tree = argv[2];
  shot = argv[3];
}

static int AddNode(const char *h5name, int usage)
{

  /********************************************************
  HDF5 datasets do not have restrictions on names whereas
  MDSplus node names are limited to 12 alphanumeric case
  insensitive characters. If the HDF5 name is a valid MDSplus
  node name, this routine will attempt to add it as is. If
  not, the non-alphanumeric characters will be replaced by
  underscores and the name will be truncated. If this fixup
  results in a duplicate node name, the last characters of the
  name will be replaced with 1,2,3,4,.... until a unique name
  is found. An additional node will be added under each node
  called: "HDF5NAME" and the original HDF5 dataset name will
  be loaded into this node.
  *********************************************************/

  int status;
  int memlen = ((strlen(h5name) < 12) ? 12 : strlen(h5name)) + 2;
  char *name = strcpy(malloc(memlen), (usage != TreeUSAGE_STRUCTURE) ? ":" : ".");
  size_t i;
  size_t len;
  int idx = 1;
  int nid;
  for (i = 0; i < strlen(h5name) &&
       ((h5name[i] < 'A' || h5name[i] > 'Z') && (h5name[i] < 'a' || h5name[i] > 'z')); i++) ;
  if (i == strlen(h5name))
    strcat(name, "ITEM");
  else
    strcat(name, &h5name[i]);
  len = strlen(name);
  if (len > 13) {
    name[13] = 0;
    len = 13;
  }
  for (i = 1; i < len; i++) {
    name[i] = toupper(name[i]);
    if ((name[i] < 'A' || name[i] > 'Z') && (name[i] < '0' || name[i] > '9') && (name[i] != '_'))
      name[i] = '_';
  }
  status = TreeAddNode(name, &nid, usage);
  while (status == TreeALREADY_THERE) {
    if (len < 13) {
      for (; len < 13; len++)
	name[len] = '0';
      name[len - 1] = '1';
      name[len] = 0;
    } else if (idx < 10) {
      name[len - 1] = '0' + idx++;
    } else if (idx < 100) {
      sprintf(&name[len - 2], "%2d", idx++);
    } else if (idx < 1000) {
      sprintf(&name[len - 3], "%3d", idx++);
    } else if (idx < 10000) {
      sprintf(&name[len - 4], "%4d", idx++);
    } else {
      printf("Can't make unique node name\n");
      exit(EXIT_FAILURE);
    }
    status = TreeAddNode(name, &nid, usage);
  }
  if (status & 1) {
    int old;
    int name_nid;
    int set_flags = NciM_COMPRESS_ON_PUT;
    struct nci_itm itmlst[] = { {0, NciSET_FLAGS, 0, 0}, {0, NciEND_OF_LIST, 0, 0} };
    struct descriptor h5name_d = { 0, DTYPE_T, CLASS_S, 0 };
    h5name_d.length = strlen(h5name);
    h5name_d.pointer = (char *)h5name;
    itmlst[0].pointer = (char *)&set_flags;
    TreeSetNci(nid, itmlst);
    TreeGetDefaultNid(&old);
    TreeSetDefaultNid(nid);
    TreeAddNode(":HDF5NAME", &name_nid, TreeUSAGE_TEXT);
    TreePutRecord(name_nid, &h5name_d, 0);
  }
  free(name);
  return nid;
}

static void PutArray(int nid, char dtype, int size, int n_dims, hsize_t * dims, void *ptr)
{
  /***************************************************
  Store array of data into MDSplus node. Use simple
  array descriptor for one dimensional arrays and a
  coefficient array descriptor for multi-dimensional
  arrays.
  ***************************************************/
  if (n_dims == 1) {
    DESCRIPTOR_A(dsc, 0, 0, 0, 0);
    dsc.length = size;
    dsc.dtype = dtype;
    dsc.pointer = ptr;
    dsc.arsize = (int)dims[0] * size;
    TreePutRecord(nid, (struct descriptor *)&dsc, 0);
  } else {
    int i;
    DESCRIPTOR_A_COEFF(dsc, 0, 0, 0, 8, 0);
    dsc.length = size;
    dsc.dtype = dtype;
    dsc.pointer = ptr;
    dsc.arsize = size;
    dsc.dimct = n_dims;
    dsc.a0 = ptr;
    for (i = 0; i < n_dims; i++) {
      dsc.m[i] = (int)dims[i];
      dsc.arsize *= (int)dims[i];
    }
    TreePutRecord(nid, (struct descriptor *)&dsc, 0);
  }
}

static void PutScalar(int nid, char dtype, int size, void *ptr)
{
  /**********************************************
  Store scalar data in MDSplus node
  **********************************************/
  struct descriptor dsc = { 0, 0, CLASS_S, 0 };
  dsc.length = size;
  dsc.dtype = dtype;
  dsc.pointer = ptr;
  TreePutRecord(nid, &dsc, 0);
}

static void PutData(hid_t obj, int nid, char dtype, int htype, int size, int n_dims, hsize_t * dims,
		    int is_attr)
{
  /*********************************************
  Read data from HDF5 file using H5Aread for
  attributes and H5Dread for datasets. Load the
  data into MDSplus nodes.
  *********************************************/
  if (dtype) {
    char *mem;
    int array_size = 1;
    int i;
    for (i = 0; i < n_dims; i++)
      array_size *= dims[i];
    mem = malloc(size * array_size);
    if (is_attr)
      H5Aread(obj, htype, (void *)mem);
    else
      H5Dread(obj, htype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)mem);
    if (n_dims > 0)
      PutArray(nid, dtype, size, n_dims, dims, mem);
    else
      PutScalar(nid, dtype, size, mem);
    free(mem);
  }
}

static int mds_find_attr(hid_t attr_id, const char *name, void *op_data __attribute__ ((unused)))
{
  hid_t obj, type;
  int nid;
  int oldnid;
  printf("\tmds_find_attr - %s\n", name);
  TreeGetDefaultNid(&oldnid);
  if ((obj = H5Aopen_name(attr_id, name)) >= 0) {
    int size;
    char dtype;
    int htype;
    int is_signed;
    hsize_t ds_dims[64];
    hid_t space = H5Aget_space(obj);
    int n_ds_dims = H5Sget_simple_extent_dims(space, ds_dims, 0);
    size_t precision;
    H5Sclose(space);
    type = H5Aget_type(obj);
    switch (H5Tget_class(type)) {
    case H5T_COMPOUND:
      {
	printf("Compound data is not supported, skipping\n");
	break;
      }
    case H5T_INTEGER:
      nid = AddNode(name, TreeUSAGE_NUMERIC);
      precision = H5Tget_precision(type);
      is_signed = (H5Tget_sign(type) != H5T_SGN_NONE);
      size = precision / 8;
      switch (precision) {
      case 8:
	dtype = is_signed ? DTYPE_B : DTYPE_BU;
	htype = is_signed ? H5T_NATIVE_CHAR : H5T_NATIVE_UCHAR;
	break;
      case 16:
	dtype = is_signed ? DTYPE_W : DTYPE_WU;
	htype = is_signed ? H5T_NATIVE_SHORT : H5T_NATIVE_USHORT;
	break;
      case 32:
	dtype = is_signed ? DTYPE_L : DTYPE_LU;
	htype = is_signed ? H5T_NATIVE_INT : H5T_NATIVE_UINT;
	break;
      case 64:
	dtype = is_signed ? DTYPE_Q : DTYPE_QU;
	htype = is_signed ? H5T_NATIVE_LLONG : H5T_NATIVE_ULLONG;
	break;
      default:
	dtype = 0;
	break;
      }
      if (dtype)
        PutData(obj, nid, dtype, htype, size, n_ds_dims, ds_dims, 1);
      break;
    case H5T_FLOAT:
      nid = AddNode(name, TreeUSAGE_NUMERIC);
      precision = H5Tget_precision(type);
      size = precision / 8;
      switch (precision) {
      case 32:
	dtype = DTYPE_NATIVE_FLOAT;
	htype = H5T_NATIVE_FLOAT;
	break;
      case 64:
	dtype = DTYPE_NATIVE_DOUBLE;
	htype = H5T_NATIVE_DOUBLE;
	break;
      default:
	dtype = 0;
	break;
      }
      if (dtype)
        PutData(obj, nid, dtype, htype, size, n_ds_dims, ds_dims, 1);
      break;
    case H5T_TIME:
      printf("dataset is time ---- UNSUPPORTED\n");
      break;
    case H5T_STRING:
      nid = AddNode(name, TreeUSAGE_TEXT);
      {
	int slen = H5Tget_size(type);
	// int siz = slen;
	if (slen < 0) {
	  printf("Badly formed string attribute\n");
	} else {
	  hid_t st_id;

#if H5_VERS_MAJOR>=1&&H5_VERS_MINOR>=6&&H5_VERS_RELEASE>=1
	  if (H5Tis_variable_str(type)) {
	    st_id = H5Tcopy(H5T_C_S1);
	    H5Tset_size(st_id, H5T_VARIABLE);
	  } else {
#endif
	    st_id = H5Tcopy(type);
	    H5Tset_cset(st_id, H5T_CSET_ASCII);
#if H5_VERS_MAJOR>=1&&H5_VERS_MINOR>=6&&H5_VERS_RELEASE>=1
	  }
#endif
	  if ((int)H5Tget_size(st_id) > slen) {
	    slen = H5Tget_size(st_id);
	  }
	  H5Tset_size(st_id, slen);
	  PutData(obj, nid, DTYPE_T, st_id, slen, n_ds_dims, ds_dims, 1);
	}
      }
      break;
    case H5T_BITFIELD:
      printf("dataset is bitfield ---- UNSUPPORTED\n");
      break;
    case H5T_OPAQUE:
      printf("dataset is opaque ---- UNSUPPORTED\n");
      break;
    case H5T_ARRAY:
      printf("dataset is array ---- UNSUPPORTED\n");
      break;
    case H5T_VLEN:
      printf("dataset is vlen ---- UNSUPPORTED\n");
      break;
    case H5T_NO_CLASS:
      printf("dataset is no_class ---- UNSUPPORTED\n");
      break;
    case H5T_REFERENCE:
      printf("dataset is reference ---- UNSUPPORTED\n");
      break;
    case H5T_ENUM:
      printf("dataset is enum ---- UNSUPPORTED\n");
      break;
    case H5T_NCLASSES:
      printf("dataset is nclasses ---- UNSUPPORTED\n");
      break;
    default:
      printf("dataset is UNRECOGNIZED dataset ---- UNSUPPORTED\n");
      break;

    }
    H5Tclose(type);
    H5Aclose(obj);
  }
  TreeSetDefaultNid(oldnid);
  return 0;
}

static int mds_find_objs(hid_t group, const char *name, void *op_data)
{
  hid_t obj, type;
  H5G_stat_t statbuf;
  int nid;
  int defnid;
  int *first_time = (int *)op_data;
  TreeGetDefaultNid(&defnid);
  if (*first_time) {
    //    H5Aiterate(group,&idx,mds_find_attr,(void *)0);
    H5Aiterate(group, NULL, mds_find_attr, (void *)0);
    *first_time = 0;
  }
  H5Gget_objinfo(group, name, 1, &statbuf);
  switch (statbuf.type) {
  case H5G_GROUP:
    if ((obj = H5Gopen(group, name)) >= 0) {
      int first_time = 1;
      nid = AddNode(name, TreeUSAGE_STRUCTURE);
      H5Giterate(obj, ".", NULL, mds_find_objs, (void *)&first_time);
      H5Gclose(obj);
    }
    break;
  case H5G_DATASET:
    if ((obj = H5Dopen(group, name)) >= 0) {
      int size;
      char dtype;
      int htype;
      int is_signed;
      hsize_t ds_dims[64];
      hid_t space = H5Dget_space(obj);
      int n_ds_dims = H5Sget_simple_extent_dims(space, ds_dims, 0);
      size_t precision;
      H5Sclose(space);
      nid = AddNode(name, TreeUSAGE_SIGNAL);
      printf("About to iterate on %s\n", name);
      //        H5Aiterate(obj,&idx,mds_find_attr,(void *)0);
      H5Aiterate(obj, NULL, mds_find_attr, (void *)0);
      type = H5Dget_type(obj);
      switch (H5Tget_class(type)) {
      case H5T_COMPOUND:
	{
	  printf("Compound data is not supported, skipping\n");
	  break;
	}
      case H5T_INTEGER:
	precision = H5Tget_precision(type);
	is_signed = (H5Tget_sign(type) != H5T_SGN_NONE);
	size = precision / 8;
	switch (precision) {
	case 8:
	  dtype = is_signed ? DTYPE_B : DTYPE_BU;
	  htype = is_signed ? H5T_NATIVE_CHAR : H5T_NATIVE_UCHAR;
	  break;
	case 16:
	  dtype = is_signed ? DTYPE_W : DTYPE_WU;
	  htype = is_signed ? H5T_NATIVE_SHORT : H5T_NATIVE_USHORT;
	  break;
	case 32:
	  dtype = is_signed ? DTYPE_L : DTYPE_LU;
	  htype = is_signed ? H5T_NATIVE_INT : H5T_NATIVE_UINT;
	  break;
	case 64:
	  dtype = is_signed ? DTYPE_Q : DTYPE_QU;
	  htype = is_signed ? H5T_NATIVE_LLONG : H5T_NATIVE_ULLONG;
	  break;
	default:
	  dtype = 0;
	  break;
	}
        if (dtype)
	  PutData(obj, nid, dtype, htype, size, n_ds_dims, ds_dims, 0);
	break;
      case H5T_FLOAT:
	precision = H5Tget_precision(type);
	size = precision / 8;
	switch (precision) {
	case 32:
	  dtype = DTYPE_NATIVE_FLOAT;
	  htype = H5T_NATIVE_FLOAT;
	  break;
	case 64:
	  dtype = DTYPE_NATIVE_DOUBLE;
	  htype = H5T_NATIVE_DOUBLE;
	  break;
	default:
	  dtype = 0;
	  break;
	}
        if (dtype)
	  PutData(obj, nid, dtype, htype, size, n_ds_dims, ds_dims, 0);
	break;
      case H5T_TIME:
	printf("dataset is time ---- UNSUPPORTED\n");
	break;
      case H5T_STRING:
	{
	  int slen = H5Tget_size(type);
	  hid_t st_id;
	  if (slen < 0) {
	    printf("Badly formed string attribute\n");
	    return 0;
	  }
#if H5_VERS_MAJOR>=1&&H5_VERS_MINOR>=6&&H5_VERS_RELEASE>=1
	  if (H5Tis_variable_str(type)) {
	    st_id = H5Tcopy(H5T_C_S1);
	    H5Tset_size(st_id, H5T_VARIABLE);
	  } else {
#endif
	    st_id = H5Tcopy(type);
	    H5Tset_cset(st_id, H5T_CSET_ASCII);
#if H5_VERS_MAJOR>=1&&H5_VERS_MINOR>=6&&H5_VERS_RELEASE>=1
	  }
#endif
	  if ((int)H5Tget_size(st_id) > slen) {
	    slen = H5Tget_size(st_id);
	  }
	  H5Tset_size(st_id, slen);
	  PutData(obj, nid, DTYPE_T, st_id, slen, n_ds_dims, ds_dims, 0);
	}
	break;
      case H5T_BITFIELD:
	printf("dataset is bitfield ---- UNSUPPORTED\n");
	break;
      case H5T_OPAQUE:
	printf("dataset is opaque ---- UNSUPPORTED\n");
	break;
      case H5T_ARRAY:
	printf("dataset is array ---- UNSUPPORTED\n");
	break;
      case H5T_VLEN:
	printf("dataset is vlen ---- UNSUPPORTED\n");
	break;
      case H5T_NO_CLASS:
        printf("dataset is no_clasee ---- UNSUPPORTED\n");
        break;
      case H5T_REFERENCE:
        printf("dataset is reference ---- UNSUPPORTED\n");
        break;
      case H5T_ENUM:
        printf("dataset is enum ---- UNSUPPORTED\n");
        break;
      case H5T_NCLASSES:
        printf("dataset is nclasses ---- UNSUPPORTED\n");
        break;
      default:
        printf("dataset is UNRECOGNIZED CASE ---- UNSUPPORTED\n");
        break;
      }
      H5Tclose(type);
      H5Dclose(obj);
    }
    break;
  case H5G_TYPE:
    nid = AddNode(name, TreeUSAGE_STRUCTURE);
    printf("Type is H5G_TYPE ---- UNSUPPORTED\n");
    break;
  default:
    nid = AddNode(name, TreeUSAGE_STRUCTURE);
    printf("Unknown type\n");
    break;
  }
  TreeSetDefaultNid(defnid);
  return 0;
}

/*-------------------------------------------------------------------------
 * Function:    main
 *
 * Purpose:     HDF5 to MDSplus tree
 *
 * Return:      Success:    0
 *              Failure:    1
 *
 * Programmer:  Tom Fredian
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int main(int argc, const char *argv[])
{
  hid_t fid;
  const char *fname = NULL;
  int first_time = 1;
  int status;

  parse_command_line(argc, argv);

  /* Disable error reporting */
  /*
     H5Eget_auto(&func, &edata);
     H5Eset_auto(NULL, NULL);
   */

  if (argv[1][0] == '\\')
    fname = &argv[1][1];
  else
    fname = argv[1];

  fid = H5Fopen(fname, 0, H5P_DEFAULT);

  if (fid < 0) {
    fprintf(stderr, "h5toMds, unable to open file \"%s\"\n", fname);
    exit(EXIT_FAILURE);
  }

  status = TreeOpenNew((char *)tree, atoi(shot));
  if (!status & 1) {
    printf("Error creating new tree for treename /%s/, shot number /%s/\n", tree, shot);
    exit(EXIT_FAILURE);
  }
  /* find all shared objects */
  H5Giterate(fid, "/", NULL, mds_find_objs, (void *)&first_time);

  if (H5Fclose(fid) < 0)
    d_status = EXIT_FAILURE;

  /*
     H5Eset_auto(func, edata);
   */
  TreeWriteTree(0, 0);
  TreeClose(0, 0);
  return d_status;
}
