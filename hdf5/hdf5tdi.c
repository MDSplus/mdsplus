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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hdf5.h>

#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <tdishr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>





typedef struct _h5item {
  int item_type;
  char *name;
  hid_t obj;
  struct _h5item *brother;
  struct _h5item *child;
  struct _h5item *attribute;
  struct _h5item *parent;
} h5item;

static h5item *current = 0;

static void PutArray(char dtype, int size, int n_dims, hsize_t * dims, void *ptr,
		     struct descriptor_xd *xd)
{
  /***************************************************
  Store array of data into MDSplus descriptor. Use simple
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
    MdsCopyDxXd((struct descriptor const *)&dsc, xd);
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
    MdsCopyDxXd((struct descriptor const *)&dsc, xd);
  }
}

static void PutScalar(char dtype, int size, void *ptr, struct descriptor_xd *xd)
{
  /**********************************************
  Store scalar data in MDSplus descriptor
  **********************************************/
  struct descriptor dsc = { 0, 0, CLASS_S, 0 };
  dsc.length = size;
  dsc.dtype = dtype;
  dsc.pointer = ptr;
  MdsCopyDxXd(&dsc, xd);
}

static void PutData(hid_t obj, char dtype, int htype, int size, int n_dims, hsize_t * dims,
		    int is_attr, struct descriptor_xd *xd)
{
  /*********************************************
  Read data from HDF5 file using H5Aread for
  attributes and H5Dread for datasets. Load the
  data into MDSplus nodes.
  *********************************************/
  if (dtype) {
    char *mem;
/*     if (dtype == DTYPE_T) { */
/*       hid_t type = H5Aget_type(obj); */
/*       int slen = H5Tget_size (type); */
/*       if (slen <0) { */
/* 	printf("Badly formed string attribute\n"); */
/*       } else { */
/* 	mem = (char *) malloc((slen+1)*sizeof(char)); */
/* 	hid_t st_id = H5Tcopy (H5T_C_S1); */
/*         size=slen; */
/* 	H5Tset_size (st_id, slen); */
/* 	if (H5Aread(obj, st_id, (void *)mem)) { */
/* 	  printf("error reading string\n"); */
/*           free(mem); */
/*           return; */
/* 	} */
/*       } */
/*     } else { */
    int array_size = 1;
    int i;
    for (i = 0; i < n_dims; i++)
      array_size *= dims[i];
    mem = malloc(size * array_size);
    if (is_attr)
      H5Aread(obj, htype, (void *)mem);
    else
      /* printf("H5Dread: obj = %p, htype = %d, status = %d\n",obj,htype, */
      H5Dread(obj, htype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)mem) /* ) */ ;
/*     } */
    if (n_dims > 0)
      PutArray(dtype, size, n_dims, dims, mem, xd);
    else
      PutScalar(dtype, size, mem, xd);
    free(mem);
  }
}

static int find_attr(hid_t attr_id, const char *name, void *op_data)
{
  hid_t obj;
  h5item *parent = (h5item *) op_data;
  h5item *item = (h5item *) malloc(sizeof(h5item));
  h5item *itm;
  item->item_type = -1;
  item->name = strcpy(malloc(strlen(name) + 1), name);
  item->child = 0;
  item->brother = 0;
  item->parent = parent;
  item->obj = 0;
  item->attribute = 0;
  if (parent) {
    if (parent->attribute == NULL) {
      parent->attribute = item;
    } else {
      for (itm = parent->attribute; itm->brother; itm = itm->brother) ;
      itm->brother = item;
    }
  }
  if ((obj = H5Aopen_name(attr_id, name)) >= 0) {
    hid_t space = H5Aget_space(obj);
    H5Sclose(space);
    item->obj = obj;
  }
  return 0;
}

