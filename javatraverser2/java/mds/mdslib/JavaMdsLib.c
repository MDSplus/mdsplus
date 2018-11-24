#include <jni.h>
#include "JavaMdsLib.h"
#include <mdsdescrip.h> //descriptor*
#include <mds_stdarg.h> //MDS_END_ARG
#include <mdsshr.h> //MdsSerialize*
#include <stdlib.h> //free
#include <string.h> //strlen
#include <mdstypes.h> //DTYPE
#include <tdishr.h> //TdiIntrinsic
#include <status.h> //STATUS_OK

static void RaiseException(JNIEnv * env, int status) {
  jclass cls = (*env)->FindClass(env, "mds/MdsException");
  jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(I)V");
  jobject exc = (*env)->NewObject(env, cls, constructor, status);
  (*env)->ExceptionClear(env);
  (*env)->Throw(env, exc);
}

JNIEXPORT jbyteArray JNICALL Java_mds_mdslib_MdsLib_evaluate(JNIEnv * env, jobject obj __attribute__((unused)), jobject jexpr, jobjectArray args) {
  int status = 1;
  jsize i,j,jargdim;
  jbyteArray jarg,result = NULL;
  jsize nargs = args ? (*env)->GetArrayLength(env,args) : 0;
  struct descriptor* list[nargs+1];
  char* const expr = (char*)(*env)->GetStringUTFChars(env, jexpr, 0);
  list[0] = &(struct descriptor){strlen(expr), DTYPE_T, CLASS_S, expr};
  for(i = 0; i < nargs && STATUS_OK ; ++i) {
    EMPTYXD(xd);
    jarg = (jbyteArray)((*env)->GetObjectArrayElement(env,args, i));
    jargdim = (*env)->GetArrayLength(env,jarg);
    jbyte* jbytes = (*env)->GetByteArrayElements(env,jarg, 0);
    //printf("1-%d\n",(int)i);
    char* bytes = malloc(jargdim*sizeof(char));
    //printf("2-%d\n",(int)i);
    for (j = 0; j < jargdim; j++)
      bytes[j] = jbytes[j];
    //printf("3-%d\n",(int)i);
    status = MdsSerializeDscIn(bytes, &xd);
    //printf("4-%d\n",(int)i);
    free(bytes);
    //printf("5-%d\n",(int)i);
    (*env)->ReleaseByteArrayElements(env, jarg, jbytes, 0);
    //printf("6-%d\n",(int)i);
    if (!(status & 1)) break;
    list[i+1] = xd.pointer;
  }
  if STATUS_OK {
    //printf("10-%d\n",(int)nargs);
    EMPTYXD(xd);
    status = TdiIntrinsic(OPC_EXECUTE, nargs+1, list, &xd);
    (*env)->ReleaseStringUTFChars(env, jexpr, expr);
    if STATUS_OK {
      EMPTYXD(xds);
      //printf("11\n");
      status = MdsSerializeDscOut(xd.pointer, &xds);
      if STATUS_OK {
        //printf("12\n");
        struct descriptor_a* bytes_d = (struct descriptor_a*)xds.pointer;
        if (bytes_d) {
          //printf("13\n");
          int size = bytes_d->arsize;
          result = (*env)->NewByteArray(env, size);
          if (result) {
            //printf("14+%d\n",(int)size);
            char* bytes = (char*)bytes_d->pointer;
            jbyte* jbytes = malloc(size*sizeof(jbyte));
            for (i = 0; i < size; i++)
              jbytes[i] = bytes[i];
            (*env)->SetByteArrayRegion(env, result, 0, size, jbytes);
            free(jbytes);
          }
        }
      }
      MdsFree1Dx(&xds, NULL);
    }
    MdsFree1Dx(&xd, NULL);
  }
  if STATUS_NOT_OK
    RaiseException(env, status);
  return result;
}
