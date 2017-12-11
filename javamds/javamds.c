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
#include <treeshr.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdstypes.h>
#include <string.h>
#include <stdlib.h>
#include <libroutines.h>
//#include "LocalDataProvider.h"
#include "jScope_LocalDataProvider.h"
#include "MdsHelper.h"
#include "jScope_MdsIpProtocolWrapper.h"
#include "../mdstcpip/mdsip_connections.h"

extern int TdiCompile(), TdiData(), TdiFloat();

static char error_message[512];

static jint DYN_JNI_CreateJavaVM(JavaVM ** jvm, void **env, JavaVMInitArgs * vm_args)
{
  int status;
  static jint(*JNI_CreateJavaVM) (JavaVM **, void **, JavaVMInitArgs *) = 0;
  if (JNI_CreateJavaVM == 0) {
    static DESCRIPTOR(javalib_d, "java");
    static DESCRIPTOR(jvmlib_d, "jvm");
    static DESCRIPTOR(javasym_d, "JNI_CreateJavaVM");
    status = LibFindImageSymbol(&javalib_d, &javasym_d, &JNI_CreateJavaVM);
    if (!(status & 1))
      status = LibFindImageSymbol(&jvmlib_d, &javasym_d, &JNI_CreateJavaVM);
    if (!(status & 1)) {
      JNI_CreateJavaVM = 0;
      printf("JNI_CreateJavaVM Not Found!\n");
      return -1;
    }
  }
  return (*JNI_CreateJavaVM) (jvm, env, vm_args);
}

#define BYTE 1
#define FLOAT 2
#define DOUBLE 3
#define LONG 4
#define QUADWORD 5
static void *MdsGetArray(char *in, int *out_dim, int type);

/*Support routine for MdsHelper */
JNIEXPORT jstring JNICALL Java_MdsHelper_getErrorString(JNIEnv * env,
							jclass cld __attribute__ ((unused)), jint jstatus) {
  char *error_msg = MdsGetMsg(jstatus);
  return (*env)->NewStringUTF(env, error_msg);
}

JNIEXPORT void JNICALL Java_MdsHelper_generateEvent
(JNIEnv * env, jclass cld __attribute__ ((unused)), jstring jevent, jint jshot) {
  const char *event = (*env)->GetStringUTFChars(env, jevent, 0);
  int shot = jshot;

  if (strlen(event) > 0)
    MDSEvent((char *)event, sizeof(int), (char *)&shot);
  (*env)->ReleaseStringUTFChars(env, jevent, event);
}

/* Support routines for jScope */
static void MdsUpdate(char *exp, int shot)
{
  static int prev_shot;
  static char prev_exp[256];
  int status;

  error_message[0] = 0;
  if (!exp)
    return;
  if (shot == prev_shot && !strcmp(prev_exp, exp))
    return;
  prev_shot = shot;
  strcpy(prev_exp, exp);
  status = TreeOpen(exp, shot, 0);
  if (!(status & 1))
    strncpy(error_message, MdsGetMsg(status), 512);
}

