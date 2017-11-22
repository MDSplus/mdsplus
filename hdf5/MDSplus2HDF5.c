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
/*
	Program MDSplus2HDF5

	Program to convert MDSplus Trees into HDF5 files.

	Joshua Stillerman
	9/3/04

	TechX Corporation.

	usage:  MDSplus2HDF5 treename shot-number

	This program will open the tree specified on the command line 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsplus.h>
#include "mds_stdarg.h"
#include "mdsdescrip.h"
#include "mdsshr.h"
#include "tdishr.h"
#include "treeshr.h"
#include "hdf5.h"

#define MAX_COMPLAINT 1024
#define MAX_FILENAME 256

#define MAX_TREENAME 12
#define MAX_DIMS 8
#define MAX_DESCRS 8

static char *tree;
static int shot;

//extern int TdiExecute();

/*
  Routine MemberMangle - return a new name for a member with an '_' in the front.

  Since MDSplus can have both members and children with the same name at any given
  level in the tree, the possibility exists that when we try to add the data for a 
  member to the HDF5 file, its name will already be taken.  If this is the case, try
  sticking an '_' at the back, and try again.

  Note: when the '_' is put in the front, certain HDF5 readers (IDL) have trouble 
  reading in files with an attribute called _name in them.  Hence the descision to 
  put the '_' at the end.
*/
static char *MemberMangle(char *name)
{
  static char ans[MAX_TREENAME + 2];
  ans[0] = 0;
  strcpy(ans, name);
  strcat(ans, "_");
  return (ans);
}

/*
  Routine ChildMangle - return a new name for a member with a '.' in the front.

  Since MDSplus can have both members and children with the same name at any given
  level in the tree, the possibility exists that when we try to add the data for a 
  member to the HDF5 file, its name will already be taken.  In the case of MDSplus 
  complex data structures we are adding a group, so a '.' is more appropriate.
*/
static char *ChildMangle(char *name)
{
  static char ans[MAX_TREENAME + 2];
  ans[0] = '.';
  ans[1] = 0;
  strcat(ans, name);
  return (ans);
}

