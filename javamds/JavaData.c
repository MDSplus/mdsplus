#include <jni.h>
#include "Data.h"
#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdstypes.h>
#include  <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <libroutines.h>

extern int TdiDecompile(), TdiCompile(), TdiFloat(), TdiData(), TdiLong(), CvtConvertFloat();

struct descriptor * ObjectToDescrip(JNIEnv *env, jobject obj);
void FreeDescrip(struct descriptor *desc);
jobject DescripToObject(JNIEnv *env, struct descriptor *desc);



JNIEXPORT jstring JNICALL Java_Data_toString(JNIEnv *env, jobject obj)
{
  EMPTYXD(out_xd);
  jstring ris;
  struct descriptor *dec_d;
  char *buf;
  int status;
  
  struct descriptor *data_d = ObjectToDescrip(env, obj);

  status = TdiDecompile(data_d, &out_xd MDS_END_ARG);
  if(!(status & 1)) 
    {
      printf("Error decompiling expression: %s\n", MdsGetMsg(status));
      return NULL;
    }
  FreeDescrip(data_d);
  dec_d = out_xd.pointer;

  buf = (char *)malloc(dec_d->length + 1);
  memcpy(buf, dec_d->pointer, dec_d->length);
  buf[dec_d->length] = 0;
  MdsFree1Dx(&out_xd, NULL);
  ris = (*env)->NewStringUTF(env, buf);
  free((char *)buf);
  return ris;
}

JNIEXPORT jint JNICALL Java_Data_evaluate
  (JNIEnv *env, jclass cls, jstring jexpr)
{
	EMPTYXD(xd);
	int status, ris = 0;
	const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
	struct descriptor expr_d = {0, DTYPE_T, CLASS_S, 0};

	expr_d.length = strlen(expr);
	expr_d.pointer = (char *)expr;
	status = TdiCompile(&expr_d, &xd MDS_END_ARG);
	if(status & 1) status = TdiData(&xd, &xd MDS_END_ARG);
	if(status & 1) status = TdiLong(&xd, &xd MDS_END_ARG);
	if(status & 1 && xd.pointer && xd.pointer->pointer) 
		ris = *(int *)xd.pointer->pointer;
	MdsFree1Dx(&xd, NULL);
	return ris;
}




JNIEXPORT jobject JNICALL Java_Data_fromExpr
  (JNIEnv *env, jclass cls, jstring jsource)
{
   EMPTYXD(out_xd);
   EMPTYXD(dec_xd);
   int status;
   const char *source = (*env)->GetStringUTFChars(env, jsource, 0);
   char *error_msg;
   struct descriptor source_d = {0, DTYPE_T, CLASS_S, 0};
   jobject ris, exc;

   source_d.length = strlen(source);
   source_d.pointer = (char *)source;
   status = TdiCompile(&source_d, &out_xd MDS_END_ARG);
   if(!(status & 1))
     {
       error_msg = (char *)MdsGetMsg(status);
       exc = (*env)->FindClass(env, "SyntaxException");
       (*env)->ThrowNew(env, exc, error_msg);
       /* //free(error_msg); */
       return NULL;
     }
   /*
   {EMPTYXD(xd);
printf("\nStart Check\n");
TdiDecompile(out_xd, &xd MDS_END_ARG);
xd.pointer->pointer[xd.pointer->length - 1]=0;
printf("\nEndCheck: %d %s\n", xd.pointer->length, xd.pointer->pointer);
   }
   */

   ris = DescripToObject(env, out_xd.pointer);
   MdsFree1Dx(&out_xd, NULL);
   return  ris;
}
  
      



