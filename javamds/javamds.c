#include <stdio.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "LocalProvider.h"
extern int TdiCompile(), TdiData();
static char error_message[512];

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
	case DTYPE_B : ris = *(char *)xd.pointer->pointer; break;
	case DTYPE_WU:
	case DTYPE_W : ris = *(short *)xd.pointer->pointer; break;
	case DTYPE_LU:
	case DTYPE_L : ris = *(int *)xd.pointer->pointer; break;
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


static void *MdsGetArray(char *in, int *out_dim, int is_float, int is_byte)
{
    float *float_ris = NULL;
    int *int_ris = NULL;
	char *byte_ris = NULL;
    int status, dim, i;
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);
    struct descriptor_a *arr_ptr;
	char *expanded_in;


    *out_dim = 0;
	if(is_float)
	{
		expanded_in = malloc(strlen(in) + 16);
		sprintf(expanded_in, "fs_float((%s))", in);
		in_d.length = strlen(expanded_in);
		in_d.pointer = expanded_in;
	}
	else
	{
		in_d.length = strlen(in);
		in_d.pointer = in;
	}
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
	if(is_float) free(expanded_in);
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
		if(!is_float) //Legal only if used to retrieve the shot number
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
	if(arr_ptr->dtype == DTYPE_F || 
		arr_ptr->dtype == DTYPE_D ||
		arr_ptr->dtype == DTYPE_G ||
		arr_ptr->dtype == DTYPE_H) 
	{
		status = TdiFloat(&xd, &xd MDS_END_ARG);
		arr_ptr = (struct descriptor_a *)xd.pointer;
	}
		
		*out_dim = dim = arr_ptr->arsize/arr_ptr->length;
    if(is_float)
        float_ris = (float *)malloc(sizeof(float) * dim);
    else if(is_byte)
		byte_ris = malloc(dim);
	else
        int_ris = (int *)malloc(sizeof(int) * dim);
    switch(arr_ptr->dtype) {
	case DTYPE_BU:
	case DTYPE_B : 
		for(i = 0; i < dim; i++)
		    if(is_float)
		    	float_ris[i] = ((char *)arr_ptr->pointer)[i];
		    else if(is_byte)
		    	byte_ris[i] = ((char *)arr_ptr->pointer)[i];
			else
		    	int_ris[i] = ((char *)arr_ptr->pointer)[i];
		break;
	case DTYPE_WU:
	case DTYPE_W : 
		for(i = 0; i < dim; i++)
		    if(is_float)
		    	float_ris[i] = ((short *)arr_ptr->pointer)[i];
		    else if(is_byte)
		    	byte_ris[i] = ((short *)arr_ptr->pointer)[i];
			else
				int_ris[i] = ((short *)arr_ptr->pointer)[i];
		break;
	case DTYPE_LU:
	case DTYPE_L : 
		for(i = 0; i < dim; i++)
		    if(is_float)
		    	float_ris[i] = ((int *)arr_ptr->pointer)[i];
		    else if(is_byte)
		    	byte_ris[i] = ((int *)arr_ptr->pointer)[i];
			else
		    	int_ris[i] = ((int *)arr_ptr->pointer)[i];
		break;
	case DTYPE_F : 
	case DTYPE_FS :
		for(i = 0; i < dim; i++)
		    if(is_float)
		    	float_ris[i] = ((float *)arr_ptr->pointer)[i];
		    else if(is_byte)
		    	byte_ris[i] = ((float *)arr_ptr->pointer)[i];
			else
		    	int_ris[i] = ((float *)arr_ptr->pointer)[i];
		break;
	case DTYPE_D :
	case DTYPE_G :
	default:	strcpy(error_message, "Not a supported type");
			return NULL;
    }


    MdsFree1Dx(&xd, NULL);
    error_message[0] = 0;
    if(is_float)
    	return float_ris;
    else if(is_byte) 
		return byte_ris;
	return int_ris;
}


