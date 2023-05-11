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
#include "MDSplus_Connection.h"
#include "MDSplus_Data.h"
#include "MDSplus_Event.h"
#include "MDSplus_REvent.h"
#include "MDSplus_Tree.h"
#include "MDSplus_TreeNode.h"
#include <ctype.h>
#include <dbidef.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsplus.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <ncidef.h>
#include <pthread_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>
#include <treeshr_messages.h>
#ifdef _WIN32
#include <windows.h>
#define setenv(name, value, overwrite) _putenv_s(name, value)
#endif

#define PTR2JLONG(ctx) (jlong)(int64_t)(intptr_t)(ctx)
#define JLONG2PTR(ctx) (void *)(intptr_t)(int64_t)(ctx)

#define CTXCALL0(FUN) (ctx ? _##FUN(ctx) : FUN())
#define CTXCALLR(FUN, ...) (ctx ? _##FUN(&ctx, __VA_ARGS__) : FUN(__VA_ARGS__))
#define CTXCALLN(FUN, ...) (ctx ? _##FUN(ctx, __VA_ARGS__) : FUN(__VA_ARGS__))

#ifdef _WIN32
static char critSectInitialized = 0;
static CRITICAL_SECTION critSect;
#else
static pthread_mutex_t evMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static void set_ctx_field(JNIEnv *env, jobject jobj, void *ctx)
{
  jclass cls = (*env)->GetObjectClass(env, jobj);
  if (!cls)
    return; // be safe during finalize
  jfieldID ctxFid = (*env)->GetFieldID(env, cls, "ctx", "J");
  if (!ctxFid)
    return; // be safe during finalize
  (*env)->SetLongField(env, jobj, ctxFid, PTR2JLONG(ctx));
}

static int doAction(void *ctx, int nid)
{
  // EXPORT int doAction(int nid)
  int rstatus = 0;
  DESCRIPTOR_NID(nid_d, &nid);
  DESCRIPTOR_LONG(status_d, &rstatus);
  int status = CTXCALLR(TdiDoTask, &nid_d, &status_d MDS_END_ARG);
  if (STATUS_NOT_OK)
    return status;
  NCI_ITM setnci[] = {
      {sizeof(int), NciSTATUS, 0, 0},
      {0, NciEND_OF_LIST, 0, 0},
  };
  setnci[0].pointer = (unsigned char *)&rstatus;
  status = _TreeSetNci(ctx, nid, setnci);
  if (STATUS_OK)
    return rstatus;
  return status;
}

extern int GetAnswerInfoTS(int sock, char *dtype, short *length, char *ndims,
                           int *dims, int *numbytes, void **dptr, void **m);

#ifdef DEBUG
static void printDecompiled(mdsdsc_t *dsc)
{
  EMPTYXD(out_xd);
  char decompiled[1024];
  TdiDecompile(dsc, &out_xd MDS_END_ARG);
  if (!out_xd.pointer)
    printf("NULL\n");
  memcpy(decompiled, out_xd.pointer->pointer, out_xd.pointer->length);
  decompiled[out_xd.pointer->length] = 0;
  printf("%s\n", decompiled);
  MdsFree1Dx(&out_xd, 0);
}
static void printDecompiled1(void *ctx, mdsdsc_t *dsc)
{
  EMPTYXD(out_xd);
  char decompiled[1024];
  CTXCALLR(TdiDecompile, dsc, &out_xd MDS_END_ARG);
  if (!out_xd.pointer)
    printf("NULL\n");
  memcpy(decompiled, out_xd.pointer->pointer, out_xd.pointer->length);
  decompiled[out_xd.pointer->length] = 0;
  printf("%s\n", decompiled);
  MdsFree1Dx(&out_xd, 0);
}
#endif

static void FreeDescrip(mdsdsc_t *desc);

static jintArray getDimensions(JNIEnv *env, void *dsc)
{
  ARRAY_COEFF(char *, 256) *arrD = dsc;
  jintArray jdims;
  int dim = (arrD->length > 0) ? arrD->arsize / arrD->length : 0;
  jdims = (*env)->NewIntArray(env, arrD->dimct);
  if (jdims)
  {
    if (arrD->dimct == 1)
      (*env)->SetIntArrayRegion(env, jdims, 0, 1, (const jint *)&dim);
    else
      (*env)->SetIntArrayRegion(env, jdims, 0, arrD->dimct,
                                (const jint *)arrD->m);
  }
  return jdims;
}

