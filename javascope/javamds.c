#include <stdio.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include "LocalProvider.h"

static char error_message[512];

static void MdsUpdate(char *exp, int shot)
{
    static int prev_shot;
    static char prev_exp[256];
    int status;
    
	
    if(shot == prev_shot && !strcmp(prev_exp, exp))
	return;

    prev_shot = shot;
    strcpy(prev_exp, exp);
    status =  TreeOpen(exp, shot, 0);
    if(!(status & 1))
	strncpy(error_message, MdsGetMsg(status), 512);
    else
	error_message[0] = 0;
} 


static char *MdsGetString(char *in)
{
    char *err, *out = NULL;
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
	case DTYPE_D :
	case DTYPE_G : ris = *(float *)xd.pointer->pointer; break; 
	default:	strcpy(error_message, "Not a supported type");
			return;
    }
    MdsFree1Dx(&xd, NULL);
    error_message[0] = 0;
    return ris;
}


static float *MdsGetFloatArray(char *in, int *out_dim)
{
    float *ris = NULL;
    int status, dim, i;
    struct descriptor in_d = {0,DTYPE_T,CLASS_S,0};
    EMPTYXD(xd);
    struct descriptor_a *arr_ptr;

    in_d.length = strlen(in);
    in_d.pointer = in;
    status = TdiCompile(&in_d, &xd MDS_END_ARG);
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
	strcpy(error_message, "Not an array");
	return 0;
    }
 
    arr_ptr = (struct descriptor_a *)xd.pointer;
    *out_dim = dim = arr_ptr->arsize/arr_ptr->length;
    ris = (float *)malloc(sizeof(float) * dim);
    switch(arr_ptr->dtype) {
	case DTYPE_BU:
	case DTYPE_B : 
		for(i = 0; i < dim; i++)
		    ris[i] = ((char *)arr_ptr->pointer)[i];
		break;
	case DTYPE_WU:
	case DTYPE_W : 
		for(i = 0; i < dim; i++)
		    ris[i] = ((short *)arr_ptr->pointer)[i];
		break;
	case DTYPE_LU:
	case DTYPE_L : 
		for(i = 0; i < dim; i++)
		    ris[i] = ((int *)arr_ptr->pointer)[i];
		break;
	case DTYPE_F : 
	case DTYPE_FS :
		for(i = 0; i < dim; i++)
		    ris[i] = ((float *)arr_ptr->pointer)[i];
		break;
	case DTYPE_D :
	case DTYPE_G : //Not really correct...
		memcpy(ris, arr_ptr->pointer, dim * sizeof(float));
		break;
	default:	strcpy(error_message, "Not a supported type");
			return NULL;
    }


    MdsFree1Dx(&xd, NULL);
    error_message[0] = 0;
    return ris;
}


JNIEXPORT void JNICALL Java_LocalProvider_Update(JNIEnv *env, jobject obj, jstring exp, jint shot)
{
    const char *exp_char = (*env)->GetStringUTFChars(env, exp, 0);
    MdsUpdate((char *)exp_char, shot);
    (*env)->ReleaseStringUTFChars(env, exp, exp_char);
} 

JNIEXPORT jstring JNICALL Java_LocalProvider_ErrorString(JNIEnv *env, jobject obj)
{

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


    out_ptr = MdsGetFloatArray((char *)in_char, &dim);

    (*env)->ReleaseStringUTFChars(env, in, in_char);

    if(error_message[0]) //Return a dummy vector without elements
    {
	return NULL;
    }
    jarr = (*env)->NewFloatArray(env, dim);
    (*env)->SetFloatArrayRegion(env, jarr, 0, dim, out_ptr);
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

	
 