JNIEXPORT void JNICALL Java_LocalProvider_Update(JNIEnv *env, jobject obj, jstring exp, jint shot)
{
    const char *exp_char;
    error_message[0] = 0;
    if(exp == NULL) return;
    exp_char = (*env)->GetStringUTFChars(env, exp, 0);
    MdsUpdate((char *)exp_char, shot);
    (*env)->ReleaseStringUTFChars(env, exp, exp_char);
} 

JNIEXPORT jstring JNICALL Java_LocalProvider_ErrorString(JNIEnv *env, jobject obj)
{

    if(!error_message[0])
	return NULL;
    return (*env)->NewStringUTF(env, error_message);
}  

JNIEXPORT jstring JNICALL Java_LocalProvider_GetString(JNIEnv *env, jobject obj, jstring in)
{
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    char *out_char = MdsGetString((char *)in_char);
	
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    if(!out_char)
	return (*env)->NewStringUTF(env, "");
    else
    	return (*env)->NewStringUTF(env, out_char);
}



JNIEXPORT jfloatArray JNICALL Java_LocalProvider_GetFloatArray(JNIEnv *env, jobject obj, jstring in)
{
    jfloatArray jarr;
    float zero = 0.;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    int dim;
    float *out_ptr;


    out_ptr = MdsGetArray((char *)in_char, &dim, 1, 0);

    (*env)->ReleaseStringUTFChars(env, in, in_char);

    if(error_message[0]) //Return a dummy vector without elements
    {
	return NULL;
    }
    jarr = (*env)->NewFloatArray(env, dim);
    (*env)->SetFloatArrayRegion(env, jarr, 0, dim, out_ptr);
    return jarr;
}

JNIEXPORT jintArray JNICALL Java_LocalProvider_GetIntArray(JNIEnv *env, jobject obj, jstring in)
{
    jintArray jarr;
    float zero = 0.;
    const char *in_char;
    int dim;
    int *out_ptr;

    in_char = (*env)->GetStringUTFChars(env, in, 0);
    out_ptr = MdsGetArray((char *)in_char, &dim, 0, 0);
 
    (*env)->ReleaseStringUTFChars(env, in, in_char);

    if(error_message[0]) //Return a dummy vector without elements
    {
	return NULL;
    }
    jarr = (*env)->NewIntArray(env, dim);
    (*env)->SetIntArrayRegion(env, jarr, 0, dim, out_ptr);
    return jarr;
}

JNIEXPORT jbyteArray JNICALL Java_LocalProvider_GetByteArray(JNIEnv *env, jobject obj, jstring in)
{
    jbyteArray jarr;
    float zero = 0.;
    const char *in_char;
    int dim;
    int *out_ptr;

	
	in_char = (*env)->GetStringUTFChars(env, in, 0);

 	out_ptr = MdsGetArray((char *)in_char, &dim, 0, 1);
 
    (*env)->ReleaseStringUTFChars(env, in, in_char);

    if(error_message[0]) //Return a dummy vector without elements
    {
	return NULL;
    }
    jarr = (*env)->NewByteArray(env, dim);
    (*env)->SetByteArrayRegion(env, jarr, 0, dim, (char *)out_ptr);
    return jarr;
}


JNIEXPORT jfloat JNICALL Java_LocalProvider_GetFloat(JNIEnv *env, jobject obj, jstring in)
{
    float ris;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);

    ris = MdsGetFloat((char *)in_char);
    (*env)->ReleaseStringUTFChars(env, in, in_char);
    return ris;
}


JNIEXPORT void JNICALL Java_LocalProvider_SetEnvironment(JNIEnv *env, jobject obj, jstring in)
{
	int status;
    const char *in_char = (*env)->GetStringUTFChars(env, in, 0);
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);
	error_message[0] = 0;
	if(in_char && *in_char)
	{
		in_d.length = strlen(in_char);
		in_d.pointer = in_char;
		status = TdiCompile(&in_d, &xd MDS_END_ARG);
		if(status & 1)
    		status = TdiData(&xd, &xd MDS_END_ARG);
		if(!(status & 1))
   			strncpy(error_message, MdsGetMsg(status), 512);
		MdsFree1Dx(&xd, NULL);
		(*env)->ReleaseStringUTFChars(env, in, in_char);
    }
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