static int find_objs(hid_t group, const char *name, void *op_data)
{
  hid_t obj;
  H5G_stat_t statbuf;
  unsigned int idx = 0;
  h5item *parent = (h5item *) op_data;
  h5item *item = (h5item *) malloc(sizeof(h5item));
  h5item *itm;
  item->item_type = 0;
  item->name = strcpy(malloc(strlen(name) + 1), name);
  item->child = 0;
  item->brother = 0;
  item->parent = parent;
  item->obj = 0;
  item->attribute = 0;
  if (parent->child == NULL) {
    parent->child = item;
  } else {
    for (itm = parent->child; itm->brother; itm = itm->brother) ;
    itm->brother = item;
  }
  if (parent->child == item)
/*     H5Aiterate(group,&idx,find_attr,(void *)item); */
    H5Aiterate(group, NULL, find_attr, (void *)item);
  //    H5Aiterate(group,&idx,find_attr,(void *)0);
  H5Gget_objinfo(group, name, 1, &statbuf);
  item->item_type = statbuf.type;
  switch (statbuf.type) {
  case H5G_GROUP:
    if ((obj = H5Gopen(group, name)) >= 0) {
      H5Giterate(obj, ".", NULL, find_objs, (void *)item);
      H5Gclose(obj);
    }
    break;
  case H5G_DATASET:
    if ((obj = H5Dopen(group, name)) >= 0) {
      item->obj = obj;
/*       H5Aiterate(obj,&idx,find_attr,(void *)item); */
      H5Aiterate(obj, &idx, find_attr, (void *)item);
    }
    break;
  default:
    break;
  }
  return 0;
}
/*
static void ListItem(h5item * item)
{
  char *name = strcpy(malloc(strlen(item->name) + 1), item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm = itm->parent) {
    tmp = malloc(strlen(name) + strlen(itm->name) + 2);
    strcpy(tmp, itm->name);
    strcat(tmp, "/");
    strcat(tmp, name);
    free(name);
    name = tmp;
  }
  free(name);
  if (item->attribute)
    ListItem(item->attribute);
  if (item->child)
    ListItem(item->child);
  if (item->brother)
    ListItem(item->brother);
}
*/
static void list_one(h5item * item, struct descriptor *xd)
{
  char *name = strcpy(malloc(strlen(item->name) + 1), item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm = itm->parent) {
    tmp = malloc(strlen(name) + strlen(itm->name) + 2);
    strcpy(tmp, itm->name);
    strcat(tmp, "/");
    strcat(tmp, name);
    free(name);
    name = tmp;
  }
  if (item->item_type == H5G_DATASET || item->item_type == -1) {
    struct descriptor name_d = { 0, DTYPE_T, CLASS_S, 0 };
    name_d.length = strlen(name);
    name_d.pointer = name;
    TdiVector(xd, &name_d, xd MDS_END_ARG);
  }
  free(name);
  if (item->attribute)
    list_one(item->attribute, xd);
  if (item->child)
    list_one(item->child, xd);
  if (item->brother)
    list_one(item->brother, xd);
}

EXPORT void hdf5list(struct descriptor *xd)
{
  MdsFree1Dx((struct descriptor_xd *)xd, 0);
  if (current)
    list_one(current, xd);
}

static int find_one(h5item * item, char *findname, hid_t * obj, int *item_type)
{
  int status = 0;
  char *name = strcpy(malloc(strlen(item->name) + 1), item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm = itm->parent) {
    tmp = malloc(strlen(name) + strlen(itm->name) + 2);
    strcpy(tmp, itm->name);
    strcat(tmp, "/");
    strcat(tmp, name);
    free(name);
    name = tmp;
  }
  if (item->item_type == H5G_DATASET || item->item_type == -1) {
    if (strcmp(findname, name) == 0) {
      *obj = item->obj;
      *item_type = item->item_type;
      status = 1;
    }
  }
  free(name);
  if ((!status) && item->attribute)
    status = find_one(item->attribute, findname, obj, item_type);
  if ((!status) && item->child)
    status = find_one(item->child, findname, obj, item_type);
  if ((!status) && item->brother)
    status = find_one(item->brother, findname, obj, item_type);
  return status;
}

