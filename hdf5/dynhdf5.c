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
#include <mdsdescrip.h>
#include <hdf5.h>

static herr_t(*H5Aclose_dyn) (hid_t) = 0;
static hid_t(*H5Aget_space_dyn) (hid_t) = 0;
static hid_t(*H5Aget_type_dyn) (hid_t) = 0;
static herr_t(*H5Aiterate_dyn) (hid_t, unsigned *, H5A_operator_t, void *) = 0;
static hid_t(*H5Aopen_name_dyn) (hid_t, const char *name) = 0;
static hid_t(*H5Aread_dyn) (hid_t, hid_t, void *) = 0;
static herr_t(*H5Dclose_dyn) (hid_t) = 0;
static hid_t(*H5Dget_space_dyn) (hid_t) = 0;
static hid_t(*H5Dget_type_dyn) (hid_t) = 0;
static hid_t(*H5Dopen_dyn) (hid_t, const char *name) = 0;
static herr_t(*H5Dread_dyn) (hid_t, hid_t, hid_t, hid_t, hid_t, void *) = 0;
static herr_t(*H5Eget_auto_dyn) (H5E_auto_t *, void **client_data) = 0;
static herr_t(*H5Eset_auto_dyn) (H5E_auto_t func, void *client_data) = 0;
static herr_t(*H5Gget_objinfo_dyn) (hid_t loc_id, const char *name,
				    hbool_t follow_link, H5G_stat_t * statbuf) = 0;
static hid_t(*H5Gopen_dyn) (hid_t loc_id, const char *name) = 0;
static hid_t(*H5Gclose_dyn) (hid_t group) = 0;
static herr_t(*H5Giterate_dyn) (hid_t loc_id, const char *name, int *idx, H5G_iterate_t op,
				void *op_data) = 0;
static hid_t(*H5Fopen_dyn) (const char *filename, unsigned flags, hid_t access_plist) = 0;
static hid_t(*H5Fclose_dyn) (hid_t) = 0;
static herr_t(*H5Sclose_dyn) (hid_t) = 0;
static int (*H5Sget_simple_extent_dims_dyn) (hid_t space_id, hsize_t dims[], hsize_t maxdims[]) = 0;
static hid_t(*H5Tclose_dyn) (hid_t type_id) = 0;
static H5T_class_t(*H5Tget_class_dyn) (hid_t type_id) = 0;
static size_t(*H5Tget_precision_dyn) (hid_t type_id) = 0;
static H5T_sign_t(*H5Tget_sign_dyn) (hid_t type_id) = 0;
static herr_t(*H5open_dyn) (void) = 0;

hid_t H5T_NATIVE_SCHAR_g;
hid_t H5T_NATIVE_UCHAR_g;
hid_t H5T_NATIVE_SHORT_g;
hid_t H5T_NATIVE_USHORT_g;
hid_t H5T_NATIVE_INT_g;
hid_t H5T_NATIVE_UINT_g;
hid_t H5T_NATIVE_LLONG_g;
hid_t H5T_NATIVE_ULLONG_g;
hid_t H5T_NATIVE_FLOAT_g;
hid_t H5T_NATIVE_DOUBLE_g;

static int Initialized = 0;

#define Find(name) name##_dyn=FindSymbol(#name)
#define FindGlobal(name) name=(hid_t)FindSymbol(#name)

static void *FindSymbol(char *name)
{
  void *rtn;
  DESCRIPTOR(image, "hdf5");
  struct descriptor routine = { 0, DTYPE_T, CLASS_S, 0 };
  int status;
  routine.length = strlen(name);
  routine.pointer = name;
  status = LibFindImageSymbol(&image, &routine, &rtn);
  if (!(status & 1)) {
    printf("Error activating hdf5 shared library\n");
    exit(1);
  }
  return rtn;
}

static void Initialize()
{
  if (!Initialized) {
    Find(H5Aclose);
    Find(H5Aget_space);
    Find(H5Aget_type);
    Find(H5Aiterate);
    Find(H5Aopen_name);
    Find(H5Aread);
    Find(H5Dclose);
    Find(H5Dget_space);
    Find(H5Dget_type);
    Find(H5Dopen);
    Find(H5Dread);
    Find(H5Eget_auto);
    Find(H5Eset_auto);
    Find(H5Gget_objinfo);
    Find(H5Gopen);
    Find(H5Giterate);
    Find(H5Gclose);
    Find(H5Fopen);
    Find(H5Fclose);
    Find(H5Sclose);
    Find(H5Sget_simple_extent_dims);
    Find(H5Tclose);
    Find(H5Tget_class);
    Find(H5Tget_precision);
    Find(H5Tget_sign);
    Find(H5open);
    FindGlobal(H5T_NATIVE_SCHAR_g);
    FindGlobal(H5T_NATIVE_UCHAR_g);
    FindGlobal(H5T_NATIVE_SHORT_g);
    FindGlobal(H5T_NATIVE_USHORT_g);
    FindGlobal(H5T_NATIVE_INT_g);
    FindGlobal(H5T_NATIVE_UINT_g);
    FindGlobal(H5T_NATIVE_LLONG_g);
    FindGlobal(H5T_NATIVE_ULLONG_g);
    FindGlobal(H5T_NATIVE_FLOAT_g);
    FindGlobal(H5T_NATIVE_DOUBLE_g);
    Initialized = 1;
  }
}

