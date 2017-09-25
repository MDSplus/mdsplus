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
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class MDSplus_Tree */

#ifndef _Included_MDSplus_Tree
#define _Included_MDSplus_Tree
#ifdef __cplusplus
extern "C" {
#endif
#undef MDSplus_Tree_DbiNAME
#define MDSplus_Tree_DbiNAME 1L
#undef MDSplus_Tree_DbiSHOTID
#define MDSplus_Tree_DbiSHOTID 2L
#undef MDSplus_Tree_DbiMODIFIED
#define MDSplus_Tree_DbiMODIFIED 3L
#undef MDSplus_Tree_DbiOPEN_FOR_EDIT
#define MDSplus_Tree_DbiOPEN_FOR_EDIT 4L
#undef MDSplus_Tree_DbiINDEX
#define MDSplus_Tree_DbiINDEX 5L
#undef MDSplus_Tree_DbiNUMBER_OPENED
#define MDSplus_Tree_DbiNUMBER_OPENED 6L
#undef MDSplus_Tree_DbiMAX_OPEN
#define MDSplus_Tree_DbiMAX_OPEN 7L
#undef MDSplus_Tree_DbiDEFAULT
#define MDSplus_Tree_DbiDEFAULT 8L
#undef MDSplus_Tree_DbiOPEN_READONLY
#define MDSplus_Tree_DbiOPEN_READONLY 9L
#undef MDSplus_Tree_DbiVERSIONS_IN_MODEL
#define MDSplus_Tree_DbiVERSIONS_IN_MODEL 10L
#undef MDSplus_Tree_DbiVERSIONS_IN_PULSE
#define MDSplus_Tree_DbiVERSIONS_IN_PULSE 11L
#undef MDSplus_Tree_TreeUSAGE_ANY
#define MDSplus_Tree_TreeUSAGE_ANY 0L
#undef MDSplus_Tree_TreeUSAGE_NONE
#define MDSplus_Tree_TreeUSAGE_NONE 1L
#undef MDSplus_Tree_TreeUSAGE_STRUCTURE
#define MDSplus_Tree_TreeUSAGE_STRUCTURE 1L
#undef MDSplus_Tree_TreeUSAGE_ACTION
#define MDSplus_Tree_TreeUSAGE_ACTION 2L
#undef MDSplus_Tree_TreeUSAGE_DEVICE
#define MDSplus_Tree_TreeUSAGE_DEVICE 3L
#undef MDSplus_Tree_TreeUSAGE_DISPATCH
#define MDSplus_Tree_TreeUSAGE_DISPATCH 4L
#undef MDSplus_Tree_TreeUSAGE_NUMERIC
#define MDSplus_Tree_TreeUSAGE_NUMERIC 5L
#undef MDSplus_Tree_TreeUSAGE_SIGNAL
#define MDSplus_Tree_TreeUSAGE_SIGNAL 6L
#undef MDSplus_Tree_TreeUSAGE_TASK
#define MDSplus_Tree_TreeUSAGE_TASK 7L
#undef MDSplus_Tree_TreeUSAGE_TEXT
#define MDSplus_Tree_TreeUSAGE_TEXT 8L
#undef MDSplus_Tree_TreeUSAGE_WINDOW
#define MDSplus_Tree_TreeUSAGE_WINDOW 9L
#undef MDSplus_Tree_TreeUSAGE_AXIS
#define MDSplus_Tree_TreeUSAGE_AXIS 10L
#undef MDSplus_Tree_TreeUSAGE_SUBTREE
#define MDSplus_Tree_TreeUSAGE_SUBTREE 11L
#undef MDSplus_Tree_TreeUSAGE_COMPOUND_DATA
#define MDSplus_Tree_TreeUSAGE_COMPOUND_DATA 1L
/*
 * Class:     MDSplus_Tree
 * Method:    getActiveTree
 * Signature: ()LMDSplus/Tree;
 */
  JNIEXPORT jobject JNICALL Java_MDSplus_Tree_getActiveTree(JNIEnv *, jclass);

/*
 * Class:     MDSplus_Tree
 * Method:    openTree
 * Signature: (Ljava/lang/String;IZ)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_openTree(JNIEnv *, jobject, jstring, jint, jboolean);

/*
 * Class:     MDSplus_Tree
 * Method:    closeTree
 * Signature: (IILjava/lang/String;I)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_closeTree(JNIEnv *, jobject, jint, jint, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    editTree
 * Signature: (Ljava/lang/String;IZ)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_editTree(JNIEnv *, jobject, jstring, jint, jboolean);

/*
 * Class:     MDSplus_Tree
 * Method:    writeTree
 * Signature: (IILjava/lang/String;I)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_writeTree(JNIEnv *, jclass, jint, jint, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    quitTree
 * Signature: (IILjava/lang/String;I)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_quitTree(JNIEnv *, jclass, jint, jint, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    findNode
 * Signature: (IILjava/lang/String;)I
 */
  JNIEXPORT jint JNICALL Java_MDSplus_Tree_findNode(JNIEnv *, jclass, jint, jint, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    switchDbid
 * Signature: (II)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_switchDbid(JNIEnv *, jclass, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    getWild
 * Signature: (IILjava/lang/String;I)[I
 */
  JNIEXPORT jintArray JNICALL Java_MDSplus_Tree_getWild
      (JNIEnv *, jclass, jint, jint, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    getDefaultNid
 * Signature: (II)I
 */
  JNIEXPORT jint JNICALL Java_MDSplus_Tree_getDefaultNid(JNIEnv *, jclass, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    setDefaultNid
 * Signature: (III)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_setDefaultNid(JNIEnv *, jclass, jint, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    getDbiFlag
 * Signature: (III)Z
 */
  JNIEXPORT jboolean JNICALL Java_MDSplus_Tree_getDbiFlag(JNIEnv *, jclass, jint, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    setDbiFlag
 * Signature: (IIZI)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_setDbiFlag(JNIEnv *, jclass, jint, jint, jboolean, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    setTreeViewDate
 * Signature: (IILjava/lang/String;)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_setTreeViewDate(JNIEnv *, jclass, jint, jint, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    setTreeTimeContext
 * Signature: (IILMDSplus/Data;LMDSplus/Data;LMDSplus/Data;)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_setTreeTimeContext
      (JNIEnv *, jclass, jint, jint, jobject, jobject, jobject);

/*
 * Class:     MDSplus_Tree
 * Method:    setCurrent
 * Signature: (Ljava/lang/String;I)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_setCurrent(JNIEnv *, jclass, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    getCurrent
 * Signature: (Ljava/lang/String;)I
 */
  JNIEXPORT jint JNICALL Java_MDSplus_Tree_getCurrent(JNIEnv *, jclass, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    createPulseFile
 * Signature: (III)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_createPulseFile(JNIEnv *, jclass, jint, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    deletePulseFile
 * Signature: (III)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_deletePulseFile(JNIEnv *, jclass, jint, jint, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    findTreeTags
 * Signature: (IILjava/lang/String;)[Ljava/lang/String;
 */
  JNIEXPORT jobjectArray JNICALL Java_MDSplus_Tree_findTreeTags
      (JNIEnv *, jclass, jint, jint, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    addTreeNode
 * Signature: (IILjava/lang/String;I)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_addTreeNode(JNIEnv *, jclass, jint, jint, jstring, jint);

/*
 * Class:     MDSplus_Tree
 * Method:    addTreeDevice
 * Signature: (IILjava/lang/String;Ljava/lang/String;)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_addTreeDevice
      (JNIEnv *, jclass, jint, jint, jstring, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    deleteTreeNode
 * Signature: (IILjava/lang/String;)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_deleteTreeNode(JNIEnv *, jclass, jint, jint, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    removeTreeTag
 * Signature: (IILjava/lang/String;)V
 */
  JNIEXPORT void JNICALL Java_MDSplus_Tree_removeTreeTag(JNIEnv *, jclass, jint, jint, jstring);

/*
 * Class:     MDSplus_Tree
 * Method:    getDatafileSize
 * Signature: (II)J
 */
  JNIEXPORT jlong JNICALL Java_MDSplus_Tree_getDatafileSize(JNIEnv *, jclass, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
