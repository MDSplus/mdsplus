#include <stdio.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdstypes.h>
#include <string.h>
#include <stdlib.h>
#include "LocalDataProvider.h"
#include "MdsHelper.h"

extern int TdiCompile(), TdiData(), TdiFloat();

static char error_message[512];


#define BYTE 1
#define FLOAT 2
#define DOUBLE 3
#define LONG 4
#define QUADWORD 5
static void *MdsGetArray(char *in, int *out_dim, int type);


/*Support routine for MdsHelper */
JNIEXPORT jstring JNICALL Java_MdsHelper_getErrorString
  (JNIEnv *env, jclass cld, jint jstatus)
{
	char *error_msg = MdsGetMsg(jstatus);
	return (*env)->NewStringUTF(env, error_msg);
}

JNIEXPORT void JNICALL Java_MdsHelper_generateEvent
  (JNIEnv *env, jclass cld, jstring jevent, jint jshot)
{
	const char *event = (*env)->GetStringUTFChars(env, jevent, 0);
	int shot = jshot;

	if(strlen(event) > 0)
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
    if(!exp) return;
    if(shot == prev_shot && !strcmp(prev_exp, exp))
		return;
    prev_shot = shot;
    strcpy(prev_exp, exp);
    status =  TreeOpen(exp, shot, 0);
    if(!(status & 1))
		strncpy(error_message, MdsGetMsg(status), 512);
} 