static char *MdsGetString(char *in)
{
  char *out = NULL;
  int status;
  struct descriptor in_d = { 0, DTYPE_T, CLASS_S, 0 };
  EMPTYXD(xd);

  in_d.length = strlen(in);
  in_d.pointer = in;
  status = TdiCompile(&in_d, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiData(&xd, &xd MDS_END_ARG);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  if (!xd.pointer) {
    strcpy(error_message, "Missing data");
    return NULL;
  }
  if (xd.pointer->dtype != DTYPE_T) {
    MdsFree1Dx(&xd, NULL);
    strcpy(error_message, "Not a string");
    return NULL;
  }
  out = (char *)malloc(xd.pointer->length + 1);
  memcpy(out, xd.pointer->pointer, xd.pointer->length);
  out[xd.pointer->length] = 0;
  MdsFree1Dx(&xd, NULL);
  error_message[0] = 0;
  return out;
}

static float MdsGetFloat(char *in)
{
  float ris = 0;
  int status;
  struct descriptor in_d = { 0, DTYPE_T, CLASS_S, 0 };
  EMPTYXD(xd);

  in_d.length = strlen(in);
  in_d.pointer = in;
  status = TdiCompile(&in_d, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiData(&xd, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiFloat(&xd, &xd MDS_END_ARG);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return 0;
  }
  if (!xd.pointer) {
    strcpy(error_message, "Missing data");
    return 0;
  }
  if (xd.pointer->class != CLASS_S) {
    strcpy(error_message, "Not a scalar");
    return 0;
  }
  switch (xd.pointer->dtype) {
  case DTYPE_BU:
  case DTYPE_B:
    ris = (float)(*(char *)xd.pointer->pointer);
    break;
  case DTYPE_WU:
  case DTYPE_W:
    ris = (float)(*(short *)xd.pointer->pointer);
    break;
  case DTYPE_LU:
  case DTYPE_L:
    ris = (float)(*(int *)xd.pointer->pointer);
    break;
  case DTYPE_F:
    ris = *(float *)xd.pointer->pointer;
    break;
  case DTYPE_FS:
    ris = *(float *)xd.pointer->pointer;
    break;
  case DTYPE_D:
  case DTYPE_G:
    ris = *(float *)xd.pointer->pointer;
    break;
  default:
    sprintf(error_message, "Not a supported type %d", xd.pointer->dtype);
    return 0;
  }
  MdsFree1Dx(&xd, NULL);
  error_message[0] = 0;
  return ris;
}

static void *MdsGetArray(char *in, int *out_dim, int type)
{
  float *float_ris = NULL;
  double *double_ris = NULL;
  int *int_ris = NULL;
  char *byte_ris = NULL;
  uint64_t *quad_ris = NULL;
  int status, dim, i;
  struct descriptor in_d = { 0, DTYPE_T, CLASS_S, 0 };
  EMPTYXD(xd);
  struct descriptor_a *arr_ptr;

  char * const expanded_in = calloc(strlen(in) + 40, sizeof(char));
  error_message[0] = 0;
  *out_dim = 0;
  switch (type) {
  case FLOAT:
    sprintf(expanded_in, "_xxx = (%s);fs_float(_xxx)", in);
    in_d.length = strlen(expanded_in);
    in_d.pointer = expanded_in;
    break;
  case DOUBLE:
    sprintf(expanded_in, "_xxx = (%s);ft_float(_xxx)", in);
    in_d.length = strlen(expanded_in);
    in_d.pointer = expanded_in;
    break;
  case BYTE:
  case LONG:
    sprintf(expanded_in, "long(%s)", in);
    in_d.length = strlen(expanded_in);
    in_d.pointer = expanded_in;
    break;
  case QUADWORD:
    sprintf(expanded_in, "%s", in);
    in_d.length = strlen(expanded_in);
    in_d.pointer = expanded_in;
    break;
  }

  status = TdiCompile(&in_d, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiData(&xd, &xd MDS_END_ARG);
  free(expanded_in);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return 0;
  }
  if (!xd.pointer) {
    strcpy(error_message, "Missing data");
    return 0;
  }
  if (xd.pointer->class != CLASS_A) {
    if (type != FLOAT && type != DOUBLE) {	/*Legal only if used to retrieve the shot number */
      int_ris = malloc(sizeof(int));
      switch (xd.pointer->dtype) {
      case DTYPE_BU:
      case DTYPE_B:
	int_ris[0] = *((char *)xd.pointer->pointer);
	break;
      case DTYPE_WU:
      case DTYPE_W:
	int_ris[0] = *((short *)xd.pointer->pointer);
	break;
      case DTYPE_LU:
      case DTYPE_L:
	int_ris[0] = *((int *)xd.pointer->pointer);
	break;
      case DTYPE_F:
      case DTYPE_FS:
	int_ris[0] = *((int *)xd.pointer->pointer);
	break;
      }
      *out_dim = 1;
      return int_ris;
    }
    strcpy(error_message, "Not an array");
    return 0;
  }
  arr_ptr = (struct descriptor_a *)xd.pointer;
  *out_dim = dim = arr_ptr->arsize / arr_ptr->length;
  switch (type) {
  case FLOAT:
    float_ris = (float *)malloc(sizeof(float) * dim);
    break;
  case DOUBLE:
    double_ris = (double *)malloc(sizeof(double) * dim);
    break;
  case BYTE:
    byte_ris = malloc(dim);
    break;
  case LONG:
    int_ris = (int *)malloc(sizeof(int) * dim);
    break;
  case QUADWORD:
    quad_ris = (uint64_t *) malloc(8 * dim);
    break;

  }
  switch (arr_ptr->dtype) {
  case DTYPE_BU:
  case DTYPE_B:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((char *)arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((char *)arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((char *)arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((char *)arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	free((char *)quad_ris);
	MdsFree1Dx(&xd, NULL);
	return NULL;
	break;
      }
    }
    break;
  case DTYPE_WU:
  case DTYPE_W:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((short *)arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((short *)arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((short *)arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((int *)arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	free((char *)quad_ris);
	MdsFree1Dx(&xd, NULL);
	return NULL;
	break;
      }
    }
    break;
  case DTYPE_LU:
  case DTYPE_L:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((int *)arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((int *)arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((int *)arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((int *)arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	free((char *)quad_ris);
	MdsFree1Dx(&xd, NULL);
	return NULL;
	break;
      }
    }
    break;
  case DTYPE_F:
  case DTYPE_FS:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((float *)arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((float *)arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((float *)arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((float *)arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	free((char *)quad_ris);
	MdsFree1Dx(&xd, NULL);
	return NULL;
	break;
      }
    }
    break;
  case DTYPE_FT:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((double *)arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((double *)arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((double *)arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((double *)arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	free((char *)quad_ris);
	MdsFree1Dx(&xd, NULL);
	return NULL;
	break;
      }
    }
    break;
  case DTYPE_Q:
  case DTYPE_QU:
    for (i = 0; i < dim; i++) {
      switch (type) {
      case FLOAT:
	float_ris[i] = ((int64_t *) arr_ptr->pointer)[i];
	break;
      case DOUBLE:
	double_ris[i] = ((int64_t *) arr_ptr->pointer)[i];
	break;
      case BYTE:
	byte_ris[i] = ((uint64_t *) arr_ptr->pointer)[i];
	break;
      case LONG:
	int_ris[i] = ((uint64_t *) arr_ptr->pointer)[i];
	break;
      case QUADWORD:
	quad_ris[i] = ((uint64_t *) arr_ptr->pointer)[i];
	break;
      }
    }
    break;

  default:
    strcpy(error_message, "Not a supported type");
    return NULL;
  }
  MdsFree1Dx(&xd, NULL);
  error_message[0] = 0;
  switch (type) {
  case FLOAT:
    return float_ris;
  case DOUBLE:
    return double_ris;
  case BYTE:
    return byte_ris;
  case LONG:
    return int_ris;
  case QUADWORD:
    return quad_ris;
  }
  return NULL;
}

/* Implementation of the native methods for LocalProvider class in jScope */
JNIEXPORT void JNICALL Java_jScope_LocalDataProvider_UpdateNative(JNIEnv * env, jobject obj __attribute__ ((unused)),
								  jstring exp, jlong shot)
{
  const char *exp_char;
  error_message[0] = 0;
  if (exp == NULL)
    return;
  exp_char = (*env)->GetStringUTFChars(env, exp, 0);
  MdsUpdate((char *)exp_char, (int)shot);
  (*env)->ReleaseStringUTFChars(env, exp, exp_char);
}

JNIEXPORT jstring JNICALL Java_jScope_LocalDataProvider_ErrorString(JNIEnv * env, jobject obj __attribute__ ((unused)))
{
  if (!error_message[0])
    return NULL;
  return (*env)->NewStringUTF(env, error_message);
}

JNIEXPORT jstring JNICALL Java_jScope_LocalDataProvider_GetString(JNIEnv * env, jobject obj __attribute__ ((unused)),
								  jstring in)
{
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
  char *out_char = MdsGetString((char *)in_char);

  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (!out_char)
    return (*env)->NewStringUTF(env, "");
  else
    return (*env)->NewStringUTF(env, out_char);
}

JNIEXPORT jfloatArray JNICALL Java_jScope_LocalDataProvider_GetFloatArrayNative(JNIEnv * env,
										jobject obj __attribute__ ((unused)),
										jstring in)
{
  jfloatArray jarr;
  //float zero = 0.;
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
  int dim;
  float *out_ptr;

  out_ptr = MdsGetArray((char *)in_char, &dim, FLOAT);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (error_message[0]) {	/*Return a dummy vector without elements */
    return NULL;
  }
  jarr = (*env)->NewFloatArray(env, dim);
  (*env)->SetFloatArrayRegion(env, jarr, 0, dim, out_ptr);
  free((char *)out_ptr);
  return jarr;
}

JNIEXPORT jdoubleArray JNICALL Java_jScope_LocalDataProvider_GetDoubleArrayNative(JNIEnv * env,
										  jobject obj __attribute__ ((unused)),
										  jstring in)
{
  jdoubleArray jarr;
  //float zero = 0.;
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
  int dim;
  double *out_ptr;



  out_ptr = MdsGetArray((char *)in_char, &dim, DOUBLE);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (error_message[0]) {	/*Return a dummy vector without elements */
    return NULL;
  }
  jarr = (*env)->NewDoubleArray(env, dim);
  (*env)->SetDoubleArrayRegion(env, jarr, 0, dim, out_ptr);
  free((char *)out_ptr);
  return jarr;
}

JNIEXPORT jdoubleArray JNICALL Java_jScope_LocalDataProvider_GetLongArrayNative(JNIEnv * env,
										jobject obj __attribute__ ((unused)),
										jstring in)
{
  jlongArray jarr;
  //float zero = 0.;
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
  int dim;
  int64_t *out_ptr;

  out_ptr = MdsGetArray((char *)in_char, &dim, QUADWORD);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (error_message[0] || !out_ptr) {	/*Return a dummy vector without elements */
    return NULL;
  }
  jarr = (*env)->NewLongArray(env, dim);
  (*env)->SetLongArrayRegion(env, jarr, 0, dim, (const jlong *)out_ptr);
  free((char *)out_ptr);
  return jarr;
}

JNIEXPORT jintArray JNICALL Java_jScope_LocalDataProvider_GetIntArray(JNIEnv * env, jobject obj __attribute__ ((unused)),
								      jstring in)
{
  jintArray jarr;
  //float zero = 0.;
  const char *in_char;
  int dim;
  int *out_ptr;

  in_char = (*env)->GetStringUTFChars(env, in, 0);

  out_ptr = MdsGetArray((char *)in_char, &dim, LONG);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (error_message[0]) {	/*Return a dummy vector without elements */
    return NULL;
  }
  jarr = (*env)->NewIntArray(env, dim);
  (*env)->SetIntArrayRegion(env, jarr, 0, dim, (const jint *)out_ptr);

  free((char *)out_ptr);
  return jarr;
}

JNIEXPORT jbyteArray JNICALL Java_jScope_LocalDataProvider_GetByteArray(JNIEnv * env, jobject obj __attribute__ ((unused)),
									jstring in)
{
  jbyteArray jarr;
  //float zero = 0.;
  const char *in_char;
  int dim;
  int *out_ptr;

  in_char = (*env)->GetStringUTFChars(env, in, 0);
  out_ptr = MdsGetArray((char *)in_char, &dim, BYTE);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  if (error_message[0]) {	/*Return a dummy vector without elements */
    return NULL;
  }
  jarr = (*env)->NewByteArray(env, dim);
  (*env)->SetByteArrayRegion(env, jarr, 0, dim, (const jbyte *)out_ptr);
  free((char *)out_ptr);
  return jarr;
}

//JNIEXPORT jfloat JNICALL Java_jScope_LocalDataProvider_GetFloatNative(JNIEnv *env, jobject obj, jstring in)
JNIEXPORT jdouble JNICALL Java_jScope_LocalDataProvider_GetFloatNative(JNIEnv * env, jobject obj __attribute__ ((unused)),
								       jstring in)
{
  double ris;
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);

  ris = MdsGetFloat((char *)in_char);
  (*env)->ReleaseStringUTFChars(env, in, in_char);
  return ris;
}

JNIEXPORT void JNICALL Java_jScope_LocalDataProvider_SetEnvironmentSpecific(JNIEnv * env,
									    jobject obj __attribute__ ((unused)), jstring in,
									    jstring jdefNode)
{
  int status, nid;
  const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
  const char *defNode;
  struct descriptor in_d = { 0, DTYPE_T, CLASS_S, 0 };
  EMPTYXD(xd);
  error_message[0] = 0;
  if (in_char && *in_char) {
    in_d.length = strlen(in_char);
    in_d.pointer = (char *)in_char;
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
    if (status & 1)
      status = TdiData(&xd, &xd MDS_END_ARG);
    if (!(status & 1))
      strncpy(error_message, MdsGetMsg(status), 512);
    MdsFree1Dx(&xd, NULL);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
  }
  if (jdefNode) {
    defNode = (*env)->GetStringUTFChars(env, jdefNode, 0);
    status = TreeFindNode((char *)defNode, &nid);
    if (status & 1)
      TreeSetDefaultNid(nid);
    (*env)->ReleaseStringUTFChars(env, jdefNode, defNode);
  }

}

/////////////////Segmented stuff/////////////////
/*
 * Class:     jScope_LocalDataProvider
 * Method:    isSegmentedNode
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_jScope_LocalDataProvider_isSegmentedNode
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName) {
  int status, nid, numSegments;
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);

  status = TreeFindNode((char *)nodeName, &nid);
  (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
  if (status & 1) {
    status = TreeGetNumSegments(nid, &numSegments);
    if ((status & 1) && numSegments > 0)
      return 1;
  }
  return 0;
}

static int needSwap()
{
  static char intg[] = { 1, 0, 0, 0 };
  return 1 & *((int *)intg);
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getSegment
 * Signature: (Ljava/lang/String;II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_jScope_LocalDataProvider_getSegment
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jint segmentIdx, jint segmentOffset) {
  int status, nid, i, nSamples;
  //int numSegments;
  jbyteArray jarr;
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  EMPTYXD(segXd);
  //EMPTYXD(decXd);
  EMPTYXD(dimXd);
  ARRAY_COEFF(char, 3) * arrPtr;
  int frameSize;
  char tmp;
  char *buf;

  status = TreeFindNode((char *)nodeName, &nid);
  (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  status = TreeGetSegment(nid, segmentIdx, &segXd, &dimXd);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  status = TdiData(&segXd, &segXd MDS_END_ARG);
  arrPtr = (void *)segXd.pointer;
  if (arrPtr->dimct < 2) {
    strcpy(error_message, "Invalid segment dimension for image frames");
    return NULL;
  }
  if (arrPtr->dimct == 2 && segmentOffset > 0) {
    printf("INTERNAL ERROR IN GET SEGMENT FRAMES!!!\n");
    return NULL;
  }
  if (arrPtr->dimct > 2 && (int)arrPtr->m[2] <= segmentOffset) {
    printf("INTERNAL ERROR IN GET SEGMENT FRAMES!!!\n");
    return NULL;
  }
  frameSize = arrPtr->m[0] * arrPtr->m[1] * arrPtr->length;
  if (needSwap()) {
    buf = arrPtr->pointer;
    nSamples = arrPtr->arsize / arrPtr->length;
    switch (arrPtr->length) {
    case 2:
      for (i = 0; i < nSamples; i++) {
	tmp = buf[2 * i];
	buf[2 * i] = buf[2 * i + 1];
	buf[2 * i + 1] = tmp;
      }
      break;
    case 4:
      for (i = 0; i < nSamples; i++) {
	tmp = buf[4 * i];
	buf[4 * i] = buf[4 * i + 3];
	buf[4 * i + 3] = tmp;
	tmp = buf[4 * i + 1];
	buf[4 * i + 1] = buf[4 * i + 2];
	buf[4 * i + 2] = tmp;
      }
      break;
    }
  }

  jarr = (*env)->NewByteArray(env, frameSize);
  (*env)->SetByteArrayRegion(env, jarr, 0, frameSize,
			     (const jbyte *)arrPtr->pointer + (int)segmentOffset * frameSize);

  MdsFree1Dx(&segXd, 0);
  MdsFree1Dx(&dimXd, 0);
  return jarr;
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getAllFrames
 * Signature: (Ljava/lang/String;II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_jScope_LocalDataProvider_getAllFrames
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jint startIdx, jint endIdx) {
  EMPTYXD(xd);
  ARRAY_COEFF(char *, 3)*arrPtr;
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  struct descriptor nodeNameD = { strlen(nodeName), DTYPE_T, CLASS_S, (char *)nodeName };
  int status, frameSize;
  jbyteArray jarr;
  char tmp;
  char *buf;
  int i, nSamples;

  status = TdiCompile(&nodeNameD, &xd MDS_END_ARG);
  (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
  if (status & 1)
    status = TdiData(&xd, &xd MDS_END_ARG);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  arrPtr = (void *)xd.pointer;
  if (arrPtr->dimct < 2) {
    sprintf(error_message, "Invalid segment dimension for image frames: %d %d", arrPtr->dimct,
	    arrPtr->arsize);
    return NULL;
  }
  if (arrPtr->dimct == 2 && endIdx > 0) {
    strcpy(error_message, "INTERNAL ERROR IN GET ALL FRAMES!!!");
    printf("INTERNAL ERROR IN GET ALL FRAMES!!!\n");
    return NULL;
  }
  if (arrPtr->dimct > 2 && (int)arrPtr->m[2] < endIdx) {
    strcpy(error_message, "INTERNAL ERROR IN GET ALL FRAMES  !!!");
    printf("INTERNAL ERROR IN GET ALL FRAMES!!!\n");
    return NULL;
  }

  frameSize = arrPtr->m[0] * arrPtr->m[1] * arrPtr->length;
  if (needSwap()) {
    buf = (char *)arrPtr->pointer;
    nSamples = arrPtr->arsize / arrPtr->length;
    switch (arrPtr->length) {
    case 2:
      for (i = 0; i < nSamples; i++) {
	tmp = buf[2 * i];
	buf[2 * i] = buf[2 * i + 1];
	buf[2 * i + 1] = tmp;
      }
      break;
    case 4:
      for (i = 0; i < nSamples; i++) {
	tmp = buf[4 * i];
	buf[4 * i] = buf[4 * i + 3];
	buf[4 * i + 3] = tmp;
	tmp = buf[4 * i + 1];
	buf[4 * i + 1] = buf[2 * i + 2];
	buf[4 * i + 2] = tmp;
      }
      break;
    }
  }

  jarr = (*env)->NewByteArray(env, frameSize * (endIdx - startIdx));
  (*env)->SetByteArrayRegion(env, jarr, 0, frameSize * (endIdx - startIdx),
			     (const jbyte *)arrPtr->pointer + (int)startIdx * frameSize);
  MdsFree1Dx(&xd, 0);
  return jarr;
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getInfo
 * Signature: (Ljava/lang/String;)[I
 */
JNIEXPORT jobject JNICALL Java_jScope_LocalDataProvider_getInfo
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jboolean isSegmented) {
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  EMPTYXD(xd);
  ARRAY_COEFF(char *, 3) * arrPtr;
  int status, nid;
  jintArray jarr;
  char dtype, dimct;
  int dims[64];
  int nextRow;
  int retNumDims;
  int retDims[64];
  int retDtype, i, retPixelSize;

  jclass clazz;
  jmethodID mid;
  jobject retObj;
  jvalue args[3];

  struct descriptor nodeNameD = { strlen(nodeName), DTYPE_T, CLASS_S, (char *)nodeName };
//Returned array: [width, height, bytesPerPixel]
  if (isSegmented) {
    status = TreeFindNode((char *)nodeName, &nid);
    (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    status = TreeGetSegmentInfo(nid, 0, &dtype, &dimct, dims, &nextRow);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    retNumDims = dimct;
    memcpy(retDims, dims, dimct * sizeof(int));
    retDtype = dtype;
    switch(dtype) {
        case DTYPE_B:
        case DTYPE_BU: retPixelSize = 1; break;
        case DTYPE_W:
        case DTYPE_WU: retPixelSize = 2; break;
        case DTYPE_L:
        case DTYPE_LU: retPixelSize = 4; break;
        case DTYPE_Q:
        case DTYPE_QU: retPixelSize = 8; break;
	default: retPixelSize = 1;
    }
   } else {
    status = TdiCompile(&nodeNameD, &xd MDS_END_ARG);
    (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
    if (status & 1)
      status = TdiData(&xd, &xd MDS_END_ARG);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    arrPtr = (void *)xd.pointer;
    retDtype = arrPtr->dtype;
    retNumDims = arrPtr->dimct;
    for(i = 0; i < retNumDims; i++)
      retDims[i] = arrPtr->m[i];
    retPixelSize = arrPtr->length;
    MdsFree1Dx(&xd, 0);
  }

//Build resulting object
  clazz = (*env)->FindClass(env, "jScope/LocalDataProviderInfo");
  if(clazz == NULL)
  {
    printf("Error finding class jScope.LocalDataProviderInfo\n");
    return NULL;
  }
  mid  = (*env)->GetMethodID(env, clazz, "<init>", "(II[I)V");
  if(mid == NULL)
  {
    printf("Error finding constructor for Scope.LocalDataProviderInfo\n");
    return NULL;
  }
  jarr = (*env)->NewIntArray(env, retNumDims);
  (*env)->SetIntArrayRegion(env, jarr, 0, retNumDims, (const jint *)retDims);

  args[0].i = retDtype;
  args[1].i = retPixelSize;
  args[2].l = jarr;
  //va_arg(args, jarr);
  retObj = (*env)->NewObjectA(env, clazz,  mid, args);
  (*env)->ReleaseIntArrayElements(env, jarr, (jint *)retDims, JNI_COMMIT);
  return retObj;
}
/*
 * Class:     jScope_LocalDataProvider
 * Method:    getInfo
 * Signature: (Ljava/lang/String;)[I
 */
JNIEXPORT jintArray JNICALL Java_jScope_LocalDataProvider_getInfoXXX
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jboolean isSegmented) {
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  EMPTYXD(xd);
  ARRAY_COEFF(char *, 3) * arrPtr;
  int status, nid;
  int retInfo[3];
  jintArray jarr;
  char dtype, dimct;
  int dims[64];
  int nextRow;
  struct descriptor nodeNameD = { strlen(nodeName), DTYPE_T, CLASS_S, (char *)nodeName };
//Returned array: [width, height, bytesPerPixel]
  if (isSegmented) {
    status = TreeFindNode((char *)nodeName, &nid);
    (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    status = TreeGetSegmentInfo(nid, 0, &dtype, &dimct, dims, &nextRow);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    retInfo[0] = dims[0];
    retInfo[1] = dims[1];
    switch (dtype) {
    case DTYPE_B:
    case DTYPE_BU:
      retInfo[2] = 1;
      break;
    case DTYPE_W:
    case DTYPE_WU:
      retInfo[2] = 2;
      break;
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_FLOAT:
      retInfo[2] = 4;
      break;
    case DTYPE_Q:
    case DTYPE_QU:
    case DTYPE_DOUBLE:
      retInfo[2] = 8;
      break;
    default:
      retInfo[2] = 1;
    }
  } else {
    status = TdiCompile(&nodeNameD, &xd MDS_END_ARG);
    (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
    if (status & 1)
      status = TdiData(&xd, &xd MDS_END_ARG);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    arrPtr = (void *)xd.pointer;
    retInfo[0] = arrPtr->m[0];
    retInfo[1] = arrPtr->m[1];
    retInfo[2] = arrPtr->length;
    MdsFree1Dx(&xd, 0);
  }
  jarr = (*env)->NewIntArray(env, 3);
  (*env)->SetIntArrayRegion(env, jarr, 0, 3, (const jint *)retInfo);
  return NULL;
}

static int getStartEndIdx(int nid, float startTime, float endTime, int *retStartIdx, int *retEndIdx)
{
  int status, nSegments, startIdx, endIdx;
  float currEnd;
  EMPTYXD(startXd);
  EMPTYXD(endXd);

  status = TreeGetNumSegments(nid, &nSegments);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return status;
  }
  for (startIdx = 0; startIdx < nSegments; startIdx++) {
    status = TreeGetSegmentLimits(nid, startIdx, &startXd, &endXd);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return status;
    }
    status = TdiData(&endXd, &endXd MDS_END_ARG);
    if (status & 1)
      status = TdiFloat(&endXd, &endXd MDS_END_ARG);
    if (endXd.pointer->length == sizeof(float))
      currEnd = *(float *)endXd.pointer->pointer;
    else
      currEnd = *(double *)endXd.pointer->pointer;
    MdsFree1Dx(&startXd, 0);
    MdsFree1Dx(&endXd, 0);
    if (currEnd >= startTime)
      break;
  }
  for (endIdx = startIdx; endIdx < nSegments; endIdx++) {
    status = TreeGetSegmentLimits(nid, endIdx, &startXd, &endXd);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return status;
    }
    status = TdiData(&startXd, &startXd MDS_END_ARG);
    if (status & 1)
      status = TdiFloat(&endXd, &endXd MDS_END_ARG);
    if (endXd.pointer->length == sizeof(float))
      currEnd = *(float *)endXd.pointer->pointer;
    else
      currEnd = *(double *)endXd.pointer->pointer;
    MdsFree1Dx(&startXd, 0);
    MdsFree1Dx(&endXd, 0);
    if (currEnd >= endTime)
      break;
  }
  *retStartIdx = startIdx;
  *retEndIdx = endIdx;
  return 1;
}

static int isSingleFramePerSegment(int nid)
{
  EMPTYXD(xd);
  EMPTYXD(dimXd);
  ARRAY_COEFF(char *, 3) * arrPtr;
  int isSingle;
  int status;

  status = TreeGetSegment(nid, 0, &xd, &dimXd);
  if (!(status & 1))
    return 0;
  arrPtr = (void *)xd.pointer;
  if (arrPtr->dimct != 3) {
    printf
	("INTERNAL ERROR IN LOCAL DATA PROVIDER: unexpected number of dimensions per segment: %d\n",
	 arrPtr->dimct);
    return 0;
  }
  //printf("Segment dimensions: %d %d %d\n", arrPtr->m[0], arrPtr->m[1], arrPtr->m[2]);
  isSingle = (arrPtr->m[2] == 1);
  MdsFree1Dx(&xd, 0);
  MdsFree1Dx(&dimXd, 0);
  return isSingle;
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getSegmentTimes
 * Signature: (Ljava/lang/String;Ljava/lang/String;FF)[F
 */
JNIEXPORT jfloatArray JNICALL Java_jScope_LocalDataProvider_getSegmentTimes
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jstring jTimeName __attribute__ ((unused)), jfloat startTime,
     jfloat endTime) {
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  //EMPTYXD(dimXd);
  EMPTYXD(segXd);
  EMPTYXD(emptyXd);
  int status, nid, startIdx, endIdx, idx, nTimes, actSegments, currIdx;
  //float currStart, currEnd;
  struct descriptor_xd *timesXds;
  struct descriptor_a *arrPtr;
  jfloatArray jarr;
  float *farr;

  status = TreeFindNode((char *)nodeName, &nid);
  (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  status = getStartEndIdx(nid, startTime, endTime, &startIdx, &endIdx);
  if (!(status & 1))
    return NULL;
  actSegments = endIdx - startIdx;
  timesXds = malloc(sizeof(struct descriptor_xd) * actSegments);
  nTimes = 0;
  if (isSingleFramePerSegment(nid)) {
    farr = malloc(actSegments * sizeof(float));
    for (idx = 0; idx < actSegments; idx++) {
      status = TreeGetSegmentLimits(nid, idx, &startXd, &endXd);
      if (status & 1)
	status = TdiData(&startXd, &startXd MDS_END_ARG);
      if (status & 1)
	status = TdiFloat(&startXd, &startXd MDS_END_ARG);
      if (!startXd.pointer)
	return NULL;
      if (startXd.pointer->length == sizeof(float))
	farr[idx] = *(float *)startXd.pointer->pointer;
      else
	farr[idx] = *(double *)startXd.pointer->pointer;
      MdsFree1Dx(&startXd, 0);
      MdsFree1Dx(&endXd, 0);
    }
    jarr = (*env)->NewFloatArray(env, actSegments);
    (*env)->SetFloatArrayRegion(env, jarr, 0, actSegments, farr);
    free((char *)farr);
    return jarr;
  } else {
    for (idx = 0; idx < actSegments; idx++) {
      timesXds[idx] = emptyXd;
      status = TreeGetSegment(nid, idx + startIdx, &segXd, &timesXds[idx]);
      if (status & 1)
	status = TdiData(&timesXds[idx], &timesXds[idx] MDS_END_ARG);
      if (status & 1)
	status = TdiFloat(&timesXds[idx], &timesXds[idx] MDS_END_ARG);
      if (!(status & 1)) {
	strncpy(error_message, MdsGetMsg(status), 512);
	return NULL;
      }
      if (timesXds[idx].pointer->class == CLASS_S)
	nTimes = nTimes + 1;
      else {
	arrPtr = (struct descriptor_a *)timesXds[idx].pointer;
	nTimes += arrPtr->arsize / arrPtr->length;
      }
      MdsFree1Dx(&segXd, 0);
    }
    jarr = (*env)->NewFloatArray(env, nTimes);
    currIdx = 0;
    for (idx = 0; idx < actSegments; idx++) {
      if (timesXds[idx].pointer->class == CLASS_S) {
	(*env)->SetFloatArrayRegion(env, jarr, currIdx, 1, (float *)timesXds[idx].pointer->pointer);
	currIdx++;
      } else {
	arrPtr = (struct descriptor_a *)timesXds[idx].pointer;
	(*env)->SetFloatArrayRegion(env, jarr, currIdx, arrPtr->arsize / arrPtr->length,
				    (float *)arrPtr->pointer);
	currIdx += arrPtr->arsize / arrPtr->length;
      }
      MdsFree1Dx(&timesXds[idx], 0);
    }
    free((char *)timesXds);
    return jarr;
  }
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getAllTimes
 * Signature: (Ljava/lang/String;Ljava/lang/String;)[F
 */
JNIEXPORT jfloatArray JNICALL Java_jScope_LocalDataProvider_getAllTimes
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName __attribute__ ((unused)), jstring jTimeName) {
  EMPTYXD(xd);
  int status;
  const char *timeName = (*env)->GetStringUTFChars(env, jTimeName, 0);
  struct descriptor timeNameD = { strlen(timeName), DTYPE_T, CLASS_S, (char *)timeName };
  struct descriptor_a *arrPtr;
  jfloatArray jarr;

  status = TdiCompile(&timeNameD, &xd MDS_END_ARG);
  (*env)->ReleaseStringUTFChars(env, jTimeName, timeName);
  if (status & 1)
    status = TdiData(&xd, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiFloat(&xd, &xd MDS_END_ARG);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  if (xd.pointer->class == CLASS_S) {
    jarr = (*env)->NewFloatArray(env, 1);
    (*env)->SetFloatArrayRegion(env, jarr, 0, 1, (float *)xd.pointer->pointer);
  } else {
    arrPtr = (struct descriptor_a *)xd.pointer;
    jarr = (*env)->NewFloatArray(env, arrPtr->arsize / arrPtr->length);
    (*env)->SetFloatArrayRegion(env, jarr, 0, arrPtr->arsize / arrPtr->length,
				(float *)arrPtr->pointer);
  }
  MdsFree1Dx(&xd, 0);
  return jarr;
}

/*
 * Class:     jScope_LocalDataProvider
 * Method:    getSegmentIdxs
 * Signature: (Ljava/lang/String;FF)[I
 */
JNIEXPORT jintArray JNICALL Java_jScope_LocalDataProvider_getSegmentIdxs
(JNIEnv * env, jclass cls __attribute__ ((unused)), jstring jNodeName, jfloat startTime, jfloat endTime) {
  const char *nodeName = (*env)->GetStringUTFChars(env, jNodeName, 0);
  int status, nid, nSegments, startIdx, endIdx, idx, currIdx;
  jintArray jarr;
  int *dims;
  int currDim[64];
  int *arr;
  char dtype, dimct;
  int nextRow, nRows, rowIdx;

  status = TreeFindNode((char *)nodeName, &nid);
  (*env)->ReleaseStringUTFChars(env, jNodeName, nodeName);
  if (!(status & 1)) {
    strncpy(error_message, MdsGetMsg(status), 512);
    return NULL;
  }
  status = getStartEndIdx(nid, startTime, endTime, &startIdx, &endIdx);
  if (!(status & 1))
    return NULL;
  nSegments = endIdx - startIdx;
  dims = (int *)malloc(nSegments * sizeof(int));
  nRows = 0;
  for (idx = 0; idx < nSegments; idx++) {
    status = TreeGetSegmentInfo(nid, idx + startIdx, &dtype, &dimct, currDim, &nextRow);
    if (!(status & 1)) {
      strncpy(error_message, MdsGetMsg(status), 512);
      return NULL;
    }
    if (dimct < 2) {
      printf("INTERNAL ERROR IN SEGMENTED FRAME: DIMENSION < 2\n");
      return NULL;
    }
    if (dimct == 2) {
      dims[idx] = 1;
      nRows++;
    } else {
      dims[idx] = currDim[2];
      nRows += currDim[2];
    }
  }
  arr = malloc(nRows * sizeof(int));
  rowIdx = 0;
  for (idx = 0; idx < nSegments; idx++) {
    for (currIdx = 0; currIdx < dims[idx]; currIdx++)
      arr[rowIdx++] = startIdx + idx;
  }
  jarr = (*env)->NewIntArray(env, nRows);
  (*env)->SetIntArrayRegion(env, jarr, 0, nRows, (const jint *)arr);
  free((char *)arr);
  free((char *)dims);
  return jarr;
}

/////////////////Events stuff////////////////////

static JavaVM *jvm;
static jobject localDataProviderInstance;

static JNIEnv *getJNIEnv()
{
  JNIEnv *jEnv;
  int retVal;

  retVal = (*jvm)->AttachCurrentThread(jvm, (void **)&jEnv, NULL);
  if (retVal)
    printf("AttachCurrentThread error %d\n", retVal);
  return jEnv;
}

static void releaseJNIEnv()
{
  (*jvm)->DetachCurrentThread(jvm);
}

static void handleEvent(void *nameIdx, int dim __attribute__ ((unused)),
			char *buf __attribute__ ((unused)))
{
  jmethodID mid;
  jvalue args[1];
  JNIEnv *env;
  jclass cls;

  env = getJNIEnv();
  cls = (*env)->GetObjectClass(env, localDataProviderInstance);
  if (!cls)
    printf("Error getting class for LocalDataProvider\n");
  mid = (*env)->GetMethodID(env, cls, "fireEvent", "(I)V");
  if (!mid)
    printf("Error getting method fireEvent for LoalDataProvider\n");
  args[0].i = (char *)nameIdx - (char *)0;
  (*env)->CallVoidMethodA(env, localDataProviderInstance, mid, args);
  releaseJNIEnv();
}

JNIEXPORT jint JNICALL Java_jScope_LocalDataProvider_registerEvent
    (JNIEnv * env, jobject obj, jstring jevent, jint idx)
{
  int evId, status;
  const char *event;

  if (jvm == 0) {
    status = (*env)->GetJavaVM(env, &jvm);
    if (status)
      printf("GetJavaVM error %d\n", status);
  }
  localDataProviderInstance = (*env)->NewGlobalRef(env, obj);
  event = (*env)->GetStringUTFChars(env, jevent, 0);
  status = MDSEventAst((char *)event, handleEvent, idx + (char *)0, &evId);
  if (!(status & 1)) {
    printf("Error calling MDSEventAst");
    evId = 0;
  }
  (*env)->ReleaseStringUTFChars(env, jevent, event);
  return evId;
}

JNIEXPORT void JNICALL Java_jScope_LocalDataProvider_unregisterEvent
(JNIEnv * env __attribute__ ((unused)), jobject obj __attribute__ ((unused)), jint evId) {

  MDSEventCan(evId);
}

/* CompositeWaveDisplay management routines for using
jScope panels outside java application */
#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif
#define MAX_WINDOWS 10000
JNIEnv *env = 0;
static jobject jobjects[MAX_WINDOWS];

EXPORT int createWindow(char *name, int idx, int enableLiveUpdate)
{
  jint res;
  jclass cls;
  jmethodID mid;
  jstring jstr;
  char classpath[2048], *curr_classpath;
  jvalue args[2];

  JavaVM *jvm;
  JavaVMInitArgs vm_args;
  JavaVMOption options[3];

  if (env == 0) {		/* Java virtual machine does not exist yet */
    vm_args.version = JNI_VERSION_1_2;	//0x00010001;
    options[0].optionString = "-Djava.compiler=NONE";	/* disable JIT */
    options[1].optionString = "-Djava.class.path=/usr/local/mdsplus/java/classes/jTraverser.jar";	/* user classes */
    options[2].optionString = "-verbose:jni";	/* print JNI-related messages */

    vm_args.nOptions = 2;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    vm_args.options = options;
    curr_classpath = getenv("CLASSPATH");

    if (curr_classpath) {
      sprintf(classpath, "%s%c%s", options[1].optionString, PATH_SEPARATOR, curr_classpath);
      options[1].optionString = classpath;
    }

    res = DYN_JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);

    if (res < 0) {
      printf("\nCannot create Java VM (result = %d)!!\n", (int)res);
      return -1;
    }
  }
  cls = (*env)->FindClass(env, "jScope/CompositeWaveDisplay");
  if (cls == 0) {
    printf("\nCannot find CompositeWaveDisplay classes!\n");
    return -1;
  }
  mid =
      (*env)->GetStaticMethodID(env, cls, "createWindow",
				"(Ljava/lang/String;Z)LjScope/CompositeWaveDisplay;");
  if (mid == 0) {
    printf("\nCannot find main\n");
    return -1;
  }
  if (name)
    jstr = (*env)->NewStringUTF(env, name);
  else
    jstr = (*env)->NewStringUTF(env, "");
  args[0].l = jstr;
  args[1].z = enableLiveUpdate;

//      jobjects[idx] = (*env)->CallStaticObjectMethod(env, cls, mid, jstr);
  if (idx == -1)		//Find the first free Object Slot
  {
    for (idx = 0; idx < MAX_WINDOWS; idx++)
      if (!jobjects[idx])
	break;
  }
  jobjects[idx] = (*env)->CallStaticObjectMethodA(env, cls, mid, args);
  return idx;
}

EXPORT int clearWindow(char *name, int idx)
{
  jclass cls;
  jmethodID mid;
  jstring jstr;

  if (env == 0) {
    printf("\nJava virtual machine not set!!\n");
    return -1;
  }
  cls = (*env)->FindClass(env, "jScope/CompositeWaveDisplay");
  if (cls == 0) {
    printf("\nCannot find CompositeWaveDisplay classes!\n");
    return -1;
  }
  if (jobjects[idx] != 0) {
    if (name)
      jstr = (*env)->NewStringUTF(env, name);
    else
      jstr = (*env)->NewStringUTF(env, "");
    mid = (*env)->GetMethodID(env, cls, "setTitle", "(Ljava/lang/String;)V");
    if (mid == 0) {
      printf("\nCannot find method setTitle in CompositeWaveDisplay!\n");
      return -1;
    }
    (*env)->CallVoidMethod(env, jobjects[idx], mid, jstr);
    mid = (*env)->GetMethodID(env, cls, "removeAllSignals", "()V");
    if (mid == 0) {
      printf("\nCannot find method removeAllSignals in CompositeWaveDisplay!\n");
      return -1;
    }
    (*env)->CallVoidMethod(env, jobjects[idx], mid);
  } else {
    printf("\nWindow %d not created!!\n", idx);
    return -1;
  }
  return 0;
}

EXPORT int addSignalWithParam(int obj_idx, float *x, float *y, int xType, int num_points, int row,
		       int column, char *colour, char *name, int inter, int marker)
{
  jstring jname, jcolour;
  jobject jobj = jobjects[obj_idx];
  jfloatArray jx, jy;
  jdoubleArray jxDouble = NULL;
  jclass cls;
  jmethodID mid;
  jboolean jinter;

  if (env == 0) {
    printf("\nJava virtual machine not set!!\n");
    return -1;
  }
  jinter = inter;
  jy = (*env)->NewFloatArray(env, num_points);
  (*env)->SetFloatArrayRegion(env, jy, 0, num_points, (jfloat *) y);
  if (xType == DTYPE_FLOAT) {
    jx = (*env)->NewFloatArray(env, num_points);
    (*env)->SetFloatArrayRegion(env, jx, 0, num_points, (jfloat *) x);
  } else {
    jxDouble = (*env)->NewDoubleArray(env, num_points);
    (*env)->SetDoubleArrayRegion(env, jxDouble, 0, num_points, (jdouble *) x);
  }
  if (name)
    jname = (*env)->NewStringUTF(env, name);
  else
    jname = (*env)->NewStringUTF(env, "");
  if (colour)
    jcolour = (*env)->NewStringUTF(env, colour);
  else
    jcolour = (*env)->NewStringUTF(env, "black");
  cls = (*env)->FindClass(env, "jScope/CompositeWaveDisplay");
  if (cls == 0) {
    printf("\nCannot find CompositeWaveDisplay classes!\n");
    return -1;
  }
  if (xType == DTYPE_FLOAT)
    mid = (*env)->GetMethodID(env, cls, "addSignal",
			      "([F[FIILjava/lang/String;Ljava/lang/String;ZI)V");
  else if (xType == DTYPE_DOUBLE)
    mid = (*env)->GetMethodID(env, cls, "addSignal",
			      "([D[FIILjava/lang/String;Ljava/lang/String;ZI)V");
  else {
    printf("\nUnsupported type for X array in CompositeWaveDisplay: %d!\n", xType);
    return -1;
  }

  if (mid == 0) {
    printf("\nCannot find method addSignal in CompositeWaveDisplay classes!\n");
    return -1;
  }
  (*env)->CallVoidMethod(env, jobj, mid, (xType == DTYPE_FLOAT) ? jx : jxDouble, jy, row, column,
			 jname, jcolour, jinter, marker);
  return 0;
}

EXPORT void addSignal(int obj_idx, float *x, float *y, int xType, int num_points, int row, int column,
	       char *colour, char *name)
{
  addSignalWithParam(obj_idx, x, y, xType, num_points, row, column, name, colour, 1, 0);
}

EXPORT int showWindow(int obj_idx, int x, int y, int width, int height)
{
  jclass cls;
  jmethodID mid;
  jobject jobj = jobjects[obj_idx];

  if (env == 0) {
    printf("\nJava virtual machine not set!!\n");
    return -1;
  }
  cls = (*env)->FindClass(env, "jScope/CompositeWaveDisplay");
  if (cls == 0) {
    printf("\nCannot find jScope classes!\n");
    return -1;
  }
  mid = (*env)->GetMethodID(env, cls, "showWindow", "(IIII)V");
  if (mid == 0) {
    printf("\nCannot find jScope classes!\n");
    return -1;
  }
  (*env)->CallVoidMethod(env, jobj, mid, x, y, width, height);
  return 0;
}

EXPORT int removeAllSignals(int obj_idx, int x, int y)
{
  jclass cls;
  jmethodID mid;
  jobject jobj = jobjects[obj_idx];

  if (env == 0) {
    printf("\nJava virtual machine not set!!\n");
    return -1;
  }
  cls = (*env)->FindClass(env, "jScope/CompositeWaveDisplay");
  if (cls == 0) {
    printf("\nCannot find jScope classes!\n");
    return -1;
  }
  mid = (*env)->GetMethodID(env, cls, "removeAllSignals", "(II)V");
  if (mid == 0) {
    printf("\nCannot find jScope classes!\n");
    return -1;
  }
  (*env)->CallVoidMethod(env, jobj, mid, x, y);
  return 0;
}

EXPORT void deviceSetup(char *deviceName, char *treeName, int shot, char *rootName, int x, int y)
{
  jint res;
  jclass cls;
  jmethodID mid;
  jstring jDeviceName, jTreeName, jRootName;
  jvalue args[6];
  char classpath[2048], *curr_classpath;

  JavaVM *jvm;
  JavaVMInitArgs vm_args;
  JavaVMOption options[3];

  if (env == 0) {		/* Java virtual machine does not exist yet */
    vm_args.version = JNI_VERSION_1_2;	//0x00010001;
    options[0].optionString = "-Djava.compiler=NONE";	/* disable JIT */
    options[1].optionString = "-Djava.class.path=.";	// /usr/local/mdsplus/java/classes/jTraverser.jar";           
    options[2].optionString = "-verbose:jni";	/* print JNI-related messages */

    vm_args.nOptions = 2;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    vm_args.options = options;
    curr_classpath = getenv("CLASSPATH");

    if (curr_classpath) {
      sprintf(classpath, "%s%c%s", options[1].optionString, PATH_SEPARATOR, curr_classpath);
      options[1].optionString = classpath;
    }

    res = DYN_JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);

    if (res < 0) {
      printf("\nCannot create Java VM (result = %d)!!\n", (int)res);
      return;
    }
  }
  cls = (*env)->FindClass(env, "DeviceSetup");
  if (cls == 0) {
    printf("\nCannot find DeviceSetup classe!");
    return;
  }
  mid = (*env)->GetStaticMethodID(env, cls, "activateDeviceSetup",
				  "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;II)V");
  if (mid == 0) {
    printf("\nCannot find method activateDeviceSetup\n");
    return;
  }
  args[0].l = jDeviceName = (*env)->NewStringUTF(env, deviceName);
  args[1].l = jTreeName = (*env)->NewStringUTF(env, treeName);
  args[2].i = shot;
  args[3].l = jRootName = (*env)->NewStringUTF(env, rootName);
  args[4].i = x;
  args[5].i = y;

  (*env)->CallStaticVoidMethodA(env, cls, mid, args);
}

//////////////////////////////////////////////////////
// MdsProtocol Plugin stuff
//////////////////////////////////////////////////////
/*
 * Class:     jScope_MdsIpProtocolWrapper
 * Method:    connectToMds
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jScope_MdsIpProtocolWrapper_connectToMds
(JNIEnv *env, jobject jobj __attribute__ ((unused)), jstring jurl)
{
    const char *url = (*env)->GetStringUTFChars(env, jurl, 0);
    int connectionId = ConnectToMds((char *)url);
   (*env)->ReleaseStringUTFChars(env, jurl, url);
    return connectionId;
}

/*
 * Class:     jScope_MdsIpProtocolWrapper
 * Method:    send
 * Signature: (I[BZ)I
 */
JNIEXPORT jint JNICALL Java_jScope_MdsIpProtocolWrapper_send
(JNIEnv *env, jobject jobj __attribute__ ((unused)), jint connectionId, jbyteArray jbuf, jboolean noWait)
{
    int size = (*env)->GetArrayLength(env, jbuf);
    char *buf = (char *)(*env)->GetByteArrayElements(env, jbuf, JNI_FALSE);
    return SendToConnection(connectionId, (const char *)buf, size, noWait);
}


/*
 * Class:     jScope_MdsIpProtocolWrapper
 * Method:    recv
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_jScope_MdsIpProtocolWrapper_recv
(JNIEnv *env, jobject jobj __attribute__ ((unused)), jint connectionId, jint size)
{
    jbyte *readBuf = malloc(size);
    int retSize = ReceiveFromConnection(connectionId, readBuf, size);
    if(retSize == -1)
    {
	free(readBuf);
	return 0;
    }
    jbyteArray jarr = (*env)->NewByteArray(env, retSize);
    (*env)->SetByteArrayRegion(env, jarr, 0, retSize, readBuf);
    free(readBuf);
    return jarr;
}

/*
 * Class:     jScope_MdsIpProtocolWrapper
 * Method:    flush
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jScope_MdsIpProtocolWrapper_flush
(JNIEnv *env __attribute__ ((unused)), jobject jobj __attribute__ ((unused)), jint connectionId)
{
    FlushConnection(connectionId);
}

/*
 * Class:     jScope_MdsIpProtocolWrapper
 * Method:    disconnect
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jScope_MdsIpProtocolWrapper_disconnect
(JNIEnv *env __attribute__ ((unused)), jobject jobj __attribute__ ((unused)), jint connectionId)
{
    DisconnectConnection(connectionId);
}

