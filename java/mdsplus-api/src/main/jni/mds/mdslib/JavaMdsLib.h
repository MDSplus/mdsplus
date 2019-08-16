#include <jni.h>
/* Header for class mds_mdslib_MdsLib */

#ifndef _Included_mds_mdslib_MdsLib
#define _Included_mds_mdslib_MdsLib
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     mds_mdslib_MdsLib
 * Method:    evaluate
 * Signature: (Ljava/lang/String;[[byte;)[byte;
 */
JNIEXPORT jbyteArray JNICALL Java_mds_mdslib_MdsLib_evaluate(JNIEnv * env, jobject obj, jstring expr, jobjectArray args);

#ifdef __cplusplus
}
#endif
#endif