static jobject DescripToObject(JNIEnv *env, void *ctx, mdsdsc_t *desc,
                               jobject helpObj, jobject unitsObj,
                               jobject errorObj, jobject validationObj)
{
  jclass cls, data_cls;
  jmethodID constr;
  int i, length, count, status;
  unsigned int opcode;
  jobject obj, exc, curr_obj, ris;
  jvalue args[8];
  jbyteArray jbytes;
  jshortArray jshorts;
  jintArray jints;
  jlongArray jlongs;
  jfloatArray jfloats;
  jdoubleArray jdoubles;
  jobjectArray jobjects;
  jfieldID data_fid, opcode_fid;
  float *float_buf;
  double *double_buf;

  char message[64];
  mdsdsc_a_t *array_d;
  struct descriptor_r *record_d;
  char *buf;
  // EMPTYXD(float_xd);
  EMPTYXD(ca_xd);
  int is_ca = 0;

  if (!desc)
  {
    return NULL;
  }
  // printf("DescripToObject dtype = %d class = %d\n", desc->dtype,
  // desc->class);

  if (desc->class == CLASS_XD)
    return DescripToObject(env, ctx, ((mdsdsc_xd_t *)desc)->pointer,
                           helpObj, unitsObj, errorObj, validationObj);
  memset(&args, 0, sizeof(args));
  switch (desc->class)
  {
  case CLASS_S:
    // printf("CLASS_S\n");
    args[1].l = helpObj;
    args[2].l = unitsObj;
    args[3].l = errorObj;
    args[4].l = validationObj;
    switch (desc->dtype)
    {
    case DTYPE_BU:
      cls = (*env)->FindClass(env, "MDSplus/Uint8");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(BLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].b = *(char *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_B:
      cls = (*env)->FindClass(env, "MDSplus/Int8");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(BLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].b = *(char *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_WU:
      cls = (*env)->FindClass(env, "MDSplus/Uint16");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(SLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].s = *(short *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_W:
      cls = (*env)->FindClass(env, "MDSplus/Int16");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(SLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].s = *(short *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_LU:
      cls = (*env)->FindClass(env, "MDSplus/Uint32");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].i = *(int *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_L:
      cls = (*env)->FindClass(env, "MDSplus/Int32");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].i = *(int *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_NID:
      cls = (*env)->FindClass(env, "MDSplus/TreeNode");
      //      constr = (*env)->GetStaticMethodID(env, cls, "getData",
      //                                         "(I)LMDSplus/TreeNode;");
      constr = (*env)->GetStaticMethodID(env, cls, "getData",
                                         "(ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                         "Data;LMDSplus/Data;)LMDSplus/TreeNode;");
      args[0].i = *(int *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_QU:
      cls = (*env)->FindClass(env, "MDSplus/Uint64");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(JLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].j = *(long *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_Q:
      cls = (*env)->FindClass(env, "MDSplus/Int64");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(JLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      args[0].j = *(long *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_OU:
      cls = (*env)->FindClass(env, "MDSplus/Uint128");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([JLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, 2);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, 2, (jlong *)desc->pointer);
      args[0].l = jlongs;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_O:
      cls = (*env)->FindClass(env, "MDSplus/Int128");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([JLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, 2);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, 2, (jlong *)desc->pointer);
      args[0].l = jlongs;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_FS:
    case DTYPE_F:
    case DTYPE_FSC:
      cls = (*env)->FindClass(env, "MDSplus/Float32");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(FLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      if (desc->dtype != DTYPE_FS)
        CvtConvertFloat(desc->pointer, desc->dtype, &args[0].f, DTYPE_FS, 0);
      else
        args[0].f = *(float *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_FTC:
    case DTYPE_FT:
    case DTYPE_D:
    case DTYPE_G:
      cls = (*env)->FindClass(env, "MDSplus/Float64");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(DLMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      CvtConvertFloat(desc->pointer, desc->dtype, &args[0].d, DTYPE_DOUBLE, 0);
      args[0].d = *(double *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_T:
      cls = (*env)->FindClass(env, "MDSplus/String");
      constr = (*env)->GetStaticMethodID(
          env, cls, "getData",
          "(Ljava/lang/String;LMDSplus/Data;LMDSplus/Data;LMDSplus/"
          "Data;LMDSplus/Data;)LMDSplus/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_IDENT:
      cls = (*env)->FindClass(env, "MDSplus/Ident");
      constr = (*env)->GetStaticMethodID(
          env, cls, "getData",
          "(Ljava/lang/String;LMDSplus/Data;LMDSplus/Data;LMDSplus/"
          "Data;LMDSplus/Data;)LMDSplus/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_PATH:
      cls = (*env)->FindClass(env, "MDSplus/TreePath");
      constr = (*env)->GetStaticMethodID(
          env, cls, "getData",
          "(Ljava/lang/String;LMDSplus/Data;LMDSplus/Data;LMDSplus/"
          "Data;LMDSplus/Data;)LMDSplus/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      ris = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
      return ris;
      // return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_DSC:
      if(desc->length == 0)
        return NULL;
      return DescripToObject(env, ctx, (mdsdsc_t *)(desc->pointer), 0, 0, 0, 0);
    case DTYPE_MISSING:
      return NULL;

    default:
      sprintf(message, "Datatype %d not supported for class CLASS_S",
              desc->dtype);
      exc = (*env)->FindClass(env, "MdsException");
      (*env)->ThrowNew(env, exc, message);
      return NULL;
    }
  case CLASS_CA:
    status = CTXCALLN(TdiData, desc, &ca_xd MDS_END_ARG);
    if (STATUS_NOT_OK)
    {
      printf("Cannot evaluate CA descriptor\n");
      return NULL;
    }
    is_ca = 1;

    MDS_ATTR_FALLTHROUGH
  case CLASS_A:
    // printf("CLASS_A\n");
    args[2].l = helpObj;
    args[3].l = unitsObj;
    args[4].l = errorObj;
    args[5].l = validationObj;
    if (is_ca)
      array_d = (mdsdsc_a_t *)ca_xd.pointer;
    else
      array_d = (mdsdsc_a_t *)desc;

    args[1].l = getDimensions(env, array_d);
    length = (array_d->length != 0) ? array_d->arsize / array_d->length : 0;
    switch (array_d->dtype)
    {
    case DTYPE_MISSING:
      return NULL;
    case DTYPE_BU:
      cls = (*env)->FindClass(env, "MDSplus/Uint8Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([B[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jbytes = (*env)->NewByteArray(env, length);
      if (jbytes)
        (*env)->SetByteArrayRegion(env, jbytes, 0, length,
                                   (jbyte *)array_d->pointer);
      args[0].l = jbytes;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_B:
      cls = (*env)->FindClass(env, "MDSplus/Int8Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([B[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jbytes = (*env)->NewByteArray(env, length);
      if (jbytes)
        (*env)->SetByteArrayRegion(env, jbytes, 0, length,
                                   (jbyte *)array_d->pointer);
      args[0].l = jbytes;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_WU:
      cls = (*env)->FindClass(env, "MDSplus/Uint16Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([S[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jshorts = (*env)->NewShortArray(env, length);
      if (jshorts)
        (*env)->SetShortArrayRegion(env, jshorts, 0, length,
                                    (jshort *)array_d->pointer);
      args[0].l = jshorts;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_W:
      cls = (*env)->FindClass(env, "MDSplus/Int16Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([S[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jshorts = (*env)->NewShortArray(env, length);
      if (jshorts)
        (*env)->SetShortArrayRegion(env, jshorts, 0, length,
                                    (jshort *)array_d->pointer);
      args[0].l = jshorts;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_LU:
      cls = (*env)->FindClass(env, "MDSplus/Uint32Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([I[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jints = (*env)->NewIntArray(env, length);
      if (jints)
        (*env)->SetIntArrayRegion(env, jints, 0, length,
                                  (jint *)array_d->pointer);
      args[0].l = jints;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_L:
      cls = (*env)->FindClass(env, "MDSplus/Int32Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([I[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jints = (*env)->NewIntArray(env, length);
      if (jints)
        (*env)->SetIntArrayRegion(env, jints, 0, length,
                                  (jint *)array_d->pointer);
      args[0].l = jints;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_QU:
      cls = (*env)->FindClass(env, "MDSplus/Uint64Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([J[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, length);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, length,
                                   (jlong *)array_d->pointer);
      args[0].l = jlongs;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_Q:
      cls = (*env)->FindClass(env, "MDSplus/Int64Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([J[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, length);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, length,
                                   (jlong *)array_d->pointer);
      args[0].l = jlongs;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_OU:
      cls = (*env)->FindClass(env, "MDSplus/Uint64Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([J[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, 2 * length);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, 2 * length,
                                   (jlong *)array_d->pointer);
      args[0].l = jlongs;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_O:
      cls = (*env)->FindClass(env, "MDSplus/Int64Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([J[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jlongs = (*env)->NewLongArray(env, 2 * length);
      if (jlongs)
        (*env)->SetLongArrayRegion(env, jlongs, 0, 2 * length,
                                   (jlong *)array_d->pointer);
      args[0].l = jlongs;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_FS:
    case DTYPE_F:
    case DTYPE_FSC:
      float_buf = malloc(sizeof(float) * length);
      if (array_d->dtype == DTYPE_FS)
        memcpy(float_buf, array_d->pointer, sizeof(float) * length);
      else
        for (i = 0; i < length; i++)
          CvtConvertFloat(&((float *)array_d->pointer)[i], desc->dtype,
                          &float_buf[i], DTYPE_FS, 0);
      cls = (*env)->FindClass(env, "MDSplus/Float32Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([F[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jfloats = (*env)->NewFloatArray(env, length);
      if (jfloats)
        (*env)->SetFloatArrayRegion(env, jfloats, 0, length,
                                    (jfloat *)float_buf);
      free(float_buf);
      args[0].l = jfloats;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_FTC:
    case DTYPE_FT:
    case DTYPE_D:
    case DTYPE_G:
      double_buf = malloc(sizeof(double) * length);
      for (i = 0; i < length; i++)
        CvtConvertFloat(&((double *)array_d->pointer)[i], desc->dtype,
                        &double_buf[i], DTYPE_DOUBLE, 0);
      cls = (*env)->FindClass(env, "MDSplus/Float64Array");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "([D[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Data;");
      jdoubles = (*env)->NewDoubleArray(env, length);
      if (jdoubles)
        (*env)->SetDoubleArrayRegion(env, jdoubles, 0, length,
                                     (jdouble *)double_buf);
      free(double_buf);
      args[0].l = jdoubles;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_T:
      cls = (*env)->FindClass(env, "MDSplus/StringArray");
      constr = (*env)->GetStaticMethodID(
          env, cls, "getData",
          "([Ljava/lang/String;[ILMDSplus/Data;LMDSplus/Data;LMDSplus/"
          "Data;LMDSplus/Data;)LMDSplus/Data;");
      data_cls = (*env)->FindClass(env, "java/lang/String");
      jobjects = (*env)->NewObjectArray(env, length, data_cls, 0);
      if (jobjects)
      {
        buf = malloc(array_d->length + 1);
        buf[array_d->length] = 0;
        for (i = 0; i < length; i++)
        {
          memcpy(buf, &array_d->pointer[i * array_d->length], array_d->length);
          (*env)->SetObjectArrayElement(
              env, jobjects, i, (jobject)(*env)->NewStringUTF(env, buf));
        }
        free(buf);
      }
      args[0].l = jobjects;
      if (is_ca)
        MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    default:
      sprintf(message, "Datatype %d not supported for class CLASS_A",
              desc->dtype);
      exc = (*env)->FindClass(env, "MdsException");
      (*env)->ThrowNew(env, exc, message);
      return NULL;
    }

  case CLASS_R:

    args[0].l = helpObj;
    args[1].l = unitsObj;
    args[2].l = errorObj;
    args[3].l = validationObj;

    record_d = (struct descriptor_r *)desc;
    // printf("CLASS_R %d\n", record_d->dtype);
    switch (record_d->dtype)
    {
    case DTYPE_PARAM:
      return DescripToObject(
          env, ctx, record_d->dscptrs[0],
          DescripToObject(env, ctx, record_d->dscptrs[1], 0, 0, 0, 0), unitsObj,
          errorObj,
          DescripToObject(env, ctx, record_d->dscptrs[2], 0, 0, 0, 0));
    case DTYPE_WITH_UNITS:
      return DescripToObject(
          env, ctx, record_d->dscptrs[0], helpObj,
          DescripToObject(env, ctx, record_d->dscptrs[1], 0, 0, 0, 0), errorObj,
          validationObj);
    case DTYPE_WITH_ERROR:
      return DescripToObject(
          env, ctx, record_d->dscptrs[0], helpObj, unitsObj,
          DescripToObject(env, ctx, record_d->dscptrs[1], 0, 0, 0, 0),
          validationObj);
    case DTYPE_SIGNAL:
      cls = (*env)->FindClass(env, "MDSplus/Signal");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Signal;");
      break;
    case DTYPE_DIMENSION:
      cls = (*env)->FindClass(env, "MDSplus/Dimension");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Dimension;");
      break;
    case DTYPE_WINDOW:
      cls = (*env)->FindClass(env, "MDSplus/Window");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Window;");
      break;
    case DTYPE_FUNCTION:
      cls = (*env)->FindClass(env, "MDSplus/Function");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Function;");
      break;
    case DTYPE_CONGLOM:
      cls = (*env)->FindClass(env, "MDSplus/Conglom");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Conglom;");
      break;
    case DTYPE_ACTION:
      cls = (*env)->FindClass(env, "MDSplus/Action");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Action;");
      break;
    case DTYPE_DISPATCH:
      cls = (*env)->FindClass(env, "MDSplus/Dispatch");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Dispatch;");
      break;
    case DTYPE_PROGRAM:
      cls = (*env)->FindClass(env, "MDSplus/Program");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Program;");
      break;
    case DTYPE_ROUTINE:
      cls = (*env)->FindClass(env, "MDSplus/Routine");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Routine;");
      break;
    case DTYPE_PROCEDURE:
      cls = (*env)->FindClass(env, "MDSplus/Procedure");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Procedure;");
      break;
    case DTYPE_METHOD:
      cls = (*env)->FindClass(env, "MDSplus/Method");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Method;");
      break;
    case DTYPE_DEPENDENCY:
      cls = (*env)->FindClass(env, "MDSplus/Dependency");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Dependency;");
      break;
    case DTYPE_CONDITION:
      cls = (*env)->FindClass(env, "MDSplus/Condition");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Condition;");
      break;
    case DTYPE_CALL:
      cls = (*env)->FindClass(env, "MDSplus/Call");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Call;");
      break;
    case DTYPE_SLOPE:
    case DTYPE_RANGE:
      cls = (*env)->FindClass(env, "MDSplus/Range");
      constr =
          (*env)->GetStaticMethodID(env, cls, "getData",
                                    "(LMDSplus/Data;LMDSplus/Data;LMDSplus/"
                                    "Data;LMDSplus/Data;)LMDSplus/Range;");
      break;
    default:
      sprintf(message, "Datatype %d not supported for class CLASS_R",
              desc->dtype);
      exc = (*env)->FindClass(env, "MdsException");
      (*env)->ThrowNew(env, exc, message);
      return NULL;
    }
    obj = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    data_cls = (*env)->FindClass(env, "MDSplus/Data");
    jobjects = (*env)->NewObjectArray(env, record_d->ndesc, data_cls, 0);
    if (jobjects)
    {
      for (i = count = 0; count < record_d->ndesc; i++, count++)
      {
        (*env)->SetObjectArrayElement(
            env, jobjects, i,
            DescripToObject(env, ctx, record_d->dscptrs[i], 0, 0, 0, 0));
      }
    }
    data_fid = (*env)->GetFieldID(env, cls, "descs", "[LMDSplus/Data;");
    (*env)->SetObjectField(env, obj, data_fid, jobjects);
    if (record_d->pointer &&
        (record_d->dtype == DTYPE_FUNCTION ||
         record_d->dtype == DTYPE_DEPENDENCY ||
         record_d->dtype == DTYPE_CONDITION || record_d->dtype == DTYPE_CALL ||
         record_d->dtype == DTYPE_DISPATCH))
    {
      opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
      opcode = 0;
      switch (record_d->length)
      {
      case 1:
        opcode = *(unsigned char *)record_d->pointer;
        break;
      case 2:
        opcode = *(unsigned short *)record_d->pointer;
        break;
      default:
        opcode = *(unsigned int *)record_d->pointer;
        break;
      }
      (*env)->SetIntField(env, obj, opcode_fid, opcode);
    }
    return obj;
  case CLASS_APD:
    args[1].l = helpObj;
    args[2].l = unitsObj;
    args[3].l = errorObj;
    args[4].l = validationObj;
    array_d = (mdsdsc_a_t *)desc;
    length = array_d->arsize / array_d->length;
    switch (desc->dtype)
    {
    case DTYPE_LIST:
      cls = (*env)->FindClass(env, "MDSplus/List");
      break;
    case DTYPE_DICTIONARY:
      cls = (*env)->FindClass(env, "MDSplus/Dictionary");
      break;
    default:
      cls = (*env)->FindClass(env, "MDSplus/Apd");
      break;
    }
    data_cls = (*env)->FindClass(env, "MDSplus/Data");
    constr = (*env)->GetStaticMethodID(
        env, cls, "getData",
        "([LMDSplus/Data;LMDSplus/Data;LMDSplus/Data;LMDSplus/Data;LMDSplus/"
        "Data;)LMDSplus/Data;");
    jobjects = (*env)->NewObjectArray(env, length, data_cls, 0);
    if (jobjects)
    {
      for (i = 0; i < length; i++)
      {
        if(((mdsdsc_t **)array_d->pointer)[i])
        {
          if ((curr_obj = DescripToObject(
                 env, ctx, ((mdsdsc_t **)array_d->pointer)[i], 0, 0, 0,
                 0)))
          {
            (*env)->SetObjectArrayElement(env, jobjects, i, curr_obj);
          }
        }
      }
    }
    args[0].l = jobjects;
    return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
  default:
    sprintf(message, "class %d not supported", desc->class);
    exc = (*env)->FindClass(env, "MdsException");
    (*env)->ThrowNew(env, exc, message);
    return NULL;
  }
  return 0;
}

static mdsdsc_t *completeDescr(mdsdsc_t *dataDscPtr,
                               mdsdsc_t *helpDscPtr,
                               mdsdsc_t *unitsDscPtr,
                               mdsdsc_t *errorDscPtr,
                               mdsdsc_t *validationDscPtr)
{
  DESCRIPTOR_PARAM(templateParam, 0, 0, 0);
  DESCRIPTOR_WITH_UNITS(templateWithUnits, 0, 0);
  DESCRIPTOR_WITH_ERROR(templateWithError, 0, 0);
  struct descriptor_param *currParamDsc;
  struct descriptor_with_units *currWithUnitsDsc;
  struct descriptor_with_error *currWithErrorDsc;

  if (errorDscPtr)
  {
    currWithErrorDsc = (struct descriptor_with_error *)malloc(
        sizeof(struct descriptor_with_error));
    memcpy(currWithErrorDsc, &templateWithError,
           sizeof(struct descriptor_with_error));
    currWithErrorDsc->error = errorDscPtr;
    currWithErrorDsc->data =
        completeDescr(dataDscPtr, helpDscPtr, unitsDscPtr, 0, validationDscPtr);
    return (mdsdsc_t *)currWithErrorDsc;
  }
  if (unitsDscPtr)
  {
    currWithUnitsDsc = (struct descriptor_with_units *)malloc(
        sizeof(struct descriptor_with_units));
    memcpy(currWithUnitsDsc, &templateWithUnits,
           sizeof(struct descriptor_with_units));
    currWithUnitsDsc->units = unitsDscPtr;
    currWithUnitsDsc->data =
        completeDescr(dataDscPtr, helpDscPtr, 0, errorDscPtr, validationDscPtr);
    return (mdsdsc_t *)currWithUnitsDsc;
  }
  if (helpDscPtr || validationDscPtr)
  {
    currParamDsc =
        (struct descriptor_param *)malloc(sizeof(struct descriptor_param));
    memcpy(currParamDsc, &templateParam, sizeof(struct descriptor_param));
    currParamDsc->help = helpDscPtr;
    currParamDsc->validation = validationDscPtr;
    currParamDsc->value =
        completeDescr(dataDscPtr, 0, unitsDscPtr, errorDscPtr, 0);
    return (mdsdsc_t *)currParamDsc;
  }
  return dataDscPtr;
}

static mdsdsc_t *ObjectToDescrip(JNIEnv *env, jobject obj)
{
  jclass cls;
  jfieldID datum_fid, descs_fid, ndescs_fid, opcode_fid, dtype_fid, dclass_fid,
      dims_fid;

  static DESCRIPTOR_A_COEFF(template_array, 0, 0, 0, (unsigned char)255, 0);
  ARRAY_COEFF(char, 255) * array_d;
  static DESCRIPTOR_A(template_apd, 0, 0, 0, 0);
  mdsdsc_a_t *apd_d;
  // mdsdsc_a_t *array_d;
  static DESCRIPTOR_R(template_rec, 0, 1);
  struct descriptor_r *record_d;
  int i, ndescs, opcode, dtype, dclass, nDims;
  jobject jdescs;
  jsize length;
  jbyteArray jbytes;
  jshortArray jshorts;
  jintArray jints, jDims;
  jlongArray jlongs;
  jfloatArray jfloats;
  jdoubleArray jdoubles;
  jobjectArray jobjects;
  jbyte *bytes;
  jshort *shorts;
  jint *ints, *dims;
  jlong *longs;
  jfloat *floats;
  jdouble *doubles;
  jstring *jstrings;
  char **strings;
  int maxlen;
  jstring java_string;
  const char *string;
  mdsdsc_t *desc;
  jfieldID getUnitsFid, getHelpFid, getValidationFid, getErrorFid;
  mdsdsc_t *unitsDscPtr, *helpDscPtr, *validationDscPtr, *errorDscPtr;

  //      printf("ObjectTodescrip %x\n", obj);

  if (!obj)
  {
    return NULL;
  }
  cls = (*env)->GetObjectClass(env, obj);
  dtype_fid = (*env)->GetFieldID(env, cls, "dtype", "I"),
  dclass_fid = (*env)->GetFieldID(env, cls, "clazz", "I");

  dtype = (*env)->GetIntField(env, obj, dtype_fid),
  dclass = (*env)->GetIntField(env, obj, dclass_fid);

  //      printf("%d %d\n", dtype, dclass);

  getUnitsFid = (*env)->GetFieldID(env, cls, "units", "LMDSplus/Data;");
  getHelpFid = (*env)->GetFieldID(env, cls, "help", "LMDSplus/Data;");
  getValidationFid =
      (*env)->GetFieldID(env, cls, "validation", "LMDSplus/Data;");
  getErrorFid = (*env)->GetFieldID(env, cls, "error", "LMDSplus/Data;");

  //      printf("ObjToDescrip %x %x %x %x\n", getUnitsFid, getHelpFid,
  //      getValidationFid, getErrorFid);

  unitsDscPtr =
      ObjectToDescrip(env, (*env)->GetObjectField(env, obj, getUnitsFid));
  helpDscPtr =
      ObjectToDescrip(env, (*env)->GetObjectField(env, obj, getHelpFid));
  validationDscPtr =
      ObjectToDescrip(env, (*env)->GetObjectField(env, obj, getValidationFid));
  errorDscPtr =
      ObjectToDescrip(env, (*env)->GetObjectField(env, obj, getErrorFid));

  switch (dclass)
  {
  case CLASS_S:
    desc = (mdsdsc_t *)malloc(sizeof(mdsdsc_t));
    desc->class = dclass;
    desc->dtype = dtype;

    //              printf("DTYPE: %d\n", dtype);

    switch (dtype)
    {
    case DTYPE_B:
    case DTYPE_BU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "B");
      desc->length = sizeof(char);
      desc->pointer = (char *)malloc(desc->length);
      *desc->pointer = (*env)->GetByteField(env, obj, datum_fid);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_W:
    case DTYPE_WU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "S");
      desc->length = sizeof(short);
      desc->pointer = (char *)malloc(desc->length);
      *(short *)desc->pointer = (*env)->GetShortField(env, obj, datum_fid);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_L:
    case DTYPE_LU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "I");
      desc->length = sizeof(int);
      desc->pointer = (char *)malloc(desc->length);
      *(int *)desc->pointer = (*env)->GetIntField(env, obj, datum_fid);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_NID:
      datum_fid = (*env)->GetFieldID(env, cls, "nid", "I");
      desc->length = sizeof(int);
      desc->pointer = (char *)malloc(desc->length);
      *(int *)desc->pointer = (*env)->GetIntField(env, obj, datum_fid);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_Q:
    case DTYPE_QU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "J");
      desc->length = sizeof(int64_t);
      desc->pointer = (char *)malloc(desc->length);
      *(int64_t *)desc->pointer = (*env)->GetLongField(env, obj, datum_fid);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_O:
    case DTYPE_OU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      desc->length = 2 * sizeof(int64_t);
      desc->pointer = (char *)malloc(desc->length);
      *(int64_t *)desc->pointer = longs[0];
      *((int64_t *)desc->pointer + 1) = longs[1];
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_T:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "Ljava/lang/String;");
      java_string = (*env)->GetObjectField(env, obj, datum_fid);
      string = (*env)->GetStringUTFChars(env, java_string, 0);
      desc->length = strlen(string);
      if (desc->length > 0)
      {
        desc->pointer = (char *)malloc(desc->length);
        memcpy(desc->pointer, string, desc->length);
      }
      else
        desc->pointer = 0;
      (*env)->ReleaseStringUTFChars(env, java_string, string);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_PATH:
      datum_fid = (*env)->GetFieldID(env, cls, "path", "Ljava/lang/String;");
      java_string = (*env)->GetObjectField(env, obj, datum_fid);
      string = (*env)->GetStringUTFChars(env, java_string, 0);
      desc->length = strlen(string);
      if (desc->length > 0)
      {
        desc->pointer = (char *)malloc(desc->length);
        memcpy(desc->pointer, string, desc->length);
      }
      else
        desc->pointer = 0;
      (*env)->ReleaseStringUTFChars(env, java_string, string);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_IDENT:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "Ljava/lang/String;");
      java_string = (*env)->GetObjectField(env, obj, datum_fid);
      string = (*env)->GetStringUTFChars(env, java_string, 0);
      desc->length = strlen(string);
      if (desc->length > 0)
      {
        desc->pointer = (char *)malloc(desc->length);
        memcpy(desc->pointer, string, desc->length);
      }
      else
        desc->pointer = 0;
      (*env)->ReleaseStringUTFChars(env, java_string, string);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_FLOAT:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "F");
      desc->length = sizeof(float);
      desc->pointer = (char *)malloc(desc->length);
      *(float *)desc->pointer = (*env)->GetFloatField(env, obj, datum_fid);
      desc->dtype = DTYPE_FLOAT;
      CvtConvertFloat(desc->pointer, DTYPE_FS, desc->pointer, desc->dtype, 0);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    case DTYPE_DOUBLE:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "D");
      desc->length = sizeof(double);
      desc->pointer = (char *)malloc(desc->length);
      *(double *)desc->pointer = (*env)->GetDoubleField(env, obj, datum_fid);
      desc->dtype = DTYPE_DOUBLE;
      CvtConvertFloat(desc->pointer, DTYPE_DOUBLE, desc->pointer, desc->dtype,
                      0);
      return completeDescr(desc, helpDscPtr, unitsDscPtr, errorDscPtr,
                           validationDscPtr);
    default:
      printf("\nUnsupported type for CLASS_S: %d\n", dtype);
      return NULL;
    }
    break;
  case CLASS_A:
    array_d = malloc(sizeof(template_array));
    memcpy(array_d, &template_array, sizeof(template_array));
    array_d->dtype = dtype;
    dims_fid = (*env)->GetFieldID(env, cls, "dims", "[I");
    jDims = (*env)->GetObjectField(env, obj, dims_fid);
    nDims = (*env)->GetArrayLength(env, jDims);
    dims = (*env)->GetIntArrayElements(env, jDims, 0);
    array_d->dimct = nDims;
    for (i = 0; i < nDims && i < 255; i++)
      array_d->m[i] = dims[i];
    (*env)->ReleaseIntArrayElements(env, jDims, dims, 0);

    switch (dtype)
    {
    case DTYPE_B:
    case DTYPE_BU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[B");
      jbytes = (*env)->GetObjectField(env, obj, datum_fid);
      bytes = (*env)->GetByteArrayElements(env, jbytes, 0);
      length = (*env)->GetArrayLength(env, jbytes);
      array_d->length = sizeof(char);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, bytes, array_d->arsize);
      (*env)->ReleaseByteArrayElements(env, jbytes, bytes, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_W:
    case DTYPE_WU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[S");
      jshorts = (*env)->GetObjectField(env, obj, datum_fid);
      shorts = (*env)->GetShortArrayElements(env, jshorts, 0);
      length = (*env)->GetArrayLength(env, jshorts);
      array_d->length = sizeof(short);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, shorts, array_d->arsize);
      (*env)->ReleaseShortArrayElements(env, jshorts, shorts, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_L:
    case DTYPE_LU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[I");
      jints = (*env)->GetObjectField(env, obj, datum_fid);
      ints = (*env)->GetIntArrayElements(env, jints, 0);
      length = (*env)->GetArrayLength(env, jints);
      array_d->length = sizeof(int);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, ints, array_d->arsize);
      (*env)->ReleaseIntArrayElements(env, jints, ints, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_Q:
    case DTYPE_QU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      length = (*env)->GetArrayLength(env, jlongs);
      array_d->length = sizeof(int64_t);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, longs, array_d->arsize);
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_O:
    case DTYPE_OU:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      length = (*env)->GetArrayLength(env, jlongs);
      array_d->length = 2 * sizeof(int64_t);
      array_d->arsize = array_d->length * length / 2;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, longs, array_d->arsize);
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_FLOAT:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[F");
      jfloats = (*env)->GetObjectField(env, obj, datum_fid);
      floats = (*env)->GetFloatArrayElements(env, jfloats, 0);
      length = (*env)->GetArrayLength(env, jfloats);
      array_d->length = sizeof(float);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      array_d->dtype = DTYPE_FLOAT;
      for (i = 0; i < length; i++)
        CvtConvertFloat(&floats[i], DTYPE_FS, &((float *)array_d->pointer)[i],
                        array_d->dtype, 0);
      (*env)->ReleaseFloatArrayElements(env, jfloats, floats, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_DOUBLE:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[D");
      jdoubles = (*env)->GetObjectField(env, obj, datum_fid);
      doubles = (*env)->GetDoubleArrayElements(env, jdoubles, 0);
      length = (*env)->GetArrayLength(env, jdoubles);
      array_d->length = sizeof(double);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      array_d->dtype = DTYPE_DOUBLE;
      for (i = 0; i < length; i++)
        CvtConvertFloat(&doubles[i], DTYPE_DOUBLE,
                        &((double *)array_d->pointer)[i], array_d->dtype, 0);
      (*env)->ReleaseDoubleArrayElements(env, jdoubles, doubles, 0);
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    case DTYPE_T:
      datum_fid = (*env)->GetFieldID(env, cls, "datum", "[Ljava/lang/String;");
      jobjects = (*env)->GetObjectField(env, obj, datum_fid);
      length = (*env)->GetArrayLength(env, jobjects);
      strings = malloc(length * sizeof(char *));
      jstrings = malloc(length * sizeof(jstring));
      maxlen = 0;
      for (i = 0; i < length; i++)
      {
        jstrings[i] = (*env)->GetObjectArrayElement(env, jobjects, i);
        strings[i] = (char *)(*env)->GetStringUTFChars(env, jstrings[i], 0);
        if (maxlen < (int)strlen(strings[i]))
          maxlen = (int)strlen(strings[i]);
      }
      array_d->pointer = (char *)malloc(length * maxlen);
      memset(array_d->pointer, ' ', length * maxlen);
      for (i = 0; i < length; i++)
      {
        memcpy(&array_d->pointer[i * maxlen], strings[i], maxlen);
        (*env)->ReleaseStringUTFChars(env, jstrings[i], strings[i]);
      }
      array_d->length = maxlen;
      array_d->arsize = array_d->length * length;
      return completeDescr((mdsdsc_t *)array_d, helpDscPtr,
                           unitsDscPtr, errorDscPtr, validationDscPtr);
    default:
      printf("\nUnsupported type for CLASS_A: %d\n", dtype);
      return NULL;
    }
  case CLASS_R:

    // printf("CLASS_R\n");

    opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
    // printf("%x\n", opcode_fid);
    opcode = (*env)->GetIntField(env, obj, opcode_fid);
    // printf("%d\n", opcode);
    descs_fid = (*env)->GetFieldID(env, cls, "descs", "[LMDSplus/Data;");
    // printf("%x\n", descs_fid);

    jdescs = (*env)->GetObjectField(env, obj, descs_fid);
    ndescs = (*env)->GetArrayLength(env, jdescs);
    record_d = (struct descriptor_r *)malloc(sizeof(struct descriptor_r) +
                                             ndescs * sizeof(void *));
    memcpy(record_d, &template_rec, sizeof(struct descriptor_r));
    record_d->dtype = dtype;
    record_d->length = 0;
    record_d->pointer = 0;
    if (dtype == DTYPE_FUNCTION || dtype == DTYPE_DEPENDENCY ||
        dtype == DTYPE_CONDITION || dtype == DTYPE_CALL ||
        dtype == DTYPE_DISPATCH)
    {
      record_d->length = sizeof(short);
      record_d->pointer = (unsigned char *)malloc(sizeof(short));
      *(short *)record_d->pointer = opcode;
    }
    if (dtype == DTYPE_CALL)
    {
      record_d->length = sizeof(char);
      record_d->pointer = (unsigned char *)malloc(sizeof(char));
      *(char *)record_d->pointer = opcode;
    }
    record_d->ndesc = ndescs;
    for (i = 0; i < ndescs; i++)
    {
      record_d->dscptrs[i] =
          ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jdescs, i));
    }
    return completeDescr((mdsdsc_t *)record_d, helpDscPtr, unitsDscPtr,
                         errorDscPtr, validationDscPtr);
  case CLASS_APD:
    descs_fid = (*env)->GetFieldID(env, cls, "descs", "[LMDSplus/Data;");
    jdescs = (*env)->GetObjectField(env, obj, descs_fid);
    ndescs_fid = (*env)->GetFieldID(env, cls, "nDescs", "I");
    ndescs = (*env)->GetIntField(env, obj, ndescs_fid);
    // ndescs = (*env)->GetArrayLength(env, jdescs);
    apd_d = (mdsdsc_a_t *)malloc(sizeof(mdsdsc_a_t));
    memcpy(apd_d, &template_apd, sizeof(mdsdsc_a_t));
    apd_d->dtype = dtype;
    apd_d->class = CLASS_APD;
    apd_d->length = sizeof(mdsdsc_t *);
    apd_d->arsize = apd_d->length * ndescs;
    apd_d->pointer = (char *)malloc(sizeof(void *) * ndescs);
    for (i = 0; i < ndescs; i++)
      ((mdsdsc_t **)(apd_d->pointer))[i] =
          ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jdescs, i));
    return completeDescr((mdsdsc_t *)apd_d, helpDscPtr, unitsDscPtr,
                         errorDscPtr, validationDscPtr);
  default:
    printf("\nUnsupported class: %d\n", dclass);
  }
  return 0;
}

static void FreeDescrip(mdsdsc_t *desc)
{
  struct descriptor_r *record_d;
  mdsdsc_a_t *array_d;
  int i;

  if (!desc)
    return;

  /*printf("FreeDescrip class %d dtype %d\n", desc->class, desc->dtype);*/

  switch (desc->class)
  {
  default:
    break;
  case CLASS_S:
    free(desc->pointer);
    break;
  case CLASS_A:
    if (desc->pointer)
      free(((mdsdsc_a_t *)desc)->pointer);
    break;
  case CLASS_R:
    record_d = (struct descriptor_r *)desc;
    if (record_d->length)
      free(record_d->pointer);
    for (i = 0; i < record_d->ndesc; i++)
      FreeDescrip(record_d->dscptrs[i]);
    break;
  case CLASS_APD:
    array_d = (mdsdsc_a_t *)desc;
    for (i = 0; i < (int)array_d->arsize / array_d->length; i++)
      FreeDescrip(((mdsdsc_t **)array_d->pointer)[i]);
    break;
  }
  free(desc);
}

/*
 * Class:     MDSplus_Data
 * Method:    serialize
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_MDSplus_Data_serialize(JNIEnv *env,
                                                         jobject obj)
{

  EMPTYXD(xd);
  jclass exc;
  int status;
  char *errorMsg;
  mdsdsc_a_t *arrPtr;
  mdsdsc_t *dscPtr = ObjectToDescrip(env, obj);
  jbyteArray jserialized;
  status = MdsSerializeDscOut(dscPtr, &xd);
  if (STATUS_NOT_OK)
  {
    errorMsg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, errorMsg);
    return NULL;
  }
  arrPtr = (mdsdsc_a_t *)xd.pointer;
  if (arrPtr->dtype != DTYPE_B && arrPtr->dtype != DTYPE_BU)
  {
    printf("FATAL ERROR: MdsSerializeDscOut returned a wrong type");
    exit(0);
  }
  jserialized = (*env)->NewByteArray(env, arrPtr->arsize);
  if (jserialized)
    (*env)->SetByteArrayRegion(env, jserialized, 0, arrPtr->arsize,
                               (const jbyte *)arrPtr->pointer);
  MdsFree1Dx(&xd, 0);
  FreeDescrip(dscPtr);
  return jserialized;
}

/*
 * Class:     MDSplus_Data
 * Method:    deserialize
 * Signature: ([B)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Data_deserialize(
    JNIEnv *env, jclass cls __attribute__((unused)), jbyteArray jserialized)
{
  EMPTYXD(xd);
  jclass exc;
  char *errorMsg;
  jobject retObj;
  char *serialized =
      (char *)(*env)->GetByteArrayElements(env, jserialized, JNI_FALSE);
  int status = MdsSerializeDscIn(serialized, &xd);
  if (STATUS_NOT_OK)
  {
    errorMsg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, errorMsg);
    return NULL;
  }

  retObj = DescripToObject(env, NULL, xd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&xd, 0);
  return retObj;
}

#define MAX_ARGS 256
/*
 * Class:     MDSplus_Data
 * Method:    compile
 * Signature: (LMDSplus/String;[LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Data_compile(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jstring jexpr,
                                                    jobjectArray jargs)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status, i, varIdx;
  const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
  char *error_msg;
  jclass exc;
  mdsdsc_t exprD = {0, DTYPE_T, CLASS_S, 0};
  jobject ris;
  jint numArgs;

  numArgs = (*env)->GetArrayLength(env, jargs);

  if (numArgs > MAX_ARGS)
    numArgs = MAX_ARGS;

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;
  arglist[1] = &exprD;
  varIdx = 2;

  for (i = 0; i < numArgs; i++)
    arglist[varIdx++] =
        ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jargs, i));
  arglist[varIdx++] = &outXd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;
  status = (int)(intptr_t)LibCallg(arglist, TdiCompile);
  (*env)->ReleaseStringUTFChars(env, jexpr, expr);
  for (i = 0; i < numArgs; i++)
    FreeDescrip(arglist[2 + i]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, NULL, outXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

/*
 * Class:     MDSplus_Data
 * Method:    decompile
 * Signature: ()LMDSplus/String;
 */
JNIEXPORT jstring JNICALL Java_MDSplus_Data_decompile(JNIEnv *env, jobject jobj,
                                                      jlong jctx)
{
  EMPTYXD(outXd);
  jstring ris;
  jclass exc;
  mdsdsc_t *decD;
  char *buf;
  char *error_msg;
  int status;
  mdsdsc_t *dataD;
  void *ctx = JLONG2PTR(jctx);

  dataD = ObjectToDescrip(env, jobj);

  if (ctx)
    status = _TdiDecompile(&ctx, dataD, &outXd MDS_END_ARG);
  else
    status = TdiDecompile(dataD, &outXd MDS_END_ARG);

  // status = CTXCALLR(TdiDecompile, dataD, &outXd MDS_END_ARG);

  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  FreeDescrip(dataD);
  decD = outXd.pointer;
  buf = (char *)malloc(decD->length + 1);
  memcpy(buf, decD->pointer, decD->length);
  buf[decD->length] = 0;
  MdsFree1Dx(&outXd, NULL);
  ris = (*env)->NewStringUTF(env, buf);
  free(buf);
  return ris;
}

/*
 * Class:     MDSplus_Data
 * Method:    cloneData
 * Signature: ()LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Data_cloneData(JNIEnv *env,
                                                      jobject jobj)
{

  mdsdsc_t *descr;
  jobject retObj;
  descr = ObjectToDescrip(env, jobj);
  retObj = DescripToObject(env, NULL, descr, 0, 0, 0, 0);
  FreeDescrip(descr);
  return retObj;
}

/*
 * Class:     MDSplus_Data
 * Method:    execute
 * Signature: (Ljava/lang/String;[LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Data_execute(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jstring jexpr,
                                                    jobjectArray jargs)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status, i, varIdx;
  const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
  char *error_msg;
  jclass exc;
  mdsdsc_t exprD = {0, DTYPE_T, CLASS_S, 0};
  jobject ris;
  jint numArgs = (*env)->GetArrayLength(env, jargs);

  if (numArgs > MAX_ARGS)
    numArgs = MAX_ARGS;

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;
  arglist[1] = &exprD;
  varIdx = 2;
  for (i = 0; i < numArgs; i++)
    arglist[varIdx++] =
        ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jargs, i));
  arglist[varIdx++] = &outXd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;
  status = (int)(intptr_t)LibCallg(arglist, TdiCompile);
  (*env)->ReleaseStringUTFChars(env, jexpr, expr);
  for (i = 0; i < numArgs; i++)
    FreeDescrip(arglist[2 + i]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  status = TdiData((mdsdsc_t *)&outXd, &outXd MDS_END_ARG);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, NULL, outXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

/*
 * Class:     MDSplus_Data
 * Method:    dataData
 * Signature: ()LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Data_dataData(JNIEnv *env, jobject jobj,
                                                     jlong jctx)
{
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(xd);
  char *error_msg;
  jobject retObj;
  jclass exc;
  int status;

  mdsdsc_t *descr;
  descr = ObjectToDescrip(env, jobj);
  status = CTXCALLR(TdiData, descr, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  retObj = DescripToObject(env, ctx, xd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&xd, 0);
  FreeDescrip(descr);
  return retObj;
}

/*
 * Class:     MDSplus_Data
 * Method:    evaluateData
 * Signature: ()LMDSplus/String;
 */
JNIEXPORT jstring JNICALL Java_MDSplus_Data_evaluateData(JNIEnv *env,
                                                         jobject jobj,
                                                         jlong jctx)
{
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(xd);
  char *error_msg;
  jobject retObj;
  jclass exc;
  mdsdsc_t *descr = ObjectToDescrip(env, jobj);
  int status = CTXCALLR(TdiEvaluate, descr, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  FreeDescrip(descr);
  retObj = DescripToObject(env, ctx, descr, 0, 0, 0, 0);
  MdsFree1Dx(&xd, 0);
  return retObj;
}

//////////////////////////////Class Tree Stuff///////////////////////////

static void throwMdsExceptionStr(JNIEnv *env, char *errorMsg)
{
  jclass exc;

  exc = (*env)->FindClass(env, "MDSplus/MdsException");
  (*env)->ThrowNew(env, exc, errorMsg);
}

static void throwMdsException(JNIEnv *env, int status)
{
  jclass exc;

  exc = (*env)->FindClass(env, "MDSplus/MdsException");
  (*env)->ThrowNew(env, exc, (char *)MdsGetMsg(status));
}

/*
 * Class:     MDSplus_Tree
 * Method:    openTree
 * Signature: (JLjava/lang/String;IB)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_openTree(JNIEnv *env, jobject jobj,
                                                  jlong jctx, jstring jname,
                                                  jint shot,
                                                  jboolean readonly)
{
  int status;
  const char *name;
  void *ctx = JLONG2PTR(jctx);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  if (strlen(name) > 0)
  {
    status = _TreeOpen(&ctx, (char *)name, shot, readonly ? 1 : 0);
    set_ctx_field(env, jobj, ctx);
    (*env)->ReleaseStringUTFChars(env, jname, name);
    if (STATUS_NOT_OK)
    {
      throwMdsException(env, status);
      return;
    }
  }
}

/*
 * Class:     MDSplus_Tree
 * Method:    closeTree
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_closeTree(JNIEnv *env, jobject jobj,
                                                   jlong jctx, jstring jname,
                                                   jint shot)
{
  int status;
  const char *name;
  void *ctx = JLONG2PTR(jctx);

  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = CTXCALLR(TreeClose, name, shot);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
  {
    set_ctx_field(env, jobj, ctx);
    throwMdsException(env, status);
  }
  else
  {
    TreeFreeDbid(ctx);
    set_ctx_field(env, jobj, NULL);
  }
}

/*
 * Class:     MDSplus_Tree
 * Method:    editTree
 * Signature: (JLjava/lang/String;IB)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_editTree(JNIEnv *env, jobject jobj,
                                                  jlong jctx, jstring jname,
                                                  jint shot, jboolean isNew)
{
  int status;
  const char *name;
  void *ctx = JLONG2PTR(jctx);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  if (isNew)
    status = _TreeOpenNew(&ctx, name, shot);
  else
    status = _TreeOpenEdit(&ctx, name, shot);
  set_ctx_field(env, jobj, ctx);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
  {
    throwMdsException(env, status);
    return;
  }
}

/*
 * Class:     MDSplus_Tree
 * Method:    writeTree
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_writeTree(JNIEnv *env, jobject jobj,
                                                   jlong jctx, jstring jname,
                                                   jint shot)
{
  const char *name;

  void *ctx = JLONG2PTR(jctx);
  int status;
  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = CTXCALLR(TreeWriteTree, name, shot);
  set_ctx_field(env, jobj, ctx);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    quitTree
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_quitTree(JNIEnv *env, jobject jobj,
                                                  jlong jctx, jstring jname,
                                                  jint shot)
{
  const char *name;
  void *ctx = JLONG2PTR(jctx);
  int status;
  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = CTXCALLR(TreeQuitTree, name, shot);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
  {
    set_ctx_field(env, jobj, ctx);
    throwMdsException(env, status);
  }
  else
  {
    TreeFreeDbid(ctx);
    set_ctx_field(env, jobj, NULL);
  }
}

/*
 * Class:     MDSplus_Tree
 * Method:    findNode
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_Tree_findNode(JNIEnv *env,
                                                  jclass cls
                                                  __attribute__((unused)),
                                                  jlong jctx, jstring jpath)
{
  int status, nid;
  const char *path;
  void *ctx = JLONG2PTR(jctx);

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = CTXCALLN(TreeFindNode, path, &nid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return nid;
}

/*
 * Class:     MDSplus_Tree
 * Method:    getWild
 * Signature: (JLjava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL Java_MDSplus_Tree_getWild(JNIEnv *env,
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jlong jctx, jstring jpath,
                                                      jint usage)
{
  int currNid, status, i;
  int numNids = 0;
  const char *path;
  int *nids;
  void *wildCtx = 0;
  void *ctx = JLONG2PTR(jctx);
  jintArray jnids;

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  while (
      (status = CTXCALLN(TreeFindNodeWild, path, &currNid, &wildCtx, usage)) &
      1)
    numNids++;
  CTXCALLN(TreeFindNodeEnd, &wildCtx);
  nids = malloc(numNids * sizeof(int));
  wildCtx = 0;
  for (i = 0; i < numNids; i++)
  {
    CTXCALLN(TreeFindNodeWild, (char *)path, &nids[i], &wildCtx, usage);
    //_TreeFindNodeWild, (char *)path, &nids[i], &wildCtx, (1 << usage));
  }
  CTXCALLN(TreeFindNodeEnd, &wildCtx);

  (*env)->ReleaseStringUTFChars(env, jpath, path);
  jnids = (*env)->NewIntArray(env, numNids);
  if (jnids)
    (*env)->SetIntArrayRegion(env, jnids, 0, numNids, (const jint *)nids);
  free(nids);
  return jnids;
}

/*
 * Class:     MDSplus_Tree
 * Method:    getDefaultNid
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_Tree_getDefaultNid(JNIEnv *env,
                                                       jclass cls
                                                       __attribute__((unused)),
                                                       jlong jctx)
{
  void *ctx = JLONG2PTR(jctx);
  int nid;
  int status = CTXCALLN(TreeGetDefaultNid, &nid);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return nid;
}

/*
 * Class:     MDSplus_Tree
 * Method:    setDefaultNid
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_setDefaultNid(JNIEnv *env,
                                                       jclass cls
                                                       __attribute__((unused)),
                                                       jlong jctx, jint nid)
{
  void *ctx = JLONG2PTR(jctx);
  int status = CTXCALLN(TreeSetDefaultNid, nid);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    getDbiFlag
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_MDSplus_Tree_getDbiFlag(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jlong jctx, jint code)
{
  int flag = 0, len = sizeof(jboolean), status;
  void *ctx = JLONG2PTR(jctx);
  struct dbi_itm dbiList[] = {{sizeof(int), 0, &flag, &len},
                              {0, DbiEND_OF_LIST, 0, 0}};

  dbiList[0].code = (short)code;
  status = CTXCALLN(TreeGetDbi, dbiList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return flag;
}

/*
 * Class:     MDSplus_Tree
 * Method:    setDbiFlag
 * Signature: (JZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_setDbiFlag(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jlong jctx, jboolean jflag,
                                                    jint code)
{
  int len = sizeof(jboolean), status, flag;
  void *ctx = JLONG2PTR(jctx);
  struct dbi_itm dbiList[] = {{sizeof(int), 0, &flag, &len},
                              {0, DbiEND_OF_LIST, 0, 0}};

  flag = jflag;
  dbiList[0].code = (short)code;
  dbiList[0].pointer = &flag;
  status = CTXCALLN(TreeSetDbi, dbiList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    setTreeViewDate
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_setTreeViewDate(
    JNIEnv *env, jclass cls __attribute__((unused)), jstring jdate)
{
  int64_t qtime;
  const char *date;
  int status;
  date = (*env)->GetStringUTFChars(env, jdate, 0);
  qtime = 0;
  status = LibConvertDateString(date, &qtime);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  status = TreeSetViewDate(&qtime);
  (*env)->ReleaseStringUTFChars(env, jdate, date);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    setTreeTimeContext
 * Signature: (JLMDSplus/Data;LMDSplus/Data;LMDSplus/Data;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_setTreeTimeContext(
    JNIEnv *env, jclass cls __attribute__((unused)), jlong jctx, jobject jstart,
    jobject jend, jobject jdelta)
{
  mdsdsc_t *start, *end, *delta;
  int status;
  void *ctx = JLONG2PTR(jctx);
  start = ObjectToDescrip(env, jstart);
  end = ObjectToDescrip(env, jend);
  delta = ObjectToDescrip(env, jdelta);
  status = CTXCALLN(TreeSetTimeContext, start, end, delta);
  FreeDescrip(start);
  FreeDescrip(end);
  FreeDescrip(delta);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    setCurrent
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_setCurrent(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jstring jname, jint shot)
{
  int status;
  const char *name;

  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = TreeSetCurrentShotId((char *)name, shot);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    getCurrent
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_Tree_getCurrent(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jstring jname)
{
  int current = 0;
  const char *name;

  name = (*env)->GetStringUTFChars(env, jname, 0);
  current = TreeGetCurrentShotId((char *)name);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  return current;
}

/*
 * Class:     MDSplus_Tree
 * Method:    createPulseFile
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_createPulseFile(
    JNIEnv *env, jclass cls __attribute__((unused)), jlong jctx, jint shot)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  status = CTXCALLN(TreeCreatePulseFile, shot, 0, NULL);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    deletePulseFile
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_deletePulseFile(
    JNIEnv *env, jclass cls __attribute__((unused)), jlong jctx, jint shot)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  status = CTXCALLN(TreeDeletePulseFile, shot, 1);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    findTreeTags
 * Signature: (JLjava/lang/String;)[Ljava/lang/String;
 */
#define MAX_TAGS 1024

JNIEXPORT jobjectArray JNICALL
Java_MDSplus_Tree_findTreeTags(JNIEnv *env, jclass cls __attribute__((unused)),
                               jlong jctx, jstring jwild)
{
  const char *wild;
  char *tagNames[MAX_TAGS];
  void *wildCtx = 0;
  int nTags = 0;
  int nidOut;
  int i;
  jobjectArray jtags;
  jclass stringCls;
  void *ctx = JLONG2PTR(jctx);

  wild = (*env)->GetStringUTFChars(env, jwild, 0);
  while (nTags < MAX_TAGS &&
         (tagNames[nTags] =
              CTXCALLN(TreeFindTagWild, (char *)wild, &nidOut, &wildCtx)))
    nTags++;
  TreeFindTagEnd(&wildCtx);

  (*env)->ReleaseStringUTFChars(env, jwild, wild);

  stringCls = (*env)->FindClass(env, "java/lang/String");

  jtags = (*env)->NewObjectArray(env, nTags, stringCls, 0);
  if (jtags)
  {
    for (i = 0; i < nTags; i++)
    {
      (*env)->SetObjectArrayElement(
          env, jtags, i, (jobject)(*env)->NewStringUTF(env, tagNames[i]));
    }
  }

  return jtags;
}
/*
 * Class:     MDSplus_Tree
 * Method:    addTreeNode
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT jint JNICALL Java_MDSplus_Tree_addTreeNode(JNIEnv *env,
                                                     jclass cls
                                                     __attribute__((unused)),
                                                     jlong jctx, jstring jpath,
                                                     jint usage)
{
  const char *path;
  void *ctx = JLONG2PTR(jctx);
  int nidOut;
  int status;

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = CTXCALLN(TreeAddNode, path, &nidOut, (char)usage);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return nidOut;
}

/*
 * Class:     MDSplus_Tree
 * Method:    addTreeDevice
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_MDSplus_Tree_addTreeDevice(JNIEnv *env, jclass cls __attribute__((unused)),
                                jlong jctx, jstring jname, jstring jtype)
{
  const char *name, *type;
  void *ctx = JLONG2PTR(jctx);
  int nidOut;
  int status;

  name = (*env)->GetStringUTFChars(env, jname, 0);
  type = (*env)->GetStringUTFChars(env, jtype, 0);
  status = CTXCALLN(TreeAddConglom, (char *)name, (char *)type, &nidOut);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  (*env)->ReleaseStringUTFChars(env, jtype, type);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    deleteTreeNode
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_deleteTreeNode(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jlong jctx,
                                                        jstring jpath)
{
  const char *path;
  int status, nid, count;
  void *ctx = JLONG2PTR(jctx);

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = CTXCALLN(TreeFindNode, (char *)path, &nid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  status = CTXCALLN(TreeDeleteNodeInitialize, nid, &count, 1);
  if (STATUS_OK)
    CTXCALL0(TreeDeleteNodeExecute);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    removeTreeTag
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Tree_removeTreeTag(
    JNIEnv *env, jclass cls __attribute__((unused)), jlong jctx, jstring jtag)
{
  const char *tag;
  int status;
  void *ctx = JLONG2PTR(jctx);

  tag = (*env)->GetStringUTFChars(env, jtag, 0);
  status = CTXCALLN(TreeRemoveTag, (char *)tag);
  (*env)->ReleaseStringUTFChars(env, jtag, tag);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_Tree
 * Method:    getDatafileSize
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_MDSplus_Tree_getDatafileSize(
    JNIEnv *env __attribute__((unused)), jclass cls __attribute__((unused)),
    jlong jctx)
{
  // int status;
  int64_t size;
  void *ctx = JLONG2PTR(jctx);

  size = CTXCALL0(TreeGetDatafileSize);
  return size;
}

/*
 * Class:     MDSplus_Tree
 * Method:    compile
 * Signature: (JLjava/lang/String;[LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Tree_compile(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jlong jctx, jstring jexpr,
                                                    jobjectArray jargs)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status, i, varIdx;
  const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
  char *error_msg;
  jclass exc;
  mdsdsc_t exprD = {0, DTYPE_T, CLASS_S, 0};
  jobject ris;
  jint numArgs;

  void *ctx = JLONG2PTR(jctx);
  numArgs = (*env)->GetArrayLength(env, jargs);
  if (numArgs > MAX_ARGS)
    numArgs = MAX_ARGS;

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;
  arglist[1] = &ctx;
  arglist[2] = &exprD;
  varIdx = 3;

  for (i = 0; i < numArgs; i++)
    arglist[varIdx++] =
        ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jargs, i));
  arglist[varIdx++] = &outXd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;
  status = (int)(intptr_t)LibCallg(arglist, _TdiCompile);
  (*env)->ReleaseStringUTFChars(env, jexpr, expr);
  for (i = 0; i < numArgs; i++)
    FreeDescrip(arglist[3 + i]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, ctx, outXd.pointer, 0, 0, 0, 0);
  // printDecompiled1,ctx, outXd.pointer);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

/*
 * Class:     MDSplus_Tree
 * Method:    execute
 * Signature: (JLjava/lang/String;[LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Tree_execute(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jlong jctx, jstring jexpr,
                                                    jobjectArray jargs)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status, i, varIdx;
  const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
  char *error_msg;
  jclass exc;
  mdsdsc_t exprD = {0, DTYPE_T, CLASS_S, 0};
  jobject ris;
  jint numArgs;

  void *ctx = JLONG2PTR(jctx);
  numArgs = (*env)->GetArrayLength(env, jargs);

  if (numArgs > MAX_ARGS)
    numArgs = MAX_ARGS;

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;
  arglist[1] = &ctx;
  arglist[2] = &exprD;
  varIdx = 3;

  for (i = 0; i < numArgs; i++)
    arglist[varIdx++] =
        ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jargs, i));
  arglist[varIdx++] = &outXd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;
  status = (int)(intptr_t)LibCallg(arglist, _TdiExecute);
  (*env)->ReleaseStringUTFChars(env, jexpr, expr);
  for (i = 0; i < numArgs; i++)
    FreeDescrip(arglist[3 + i]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, ctx, outXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

/*
 * Class:     MDSplus_Tree
 * Method:    data
 * Signature: (LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Tree_data(JNIEnv *env,
                                                 jclass cls
                                                 __attribute__((unused)),
                                                 jlong jctx, jobject jdata)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status;
  char *error_msg;
  jclass exc;
  jobject ris;

  void *ctx = JLONG2PTR(jctx);

  arglist[1] = &ctx;
  arglist[2] = ObjectToDescrip(env, jdata);
  arglist[3] = &outXd;
  arglist[4] = MdsEND_ARG;
  *(int *)&arglist[0] = 4;
  status = (int)(intptr_t)LibCallg(arglist, _TdiData);
  FreeDescrip(arglist[2]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, ctx, outXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

/*
 * Class:     MDSplus_Tree
 * Method:    evaluate
 * Signature: (LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_Tree_evaluate(JNIEnv *env,
                                                     jclass cls
                                                     __attribute__((unused)),
                                                     jlong jctx,
                                                     jobject jdata)
{
  EMPTYXD(outXd);
  void *arglist[MAX_ARGS];
  int status;
  char *error_msg;
  jclass exc;
  jobject ris;

  void *ctx = JLONG2PTR(jctx);

  arglist[1] = &ctx;
  arglist[2] = ObjectToDescrip(env, jdata);
  arglist[3] = &outXd;
  arglist[4] = MdsEND_ARG;
  *(int *)&arglist[0] = 4;
  status = (int)(intptr_t)LibCallg(arglist, _TdiEvaluate);
  FreeDescrip(arglist[2]);
  if (STATUS_NOT_OK)
  {
    error_msg = (char *)MdsGetMsg(status);
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, error_msg);
    return NULL;
  }
  ris = DescripToObject(env, ctx, outXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&outXd, NULL);
  return ris;
}

////////////////////////////////TreeNode Stuff////////////////////////

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNci
 * Signature: (IJI)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_TreeNode_getNci(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jint nid, jlong jctx,
                                                    jint nciType)
{
  int status;
  int retNci = 0, retNciLen = sizeof(int);

  struct nci_itm nciList[] = {{sizeof(int), 0, &retNci, &retNciLen},
                              {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);
  nciList[0].code = (short)nciType;
  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return retNci;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNciLong
 * Signature: (IJI)J
 */
JNIEXPORT jlong JNICALL Java_MDSplus_TreeNode_getNciLong(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint nciType)
{
  int status;
  int64_t retNci = 0;
  int retNciLen = sizeof(int);

  struct nci_itm nciList[] = {{sizeof(int64_t), 0, &retNci, &retNciLen},
                              {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);

  nciList[0].code = (short)nciType;
  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return retNci;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNciString
 * Signature: (IJI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_MDSplus_TreeNode_getNciString(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint nciType)
{
  int status;
  char path[1024];
  int pathLen = 0;
  struct nci_itm nciList[] = {{sizeof(path) - 1, 0, path, &pathLen},
                              {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);

  nciList[0].code = (short)nciType;
  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  path[pathLen] = 0;
  return (*env)->NewStringUTF(env, path);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    setNciFlag
 * Signature: (IJIZ)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_setNciFlag(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint flagOfs, jboolean flag)
{
  int status;
  int nciFlags;
  int nciFlagsLen = sizeof(int);
  struct nci_itm nciList[] = {
      {4, flag ? NciSET_FLAGS : NciCLEAR_FLAGS, &nciFlags, &nciFlagsLen},
      {0, NciEND_OF_LIST, 0, 0}};
  void *ctx = JLONG2PTR(jctx);
  nciFlags = flagOfs;
  status = CTXCALLN(TreeSetNci, nid, nciList);

  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    setNciFlags
 * Signature: (IJI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_setNciFlags(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint flags)
{
  int status;
  int nciFlags;
  int nciFlagsLen = sizeof(int);
  struct nci_itm nciList[] = {{4, NciSET_FLAGS, &nciFlags, &nciFlagsLen},
                              {0, NciEND_OF_LIST, 0, 0}};
  void *ctx = JLONG2PTR(jctx);
  nciFlags = flags;
  status = CTXCALLN(TreeSetNci, nid, nciList);

  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNciFlag
 * Signature: (IJI)Z
 */
JNIEXPORT jboolean JNICALL Java_MDSplus_TreeNode_getNciFlag(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint flagOfs)
{
  int status;
  int nciFlags;
  int nciFlagsLen = sizeof(int);
  struct nci_itm nciList[] = {
      {sizeof(int), NciGET_FLAGS, &nciFlags, &nciFlagsLen},
      {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);

  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  return (nciFlags & flagOfs) ? 1 : 0;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNciFlag
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_TreeNode_getNciFlags(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx)
{
  int status;
  int nciFlags;
  int nciFlagsLen = sizeof(int);
  struct nci_itm nciList[] = {
      {sizeof(int), NciGET_FLAGS, &nciFlags, &nciFlagsLen},
      {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);
  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  return nciFlags;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNciNids
 * Signature: (IJII)[I
 */
JNIEXPORT jintArray JNICALL Java_MDSplus_TreeNode_getNciNids(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint nciNumCode, jint nciCode)
{
  int status;
  int nNids, nNidsLen = sizeof(int);
  int retLen = 0;
  int *nids;
  jintArray jnids;

  struct nci_itm nciList[] = {{sizeof(int), 0, (char *)&nNids, &nNidsLen},
                              {NciEND_OF_LIST, 0, 0, 0}};
  struct nci_itm nciList1[] = {{0, 0, 0, &retLen}, {NciEND_OF_LIST, 0, 0, 0}};
  void *ctx = JLONG2PTR(jctx);
  nciList[0].code = (short)nciNumCode;
  status = CTXCALLN(TreeGetNci, nid, nciList);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  nids = malloc(nNids * sizeof(int));
  nciList1[0].code = (short)nciCode;
  nciList1[0].buffer_length = sizeof(int) * nNids;
  nciList1[0].pointer = nids;
  status = CTXCALLN(TreeGetNci, nid, nciList1);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  jnids = (*env)->NewIntArray(env, nNids);
  if (jnids)
    (*env)->SetIntArrayRegion(env, jnids, 0, nNids, (const jint *)nids);
  free(nids);
  return jnids;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    turnOn
 * Signature: (IJZ)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_turnOn(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jint nid, jlong jctx,
                                                    jboolean on)
{
  int status;
  void *ctx = JLONG2PTR(jctx);

  if (on)
    status = CTXCALLN(TreeTurnOn, nid);
  else
    status = CTXCALLN(TreeTurnOff, nid);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    isOn
 * Signature: (IJ)Z
 */
JNIEXPORT jboolean JNICALL Java_MDSplus_TreeNode_isOn(JNIEnv *env
                                                      __attribute__((unused)),
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jint nid, jlong jctx)
{
  void *ctx = JLONG2PTR(jctx);
  return CTXCALLN(TreeIsOn, nid) == TreeON;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    isParentOff
 * Signature: (IJ)Z
 */
JNIEXPORT jboolean JNICALL Java_MDSplus_TreeNode_isParentOff(
    JNIEnv *env __attribute__((unused)), jclass cls __attribute__((unused)),
    jint nid, jlong jctx)
{
  void *ctx = JLONG2PTR(jctx);
  return CTXCALLN(TreeIsOn, nid) == TreePARENT_OFF;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getData
 * Signature: (IJZI)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getData(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jint nid, jlong jctx)
{
  int status;
  EMPTYXD(xd);
  jobject retObj;

  void *ctx = JLONG2PTR(jctx);

  status = CTXCALLN(TreeGetRecord, nid, &xd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, xd.pointer, NULL, NULL, NULL, NULL);
  MdsFree1Dx(&xd, 0);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    putData
 * Signature: (IJLMDSplus/Data;ZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_putData(JNIEnv *env,
                                                     jclass cls
                                                     __attribute__((unused)),
                                                     jint nid, jlong jctx,
                                                     jobject jdata)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  dataD = ObjectToDescrip(env, jdata);
  status = CTXCALLN(TreePutRecord, nid, dataD, 0);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  FreeDescrip(dataD);
}
/*
 * Class:     MDSplus_TreeNode
 * Method:    getExtendedAttribute
 * Signature: (IJLjava/lang/String;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getExtendedAttribute(
    JNIEnv *env, jclass class __attribute__((unused)), jint nid, jlong jctx,
    jstring jname)
{
  int status;
  EMPTYXD(xd);
  jobject retObj;
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);
  void *ctx = JLONG2PTR(jctx);

  status = CTXCALLN(TreeGetXNci, nid, name, &xd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, xd.pointer, NULL, NULL, NULL, NULL);
  MdsFree1Dx(&xd, 0);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    setExtendedAttribute
 * Signature: (IJLjava/lang/String;LMDSplus/Data;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_setExtendedAttribute(
    JNIEnv *env, jclass class __attribute__((unused)), jint nid, jlong jctx,
    jstring jname, jobject jdata)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);

  dataD = ObjectToDescrip(env, jdata);
  status = CTXCALLN(TreeSetXNci, nid, name, dataD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  FreeDescrip(dataD);
  (*env)->ReleaseStringUTFChars(env, jname, name);
}
/*
 * Class:     MDSplus_TreeNode
 * Method:    deleteData
 * Signature: (IJZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_deleteData(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jint nid, jlong jctx)
{
  EMPTYXD(emptyXd);
  int status;
  void *ctx = JLONG2PTR(jctx);
  status = CTXCALLN(TreePutRecord, nid, (mdsdsc_t *)&emptyXd, 0);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    doMethod
 * Signature: (IJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_doMethod(JNIEnv *env,
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jint nid, jlong jctx,
                                                      jstring jmethod)
{
  const char *method;
  int status;
  void *ctx = JLONG2PTR(jctx);
  mdsdsc_t nidD = {sizeof(int), DTYPE_NID, CLASS_S, 0};
  mdsdsc_t methodD = {0, DTYPE_T, CLASS_S, 0};
  EMPTYXD(xd);

  method = (*env)->GetStringUTFChars(env, jmethod, 0);
  methodD.length = strlen(method);
  methodD.pointer = (char *)method;
  nidD.pointer = (char *)&nid;

  status = CTXCALLN(TreeDoMethod, &nidD, &methodD, &xd MDS_END_ARG);
  MdsFree1Dx(&xd, 0);
  (*env)->ReleaseStringUTFChars(env, jmethod, method);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getTags
 * Signature: (IJ)[Ljava/lang/String;
 */
#define MAX_TAGS 1024
JNIEXPORT jobjectArray JNICALL Java_MDSplus_TreeNode_getTags(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx)
{
  void *wildCtx = 0;
  int nTags = 0;
  int i;
  jobjectArray jtags;
  jclass stringCls;
  void *ctx = JLONG2PTR(jctx);
  char *tagNames[MAX_TAGS];

  while (nTags < MAX_TAGS &&
         (tagNames[nTags] = CTXCALLN(TreeFindNodeTags, nid, &wildCtx)))
    nTags++;

  stringCls = (*env)->FindClass(env, "java/lang/String");
  jtags = (*env)->NewObjectArray(env, nTags, stringCls, 0);
  if (jtags)
  {
    for (i = 0; i < nTags; i++)
    {
      (*env)->SetObjectArrayElement(
          env, jtags, i, (jobject)(*env)->NewStringUTF(env, tagNames[i]));
      TreeFree(tagNames[i]);
    }
  }
  return jtags;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    makeSegment
 * Signature: (IJLMDSplus/Data;LMDSplus/Data;LMDSplus/Data;LMDSplus/Data;IZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_makeSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jstart, jobject jend, jobject jdim, jobject jdata,
    jint filledRows __attribute__((unused)))
{
  mdsdsc_t *startD, *endD, *dimD, *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  startD = ObjectToDescrip(env, jstart);
  endD = ObjectToDescrip(env, jend);
  dimD = ObjectToDescrip(env, jdim);
  dataD = ObjectToDescrip(env, jdata);

  status = CTXCALLN(TreeMakeSegment, nid, startD, endD, dimD,
                    (mdsdsc_a_t *)dataD, -1, filledRows);

  FreeDescrip(startD);
  FreeDescrip(endD);
  FreeDescrip(dimD);
  FreeDescrip(dataD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    beginSegment
 * Signature: (IJLMDSplus/Data;LMDSplus/Data;LMDSplus/Data;LMDSplus/Data;ZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_beginSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jstart, jobject jend, jobject jdim, jobject jdata)
{
  mdsdsc_t *startD, *endD, *dimD, *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  startD = ObjectToDescrip(env, jstart);
  endD = ObjectToDescrip(env, jend);
  dimD = ObjectToDescrip(env, jdim);
  dataD = ObjectToDescrip(env, jdata);

  status = CTXCALLN(TreeBeginSegment, nid, startD, endD, dimD,
                    (mdsdsc_a_t *)dataD, -1);

  FreeDescrip(startD);
  FreeDescrip(endD);
  FreeDescrip(dimD);
  FreeDescrip(dataD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    putSegment
 * Signature: (IJLMDSplus/Data;IZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_putSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jdata, jint offset)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  dataD = ObjectToDescrip(env, jdata);
  status = CTXCALLN(TreePutSegment, nid, offset, (mdsdsc_a_t *)dataD);
  FreeDescrip(dataD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    updateSegment
 * Signature: (IJLMDSplus/Data;LMDSplus/Data;LMDSplus/Data;ZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_updateSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint segmentOffset, jobject jstart, jobject jend, jobject jdim)
{
  mdsdsc_t *startD, *endD, *dimD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  startD = ObjectToDescrip(env, jstart);
  endD = ObjectToDescrip(env, jend);
  dimD = ObjectToDescrip(env, jdim);
  status = CTXCALLN(TreeUpdateSegment, nid, startD, endD, dimD, segmentOffset);

  FreeDescrip(startD);
  FreeDescrip(endD);
  FreeDescrip(dimD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    beginTimestampedSegment
 * Signature: (IJLMDSplus/Data;ZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_beginTimestampedSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jdata)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  dataD = ObjectToDescrip(env, jdata);

  //  printDecompiled(dataD);
  status = CTXCALLN(TreeBeginTimestampedSegment, nid,
                    (mdsdsc_a_t *)dataD, -1);
  FreeDescrip(dataD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    makeTimestampedSegment
 * Signature: (IJLMDSplus/Data;[JZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_makeTimestampedSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jdata, jlongArray jtimesArray)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);
  int numTimes;
  jlong *jtimes;

  numTimes = (*env)->GetArrayLength(env, jtimesArray);
  jtimes = (*env)->GetLongArrayElements(env, jtimesArray, NULL);
  dataD = ObjectToDescrip(env, jdata);

  // printDecompiled(dataD);

  status = CTXCALLN(TreeMakeTimestampedSegment, nid, (int64_t *)jtimes,
                    (mdsdsc_a_t *)dataD, -1, numTimes);
  FreeDescrip(dataD);
  (*env)->ReleaseLongArrayElements(env, jtimesArray, jtimes, JNI_ABORT);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    putTimestampedSegment
 * Signature: (IJLMDSplus/Data;[JZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_putTimestampedSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jdata, jlongArray jtimesArray)
{
  mdsdsc_t *dataD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  jlong *jtimes;
  if ((*env)->GetArrayLength(env, jtimesArray) < 1)
    throwMdsException(env, TreeBUFFEROVF);
  jtimes = (*env)->GetLongArrayElements(env, jtimesArray, NULL);
  dataD = ObjectToDescrip(env, jdata);

  status = CTXCALLN(TreePutTimestampedSegment, nid, (int64_t *)jtimes,
                    (mdsdsc_a_t *)dataD);

  FreeDescrip(dataD);
  (*env)->ReleaseLongArrayElements(env, jtimesArray, jtimes, JNI_ABORT);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

#define PUTROW_BUFSIZE 1024

/*
 * Class:     MDSplus_TreeNode
 * Method:    putRow
 * Signature: (IJLMDSplus/Data;JZI)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_putRow(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jobject jrow, jlong jtime, jint size)
{
  mdsdsc_t *rowD;
  int status;
  void *ctx = JLONG2PTR(jctx);

  rowD = ObjectToDescrip(env, jrow);
  status = CTXCALLN(TreePutRow, nid, size, (int64_t *)&jtime,
                    (mdsdsc_a_t *)rowD);

  FreeDescrip(rowD);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getNumSegments
 * Signature: (IJZI)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_TreeNode_getNumSegments(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx)
{
  int status, numSegments;
  void *ctx = JLONG2PTR(jctx);

  status = CTXCALLN(TreeGetNumSegments, nid, &numSegments);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return numSegments;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getSegmentStart
 * Signature: (IJIZI)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getSegmentStart(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint idx)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  jobject retObj;

  status = CTXCALLN(TreeGetSegmentLimits, nid, idx, &startXd, &endXd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, startXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&startXd, 0);
  MdsFree1Dx(&endXd, 0);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getSegmentEnd
 * Signature: (IJIZI)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getSegmentEnd(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint idx)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  jobject retObj;

  status = CTXCALLN(TreeGetSegmentLimits, nid, idx, &startXd, &endXd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, endXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&startXd, 0);
  MdsFree1Dx(&endXd, 0);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getSegmentDim
 * Signature: (IJIZI)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getSegmentDim(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint idx)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(dataXd);
  EMPTYXD(timeXd);
  jobject retObj;

  status = CTXCALLN(TreeGetSegment, nid, idx, &dataXd, &timeXd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, timeXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&dataXd, 0);
  MdsFree1Dx(&timeXd, 0);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    getSegment
 * Signature: (IJIZI)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL Java_MDSplus_TreeNode_getSegment(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint idx)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  EMPTYXD(dataXd);
  EMPTYXD(timeXd);
  jobject retObj;

  status = CTXCALLN(TreeGetSegment, nid, idx, &dataXd, &timeXd);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);

  retObj = DescripToObject(env, ctx, dataXd.pointer, 0, 0, 0, 0);
  MdsFree1Dx(&dataXd, 0);
  MdsFree1Dx(&timeXd, 0);
  return retObj;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    addNode
 * Signature: (IJI)I
 */
JNIEXPORT jint JNICALL
Java_MDSplus_TreeNode_addNode(JNIEnv *env, jclass cls __attribute__((unused)),
                              jint nid, jlong jctx, jstring jname, jint usage)
{
  int status, defNid, newNid = -1;
  void *ctx = JLONG2PTR(jctx);
  const char *name;

  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = CTXCALLN(TreeGetDefaultNid, &defNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeSetDefaultNid, nid);
  if (STATUS_OK)
    status = CTXCALLN(TreeAddNode, (char *)name, &newNid, (char)usage);
  if (STATUS_OK)
    status = CTXCALLN(TreeSetDefaultNid, defNid);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return newNid;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    deleteNode
 * Signature: (IJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_deleteNode(
    JNIEnv *env, jclass cls __attribute__((unused)),
    jint nid __attribute__((unused)), jlong jctx, jstring jpath)
{
  int status, defNid, delNid, count;
  const char *path;
  void *ctx = JLONG2PTR(jctx);

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = CTXCALLN(TreeGetDefaultNid, &defNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeFindNode, (char *)path, &delNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeDeleteNodeInitialize, delNid, &count, 1);
  if (STATUS_OK)
    CTXCALL0(TreeDeleteNodeExecute);
  if (STATUS_OK)
    status = CTXCALLN(TreeSetDefaultNid, defNid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    renameNode
 * Signature: (IJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_renameNode(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jint nid, jlong jctx,
                                                        jstring jname)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  const char *name;

  name = (*env)->GetStringUTFChars(env, jname, 0);
  status = CTXCALLN(TreeRenameNode, nid, (char *)name);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    addTag
 * Signature: (IJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_addTag(JNIEnv *env,
                                                    jclass cls
                                                    __attribute__((unused)),
                                                    jint nid, jlong jctx,
                                                    jstring jtag)
{
  int status;
  void *ctx = JLONG2PTR(jctx);
  const char *tag;

  tag = (*env)->GetStringUTFChars(env, jtag, 0);
  status = CTXCALLN(TreeAddTag, nid, (char *)tag);
  (*env)->ReleaseStringUTFChars(env, jtag, tag);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    removeTag
 * Signature: (IJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_removeTag(JNIEnv *env,
                                                       jclass cls
                                                       __attribute__((unused)),
                                                       jint nid, jlong jctx,
                                                       jstring jtag)
{
  int status, currNid;
  void *ctx = JLONG2PTR(jctx);
  const char *tag;
  char *bTag;

  tag = (*env)->GetStringUTFChars(env, jtag, 0);
  bTag = malloc(strlen(tag) + 2);
  sprintf(bTag, "\\%s", tag);

  status = CTXCALLN(TreeFindNode, bTag, &currNid);
  free(bTag);
  if (STATUS_NOT_OK)
  {
    (*env)->ReleaseStringUTFChars(env, jtag, tag);
    throwMdsException(env, status);
  }
  if (currNid != nid)
  {
    (*env)->ReleaseStringUTFChars(env, jtag, tag);
    throwMdsExceptionStr(env, "No such tag for this tree node");
  }
  status = CTXCALLN(TreeRemoveTag, (char *)tag);
  (*env)->ReleaseStringUTFChars(env, jtag, tag);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    addDevice
 * Signature: (IJLjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_TreeNode_addDevice(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jstring jname, jstring jtype)
{
  const char *name;
  const char *type;
  int status, newNid = -1, defNid = -1;
  void *ctx = JLONG2PTR(jctx);

  name = (*env)->GetStringUTFChars(env, jname, 0);
  type = (*env)->GetStringUTFChars(env, jtype, 0);
  status = CTXCALLN(TreeGetDefaultNid, &defNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeSetDefaultNid, nid);
  if (STATUS_OK)
    status = CTXCALLN(TreeAddConglom, (char *)name, (char *)type, &newNid);
  CTXCALLN(TreeSetDefaultNid, defNid);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  (*env)->ReleaseStringUTFChars(env, jtype, type);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
  return newNid;
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    setSubtree
 * Signature: (IJZ)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_setSubtree(JNIEnv *env,
                                                        jclass cls
                                                        __attribute__((unused)),
                                                        jint nid, jlong jctx,
                                                        jboolean isSubtree)
{
  int status;
  void *ctx = JLONG2PTR(jctx);

  if (isSubtree)
    status = CTXCALLN(TreeSetSubtree, nid);
  else
    status = CTXCALLN(TreeSetNoSubtree, nid);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/*
 * Class:     MDSplus_TreeNode
 * Method:    moveNode
 * Signature: (IJILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_TreeNode_moveNode(
    JNIEnv *env, jclass cls __attribute__((unused)), jint nid, jlong jctx,
    jint parentNid, jstring jpath)
{
  int status, defNid;
  void *ctx = JLONG2PTR(jctx);
  const char *path;

  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = CTXCALLN(TreeGetDefaultNid, &defNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeSetDefaultNid, parentNid);
  if (STATUS_OK)
    status = CTXCALLN(TreeRenameNode, nid, (char *)path);
  CTXCALLN(TreeSetDefaultNid, defNid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (STATUS_NOT_OK)
    throwMdsException(env, status);
}

/* Class:     MDSplus_TreeNode
 * Method:    doAction
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_TreeNode_doAction(JNIEnv *env
                                                      __attribute__((unused)),
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jint nid, jlong jctx)
{
  return doAction(JLONG2PTR(jctx), nid);
}

static JavaVM *jvm;

static JNIEnv *getJNIEnv()
{
  JNIEnv *jEnv;
  int retVal;
  retVal = (*jvm)->AttachCurrentThread(jvm, (void **)&jEnv, NULL);
  if (retVal)
  {
    printf("AttachCurrentThread error %d\n", retVal);
    return NULL;
  }
  return jEnv;
}

static void releaseJNIEnv() { (*jvm)->DetachCurrentThread(jvm); }

static void handleEvent(void *objPtr, int dim, char *buf)
{
  jmethodID mid;
  JNIEnv *env;
  jclass cls;
  jvalue args[2];
  jbyteArray jbuf;
  int64_t time;
  jobject obj = (jobject)objPtr;
#ifdef _WIN32
  EnterCriticalSection(&critSect);
#else
  pthread_mutex_lock(&evMutex);
#endif

  env = getJNIEnv();
  if (!env)
    return;
  cls = (*env)->GetObjectClass(env, obj);
  if (!cls)
    printf("Error getting class for MDSplus.Event\n");
  mid = (*env)->GetMethodID(env, cls, "intRun", "([BJ)V");
  if (!mid)
    printf("Error getting method intRun for MDSplus.Event\n");
  jbuf = (*env)->NewByteArray(env, dim);
  if (jbuf)
    (*env)->SetByteArrayRegion(env, jbuf, 0, dim, (const jbyte *)buf);
  args[0].l = jbuf;
  LibConvertDateString("now", &time);
  args[1].j = time;
  (*env)->CallVoidMethodA(env, obj, mid, args);
  releaseJNIEnv();
#ifdef _WIN32
  LeaveCriticalSection(&critSect);
#else
  pthread_mutex_unlock(&evMutex);
#endif
}
/*
static void handleREvent(char *evName, char *buf, int dim, void *objPtr)
{
  handleEvent(objPtr, dim, buf);
}
*/
// Record eventObj instances retrieved by NewGlobalref. They will be released
// then the event is disposed (indexed by eventId)
struct EventDescr
{
  jobject eventObj;
  int64_t eventId;
  struct EventDescr *nxt;
};

static pthread_mutex_t eventMutex = PTHREAD_MUTEX_INITIALIZER;
static struct EventDescr *eventDescrHead = 0;
static void addEventDescr(jobject eventObj, int64_t eventId)
{
  struct EventDescr *newDescr = malloc(sizeof(struct EventDescr));
  pthread_mutex_lock(&eventMutex);
  newDescr->eventId = eventId;
  newDescr->eventObj = eventObj;
  newDescr->nxt = eventDescrHead;
  eventDescrHead = newDescr;
  pthread_mutex_unlock(&eventMutex);
}

static jobject releaseEventDescr(int64_t eventId)
{
  jobject retObj = 0;
  struct EventDescr *currDescr, *prevDescr;
  pthread_mutex_lock(&eventMutex);
  currDescr = eventDescrHead;
  prevDescr = eventDescrHead;
  while (currDescr && currDescr->eventId != eventId)
  {
    prevDescr = currDescr;
    currDescr = currDescr->nxt;
  }
  if (currDescr)
  {
    if (prevDescr != currDescr)
      prevDescr->nxt = currDescr->nxt;
    else
      eventDescrHead = currDescr->nxt;
    retObj = currDescr->eventObj;
    free(currDescr);
  }
  pthread_mutex_unlock(&eventMutex);
  return retObj;
}

/*
 * Class:     MDSplus_Event
 * Method:    registerEvent
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jlong JNICALL Java_MDSplus_Event_registerEvent(JNIEnv *env,
                                                         jobject obj,
                                                         jstring jevent)
{
  const char *event;
  int eventId = -1, status;
  jobject eventObj = (*env)->NewGlobalRef(env, obj);
  if (jvm == 0)
  {
    status = (*env)->GetJavaVM(env, &jvm);
    if (status)
      printf("GetJavaVM error %d\n", status);
  }
  event = (*env)->GetStringUTFChars(env, jevent, 0);
  // make sure this Event instance will not be released by the garbage collector
#ifdef _WIN32
  if (!critSectInitialized)
  {
    critSectInitialized = 1;
    InitializeCriticalSection(&critSect);
  }
#endif

  status = MDSEventAst((char *)event, handleEvent, (void *)eventObj, &eventId);
  addEventDescr(eventObj, (int64_t)eventId);
  (*env)->ReleaseStringUTFChars(env, jevent, event);
  if (STATUS_NOT_OK)
    return -1;
  return eventId;
}

/*
 * Class:     MDSplus_Event
 * Method:    unregisterEvent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Event_unregisterEvent(
    JNIEnv *env, jobject obj __attribute__((unused)), jlong eventId)
{
  jobject delObj = releaseEventDescr(eventId);
  MDSEventCan(eventId);
  // Allow Garbage Collector reclaim the Event object
  (*env)->DeleteGlobalRef(env, delObj);
}

/*
 * Class:     MDSplus_Event
 * Method:    setEventRaw
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Event_setEventRaw(JNIEnv *env,
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jstring jevent,
                                                      jbyteArray jbuf)
{
  int dim = (*env)->GetArrayLength(env, jbuf);
  char *buf = (char *)(*env)->GetByteArrayElements(env, jbuf, JNI_FALSE);
  const char *event = (*env)->GetStringUTFChars(env, jevent, 0);
  MDSEvent((char *)event, dim, buf);
  (*env)->ReleaseStringUTFChars(env, jevent, event);
}

/*
 * Class:     MDSplus_Data
 * Method:    convertToDate
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_MDSplus_Data_convertToDate(
    JNIEnv *env, jclass cls __attribute__((unused)), jlong time)
{
  mdsdsc_d_t dateDsc = {0, DTYPE_T, CLASS_D, 0};
  unsigned short len;
  jstring jdate;
  char *date;

  LibSysAscTim(&len, (mdsdsc_t *)&dateDsc, (int *)&time);
  date = malloc(dateDsc.length + 1);
  memcpy(date, dateDsc.pointer, dateDsc.length);
  date[dateDsc.length] = 0;
  jdate = (*env)->NewStringUTF(env, (const char *)date);
  free(date);
  StrFree1Dx(&dateDsc);
  return jdate;
}

/*
 * Class:     MDSplus_Data
 * Method:    getMdsMsg
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_MDSplus_Data_getMdsMsg(JNIEnv *env,
                                                      jclass cls
                                                      __attribute__((unused)),
                                                      jint status)
{
  char *msg = MdsGetMsg(status);
  jstring jmsg = (*env)->NewStringUTF(env, (const char *)msg);
  return jmsg;
}

/*
 * Class:     MDSplus_Data
 * Method:    getTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_MDSplus_Data_getTime(JNIEnv *env
                                                  __attribute__((unused)),
                                                  jclass cls
                                                  __attribute__((unused)))
{
  int64_t time;
  LibConvertDateString("now", &time);
  return (long)time;
}

/////////////////////Connection stuff //////////////////////
/// NOTE put it in the end of this source file so that ipdesc.h does not harm
////////////////////////////////////////////////////////////
#include <ipdesc.h>
/*
 * Class:     MDSplus_Connection
 * Method:    connectToMds
 * Signature: (LMDSplus/String;)I
 */
JNIEXPORT jint JNICALL Java_MDSplus_Connection_connectToMds(
    JNIEnv *env, jobject obj __attribute__((unused)), jstring jAddr)
{
  const char *addr = (*env)->GetStringUTFChars(env, jAddr, 0);
  int sockId = ConnectToMds((char *)addr);
  (*env)->ReleaseStringUTFChars(env, jAddr, addr);
  return sockId;
}

/*
 * Class:     MDSplus_Connection
 * Method:    disconnectFromMds
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Connection_disconnectFromMds(
    JNIEnv *env __attribute__((unused)), jobject obj __attribute__((unused)),
    jint sockId)
{
  DisconnectFromMds(sockId);
}

/*
 * Class:     MDSplus_Connection
 * Method:    openTree
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Connection_openTree(
    JNIEnv *env, jobject obj __attribute__((unused)), jint sockId,
    jstring jname, jint shot)
{
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);
  jobject exc;
  int status = MdsOpen(sockId, (char *)name, shot);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if (STATUS_NOT_OK)
  {
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
  }
}

/*
 * Class:     MDSplus_Connection
 * Method:    closeTree
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Connection_closeTree(
    JNIEnv *env, jobject obj __attribute__((unused)), jint sockId)
{
  jobject exc;
  int status = MdsClose(sockId);
  if (STATUS_NOT_OK)
  {
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
  }
}

/*
 * Class:     MDSplus_Connection
 * Method:    setDefault
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Connection_setDefault(
    JNIEnv *env, jobject obj __attribute__((unused)), jint sockId,
    jstring jpath)
{
  const char *path = (*env)->GetStringUTFChars(env, jpath, 0);
  // jobject exc;
  MdsSetDefault(sockId, (char *)path);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
}

static int convertType(int mdsType)
{
  switch (mdsType)
  {
  case DTYPE_B:
    return DTYPE_CHAR;
  case DTYPE_BU:
    return DTYPE_UCHAR;
  case DTYPE_W:
    return DTYPE_SHORT;
  case DTYPE_WU:
    return DTYPE_USHORT;
  case DTYPE_L:
    return DTYPE_LONG;
  case DTYPE_LU:
    return DTYPE_ULONG;
  case DTYPE_Q:
    return DTYPE_LONGLONG;
  case DTYPE_QU:
    return DTYPE_ULONGLONG;
  case DTYPE_FS:
    return DTYPE_FLOAT;
  case DTYPE_FT:
    return DTYPE_DOUBLE;
  case DTYPE_T:
    return DTYPE_CSTRING;
  default:
    return -1;
  }
}

static char getDType(mdsdsc_t *dsc) { return convertType(dsc->dtype); }

static char getNDims(mdsdsc_t *dsc)
{
  if (dsc->class == CLASS_S)
    return 0;

  return ((mdsdsc_a_t *)dsc)->dimct;
}

static void getDims(mdsdsc_t *dsc, int *dims)
{
  ARRAY_BOUNDS(char *, MAX_DIMS) * arrPtr;
  int i;

  if (dsc->class != CLASS_A)
    return;
  arrPtr = (void *)(ARRAY_BOUNDS(char *, 64) *)dsc;
  for (i = 0; i < arrPtr->dimct; i++)
    dims[i] = arrPtr->m[i];
}

static short getLength(mdsdsc_t *dsc) { return dsc->length; }

static void *getPtr(mdsdsc_t *dsc)
{
  if (dsc->class == CLASS_S)
    return dsc->pointer;

  return ((mdsdsc_a_t *)dsc)->pointer;
}

/*
 * Class:     MDSplus_Connection
 * Method:    get
 * Signature: (ILjava/lang/String;[LMDSplus/Data;)LMDSplus/Data;
 */
JNIEXPORT jobject JNICALL
Java_MDSplus_Connection_get(JNIEnv *env, jobject obj __attribute__((unused)),
                            jint sockId, jstring jExpr, jobjectArray jargs)
{
  const char *expr;
  jobject exc, currArg, retObj;
  int nArgs, i, status;
  mdsdsc_t **dscs;
  char dtype, nDims;
  short length;
  void *ptr;
  int dims[MAX_DIMS];
  int numBytes;
  void *mem = 0;
  mdsdsc_t scalarDsc = {0, 0, CLASS_S, 0};
  DESCRIPTOR_A_COEFF(arrayDsc, 0, 0, 0, MAX_DIMS, 0);

  expr = (*env)->GetStringUTFChars(env, jExpr, 0);
  nArgs = (*env)->GetArrayLength(env, jargs);
  status = SendArg(sockId, 0, DTYPE_CSTRING, nArgs + 1, strlen(expr), 0, 0,
                   (char *)expr);
  (*env)->ReleaseStringUTFChars(env, jExpr, expr);

  dscs = (mdsdsc_t **)malloc(nArgs * sizeof(mdsdsc_t *));
  for (i = 0; i < nArgs; i++)
  {
    currArg = (*env)->GetObjectArrayElement(env, jargs, i);
    dscs[i] = ObjectToDescrip(env, currArg);

    dtype = getDType(dscs[i]);
    nDims = getNDims(dscs[i]);
    length = getLength(dscs[i]);
    getDims(dscs[i], dims);
    ptr = getPtr(dscs[i]);

    status = SendArg(sockId, i + 1, dtype, nArgs + 1, length, nDims, dims, ptr);
    FreeDescrip(dscs[i]);
    if (STATUS_NOT_OK)
    {
      free(dscs);
      exc = (*env)->FindClass(env, "MDSplus/MdsException");
      (*env)->ThrowNew(env, exc, MdsGetMsg(status));
      return NULL;
    }
  }

  free(dscs);
  status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, dims, &numBytes,
                           &ptr, &mem);
  if (STATUS_NOT_OK)
  {
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
    return NULL;
  }
  //   printf("RECEIVED dtype: %d length: %d ndims: %d dim1: %d dim2: %d
  //   numBytes: %d\n",
  //	dtype, length, nDims, dims[0], dims[1], numBytes);
  if (nDims == 0)
  {
    scalarDsc.length = numBytes;
    scalarDsc.pointer = ptr;
    switch (dtype)
    {
    case DTYPE_CHAR:
      scalarDsc.dtype = DTYPE_B;
      break;
    case DTYPE_UCHAR:
      scalarDsc.dtype = DTYPE_BU;
      break;
    case DTYPE_SHORT:
      scalarDsc.dtype = DTYPE_W;
      break;
    case DTYPE_USHORT:
      scalarDsc.dtype = DTYPE_WU;
      break;
    case DTYPE_LONG:
      scalarDsc.dtype = DTYPE_L;
      break;
    case DTYPE_ULONG:
      scalarDsc.dtype = DTYPE_LU;
      break;
    case DTYPE_LONGLONG:
      scalarDsc.dtype = DTYPE_Q;
      break;
    case DTYPE_ULONGLONG:
      scalarDsc.dtype = DTYPE_QU;
      break;
    case DTYPE_FLOAT:
      scalarDsc.dtype = DTYPE_FS;
      break;
    case DTYPE_DOUBLE:
      scalarDsc.dtype = DTYPE_FT;
      break;
    case DTYPE_CSTRING:
      scalarDsc.dtype = DTYPE_T;
      break;
    default:
      exc = (*env)->FindClass(env, "MDSplus/MdsException");
      (*env)->ThrowNew(env, exc,
                       "Unexpected returned data type in mdsip connection");
      return NULL;
    }
    retObj = DescripToObject(env, NULL, &scalarDsc, 0, 0, 0, 0);
  }
  else // nDims > 0
  {
    arrayDsc.length = length;
    arrayDsc.arsize = numBytes;
    arrayDsc.pointer = ptr;
    arrayDsc.dimct = nDims;
    memcpy(&arrayDsc.m, dims, nDims * sizeof(int));
    switch (dtype)
    {
    case DTYPE_CHAR:
      arrayDsc.dtype = DTYPE_B;
      break;
    case DTYPE_UCHAR:
      arrayDsc.dtype = DTYPE_BU;
      break;
    case DTYPE_SHORT:
      arrayDsc.dtype = DTYPE_W;
      break;
    case DTYPE_USHORT:
      arrayDsc.dtype = DTYPE_WU;
      break;
    case DTYPE_LONG:
      arrayDsc.dtype = DTYPE_L;
      break;
    case DTYPE_ULONG:
      arrayDsc.dtype = DTYPE_LU;
      break;
    case DTYPE_LONGLONG:
      arrayDsc.dtype = DTYPE_Q;
      break;
    case DTYPE_ULONGLONG:
      arrayDsc.dtype = DTYPE_QU;
      break;
    case DTYPE_FLOAT:
      arrayDsc.dtype = DTYPE_FS;
      break;
    case DTYPE_DOUBLE:
      arrayDsc.dtype = DTYPE_FT;
      break;
    case DTYPE_CSTRING:
      arrayDsc.dtype = DTYPE_T;
      break;
    default:
      exc = (*env)->FindClass(env, "MDSplus/MdsException");
      (*env)->ThrowNew(env, exc,
                       "Unexpected returned data type in mdsip connection");
      return NULL;
    }
    retObj =
        DescripToObject(env, NULL, (mdsdsc_t *)&arrayDsc, 0, 0, 0, 0);
  }
  if (mem)
    FreeMessage(mem);
  return retObj;
}

/*
 * Class:     MDSplus_Connection
 * Method:    put
 * Signature: (ILjava/lang/String;Ljava/lang/String;[LMDSplus/Data;)V
 */
JNIEXPORT void JNICALL Java_MDSplus_Connection_put(
    JNIEnv *env, jobject obj __attribute__((unused)), jint sockId,
    jstring jPath, jstring jExpr, jobjectArray jArgs)
{
  const char *expr = (*env)->GetStringUTFChars(env, jExpr, 0);
  const char *inPath = (*env)->GetStringUTFChars(env, jPath, 0);
  char *path, *putExpr;
  jobject exc, currArg;
  int nArgs, i, status;
  mdsdsc_t **dscs;
  char dtype, nDims;
  short length;
  void *ptr;
  int dims[MAX_DIMS];
  int numBytes, varIdx;
  void *mem = 0;

  nArgs = (*env)->GetArrayLength(env, jArgs);

  // Double backslashes!!
  path = malloc(strlen(inPath) + 2);
  if (inPath[0] == '\\')
  {
    path[0] = '\\';
    strcpy(&path[1], inPath);
  }
  else
    strcpy(path, inPath);

  putExpr = malloc(strlen("TreePut(") + strlen(expr) + strlen(path) + 5 +
                   nArgs * 2 + 2);
  if (nArgs > 0)
    sprintf(putExpr, "TreePut(\'%s\',\'%s\',", path, expr);
  else
    sprintf(putExpr, "TreePut(\'%s\',\'%s\'", path, expr);
  for (varIdx = 0; varIdx < nArgs; varIdx++)
  {
    if (varIdx < nArgs - 1)
      sprintf(&putExpr[strlen(putExpr)], "$,");
    else
      sprintf(&putExpr[strlen(putExpr)], "$");
  }
  sprintf(&putExpr[strlen(putExpr)], ")");

  free(path);
  (*env)->ReleaseStringUTFChars(env, jExpr, expr);
  (*env)->ReleaseStringUTFChars(env, jPath, inPath);

  status = SendArg(sockId, 0, DTYPE_CSTRING, nArgs + 1, strlen(putExpr), 0, 0,
                   putExpr);
  free(putExpr);

  dscs = (mdsdsc_t **)malloc(nArgs * sizeof(mdsdsc_t *));
  for (i = 0; i < nArgs; i++)
  {
    currArg = (*env)->GetObjectArrayElement(env, jArgs, i);
    dscs[i] = ObjectToDescrip(env, currArg);
    dtype = getDType(dscs[i]);
    nDims = getNDims(dscs[i]);
    length = getLength(dscs[i]);
    getDims(dscs[i], dims);
    ptr = getPtr(dscs[i]);
    status = SendArg(sockId, i + 1, dtype, nArgs + 1, length, nDims, dims, ptr);
    FreeDescrip(dscs[i]);
    if (STATUS_NOT_OK)
    {
      free(dscs);
      exc = (*env)->FindClass(env, "MDSplus/MdsException");
      (*env)->ThrowNew(env, exc, MdsGetMsg(status));
      return;
    }
  }
  free(dscs);
  status = GetAnswerInfoTS(sockId, &dtype, &length, &nDims, dims, &numBytes,
                           &ptr, &mem);
  if (STATUS_NOT_OK)
  {
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
    return;
  }
  if (STATUS_OK && dtype == DTYPE_LONG && nDims == 0 &&
      numBytes == sizeof(int))
    memcpy(&status, ptr, numBytes);
  if (mem)
    FreeMessage(mem);
  if (STATUS_NOT_OK)
  {
    exc = (*env)->FindClass(env, "MDSplus/MdsException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
  }
}

///////////////////GetDeviceField for Netbeans Builder

EXPORT mdsdsc_xd_t *getDeviceFields(char *deviceName)
{
  int status, nid, curr_nid, i;
  char *names, *path;
  static int conglomerate_nids, conglomerate_nids_len;
  struct nci_itm nci_list[] = {
      {4, NciNUMBER_OF_ELTS, &conglomerate_nids, &conglomerate_nids_len},
      {NciEND_OF_LIST, 0, 0, 0}};
  static EMPTYXD(xd);
  DESCRIPTOR_A(dsc, 1, DTYPE_B, 0, 0);
  conglomerate_nids = 0;
  path = getenv("device_beans_path");
  if (!path || !*path)
  {
    setenv("device_beans_path", getenv("HOME"), 0);
  }
  status = TreeOpenNew("device_beans", -1);
  printf("%s\n", MdsGetMsg(status));
  if (STATUS_NOT_OK)
    return &xd;
  status = TreeAddNode("Boh", &nid, TreeUSAGE_STRUCTURE);
  TreeSetDefaultNid(nid);
  status = TreeAddConglom("TEST", deviceName, &nid);
  printf("%s\n", MdsGetMsg(status));
  if (STATUS_OK)
    status = TreeGetNci(nid, nci_list);
  if (STATUS_NOT_OK || conglomerate_nids == 0)
  {
    TreeQuitTree("device_beans", -1);
    return &xd;
  }
  conglomerate_nids--;
  names = (char *)malloc(256 * conglomerate_nids);
  TreeSetDefaultNid(nid);
  curr_nid = nid + 1;
  names[0] = 0;
  for (i = 0; i < conglomerate_nids; i++, curr_nid++)
    sprintf(&names[strlen(names)], "%s ", TreeGetMinimumPath(&nid, curr_nid));
  dsc.arsize = strlen(names);
  dsc.pointer = names;
  MdsCopyDxXd((mdsdsc_t *)&dsc, &xd);
  printf("%s\n", names);
  free(names);
  status = TreeQuitTree("device_beans", -1);
  return &xd;
}