jobject DescripToObject(JNIEnv *env, struct descriptor *desc)
{
  jclass cls, data_cls;
  jmethodID constr;
  int i, length, count, status;
  unsigned int opcode;
  jboolean is_unsigned = 0;
  jobject obj, exc, curr_obj;
  jvalue args[4];
  jbyteArray jbytes;
  jshortArray jshorts;
  jintArray jints;
  jlongArray jlongs;
  jfloatArray jfloats;
  jdoubleArray jdoubles;
  jobjectArray jobjects;
  jfieldID data_fid, opcode_fid;
  char message[64];
  struct descriptor_a *array_d;
  struct descriptor_r *record_d;
  char *buf;
  EMPTYXD(float_xd);
  EMPTYXD(ca_xd);
  int is_ca = 0;


  if(!desc)
    {
      return  NULL;
    }
 /*printf("DescripToObject dtype = %d class = %d\n", desc->dtype, desc->class);*/
     
  if(desc->class == CLASS_XD)
    return DescripToObject(env, ((struct descriptor_xd *)desc)->pointer);

  switch(desc->class) {
    case CLASS_S:

      switch(desc->dtype) {
        case DTYPE_BU: is_unsigned =1;
        case DTYPE_B : 
	  cls = (*env)->FindClass(env, "ByteData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(BZ)LData;");
	  args[0].b = *(char *)desc->pointer;
	  args[1].z = is_unsigned;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_WU: is_unsigned =1;
        case DTYPE_W : 
	  cls = (*env)->FindClass(env, "ShortData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(SZ)LData;");
	  args[0].s = *(short *)desc->pointer;
	  args[1].z = is_unsigned;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_LU: is_unsigned =1;
        case DTYPE_L : 
	  cls = (*env)->FindClass(env, "IntData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(IZ)LData;");
	  args[0].i = *(int *)desc->pointer;
	  args[1].z = is_unsigned;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_NID : 
	  cls = (*env)->FindClass(env, "NidData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
	  args[0].i = *(int *)desc->pointer;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_QU: is_unsigned =1;
        case DTYPE_Q : 
	  cls = (*env)->FindClass(env, "QuadData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(JZ)LData;");
	  args[0].j = *(long *)desc->pointer;
	  args[1].z = is_unsigned;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_OU: is_unsigned =1;
        case DTYPE_O : 
	  cls = (*env)->FindClass(env, "OctaData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)LData;");
	  jlongs = (*env)->NewLongArray(env, 2);
	  (*env)->SetLongArrayRegion(env, jlongs, 0, 2, (jlong *)desc->pointer);
	  args[0].l = jlongs;
	  args[1].z = is_unsigned;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_FS :
        case DTYPE_F:
	  cls = (*env)->FindClass(env, "FloatData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(F)LData;");
	  if(desc->dtype == DTYPE_F)
	      CvtConvertFloat(desc->pointer, DTYPE_F, &args[0].f, DTYPE_FS, 0);
	  else
	    args[0].f = *(float *)desc->pointer;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_DOUBLE : 
	  cls = (*env)->FindClass(env, "DoubleData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(D)LData;");
	  args[0].d = *(float *)desc->pointer;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_T :
	  cls = (*env)->FindClass(env, "StringData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)LData;");
	  buf = (char *)malloc(desc->length+1);
	  memcpy(buf, desc->pointer, desc->length);
	  buf[desc->length] = 0;
	  args[0].l = (*env)->NewStringUTF(env, buf);
	  free(buf);
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_PATH :
	  cls = (*env)->FindClass(env, "PathData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)LData;");
	  buf = (char *)malloc(desc->length+1);
	  memcpy(buf, desc->pointer, desc->length);
	  buf[desc->length] = 0;
	  args[0].l = (*env)->NewStringUTF(env, buf);
	  free(buf);
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
        case DTYPE_EVENT :
	  cls = (*env)->FindClass(env, "EventData");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)LData;");
	  buf = (char *)malloc(desc->length+1);
	  memcpy(buf, desc->pointer, desc->length);
	  buf[desc->length] = 0;
	  args[0].l = (*env)->NewStringUTF(env, buf);
	  free(buf);
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
	  
        default: 
	  sprintf(message, "Datatype %d not supported for class CLASS_S", desc->dtype);
	  exc = (*env)->FindClass(env, "UnsupportedDataException");
	  (*env)->ThrowNew(env, exc, message);
      }
	  case CLASS_CA: 
		status = TdiData(desc, &ca_xd MDS_END_ARG);
		if(!(status & 1)) 
		{
			printf("Cannot evaluate CA descriptor\n");
			return NULL;
		}
		is_ca = 1;
	  case CLASS_A:
		if(is_ca)
			array_d = (struct descriptor_a *)ca_xd.pointer;
		else
			array_d = (struct descriptor_a *)desc;
		length =array_d->arsize/array_d->length; 
		switch(array_d->dtype) {
			case DTYPE_BU: is_unsigned = 1;
			case DTYPE_B: 
				cls = (*env)->FindClass(env, "ByteArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([BZ)LData;");
				jbytes = (*env)->NewByteArray(env, length);
				(*env)->SetByteArrayRegion(env, jbytes, 0, length, (jbyte *)array_d->pointer);
				args[0].l = jbytes;
				args[1].z = is_unsigned;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
			case DTYPE_WU: is_unsigned = 1;
			case DTYPE_W:  
				cls = (*env)->FindClass(env, "ShortArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([SZ)LData;");
				jshorts = (*env)->NewShortArray(env, length);
				(*env)->SetShortArrayRegion(env, jshorts, 0, length, (jshort *)array_d->pointer);
				args[0].l = jshorts;
				args[1].z = is_unsigned;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
			case DTYPE_LU: is_unsigned = 1;
			case DTYPE_L: 
				cls = (*env)->FindClass(env, "IntArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([IZ)LData;");
				jints = (*env)->NewIntArray(env, length);
				(*env)->SetIntArrayRegion(env, jints, 0, length, (jint *)array_d->pointer);
				args[0].l = jints;
				args[1].z = is_unsigned;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
			case DTYPE_QU: is_unsigned = 1;
			case DTYPE_Q: 
				cls = (*env)->FindClass(env, "QuadArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)LData;");
				jlongs = (*env)->NewLongArray(env, length);
				(*env)->SetLongArrayRegion(env, jlongs, 0, length, (jlong *)array_d->pointer);
				args[0].l = jlongs;
				args[1].z = is_unsigned;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
			case DTYPE_OU: is_unsigned = 1;
			case DTYPE_O: 
				cls = (*env)->FindClass(env, "OctaArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)LData;");
				jlongs = (*env)->NewLongArray(env, 2*length);
				(*env)->SetLongArrayRegion(env, jlongs, 0, 2*length, (jlong *)array_d->pointer);
				args[0].l = jlongs;
				args[1].z = is_unsigned;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
			case DTYPE_D:
			case DTYPE_G:
			case DTYPE_H:
			case DTYPE_F: 
			case DTYPE_FLOAT: /* //Let Tdi handle all the floating point stuff */
			{	
				status = TdiFloat(array_d, &float_xd MDS_END_ARG);
				if(!(status & 1))
				{
					printf(MdsGetMsg(status));
					exit(0);
				}
			}
				array_d = (struct descriptor_a *)float_xd.pointer;
				cls = (*env)->FindClass(env, "FloatArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([F)LData;");
				jfloats = (*env)->NewFloatArray(env, length);
				(*env)->SetFloatArrayRegion(env, jfloats, 0, length, (jfloat *)array_d->pointer);
				args[0].l = jfloats;
				MdsFree1Dx(&float_xd, 0);
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

	  /*case DTYPE_FLOAT:
	  cls = (*env)->FindClass(env, "FloatArray");
	  constr = (*env)->GetStaticMethodID(env, cls, "getData", "([F)LData;");
	  jfloats = (*env)->NewFloatArray(env, length);
	  (*env)->SetFloatArrayRegion(env, jfloats, 0, length, (jfloat *)array_d->pointer);
	  args[0].l = jfloats;
	  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);*/
			case DTYPE_DOUBLE: 
				cls = (*env)->FindClass(env, "DoubleArray");
				constr = (*env)->GetStaticMethodID(env, cls, "getData", "([D)LData;");
				jdoubles = (*env)->NewDoubleArray(env, length);
				(*env)->SetDoubleArrayRegion(env, jdoubles, 0, length, (jdouble *)array_d->pointer);
				args[0].l = jdoubles;
				if(is_ca) MdsFree1Dx(&ca_xd, 0);
				return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
		}
      case CLASS_R :
	record_d = (struct descriptor_r *)desc;
	switch(record_d->dtype) {
	case DTYPE_PARAM: cls = (*env)->FindClass(env, "ParameterData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break; 
	case DTYPE_SIGNAL: cls = (*env)->FindClass(env, "SignalData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break; 
	case DTYPE_DIMENSION: cls = (*env)->FindClass(env, "DimensionData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_WINDOW: cls = (*env)->FindClass(env, "WindowData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_FUNCTION: cls = (*env)->FindClass(env, "FunctionData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_CONGLOM: cls = (*env)->FindClass(env, "ConglomData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_ACTION: cls = (*env)->FindClass(env, "ActionData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_DISPATCH: cls = (*env)->FindClass(env, "DispatchData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_PROGRAM: cls = (*env)->FindClass(env, "ProgramData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_ROUTINE: cls = (*env)->FindClass(env, "RoutineData");  
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_PROCEDURE: cls = (*env)->FindClass(env, "ProcedureData");   
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_METHOD: cls = (*env)->FindClass(env, "MethodData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
      	case DTYPE_DEPENDENCY: cls = (*env)->FindClass(env, "DependencyData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_CONDITION: cls = (*env)->FindClass(env, "ConditionData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_WITH_UNITS: cls = (*env)->FindClass(env, "WithUnitsData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	case DTYPE_CALL: cls = (*env)->FindClass(env, "CallData");
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
 	case DTYPE_WITH_ERROR: cls = (*env)->FindClass(env, "WithErrorData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
 	case DTYPE_RANGE: cls = (*env)->FindClass(env, "RangeData"); 
	                  constr = (*env)->GetStaticMethodID(env, cls, "getData", "()LData;");
			  break;   
	}
        obj = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
	data_cls = (*env)->FindClass(env, "Data");
	jobjects = (*env)->NewObjectArray(env, record_d->ndesc, data_cls, 0);
	for(i = count = 0; count < record_d->ndesc;i++, count++)
	  {
	    (*env)->SetObjectArrayElement(env, jobjects, i, DescripToObject(env, record_d->dscptrs[i]));
	  }
        data_fid = (*env)->GetFieldID(env, cls, "descs", "[LData;");
        (*env)->SetObjectField(env, obj, data_fid, jobjects);
	if(record_d->pointer &&(record_d->dtype == DTYPE_FUNCTION || record_d->dtype == DTYPE_DEPENDENCY
	     || record_d->dtype == DTYPE_CONDITION || record_d->dtype == DTYPE_CALL || 
	        record_d->dtype == DTYPE_DISPATCH))
        {
	    opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
	    opcode = 0;
	    switch(record_d->length) {
	      case 1 : opcode = *( unsigned char *)record_d->pointer; break;
	      case 2 : opcode = *( unsigned short *)record_d->pointer; break;
	      default: opcode = *( unsigned int *)record_d->pointer; break;
	    } 
	    (*env)->SetIntField(env, obj, opcode_fid, opcode);
	}
	return obj;
      case CLASS_APD :
	array_d = (struct descriptor_a *)desc;
	length = array_d->arsize/array_d->length;
	cls = (*env)->FindClass(env, "ApdData"); 
	data_cls = (*env)->FindClass(env, "Data");
	constr = (*env)->GetStaticMethodID(env, cls, "getData", "([LData;)LData;");
	jobjects = (*env)->NewObjectArray(env, length, data_cls, 0);
	for(i = 0; i < length; i++)
	  {
	    if(curr_obj =  DescripToObject(env, ((struct descriptor **)array_d->pointer)[i]))
	      (*env)->SetObjectArrayElement(env, jobjects, i, curr_obj);
	  }
	args[0].l = jobjects;
	return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
  }
  return 0;
}


struct descriptor * ObjectToDescrip(JNIEnv *env, jobject obj)
  {

    jclass cls;
    jfieldID
      datum_fid,
      descs_fid,
      opcode_fid,
      dtype_fid,
      dclass_fid;

    static DESCRIPTOR_A(template_array, 0, 0, 0, 0);
    struct descriptor_a *array_d;
    static DESCRIPTOR_R(template_rec, 0, 1);
    struct descriptor_r *record_d;

    int i,
      ndescs,
      opcode,
      dtype,
      dclass;

    jobject jdescs;

    jsize length;
    jbyteArray jbytes;
    jshortArray jshorts;
    jintArray jints;
    jlongArray jlongs;
    jfloatArray jfloats;
    jdoubleArray jdoubles;

    jbyte *bytes;
    jshort *shorts;
    jint *ints;
    jlong *longs;
    jfloat *floats;
    jdouble *doubles;



    jstring java_string;
    const char *string; 

    struct descriptor *desc;

    if(!obj)
      {
        return NULL;
      }
    cls = (*env)->GetObjectClass(env, obj);
    dtype_fid = (*env)->GetFieldID(env, cls, "dtype", "I"),
    dclass_fid = (*env)->GetFieldID(env, cls, "dclass", "I");


    dtype = (*env)->GetIntField(env, obj, dtype_fid),
    dclass = (*env)->GetIntField(env, obj, dclass_fid);

	
	switch(dclass) {
      case CLASS_S :
	desc = (struct descriptor *)malloc(sizeof(struct descriptor));
        desc->class = dclass;
	desc->dtype = dtype;
	switch(dtype) {
	  case DTYPE_B:
	  case DTYPE_BU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "B");
	    desc->length = sizeof(char);
	    desc->pointer = (char *)malloc(desc->length);
	    *desc->pointer = (*env)->GetByteField(env, obj, datum_fid);
	    return desc;
	  
  	  case DTYPE_W:
	  case DTYPE_WU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "S");
	    desc->length = sizeof(short);
	    desc->pointer = (char *)malloc(desc->length);
	    *(short *)desc->pointer = (*env)->GetShortField(env, obj, datum_fid);
	    return desc;

  	  case DTYPE_L:
	  case DTYPE_LU:
	  case DTYPE_NID:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "I");
	    desc->length = sizeof(int);
	    desc->pointer = (char *)malloc(desc->length);
	    *(int *)desc->pointer = (*env)->GetIntField(env, obj, datum_fid);
	    return desc;

  	  case DTYPE_Q:
	  case DTYPE_QU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "J");
	    desc->length = sizeof(_int64);
	    desc->pointer = (char *)malloc(desc->length);
	    *(_int64 *)desc->pointer =(*env)->GetLongField(env, obj, datum_fid);
	    return desc;

  	  case DTYPE_O:
	  case DTYPE_OU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
	    jlongs = (*env)->GetObjectField(env, obj, datum_fid);
	    longs =  (*env)->GetLongArrayElements(env, jlongs,0);
	    desc->length = 2 * sizeof(_int64);
	    desc->pointer = (char *)malloc(desc->length);
	    *(_int64 *)desc->pointer = longs[0];
	    *((_int64 *)desc->pointer + 1) = longs[1];
	    (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
	    return desc;

	  case DTYPE_T:
	  case DTYPE_PATH:
	  case DTYPE_EVENT:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "Ljava/lang/String;");
	    java_string =(*env)->GetObjectField(env, obj, datum_fid);
	    string = (*env)->GetStringUTFChars(env, java_string, 0);
	    desc->length = strlen(string);
	    desc->pointer = (char *)malloc(desc->length);
	    memcpy(desc->pointer, string, desc->length);
	    (*env)->ReleaseStringUTFChars(env, java_string, string);
	    return desc;

	  case DTYPE_FLOAT:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "F");
	    desc->length = sizeof(float);
	    desc->pointer = (char *)malloc(desc->length);
	    *(float *)desc->pointer = (*env)->GetFloatField(env, obj, datum_fid);
	    return desc;

	  case DTYPE_DOUBLE:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "D");
	    desc->length = sizeof(double);
	    desc->pointer = (char *)malloc(desc->length);
	    *(double *)desc->pointer = (*env)->GetDoubleField(env, obj, datum_fid);
	    return desc;
	default:
	  printf("\nUnsupported type for CLASS_S: %d\n", dtype);
	}
	break;
      case CLASS_A:
	array_d = (struct descriptor_a *)malloc(sizeof(struct descriptor_a));
	memcpy(array_d, &template_array, sizeof(struct descriptor_a));
	array_d->dtype = dtype;
	switch(dtype) {
	  case DTYPE_B:
	  case DTYPE_BU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[B");
	    jbytes = (*env)->GetObjectField(env, obj, datum_fid);
	    bytes =  (*env)->GetByteArrayElements(env, jbytes,0);
	    length = (*env)->GetArrayLength(env, jbytes);
	    array_d->length = sizeof(char);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, bytes, array_d->arsize);
	    (*env)->ReleaseByteArrayElements(env, jbytes, bytes, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_W:
	  case DTYPE_WU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[S");
	    jshorts = (*env)->GetObjectField(env, obj, datum_fid);
	    shorts =  (*env)->GetShortArrayElements(env, jshorts,0);
	    length = (*env)->GetArrayLength(env, jshorts);
	    array_d->length = sizeof(short);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, shorts, array_d->arsize);
	    (*env)->ReleaseShortArrayElements(env, jshorts, shorts, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_L:
	  case DTYPE_LU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[I");
	    jints = (*env)->GetObjectField(env, obj, datum_fid);
	    ints =  (*env)->GetIntArrayElements(env, jints,0);
	    length = (*env)->GetArrayLength(env, jints);
	    array_d->length = sizeof(int);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, ints, array_d->arsize);
	    (*env)->ReleaseIntArrayElements(env, jints, ints, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_Q:
	  case DTYPE_QU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
	    jlongs = (*env)->GetObjectField(env, obj, datum_fid);
	    longs =  (*env)->GetLongArrayElements(env, jlongs,0);
	    length = (*env)->GetArrayLength(env, jlongs);
	    array_d->length = sizeof(long);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, longs, array_d->arsize);
	    (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_O:
	  case DTYPE_OU:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[J");
	    jlongs = (*env)->GetObjectField(env, obj, datum_fid);
	    longs =  (*env)->GetLongArrayElements(env, jlongs,0);
	    length = (*env)->GetArrayLength(env, jlongs);
	    array_d->length = 2*sizeof(long);
	    array_d->arsize = array_d->length * length/2;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, longs, array_d->arsize);
	    (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_FLOAT:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[F");
	    jfloats = (*env)->GetObjectField(env, obj, datum_fid);
	    floats =  (*env)->GetFloatArrayElements(env, jfloats,0);
	    length = (*env)->GetArrayLength(env, jfloats);
	    array_d->length = sizeof(float);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, floats, array_d->arsize);
	    (*env)->ReleaseFloatArrayElements(env, jfloats, floats, 0);
	    return (struct descriptor *)array_d;

	  case DTYPE_DOUBLE:
	    datum_fid = (*env)->GetFieldID(env, cls, "datum", "[D");
	    jdoubles = (*env)->GetObjectField(env, obj, datum_fid);
	    doubles =  (*env)->GetDoubleArrayElements(env, jdoubles,0);
	    length = (*env)->GetArrayLength(env, jdoubles);
	    array_d->length = sizeof(double);
	    array_d->arsize = array_d->length * length;
	    array_d->pointer = (char *)malloc(array_d->arsize);
	    memcpy(array_d->pointer, doubles, array_d->arsize);
	    (*env)->ReleaseDoubleArrayElements(env, jdoubles, doubles, 0);
	    return (struct descriptor *)array_d;
	  default:
	    printf("\nUnsupported type for CLASS_A: %d\n", dtype);
	    break;
	}
      case CLASS_R:
	opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
	opcode = (*env)->GetIntField(env, obj, opcode_fid);
	descs_fid = (*env)->GetFieldID(env, cls, "descs", "[LData;");
        jdescs = (*env)->GetObjectField(env, obj, descs_fid);
	ndescs = (*env)->GetArrayLength(env, jdescs);
	record_d = (struct descriptor_r *)malloc(sizeof(struct descriptor_r) + ndescs * sizeof(void *));
	memcpy(record_d, &template_rec, sizeof(struct descriptor_r));
	record_d->dtype = dtype;
	record_d->length = 0;
	record_d->pointer = 0;
	if(dtype == DTYPE_FUNCTION || dtype == DTYPE_DEPENDENCY || dtype == DTYPE_CONDITION
	   || dtype == DTYPE_CALL || dtype == DTYPE_DISPATCH)
	  {
	    record_d->length = sizeof(short);
	    record_d->pointer = (unsigned char *)malloc(sizeof(short));
	    *(short *)record_d->pointer = opcode;
	  }
	if(dtype == DTYPE_CALL)
	  {
	    record_d->length = sizeof(char);
	    record_d->pointer = (unsigned char *)malloc(sizeof(char));
	    *(char *)record_d->pointer = opcode;
	  }
	record_d->ndesc = ndescs;
	for(i = 0; i < ndescs; i++)
	  {
	    record_d->dscptrs[i] = ObjectToDescrip(env,(*env)->GetObjectArrayElement(env,jdescs, i));
	  }
	return (struct descriptor *)record_d;

       case CLASS_APD:
	descs_fid = (*env)->GetFieldID(env, cls, "descs", "[LData;");
        jdescs = (*env)->GetObjectField(env, obj, descs_fid);
	ndescs = (*env)->GetArrayLength(env, jdescs);
	array_d = (struct descriptor_a *)malloc(sizeof(struct descriptor_a));
	memcpy(array_d, &template_array, sizeof(struct descriptor_a));
	array_d->dtype = dtype;
	array_d->pointer = (char *)malloc(sizeof(void *) * ndescs);
	for(i = 0; i < ndescs; i++)
	 ((struct descriptor **)(array_d->pointer))[i] = ObjectToDescrip(env,(*env)->GetObjectArrayElement(env,jdescs, i));

	return (struct descriptor *)array_d;
   default:
      printf("\nUnsupported class: %d\n", dclass);
    }
	return 0;
  }

       
void FreeDescrip(struct descriptor *desc)
{
  struct descriptor_r *record_d;
  struct descriptor_a *array_d;
  int i;

  if(!desc)
    return;

/*printf("FreeDescrip class %d dtype %d\n", desc->class, desc->dtype);*/

  switch(desc->class) {
    case CLASS_S : free(desc->pointer); break;
    case CLASS_A : free(((struct descriptor_a *)desc)->pointer); break;
    case CLASS_R :
      record_d = (struct descriptor_r *)desc;
      if(record_d->length)
	free(record_d->pointer);
      for(i = 0; i < record_d->ndesc; i++)
	FreeDescrip(record_d->dscptrs[i]);
      break;
    case CLASS_APD :
      array_d = (struct descriptor_a *)desc;
      for(i = 0; i < (int)array_d->arsize/array_d->length; i++)
	FreeDescrip(((struct descriptor **)array_d->pointer)[i]);
      break;
  }
  free((char *)desc);
}   






