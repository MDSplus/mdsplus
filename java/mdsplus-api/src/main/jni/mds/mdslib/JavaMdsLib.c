#include <jni.h>
#include "JavaMdsLib.h"
#include <mdsdescrip.h> //descriptor*
#include <mds_stdarg.h> //MDS_END_ARG
#include <mdsshr.h> //MdsSerialize*
#include <stdlib.h> //free
#include <string.h> //strlen
#include <mdstypes.h> //DTYPE
#include <status.h> //DTYPE

extern int TdiIntrinsic();
extern int TdiEvaluate();
extern int MdsSerializeDscOut(mdsdsc_t const *in, mdsdsc_xd_t *out);
extern int MdsSerializeDscIn(char const *in, mdsdsc_xd_t *out);

static void RaiseException(JNIEnv * env, int status) {
  jclass cls = (*env)->FindClass(env, "mds/MdsException");
  jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(I)V");
  jobject exc = (*env)->NewObject(env, cls, constructor, status);
  (*env)->ExceptionClear(env);
  (*env)->Throw(env, exc);
}

int thisTdiExecute(int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  int status = 1;
  EMPTYXD(tmp);
  status = TdiIntrinsic(99, narg, list, &tmp);
  if STATUS_OK
    status = TdiEvaluate(tmp.pointer, out_ptr MDS_END_ARG);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

typedef struct {
  jsize      len;
  mdsdsc_t **list;
} cln_t;

static void cleanup_list(cln_t* const cln) {
  // free all but list[0]
  while(cln->len > 0) free(cln->list[cln->len--]);
}

JNIEXPORT jbyteArray JNICALL Java_mds_mdslib_MdsLib_evaluate(JNIEnv * const env, jobject obj __attribute__((unused)), const jobject jexpr, const jobjectArray args) {
  jbyteArray result;
  const jsize nargs = args ? (*env)->GetArrayLength(env,args) : 0;
  mdsdsc_t* list[nargs+1];
  cln_t cln = {0, list};
  pthread_cleanup_push((void*)cleanup_list,(void*)&cln);
  result = NULL;
  jbyteArray jarg;
  jsize i,jargdim;
  char* const expr = (char*)(*env)->GetStringUTFChars(env, jexpr, 0);
  list[0] = &(mdsdsc_t){strlen(expr), DTYPE_T, CLASS_S, expr};
  char* bytes = NULL;
  int status = 1;
  while(cln.len < nargs) {
    EMPTYXD(xd);
    jarg = (jbyteArray)((*env)->GetObjectArrayElement(env,args, cln.len));
    jargdim = (*env)->GetArrayLength(env,jarg);
    jbyte* jbytes = (*env)->GetByteArrayElements(env,jarg, 0);
    bytes = realloc(bytes,jargdim*sizeof(char));
    for (i = 0; i < jargdim; i++)
      bytes[i] = jbytes[i];
    status = MdsSerializeDscIn(bytes, &xd);
    (*env)->ReleaseByteArrayElements(env, jarg, jbytes, 0);
    if STATUS_NOT_OK break;
    list[++cln.len] = xd.pointer;
  }
  free(bytes);
  if STATUS_OK {
    EMPTYXD(xd);
    status = thisTdiExecute(nargs+1, list, &xd);
    (*env)->ReleaseStringUTFChars(env, jexpr, expr);
    if STATUS_OK {
      EMPTYXD(xds);
      status = MdsSerializeDscOut(xd.pointer, &xds);
      if STATUS_OK {
        mdsdsc_a_t* bytes_d = (mdsdsc_a_t*)xds.pointer;
        if (bytes_d) {
          int size = bytes_d->arsize;
          result = (*env)->NewByteArray(env, size);
          if (result) {
            char* const bytes = (char*)bytes_d->pointer;
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
  pthread_cleanup_pop(1);
  return result;
}
