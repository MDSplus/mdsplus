#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>
#include <hdf5.h>

static const char  *progname = "h5toMds";

static int          d_status = EXIT_SUCCESS;

static const char *tree = 0;
static const char *shot = 0;

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
 
static void PutArray(char dtype, int size, int n_dims, hsize_t *dims, void *ptr, struct descriptor_xd *xd)
{
  /***************************************************
  Store array of data into MDSplus descriptor. Use simple
  array descriptor for one dimensional arrays and a
  coefficient array descriptor for multi-dimensional
  arrays.
  ***************************************************/
  if (n_dims == 1)
  {
    DESCRIPTOR_A(dsc,0,0,0,0);
    dsc.length = size;
    dsc.dtype = dtype;
    dsc.pointer = ptr;
    dsc.arsize = (int)dims[0] * size;
    MdsCopyDxXd(&dsc,xd);
  }
  else
  {
    int i;
    DESCRIPTOR_A_COEFF(dsc, 0,0,0,8,0);
    dsc.length = size;
    dsc.dtype = dtype;
    dsc.pointer = ptr;
    dsc.arsize = size;
    dsc.dimct = n_dims;
    dsc.a0 = ptr;
    for (i=0;i<n_dims;i++)
    {
      dsc.m[i] = (int)dims[i];
      dsc.arsize *= (int)dims[i];
    }   
    MdsCopyDxXd(&dsc,xd);
  }
}

static void PutScalar(char dtype, int size, void *ptr, struct descriptor_xd *xd)
{
  /**********************************************
  Store scalar data in MDSplus descriptor
  **********************************************/
  struct descriptor dsc = {0,0,CLASS_S,0};
  dsc.length = size;
  dsc.dtype = dtype;
  dsc.pointer = ptr;
  MdsCopyDxXd(&dsc,xd);
}

static void PutData(hid_t obj, char dtype, int htype, int size, int n_dims, hsize_t *dims,int is_attr, struct descriptor_xd *xd)
{
  /*********************************************
  Read data from HDF5 file using H5Aread for
  attributes and H5Dread for datasets. Load the
  data into MDSplus nodes.
  *********************************************/
  if (dtype)
  {
    char *mem;
    int  array_size = 1;
    int i;
    for (i=0;i<n_dims;i++) array_size *= dims[i];
    mem = malloc(size*array_size);
    if (is_attr)
      H5Aread ( obj, htype, (void *)mem);
    else
      H5Dread ( obj, htype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)mem);
    if (n_dims > 0)
      PutArray(dtype, size, n_dims, dims, mem, xd);
    else
      PutScalar(dtype, size, mem, xd);
    free(mem);
  }
}

static int find_attr(hid_t attr_id, const char *name, void *op_data)
{
  hid_t obj, type;
  int status;
  h5item *parent = (h5item *)op_data;
  h5item *item = (h5item *)malloc(sizeof(h5item));
  h5item *itm;
  item->item_type = -1;
  item->name=strcpy(malloc(strlen(name)+1),name);
  item->child = 0;
  item->brother = 0;
  item->parent = parent;
  item->obj = 0;
  item->attribute = 0;
  if (parent->attribute == NULL)
  {
    parent->attribute = item;
  }
  else
  {
    for (itm = parent->attribute; itm->brother; itm = itm->brother);
    itm->brother = item;
  }
  if ((obj = H5Aopen_name(attr_id,name)) >= 0) {
    int size;
    char dtype;
    int htype;
    int is_signed;
    hsize_t ds_dims[64];
    hid_t space = H5Aget_space(obj);
    int n_ds_dims = H5Sget_simple_extent_dims(space,ds_dims,0);
    size_t precision;
    H5Dclose(space);
    item->obj = obj;
  }
  return 0;
}