herr_t H5Aclose(hid_t attr_id)
{
  return (H5Aclose_dyn) (attr_id);
}

hid_t H5Aget_space(hid_t attr_id)
{
  return (H5Aget_space_dyn) (attr_id);
}

hid_t H5Aget_type(hid_t attr_id)
{
  return (H5Aget_type_dyn) (attr_id);
}

herr_t H5Aiterate(hid_t loc_id, unsigned *attr_num, H5A_operator_t op, void *op_data)
{
  return (H5Aiterate_dyn) (loc_id, attr_num, op, op_data);
}

hid_t H5Aopen_name(hid_t loc_id, const char *name)
{
  return (H5Aopen_name_dyn) (loc_id, name);
}

herr_t H5Aread(hid_t attr_id, hid_t type_id, void *buf)
{
  Initialize();
  return (H5Aread_dyn) (attr_id, type_id, buf);
}

herr_t H5Dclose(hid_t dset_id)
{
  return (H5Dclose_dyn) (dset_id);
}

hid_t H5Dget_space(hid_t dset_id)
{
  return (H5Dget_space_dyn) (dset_id);
}

hid_t H5Dget_type(hid_t dset_id)
{
  return (H5Dget_type_dyn) (dset_id);
}

hid_t H5Dopen(hid_t file_id, const char *name)
{
  return (H5Dopen_dyn) (file_id, name);
}

herr_t H5Dread(hid_t dset_id, hid_t mem_type, hid_t mem_space_id, hid_t file_space_id,
	       hid_t plist_id, void *buf)
{
  return (H5Dread_dyn) (dset_id, mem_type, mem_space_id, file_space_id, plist_id, buf);
}

herr_t H5Eget_auto(H5E_auto_t * func, void **client_data)
{
  Initialize();
  return (H5Eget_auto_dyn) (func, client_data);
}

herr_t H5Eset_auto(H5E_auto_t func, void *client_data)
{
  Initialize();
  return (H5Eset_auto_dyn) (func, client_data);
}

herr_t H5Gget_objinfo(hid_t loc_id, const char *name, hbool_t follow_link, H5G_stat_t * statbuf)
{
  return (H5Gget_objinfo_dyn) (loc_id, name, follow_link, statbuf);
}

hid_t H5Gopen(hid_t loc_id, const char *name)
{
  return (H5Gopen_dyn) (loc_id, name);
}

herr_t H5Gclose(hid_t group)
{
  return (H5Gclose_dyn) (group);
}

herr_t H5Giterate(hid_t loc_id, const char *name, int *idx, H5G_iterate_t op, void *op_data)
{
  return (H5Giterate_dyn) (loc_id, name, idx, op, op_data);
}

hid_t H5Fopen(const char *filename, unsigned flags, hid_t access_plist)
{
  Initialize();
  return (H5Fopen_dyn) (filename, flags, access_plist);
}

hid_t H5Fclose(hid_t fid)
{
  return (H5Fclose_dyn) (fid);
}

herr_t H5Sclose(hid_t attr_id)
{
  return (H5Sclose_dyn) (attr_id);
}

int H5Sget_simple_extent_dims(hid_t space_id, hsize_t dims[], hsize_t maxdims[])
{
  return (H5Sget_simple_extent_dims_dyn) (space_id, dims, maxdims);
}

hid_t H5Tclose(hid_t type_id)
{
  return (H5Tclose_dyn) (type_id);
}

H5T_class_t H5Tget_class(hid_t type_id)
{
  return (H5Tget_class_dyn) (type_id);
}

size_t H5Tget_precision(hid_t type_id)
{
  return (H5Tget_precision_dyn) (type_id);
}

H5T_sign_t H5Tget_sign(hid_t type_id)
{
  return (H5Tget_sign_dyn) (type_id);
}

herr_t H5open(void)
{
  Initialize();
  return (H5open_dyn) ();
}

/*

H5T_NATIVE_SCHAR_g
H5T_NATIVE_UCHAR_g
H5T_NATIVE_SHORT_g
H5T_NATIVE_USHORT_g
H5T_NATIVE_INT_g
H5T_NATIVE_UINT_g
H5T_NATIVE_LLONG_g
H5T_NATIVE_ULLONG_g
H5T_NATIVE_FLOAT_g
H5T_NATIVE_DOUBLE_g

H5open

*/