void createWindow(char *name, int idx)
{
	JavaVM *jvm;
	JDK1_1InitArgs vm_args;
	jint res;
	jclass cls;
	jmethodID mid;
	jstring jstr;
	char classpath[2048], *curr_classpath;
	jobject jobj;

	if(env == 0) /* Java virtual machine does not exist yet */
	{
		vm_args.version = 0x00010001;
		JNI_GetDefaultJavaVMInitArgs(&vm_args);
		curr_classpath = getenv("CLASSPATH");
		if(curr_classpath)
		{
			sprintf(classpath, "%s%c%s", vm_args.classpath, PATH_SEPARATOR, curr_classpath);
			vm_args.classpath = classpath;
		}
		res = JNI_CreateJavaVM(&jvm, &env, &vm_args);
		if(res < 0)
		{
			printf("\nCannot create Java VM!!\n");
			return ;
		}
	}
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find jScope classes!");
		return;
	}
	mid = (*env)->GetStaticMethodID(env, cls, "createWindow", "(Ljava/lang/String;)LCompositeWaveDisplay;");
	if(mid == 0)
	{
		printf("\nCannot find main\n");
		return;
	}
	if(name)
		jstr = (*env)->NewStringUTF(env, name);
	else
		jstr = (*env)->NewStringUTF(env, "");

	jobjects[idx] = (*env)->CallStaticObjectMethod(env, cls, mid, jstr);
}


void addSignal(int obj_idx, float *x, float *y, int num_points, int row, int column, 
			   char *name, char *colour)
{
	jstring jname, jcolour;
	jobject jobj = jobjects[obj_idx];
	jfloatArray jx, jy;
	jclass cls;
	jmethodID mid;

	if(env == 0)
	{
		printf("\nJava virtual machine not set!!\n");
		return;
	}

	jx = (*env)->NewFloatArray(env, num_points);
	jy = (*env)->NewFloatArray(env, num_points);
	(*env)->SetFloatArrayRegion(env, jx, 0, num_points, (jfloat *)x);
	(*env)->SetFloatArrayRegion(env, jy, 0, num_points, (jfloat *)y);
	if(name)
		jname = (*env)->NewStringUTF(env, name);
	else
		jname = (*env)->NewStringUTF(env, "");
	if(colour)
		jcolour = (*env)->NewStringUTF(env, colour);
	else
		jcolour = (*env)->NewStringUTF(env, "black");
		
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find jScope classes!");
		return;
	}
	mid = (*env)->GetMethodID(env, cls, "addSignal", 
		"([F[FIILjava/lang/String;Ljava/lang/String;)V");
    if (mid == 0) 
	{
		printf("\nCannot find jScope classes!");
		return;
	}


    (*env)->CallVoidMethod(env, jobj, mid, jx, jy, row, column, jname, jcolour);
}



void showWindow(int obj_idx, int x, int y, int width, int height)
{
	jclass cls;
	jmethodID mid;
	jobject jobj = jobjects[obj_idx];

	if(env == 0)
	{
		printf("\nJava virtual machine not set!!\n");
		return;
	}
	cls = (*env)->FindClass(env, "CompositeWaveDisplay");
	if(cls == 0)
	{
		printf("\nCannot find jScope classes!");
		return;
	}
	mid = (*env)->GetMethodID(env, cls, "showWindow", "(IIII)V");
    if (mid == 0) 
	{
		printf("\nCannot find jScope classes!");
		return;
	}


    (*env)->CallVoidMethod(env, jobj, mid, x,y,width,height);
}




	
 