static char *MdsGetString(char *in)
{
    char *out = NULL;
    int status;
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);

    in_d.length = strlen(in);
    in_d.pointer = in;
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
    if(status & 1)
		status = TdiData(&xd, &xd MDS_END_ARG);
    if(!(status & 1))
    {
    	strncpy(error_message, MdsGetMsg(status), 512);
		return NULL;
    }
    if(!xd.pointer)
    {
		strcpy(error_message, "Missing data");
		return NULL;
    }
    if(xd.pointer->dtype != DTYPE_T)
    { 
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
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);

    in_d.length = strlen(in);
    in_d.pointer = in;
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
    if(status & 1)
    	status = TdiData(&xd, &xd MDS_END_ARG);
    if(status & 1)
    	status = TdiFloat(&xd, &xd MDS_END_ARG);
    if(!(status & 1))
    {
    	strncpy(error_message, MdsGetMsg(status), 512);
	    return 0;
    }
    if(!xd.pointer)
    {
		strcpy(error_message, "Missing data");
		return 0;
    }
    if(xd.pointer->class != CLASS_S)
    {
		strcpy(error_message, "Not a scalar");
		return 0;
    }
    switch(xd.pointer->dtype) {
	case DTYPE_BU:
	case DTYPE_B : ris = (float)(*(char *)xd.pointer->pointer); break;
	case DTYPE_WU:
	case DTYPE_W : ris = (float)(*(short *)xd.pointer->pointer); break;
	case DTYPE_LU:
	case DTYPE_L : ris = (float)(*(int *)xd.pointer->pointer); break;
	case DTYPE_F : ris = *(float *)xd.pointer->pointer; break;
	case DTYPE_FS: ris = *(float *)xd.pointer->pointer; break;
	case DTYPE_D :
	case DTYPE_G : ris = *(float *)xd.pointer->pointer; break; 
	default:	sprintf(error_message, "Not a supported type %d", xd.pointer->dtype);
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
	_int64u *quad_ris = NULL;
    int status, dim, i;
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);
    struct descriptor_a *arr_ptr;
	char *expanded_in;

    error_message[0] = 0;
    *out_dim = 0;
	switch(type) {
		case FLOAT:
			expanded_in = malloc(strlen(in) + 16);
			sprintf(expanded_in, "fs_float((%s))", in);
			in_d.length = strlen(expanded_in);
			in_d.pointer = expanded_in;
			break;
		case DOUBLE:
			expanded_in = malloc(strlen(in) + 16);
			sprintf(expanded_in, "ft_float((%s))", in);
			in_d.length = strlen(expanded_in);
			in_d.pointer = expanded_in;
			break;
		case BYTE:
		case LONG:
			expanded_in = malloc(strlen(in) + 16);
			sprintf(expanded_in, "long((%s))", in);
			in_d.length = strlen(expanded_in);
			in_d.pointer = expanded_in;
			break;
		case QUADWORD:
			expanded_in = malloc(strlen(in) + 16);
			sprintf(expanded_in, "%s",in);
			in_d.length = strlen(expanded_in);
			in_d.pointer = expanded_in;
			break;
	}
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
	free(expanded_in);
    if(status & 1)
    	status = TdiData(&xd, &xd MDS_END_ARG);
    if(!(status & 1))
    {
    	strncpy(error_message, MdsGetMsg(status), 512);
		return 0;
    }
    if(!xd.pointer)
    {
		strcpy(error_message, "Missing data");
		return 0;
    }
    if(xd.pointer->class != CLASS_A)
    {
		if(type != FLOAT && type != DOUBLE) /*Legal only if used to retrieve the shot number*/
		{
			int_ris = malloc(sizeof(int));
			switch(xd.pointer->dtype)
			{
				case DTYPE_BU:
				case DTYPE_B : int_ris[0] = *((char *)xd.pointer->pointer); break;
				case DTYPE_WU:
				case DTYPE_W : int_ris[0] = *((short *)xd.pointer->pointer); break;
				case DTYPE_LU:
				case DTYPE_L : int_ris[0] = *((int *)xd.pointer->pointer); break;
				case DTYPE_F : 
				case DTYPE_FS : int_ris[0] = *((int *)xd.pointer->pointer); break;
			}
			*out_dim = 1;
			return int_ris;
		}
		strcpy(error_message, "Not an array");
		return 0;
	}
 	arr_ptr = (struct descriptor_a *)xd.pointer;
	*out_dim = dim = arr_ptr->arsize/arr_ptr->length;
	switch(type) {
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
			quad_ris = (_int64 *)malloc(8 * dim);
			break;

	}
	switch(arr_ptr->dtype) {
	case DTYPE_BU:
	case DTYPE_B : 
		for(i = 0; i < dim; i++)
		{
			switch(type) {
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
	case DTYPE_W : 
		for(i = 0; i < dim; i++)
		{
			switch(type) {
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
	case DTYPE_L : 
		for(i = 0; i < dim; i++)
		{
			switch(type) {
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
	case DTYPE_F : 
	case DTYPE_FS :
		for(i = 0; i < dim; i++)
		{
			switch(type) {
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
	case DTYPE_FT :
		for(i = 0; i < dim; i++)
		{
			switch(type) {
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
	case DTYPE_Q :
	case DTYPE_QU:
		for(i = 0; i < dim; i++)
		{
			switch(type) {
				case FLOAT: 
					float_ris[i] = ((_int64 *)arr_ptr->pointer)[i];
					break;
				case DOUBLE:
					double_ris[i] = ((_int64 *)arr_ptr->pointer)[i];
					break;
				case BYTE: 
		    		byte_ris[i] = ((_int64u *)arr_ptr->pointer)[i];
					break;
				case LONG:
		    	    int_ris[i] = ((_int64u *)arr_ptr->pointer)[i];
					break;
				case QUADWORD:
		    	    quad_ris[i] = ((_int64u *)arr_ptr->pointer)[i];
					break;
			}
		}
		break;

	default:	strcpy(error_message, "Not a supported type");
			return NULL;
    }
    MdsFree1Dx(&xd, NULL);
    error_message[0] = 0;
	switch(type) {
		case FLOAT: return float_ris;
		case DOUBLE: return double_ris;
		case BYTE: return byte_ris;
		case LONG: return int_ris;
		case QUADWORD: return quad_ris;
	}
	return NULL;
}


/* Implementation of the native methods for LocalProvider class in jScope */
JNIEXPORT void JNICALL Java_LocalDataProvider_UpdateNative(JNIEnv *env, jobject obj, jstring exp, jlong shot)
{
    const char *exp_char;
    error_message[0] = 0;
    if(exp == NULL) return;
    exp_char = (*env)->GetStringUTFChars(env, exp, 0);
    MdsUpdate((char *)exp_char, (int)shot);
    (*env)->ReleaseStringUTFChars(env, exp, exp_char);
} 

JNIEXPORT jstring JNICALL Java_LocalDataProvider_ErrorString(JNIEnv *env, jobject obj)
{
    if(!error_message[0])
		return NULL;
    return (*env)->NewStringUTF(env, error_message);
}  

JNIEXPORT jstring JNICALL Java_LocalDataProvider_GetString(JNIEnv *env, jobject obj, jstring in)
{
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    char *out_char = MdsGetString((char *)in_char);
	
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(!out_char)
		return (*env)->NewStringUTF(env, "");
    else
    	return (*env)->NewStringUTF(env, out_char);
}

JNIEXPORT jfloatArray JNICALL Java_LocalDataProvider_GetFloatArrayNative(JNIEnv *env, jobject obj, jstring in)
{
    jfloatArray jarr;
    float zero = 0.;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    int dim;
    float *out_ptr;

	out_ptr = MdsGetArray((char *)in_char, &dim, FLOAT);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(error_message[0]) /*Return a dummy vector without elements*/
    {
		return NULL;
    }
    jarr = (*env)->NewFloatArray(env, dim);
    (*env)->SetFloatArrayRegion(env, jarr, 0, dim, out_ptr);
	free((char *)out_ptr);
    return jarr;
}

JNIEXPORT jdoubleArray JNICALL Java_LocalDataProvider_GetDoubleArrayNative(JNIEnv *env, jobject obj, jstring in)
{
    jdoubleArray jarr;
    float zero = 0.;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    int dim;
    double *out_ptr;
    
	out_ptr = MdsGetArray((char *)in_char, &dim, DOUBLE);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(error_message[0]) /*Return a dummy vector without elements*/
    {
		return NULL;
    }
    jarr = (*env)->NewDoubleArray(env, dim);
    (*env)->SetDoubleArrayRegion(env, jarr, 0, dim, out_ptr);
	free((char *)out_ptr);
    return jarr;
}

JNIEXPORT jdoubleArray JNICALL Java_LocalDataProvider_GetLongArrayNative(JNIEnv *env, jobject obj, jstring in)
{
    jlongArray jarr;
    float zero = 0.;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    int dim;
    _int64u *out_ptr;
    
	out_ptr = MdsGetArray((char *)in_char, &dim, QUADWORD);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(error_message[0] || !out_ptr) /*Return a dummy vector without elements*/
    {
		return NULL;
    }
    jarr = (*env)->NewLongArray(env, dim);
    (*env)->SetLongArrayRegion(env, jarr, 0, dim, out_ptr);
	free((char *)out_ptr);
    return jarr;
}

JNIEXPORT jintArray JNICALL Java_LocalDataProvider_GetIntArray(JNIEnv *env, jobject obj, jstring in)
{
    jintArray jarr;
    float zero = 0.;
    const char *in_char;
    int dim;
    int *out_ptr;



   in_char = (*env)->GetStringUTFChars(env, in, 0);

   out_ptr = MdsGetArray((char *)in_char, &dim, LONG);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(error_message[0]) /*Return a dummy vector without elements*/
    {
		return NULL;
    }
    jarr = (*env)->NewIntArray(env, dim);
    (*env)->SetIntArrayRegion(env, jarr, 0, dim, out_ptr);
	
	free((char *)out_ptr);
   return jarr;
}
JNIEXPORT jbyteArray JNICALL Java_LocalDataProvider_GetByteArray(JNIEnv *env, jobject obj, jstring in)
{
    jbyteArray jarr;
    float zero = 0.;
    const char *in_char;
    int dim;
    int *out_ptr;

	in_char = (*env)->GetStringUTFChars(env, in, 0);
 	out_ptr = MdsGetArray((char *)in_char, &dim, BYTE);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(error_message[0]) /*Return a dummy vector without elements*/
    {
	return NULL;
    }
    jarr = (*env)->NewByteArray(env, dim);
    (*env)->SetByteArrayRegion(env, jarr, 0, dim, (char *)out_ptr);
	free((char *)out_ptr);
    return jarr;
}


JNIEXPORT jfloat JNICALL Java_LocalDataProvider_GetFloatNative(JNIEnv *env, jobject obj, jstring in)
{
    float ris;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);

    ris = MdsGetFloat((char *)in_char);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    return ris;
}

JNIEXPORT void JNICALL Java_LocalDataProvider_SetEnvironmentSpecific(JNIEnv *env, jobject obj, 
																	 jstring in, jstring jdefNode)
{
	int status, nid;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
	const char *defNode;
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);
	error_message[0] = 0;
	if(in_char && *in_char)
	{
		in_d.length = strlen(in_char);
		in_d.pointer = (char *)in_char;
		status = TdiCompile(&in_d, &xd MDS_END_ARG);
		if(status & 1)
    		status = TdiData(&xd, &xd MDS_END_ARG);
		if(!(status & 1))
   			strncpy(error_message, MdsGetMsg(status), 512);
		MdsFree1Dx(&xd, NULL);
		(*env)->ReleaseStringUTFChars(env, in, in_char);
    }
	if(jdefNode)
	{
		defNode = (*env)->GetStringUTFChars(env, jdefNode, 0);
		status = TreeFindNode((char *)defNode, &nid);
		if(status & 1)
			TreeSetDefaultNid(nid);
		(*env)->ReleaseStringUTFChars(env, jdefNode, defNode);
	}

}





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


static void handleEvent(void *nameIdx, int dim ,char *buf)
{
	jmethodID mid;
	jvalue args[1];
	JNIEnv *env;
	jclass cls;

	env = getJNIEnv();
	cls = (*env)->GetObjectClass(env, localDataProviderInstance);
	if(!cls) printf("Error getting class for LocalDataProvider\n");
	mid = (*env)->GetMethodID(env, cls,  "fireEvent", "(I)V");
	if(!mid) printf("Error getting method fireEvent for LoalDataProvider\n");
	args[0].i = (char *)nameIdx-(char *)0;
	(*env)->CallVoidMethodA(env, localDataProviderInstance, mid, args);
	releaseJNIEnv();
}



JNIEXPORT jint JNICALL Java_LocalDataProvider_registerEvent
  (JNIEnv *env, jobject obj, jstring jevent , jint idx)

{
	int evId, status;
	const char *event;

	if(jvm == 0)
	{
		status = (*env)->GetJavaVM(env, &jvm);
		if (status) 
			printf("GetJavaVM error %d\n", status);
	}
	localDataProviderInstance = (*env)->NewGlobalRef(env, obj);
 	event = (*env)->GetStringUTFChars(env, jevent, 0);
	status = MDSEventAst((char *)event, handleEvent, idx+(char *)0, &evId);
	if(!(status & 1))
	{
		printf("Error calling MDSEventAst");
		evId = 0;
	}
	(*env)->ReleaseStringUTFChars(env, jevent, event);
	return evId;
}
	

JNIEXPORT void JNICALL Java_LocalDataProvider_unregisterEvent
  (JNIEnv *env, jobject obj, jint evId)
{

	MDSEventCan(evId);
}



/* CompositeWaveDisplay management routines for using
jScope panels outside java application */
#ifdef HAVE_WINDOWS_H
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

JNIEnv *env = 0;
static jobject jobjects[1024];

int createWindow(char *name, int idx, int enableLiveUpdate)
{
	jint res;
	jclass cls;
	jmethodID mid;
	jstring jstr;
	char classpath[2048], *curr_classpath;
	jvalue args[2];

	JavaVM *jvm;
	JavaVMInitArgs vm_args;
	JavaVMOption   options[3];

	if(env == 0) /* Java virtual machine does not exist yet */
	{
	        vm_args.version = JNI_VERSION_1_2;//0x00010001;
		options[0].optionString = "-Djava.compiler=NONE";           /* disable JIT */
		options[1].optionString = "-Djava.class.path=/usr/local/mdsplus/java/classes/jTraverser.jar";            /* user classes */
		options[2].optionString = "-verbose:jni";                   /* print JNI-related messages */

		vm_args.nOptions = 2;
		vm_args.ignoreUnrecognized = JNI_FALSE;
		vm_args.options = options;
		curr_classpath = getenv("CLASSPATH");

		if(curr_classpath)
		{
			sprintf(classpath, "%s%c%s",options[1].optionString , PATH_SEPARATOR, curr_classpath);
			options[1].optionString = classpath;
		}





		res = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);

		if(res < 0)
		{
			printf("\nCannot create Java VM (result = %d)!!\n", res);
			return -1;
		}
	}
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find CompositeWaveDisplay classes!\n");
		return -1;
	}	
	mid = (*env)->GetStaticMethodID(env, cls, "createWindow", "(Ljava/lang/String;Z)LCompositeWaveDisplay;");
	if(mid == 0)
	{
		printf("\nCannot find main\n");
		return -1;
	}
	if(name)
		jstr = (*env)->NewStringUTF(env, name);
	else
		jstr = (*env)->NewStringUTF(env, "");
	args[0].l = jstr;
	args[1].z = enableLiveUpdate;

//	jobjects[idx] = (*env)->CallStaticObjectMethod(env, cls, mid, jstr);
	jobjects[idx] = (*env)->CallStaticObjectMethodA(env, cls, mid, args);
	return 0;
}

int clearWindow(char *name, int idx)
{
	jclass cls;
	jmethodID mid;
	jstring jstr;

	if(env == 0)
	{
		printf("\nJava virtual machine not set!!\n");
		return -1;
	}
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find CompositeWaveDisplay classes!\n");
		return -1;
	}	
	if(jobjects[idx] != 0)
	{
		if(name)
			jstr = (*env)->NewStringUTF(env, name);
		else
			jstr = (*env)->NewStringUTF(env, "");
		mid = (*env)->GetMethodID(env, cls, "setTitle","(Ljava/lang/String;)V");
		if (mid == 0)
		{
			printf("\nCannot find method setTitle in CompositeWaveDisplay!\n");
			return -1;
		}
		(*env)->CallVoidMethod(env, jobjects[idx], mid, jstr);
		mid = (*env)->GetMethodID(env, cls, "removeAllSignals","()V");
		if (mid == 0)
		{
			printf("\nCannot find method removeAllSignals in CompositeWaveDisplay!\n");
			return -1;
		}
		(*env)->CallVoidMethod(env, jobjects[idx], mid);
	}
	else
	{
		printf("\nWindow %d not created!!\n", idx);
		return -1;
	}
}

int addSignalWithParam(int obj_idx, float *x, float *y, int num_points, int row, int column,
			   char *colour, char *name, int inter, int marker)
{
	jstring jname, jcolour;
	jobject jobj = jobjects[obj_idx];
	jfloatArray jx, jy;
	jclass cls;
	jmethodID mid;
	jboolean jinter;

	if(env == 0)
	{
		printf("\nJava virtual machine not set!!\n");
		return -1;
	}
	jinter = inter;
	jx = (*env)->NewFloatArray(env,  num_points);
	jy = (*env)->NewFloatArray(env,  num_points);
	(*env)->SetFloatArrayRegion(env,  jx, 0, num_points, (jfloat *)x);
	(*env)->SetFloatArrayRegion(env,  jy, 0, num_points, (jfloat *)y);
	if(name)
		jname = (*env)->NewStringUTF(env,  name);
	else
		jname = (*env)->NewStringUTF(env,  "");
	if(colour)
		jcolour = (*env)->NewStringUTF(env,  colour);
	else
		jcolour = (*env)->NewStringUTF(env,  "black");
	cls = (*env)->FindClass(env,  "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find CompositeWaveDisplay classes!\n");
		return -1;
	}
	mid = (*env)->GetMethodID(env,  cls, "addSignal",
		"([F[FIILjava/lang/String;Ljava/lang/String;ZI)V");
    if (mid == 0)
	{
		printf("\nCannot find method addSignal in CompositeWaveDisplay classes!\n");
		return -1;
	}

    (*env)->CallVoidMethod(env, jobj, mid, jx, jy, row, column, jname, jcolour, jinter, marker);
    return 0;
}
void addSignal(int obj_idx, float *x, float *y, int num_points, int row, int column,
			   char *colour, char *name)
{
   addSignalWithParam(obj_idx, x, y, num_points, row, column, name, colour, 1, 0);
}

int showWindow(int obj_idx, int x, int y, int width, int height)
{
	jclass cls;
	jmethodID mid;
	jobject jobj = jobjects[obj_idx];

	if(env == 0)
	{
		printf("\nJava virtual machine not set!!\n");
		return -1;
	}
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find jScope classes!\n");
		return -1;
	}
	mid = (*env)->GetMethodID(env, cls, "showWindow", "(IIII)V");
    if (mid == 0)
	{
		printf("\nCannot find jScope classes!\n");
		return -1;
	}
    (*env)->CallVoidMethod(env, jobj, mid, x,y,width,height);
    return 0;
}



void deviceSetup(char *deviceName, char *treeName, int shot, char *rootName, int x, int y)
{
	jint res;
	jclass cls;
	jmethodID mid;
	jstring jDeviceName, jTreeName, jRootName;
	jvalue args[6];
	char classpath[2048], *curr_classpath;

	JavaVM *jvm;
	JavaVMInitArgs vm_args;
	JavaVMOption   options[3];

	if(env == 0) /* Java virtual machine does not exist yet */
	{
	        vm_args.version = JNI_VERSION_1_2;//0x00010001;
		options[0].optionString = "-Djava.compiler=NONE";           /* disable JIT */
		options[1].optionString = "-Djava.class.path=.";// /usr/local/mdsplus/java/classes/jTraverser.jar";           
		options[2].optionString = "-verbose:jni";                   /* print JNI-related messages */

		vm_args.nOptions = 2;
		vm_args.ignoreUnrecognized = JNI_FALSE;
		vm_args.options = options;
		curr_classpath = getenv("CLASSPATH");

		if(curr_classpath)
		{
			sprintf(classpath, "%s%c%s",options[1].optionString , PATH_SEPARATOR, curr_classpath);
			options[1].optionString = classpath;
		}





		res = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);

		if(res < 0)
		{
			printf("\nCannot create Java VM (result = %d)!!\n", res);
			return ;
		}
	}
	cls = (*env)->FindClass(env, "DeviceSetup");
	if(cls == 0)
	{
		printf("\nCannot find DeviceSetup classe!");
		return;
	}
	mid = (*env)->GetStaticMethodID(env, cls, "activateDeviceSetup", 
		"(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;II)V");
	if(mid == 0)
	{
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