static int find_objs(hid_t group, const char *name, void *op_data)
{
  hid_t obj;
  H5G_stat_t statbuf;
  unsigned int idx = 0;
  int status;
  h5item *parent = (h5item *)op_data;
  h5item *item = (h5item *)malloc(sizeof(h5item));
  h5item *itm;
  item->item_type = 0;
  item->name=strcpy(malloc(strlen(name)+1),name);
  item->child = 0;
  item->brother = 0;
  item->parent = parent;
  item->obj = 0;
  item->attribute = 0;
  if (parent->child == NULL)
  {
    parent->child = item;
  }
  else
  {
    for (itm = parent->child; itm->brother; itm = itm->brother);
    itm->brother = item;
  }
  if (parent->child == item)
    H5Aiterate(group,&idx,find_attr,(void *)0);
  H5Gget_objinfo(group, name, 1, &statbuf);
  item->item_type = statbuf.type;
  switch (statbuf.type) {
  case H5G_GROUP:
    if ((obj = H5Gopen(group, name)) >= 0) {
      H5Giterate(obj, ".", NULL, find_objs, (void *)item);
      H5Gclose (obj);
    }
    break;
  case H5G_DATASET:
    if ((obj = H5Dopen (group, name)) >= 0) {
      item->obj = obj;
      H5Aiterate(obj,&idx,find_attr,(void *)item);
    }
    break;
  }
  return 0;
}