/*
  Routine GetNidNCI - return a Node ID given an expresion and a
  NID to substitiute into it. 

  Arguments: nid - node id to subsitute into the expression
             expr - Expression to evaluate.  For example:
			        getnci($, "child")

  Will return any integer answer, and zero for all other cases.
*/
static int GetNidNCI(int nid, char *expr)
{
  int status;
  int ans = 0;
  static EMPTYXD(ans_xd);
  DESCRIPTOR_NID(nid_dsc, &nid);
  DESCRIPTOR_FROM_CSTRING(getnci, expr);
  status = TdiExecute(&getnci, &nid_dsc, &ans_xd MDS_END_ARG);
  if (status & 1) {
    struct descriptor *d_ptr;
    for (d_ptr = (struct descriptor *)&ans_xd;
	 d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;
    switch (d_ptr->dtype) {
    case DTYPE_NID:
    case DTYPE_L:
      ans = *(int *)d_ptr->pointer;
      break;
    case DTYPE_LU:
      ans = *(unsigned int *)d_ptr->pointer;
      break;
    case DTYPE_W:
      ans = *(short *)d_ptr->pointer;
      break;
    case DTYPE_WU:
      ans = *(unsigned short *)d_ptr->pointer;
      break;
    case DTYPE_B:
      ans = *(char *)d_ptr->pointer;
      break;
    case DTYPE_BU:
      ans = *(unsigned char *)d_ptr->pointer;
      break;
    default:
      ans = 0;
      break;
    }
  } else
    ans = 0;
  return (ans);
}

/*
  Routines for traversing the children and members of a node.
*/
static int FirstChild(int nid)
{
  return (GetNidNCI(nid, "GETNCI($,'child')"));
}

static int FirstMember(int nid)
{
  return (GetNidNCI(nid, "GETNCI($,'member')"));
}

static int NextSibling(int nid)
{
  return (GetNidNCI(nid, "GETNCI($,'brother')"));
}

/* 
  Routine GetNidInt - routine to return an integer given an 
                      expression and an integer to substitute
					  into it.

  Since NIDs are Ints just use GetNidNCI
*/
static int GetNidInt(int nid, char *expr)
{
  return (GetNidNCI(nid, expr));
}

/*
  Routine GetNidString - Routine to return a string given an
                         expression and a string to substitute
						 into it.

 Arguments
	nid - (int) node identifier to subsitute into the expression
	expression (char *) - expression to evaluate returning a string. 
		For example: getnci($, 'pathname')
 Returns 
	String answer from expression or "" if error or non string 
	type.
*/
static char *GetNidString(int nid, char *expr)
{
  DESCRIPTOR_NID(nid_dsc, &nid);
  DESCRIPTOR_FROM_CSTRING(expr_d, expr);
  static EMPTYXD(ans_xd);
  int status;
  status = TdiExecute(&expr_d, &nid_dsc, &ans_xd MDS_END_ARG);
  if (status & 1) {
    struct descriptor *d_ptr;
    for (d_ptr = (struct descriptor *)&ans_xd;
	 d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;
    if (d_ptr->dtype == DTYPE_T) {
      d_ptr->pointer[d_ptr->length] = 0;
      return (char *)d_ptr->pointer;
    } else
      return ("");
  } else
    return ("");
}

static int has_descendants(int nid)
{
  return (GetNidInt(nid, "GETNCI($, 'NUMBER_OF_CHILDREN')") +
	  GetNidInt(nid, "GETNCI($, 'NUMBER_OF_MEMBERS')") != 0);
}

static int is_child(int nid)
{
  int ans;
  if (nid == 0) {
    ans = 1;
  } else {
    ans = GetNidInt(nid, "GETNCI($, 'is_child')");
  }
  return ans;
}

void ExitOnMDSError(int status, const char *msg)
{
  if (!(status & 1)) {
    fprintf(stderr, "MDS Error\n%s\n%s\n", msg, MdsGetMsg(status));
    exit(0);
  }
}

static void usage(const char *cmd)
{
  fprintf(stderr, "Usage %s tree shot\n", cmd);
}

static void parse_cmdline(int argc, const char *argv[])
{
  if (argc < 3) {
    usage(argv[0]);
    exit(0);
  }
  tree = (char *)argv[1];
  shot = atoi(argv[2]);
}

static hid_t CreateHDF5(char *tree, int shot)
{

  hid_t file_id;
  char filename[MAX_FILENAME];
  sprintf(filename, "%s_%d.h5", tree, shot);
  file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0) {
    fprintf(stderr, "error creating HDF5 file %s\n", filename);
    exit(0);
  }
  return (file_id);
}

hid_t MdsType2HDF5Type(unsigned char type)
{
  switch (type) {
  case DTYPE_FS:
    return (H5T_NATIVE_FLOAT);
  case DTYPE_FT:
    return (H5T_NATIVE_DOUBLE);
  case DTYPE_L:
    return (H5T_NATIVE_LONG);
  case DTYPE_LU:
    return (H5T_NATIVE_ULONG);
  case DTYPE_W:
    return (H5T_NATIVE_SHORT);
  case DTYPE_WU:
    return (H5T_NATIVE_USHORT);
  case DTYPE_B:
    return (H5T_NATIVE_CHAR);
  case DTYPE_BU:
    return (H5T_NATIVE_UCHAR);
  default:
    fprintf(stderr, "type %d not yet supported\n", type);
  }
  return (0);
}

static void PutNumeric(hid_t parent, char *name, struct descriptor *dsc)
{
  //  herr_t status;
  hid_t type = MdsType2HDF5Type(dsc->dtype);
  if (type != 0) {
    hid_t ds_id = H5Screate(H5S_SCALAR);
    hid_t a_id = H5Acreate(parent, name, MdsType2HDF5Type(dsc->dtype), ds_id, H5P_DEFAULT);
    if (a_id < 0) {
      char *new_name = MemberMangle(name);
      a_id = H5Acreate(parent, new_name, MdsType2HDF5Type(dsc->dtype), ds_id, H5P_DEFAULT);
    }
    if (a_id > 0) {
      //      status = H5Awrite(a_id, MdsType2HDF5Type(dsc->dtype), dsc->pointer);
      H5Awrite(a_id, MdsType2HDF5Type(dsc->dtype), dsc->pointer);
      H5Aclose(a_id);
    } else
      fprintf(stderr, "could not create attribute to store scalar in %s\n", name);
  }
}

typedef ARRAY_COEFF(char, MAX_DIMS) ARRAY_AC;

static void PutArray(hid_t parent, char *name, struct descriptor *dsc)
{
  int j;
  //  herr_t status;
  struct descriptor_a *adsc = (struct descriptor_a *)dsc;
  ARRAY_AC *ac_dsc = (ARRAY_AC *) dsc;
  hid_t space_id;
  hid_t ds_id;
  int rank = adsc->dimct;
  hsize_t dim[MAX_DIMS];
  hid_t dtype = MdsType2HDF5Type(dsc->dtype);
  if (dtype > 0) {
    if (adsc->aflags.coeff) {
      for (j = 0; j < rank; j++) {
	dim[j] = ac_dsc->m[adsc->dimct - j - 1];
      }
    } else {
      rank = 1;
      dim[0] = adsc->arsize / adsc->length;
    }
    space_id = H5Screate_simple(rank, dim, NULL);
    ds_id = H5Dcreate(parent, name, dtype, space_id, H5P_DEFAULT);
    if (ds_id < 0) {
      char *new_name = MemberMangle(name);
      ds_id = H5Acreate(parent, new_name, MdsType2HDF5Type(dsc->dtype), ds_id, H5P_DEFAULT);
    }
    if (ds_id > 0) {
      //      status = H5Dwrite(ds_id, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dsc->pointer);
      H5Dwrite(ds_id, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dsc->pointer);
      H5Dclose(ds_id);
      H5Sclose(space_id);
    } else
      fprintf(stderr, "could not create atribute to store array in %s\n", name);
  }
}

static void PutScalar(hid_t parent, char *name, struct descriptor *dsc)
{
  switch (dsc->dtype) {
  case DTYPE_T:
    if (dsc->length > 0) {
      //      herr_t status;
      hid_t ds_id = H5Screate(H5S_SCALAR);
      hsize_t size = dsc->length;
      hid_t type = H5Tcopy(H5T_C_S1);
      H5Tset_size(type, size);
      hid_t a_id = H5Acreate(parent, name, type, ds_id, H5P_DEFAULT);
      if (a_id < 0) {
	char *new_name = MemberMangle(name);
	a_id = H5Acreate(parent, new_name, MdsType2HDF5Type(dsc->dtype), ds_id, H5P_DEFAULT);
      }
      if (a_id > 0) {
	//	status = H5Awrite(a_id, type, dsc->pointer);
	//status = H5Aclose(a_id);
	H5Awrite(a_id, type, dsc->pointer);
	H5Aclose(a_id);
      } else {
	fprintf(stderr, "could not create attribute called %s\n", name);
      }
      //      status = H5Sclose(ds_id);
      H5Sclose(ds_id);
      break;
    }
    MDS_ATTR_FALLTHROUGH
  default:
    PutNumeric(parent, name, dsc);
    break;
  }
  return;
}

typedef RECORD(MAX_DESCRS) RDSC;

static void WriteData(hid_t parent, char *name, struct descriptor *dsc)
{
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  int status;

  struct descriptor *d_ptr;
  if (xd == 0)
    return;
  if ((xd->class == CLASS_XD) || (xd->class == CLASS_XS))
    if (xd->l_length == 0)
      return;
  if ((xd->class == CLASS_D) || (xd->class == CLASS_S))
    if (xd->length == 0)
      return;

  for (d_ptr = (struct descriptor *)xd;
       d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;
  switch (d_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    PutScalar(parent, name, d_ptr);
    break;
  case CLASS_A:
    PutArray(parent, name, d_ptr);
    break;
  case CLASS_CA:{
      static EMPTYXD(xd3);
      status = TdiEvaluate(d_ptr, &xd3 MDS_END_ARG);
      if (status & 1) {
	status = TdiData((struct descriptor *)&xd3, &xd3 MDS_END_ARG);
	if (status & 1) {
	  for (d_ptr = (struct descriptor *)&xd3;
	       d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;

	  PutArray(parent, name, d_ptr);
	}
      }
    }
    break;
  case CLASS_R:{
      RDSC *r_ptr = (RDSC *) d_ptr;
      switch (d_ptr->dtype) {
      case DTYPE_SIGNAL:{
	  int i;
	  hid_t g_id;
	  g_id = H5Gcreate(parent, name, 0);
	  if (g_id < 0) {
	    char *new_name = ChildMangle(name);
	    g_id = H5Gcreate(parent, new_name, 0);
	  }
	  if (g_id > 0) {
	    WriteData(g_id, "data", r_ptr->dscptrs[0]);
	    WriteData(g_id, "raw", r_ptr->dscptrs[1]);
	    for (i = 2; i < r_ptr->ndesc; i++) {
	      char name[8];
	      sprintf(name, "dim%1.1d", i - 2);
	      WriteData(g_id, name, r_ptr->dscptrs[i]);
	    }
	  } else
	    fprintf(stderr, "could not create group for signal components of %s \n", name);
	  break;
	}
      case DTYPE_WITH_UNITS:{
	  hid_t g_id;
	  g_id = H5Gcreate(parent, name, 0);
	  if (g_id < 0) {
	    char *new_name = ChildMangle(name);
	    g_id = H5Gcreate(parent, new_name, 0);
	  }
	  if (g_id > 0) {
	    WriteData(g_id, "data", r_ptr->dscptrs[0]);
	    WriteData(g_id, "units", r_ptr->dscptrs[1]);
	  } else
	    fprintf(stderr, "could not create group for with_units components of %s \n", name);
	  break;
	}
      default:{
	  //       static EMPTYXD(xd2);
	  //      status = TdiData(d_ptr, &xd2);
	  //      if (status & 1)
	  //        WriteData(parent, name, &xd2);
	}
      }
    }

    break;
  default:
    fprintf(stderr, "only scalars and arrays supported at this time\n");
  }
}

/*
  Routine WriteDataNID - Routine to get the data from a nid and call WriteData
		to add an attribute to the HDF5 file for it. 

  Arguments:
		parent - (hid_t) The HDF5 group to add this attribute to.
		name - (char *) name of the attribute to store the data into.
		nid - (int) the nid of the node to evaluate so that it can
			be written to the file.

  Note:  This routine calls WriteData to pick apart the data read back from 
		the node, so that high level MDSplus structures can be recursively
		expanded into the HDF5 file as small groups.  (Signals and With_units)
*/
static void WriteDataNID(hid_t parent, char *name, int nid)
{
  static EMPTYXD(xd);
  DESCRIPTOR_NID(nid_dsc, &nid);
  int status;
  status = TdiEvaluate(&nid_dsc, &xd MDS_END_ARG);
  if (status & 1) {
    WriteData(parent, name, (struct descriptor *)&xd);
  }
}

/*
  Routine AddBranch - main routine of the converter.  Recursively
		traverses all of the members and children of a node creating
		HDF5 Groups if there are descendents, recursing, and finally 
		adding an attribute to hold the node's data if any exisits.

  Arguments:
		nid - (int) Node id to add to the HDF5 hierarchy. If node is not
			a leaf, create a group for it and recursively add its members 
			and children
		parent_id - (hid_t) - The HDF5 parent structure to hang the groups and
			attributes associated with this node from.

  Note:  To handle the case that a node may have a member and a child of the 
		same name, the name of child branches are prefixed with '.'.  In the
		end, this may prove akward, and could be replaced by a name mangling
		which is only done when needed.
*/
static void AddBranch(int nid, hid_t parent_id)
{
  int mem_nid;
  int child_nid;
  hid_t g_id = 0;
  char name[MAX_TREENAME + 1];
  int _is_child;
  int _has_descendants;
  bzero(name, sizeof(name));
  if (nid == 0)
    strcpy(name, "\\TOP");
  else
    strcpy(name, GetNidString(nid, "TRIM(GETNCI($, 'NODE_NAME'))"));
  _is_child = is_child(nid);
  _has_descendants = has_descendants(nid);

  if (_is_child || _has_descendants) {
    g_id = H5Gcreate(parent_id, name, 0);
    /* if it fails assume it was because 
       the name was already taken.  Since the members are 
       added first, tack a '.' in the front.
     */
    if (g_id < 0) {
      char *child_name = ChildMangle(name);
      g_id = H5Gcreate(parent_id, child_name, 0);
    }
    if (g_id >= 0) {
      for (mem_nid = FirstMember(nid); mem_nid; mem_nid = NextSibling(mem_nid)) {
	AddBranch(mem_nid, g_id);
      }
      for (child_nid = FirstChild(nid); child_nid; child_nid = NextSibling(child_nid)) {
	AddBranch(child_nid, g_id);
      }
    } else {
      fprintf(stderr, "Error adding HDF5 Group for %s\n", name);
    }
  }

  if (!_is_child) {
    if (_has_descendants) {
      char *member_name = MemberMangle(name);
      WriteDataNID(parent_id, member_name, nid);
    } else
      WriteDataNID(parent_id, name, nid);
  }
  if (_is_child || _has_descendants)
    H5Gclose(g_id);
}

int main(int argc, const char *argv[])
{
  hid_t file_id;
  parse_cmdline(argc, argv);
  ExitOnMDSError(TreeOpen(tree, shot, 0), "Error opening tree");
  file_id = CreateHDF5(tree, shot);
  /*
   * add \top and all of its members and children 
   */
  AddBranch(0, file_id);
  H5Fclose(file_id);
  return 0;
}