static int FindItem(char *namein, hid_t * obj, int *item_type)
{
  int status;
  int i;
  char *name = strcpy(malloc(strlen(namein) + 1), namein);
  for (i = strlen(name) - 1; i >= 0; i--)
    if (name[i] == 32)
      name[i] = 0;
    else
      break;
  status = find_one(current, name, obj, item_type);
  free(name);
  return status;
}

EXPORT int hdf5read(char *name, struct descriptor_xd *xd)
{
  hid_t obj, type;
  int item_type;
  int status = FindItem(name, &obj, &item_type);
  if (status & 1) {
    if (item_type == H5G_DATASET) {
      int size;
      char dtype;
      int htype = 42;
      int is_signed;
      hsize_t ds_dims[64];
      hid_t space = H5Dget_space(obj);
      int n_ds_dims = H5Sget_simple_extent_dims(space, ds_dims, 0);
      size_t precision;
      H5Sclose(space);
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
	PutData(obj, dtype, htype, size, n_ds_dims, ds_dims, 0, xd);
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
	PutData(obj, dtype, htype, size, n_ds_dims, ds_dims, 0, xd);
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
	  PutData(obj, DTYPE_T, st_id, slen, n_ds_dims, ds_dims, 0, xd);
	}
/*        printf("dataset is string\n"); */
/* 	  dtype = DTYPE_T; */
/* 	  htype = H5T_STRING; */
/* 	  PutData(obj, dtype, htype, 0, 0, 0, 1, xd); */
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
      default:
        break;
      }
      H5Tclose(type);
    } else {
      int size;
      char dtype;
      int htype = 42;
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
	PutData(obj, dtype, htype, size, n_ds_dims, ds_dims, 1, xd);
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
	PutData(obj, dtype, htype, size, n_ds_dims, ds_dims, 1, xd);
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
	  PutData(obj, DTYPE_T, st_id, slen, n_ds_dims, ds_dims, 1, xd);
	}
/* 	  dtype = DTYPE_T; */
/* 	  htype = H5T_STRING; */
/* 	  PutData(obj, dtype, htype, 0, 0, 0, 1, xd); */
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
      default:
        break;
      }
      H5Tclose(type);
    }
  }
  return status;
}

static void FreeObj(h5item * item)
{
  if (item->name)
    free(item->name);
  if (item->item_type == H5G_DATASET && item->obj != 0)
    H5Dclose(item->obj);
  else if (item->item_type == -1 && item->obj != 0)
    H5Aclose(item->obj);
  else if (item->item_type == -2 && item->obj != 0)
    H5Fclose(item->obj);
  if (item->brother)
    FreeObj(item->brother);
  if (item->child)
    FreeObj(item->child);
  if (item->attribute)
    FreeObj(item->attribute);
  free(item);
}

EXPORT void hdf5close()
{
  if (current)
    FreeObj(current);
  current = 0;
}

EXPORT int hdf5open(char *fname)
{
  hid_t fid;

  /* Disable error reporting */
  /*
     H5Eget_auto(&func, &edata);
     H5Eset_auto(NULL, NULL);
   */
  fid = H5Fopen(fname, 0, H5P_DEFAULT);

  if (fid < 0) {
    fprintf(stderr, "hdf5open, unable to open file \"%s\"\n", fname);
    return (-1);
  }
  if (current)
    hdf5close();
  current = (h5item *) malloc(sizeof(h5item));
  current->name = strcpy(malloc(1), "");
  current->item_type = -2;
  current->obj = fid;
  current->child = 0;
  current->brother = 0;
  current->parent = 0;
  current->attribute = 0;
  H5Giterate(fid, "/", NULL, find_objs, (void *)current);
  /*
     ListItem(h5files);
   */
  return fid;
}