static void ListItem(h5item *item)
{
  char *name = strcpy(malloc(strlen(item->name)+1),item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm=itm->parent) {
    tmp = malloc(strlen(name)+strlen(itm->name)+2);
    strcpy(tmp,itm->name);
    strcat(tmp,"/");
    strcat(tmp,name);
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
  

static int list_one(h5item *item, struct descriptor *xd)
{
  char *name = strcpy(malloc(strlen(item->name)+1),item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm=itm->parent) {
    tmp = malloc(strlen(name)+strlen(itm->name)+2);
    strcpy(tmp,itm->name);
    strcat(tmp,"/");
    strcat(tmp,name);
    free(name);
    name = tmp;
  }
  if (item->item_type == H5G_DATASET || item->item_type == -1)
  {
    struct descriptor name_d = {0,DTYPE_T,CLASS_S,0};
    name_d.length = strlen(name);
    name_d.pointer = name;
    TdiVector(xd,&name_d,xd MDS_END_ARG);
  }
  free(name);
  if (item->attribute)
    list_one(item->attribute,xd);
  if (item->child)
    list_one(item->child,xd);
  if (item->brother)
    list_one(item->brother,xd);
}

int hdf5list(struct descriptor *xd)
{
  MdsFree1Dx(xd,0);
  if (current)
    list_one(current,xd);
}

static int find_one(h5item *item, char *findname, hid_t *obj, int *item_type)
{
  int status = 0;
  char *name = strcpy(malloc(strlen(item->name)+1),item->name);
  char *tmp;
  h5item *itm;
  for (itm = item->parent; itm; itm=itm->parent) {
    tmp = malloc(strlen(name)+strlen(itm->name)+2);
    strcpy(tmp,itm->name);
    strcat(tmp,"/");
    strcat(tmp,name);
    free(name);
    name = tmp;
  }
  if (item->item_type == H5G_DATASET || item->item_type == -1)
  {
    if (strcmp(findname,name) == 0)
    {
      *obj = item->obj;
      *item_type = item->item_type;
      status = 1;
    }
  }
  free(name);
  if ((!status) && item->attribute)
    status = find_one(item->attribute,findname,obj,item_type);
  if ((!status) && item->child)
    status = find_one(item->child,findname,obj,item_type);
  if ((!status) && item->brother)
    status = find_one(item->brother,findname,obj,item_type);
  return status;
}

static int FindItem(char *namein, hid_t *obj, int *item_type)
{
  int status;
  int i;
  char *name = strcpy(malloc(strlen(namein)+1),namein);
  for (i=strlen(name)-1;i >= 0; i--) if (name[i] == 32) name[i]=0;
  status = find_one(current,name,obj,item_type);
  free(name);
  return status;
}
  
int hdf5read(char *name, struct descriptor_xd *xd)
{
  hid_t obj,type;
  H5G_stat_t statbuf;
  int item_type;
  int idx = 0;
  int status = FindItem(name, &obj, &item_type);
  if (status & 1)
  {
    if (item_type == H5G_DATASET) {
      int size;
      char dtype;
      int htype;
      int is_signed;
      hsize_t ds_dims[64];
      hid_t space = H5Dget_space(obj);
      int n_ds_dims = H5Sget_simple_extent_dims(space,ds_dims,0);
      size_t precision;
      H5Dclose(space);
      type = H5Dget_type(obj);
      H5Gget_objinfo(type, ".", 1, &statbuf);
      switch (H5Tget_class(type))
	{
	case H5T_COMPOUND:
	  {
            printf("Compound data is not supported, skipping\n");
            break;
          }
	case H5T_INTEGER:
	  precision = H5Tget_precision(type);
	  is_signed = (H5Tget_sign(type) != H5T_SGN_NONE);
          size = precision/8;
	  switch (precision)
            {
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
	  PutData(obj, dtype, htype, size, n_ds_dims, ds_dims,0,xd);
	  break;
	case H5T_FLOAT:
	  precision = H5Tget_precision(type);
          size = precision/8;
	  switch (precision)
	    {
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
	  PutData(obj, dtype, htype, size, n_ds_dims, ds_dims,0,xd);
	  break;
	case H5T_TIME:
	  printf("dataset is time ---- UNSUPPORTED\n"); break;
	case H5T_STRING:
	  printf("dataset is string\n"); break;
	case H5T_BITFIELD:
	  printf("dataset is bitfield ---- UNSUPPORTED\n"); break;
	case H5T_OPAQUE:
	  printf("dataset is opaque ---- UNSUPPORTED\n"); break;
        case H5T_ARRAY:
	  printf("dataset is array ---- UNSUPPORTED\n"); break;
        case H5T_VLEN:
	  printf("dataset is vlen ---- UNSUPPORTED\n"); break;
        }
      H5Tclose(type);
    }
    else {
      int size;
      char dtype;
      int htype;
      int is_signed;
      hsize_t ds_dims[64];
      hid_t space = H5Aget_space(obj);
      int n_ds_dims = H5Sget_simple_extent_dims(space,ds_dims,0);
      size_t precision;
      H5Dclose(space);
      type = H5Aget_type(obj);
      switch (H5Tget_class(type))
	{
	case H5T_COMPOUND:
	{
	  printf("Compound data is not supported, skipping\n");
	  break;
	}
	case H5T_INTEGER:
	  precision = H5Tget_precision(type);
	  is_signed = (H5Tget_sign(type) != H5T_SGN_NONE);
	  size = precision/8;
	  switch (precision)
	    {
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
	  size = precision/8;
	  switch (precision)
	    {
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
	  PutData(obj, dtype, htype, size, n_ds_dims, ds_dims,1, xd);
	  break;
	case H5T_TIME:
	  printf("dataset is time ---- UNSUPPORTED\n"); break;
	case H5T_STRING:
	  printf("dataset is string\n"); break;
	case H5T_BITFIELD:
	  printf("dataset is bitfield ---- UNSUPPORTED\n"); break;
	case H5T_OPAQUE:
	  printf("dataset is opaque ---- UNSUPPORTED\n"); break;
	case H5T_ARRAY:
	  printf("dataset is array ---- UNSUPPORTED\n"); break;
	case H5T_VLEN:
	  printf("dataset is vlen ---- UNSUPPORTED\n"); break;
	}
      H5Tclose(type);
    }
  }
  return status;
}
  
static void FreeObj(h5item *item)
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

int hdf5close()
{
  if (current)
    FreeObj(current);
  current = 0;
}

int hdf5open(char *fname)
{
    hid_t               fid, gid;
    void               *edata;
    hid_t               (*func)(void*);
    int                 i;
    int status;
    h5item *itm; 

    /* Disable error reporting */
    H5Eget_auto(&func, &edata);
    H5Eset_auto(NULL, NULL);

    fid = H5Fopen(fname, 0, H5P_DEFAULT);

    if (fid < 0) {
        fprintf(stderr,"hdf5open, unable to open file \"%s\"\n", fname);
        return(-1);
    }
    if (current)
      hdf5close();
    current = (h5item *)malloc(sizeof(h5item));
    current->name=strcpy(malloc(1),"");
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

