#include <jni.h>
#include "Database.h"
#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <treeshr.h>
#include <ncidef.h>
#include <libroutines.h>

extern struct descriptor * ObjectToDescrip(JNIEnv *env, jobject obj);
extern jobject DescripToObject(JNIEnv *env, struct descriptor *desc);
extern void FreeDescrip(struct descriptor *desc);


static char **getDeviceFields(char *deviceName, int *num_fields)
{
	int status, nid, curr_nid, i;
	char **names;
	static int conglomerate_nids, conglomerate_nids_len;
	struct nci_itm nci_list[] = 
	{{4, NciNUMBER_OF_ELTS, &conglomerate_nids, &conglomerate_nids_len}, 
	{NciEND_OF_LIST, 0, 0, 0}};


	conglomerate_nids = 0;
	status = TreeOpenNew("device", -1);
	if(!(status & 1)) return NULL;
	status = TreeAddConglom("TEST", deviceName, &nid);
	if(status & 1) status = TreeGetNci(nid, nci_list);
	if(!(status & 1) || conglomerate_nids == 0) 
	{
		TreeQuitTree("TEST", -1);
		return NULL;
	}
	conglomerate_nids--;
	names = (char **)malloc(sizeof(char *) * conglomerate_nids);
	TreeSetDefaultNid(nid);
	curr_nid = nid + 1;
	for(i = 0; i < conglomerate_nids; i++, curr_nid++)
	{
		names[i] = TreeGetMinimumPath(&nid, curr_nid);
//		printf(names[i]);
	}
	TreeQuitTree("TEST", -1);
	*num_fields = conglomerate_nids;
	return names;
}





static void RaiseException(JNIEnv *env, char *msg)
{
   jclass exc = (*env)->FindClass(env, "DatabaseException");
   (*env)->ThrowNew(env, exc, msg);
   //free(msg);
}
  

JNIEXPORT void JNICALL Java_Database_open
  (JNIEnv *env, jobject obj)
{
  int status;
  jfieldID name_fid, readonly_fid, editable_fid, shot_fid;
  jclass cls = (*env)->GetObjectClass(env, obj);
  const char *name;
  jobject jname;
  int is_editable, is_readonly, shot;

  name_fid =  (*env)->GetFieldID(env, cls, "name", "Ljava/lang/String;");
  readonly_fid = (*env)->GetFieldID(env, cls, "is_readonly", "Z");
  editable_fid = (*env)->GetFieldID(env, cls, "is_editable", "Z");
  shot_fid = (*env)->GetFieldID(env, cls, "shot", "I");
  jname = (*env)->GetObjectField(env, obj, name_fid);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  is_editable = (*env)->GetBooleanField(env, obj, editable_fid);
  is_readonly = (*env)->GetBooleanField(env, obj, readonly_fid);
  shot = (*env)->GetIntField(env, obj, shot_fid);

  if(is_editable)
    {
      status =  TreeOpenEdit((char *)name, shot);
      if(!(status & 1))
	status = TreeOpenNew((char *)name, shot);
    }
  else
    status = TreeOpen((char *)name, shot, is_readonly);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if(!(status & 1))
    RaiseException(env, MdsGetMsg(status));
}


JNIEXPORT void JNICALL Java_Database_write
  (JNIEnv *env, jobject obj)
{
  int status;
  jfieldID name_fid, shot_fid;
  jclass cls = (*env)->GetObjectClass(env, obj);
  const char *name;
  jobject jname;
  int shot;

  name_fid =  (*env)->GetFieldID(env, cls, "name", "Ljava/lang/String;");
  shot_fid = (*env)->GetFieldID(env, cls, "shot", "I");
  jname = (*env)->GetObjectField(env, obj, name_fid);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  shot = (*env)->GetIntField(env, obj, shot_fid);

  status = TreeWriteTree((char *)name, shot);
  if(!(status & 1))
    RaiseException(env, MdsGetMsg(status));
}


      
JNIEXPORT void JNICALL Java_Database_close (JNIEnv *env, jobject obj)
{
  int status = 1;
  jfieldID name_fid,shot_fid;
  jclass cls = (*env)->GetObjectClass(env, obj);
  const char *name;
  jobject jname;
  int shot;

  name_fid =  (*env)->GetFieldID(env, cls, "name", "Ljava/lang/String;");
  shot_fid = (*env)->GetFieldID(env, cls, "shot", "I");
  jname = (*env)->GetObjectField(env, obj, name_fid);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  shot = (*env)->GetIntField(env, obj, shot_fid);
  status = TreeClose((char *)name, shot); 
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if(!(status & 1))
   RaiseException(env, MdsGetMsg(status));
}

      
JNIEXPORT void JNICALL Java_Database_quit (JNIEnv *env, jobject obj)
{
  int status;
  jfieldID name_fid,shot_fid;
  jclass cls = (*env)->GetObjectClass(env, obj);
  const char *name;
  jobject jname;
  int shot;

  name_fid =  (*env)->GetFieldID(env, cls, "name", "Ljava/lang/String;");
  shot_fid = (*env)->GetFieldID(env, cls, "shot", "I");
  jname = (*env)->GetObjectField(env, obj, name_fid);
  name = (*env)->GetStringUTFChars(env, jname, 0);
  shot = (*env)->GetIntField(env, obj, shot_fid);
  status = TreeQuitTree((char *)name, shot); 
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if(!(status & 1))
    RaiseException(env, MdsGetMsg(status));
}
      

JNIEXPORT jobject JNICALL Java_Database_getData
 (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, status;
  jfieldID nid_fid;
  jclass cls;
  EMPTYXD(xd);
  EMPTYXD(out_xd);
  jobject ris;


  cls = (*env)->GetObjectClass(env, jnid);
  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);


  status = TreeGetRecord(nid, &xd);

 // printf("\nletti %d bytes", xd.l_length);
/*
  status = TdiDecompile(&xd, &out_xd MDS_END_ARG);
printf("\nEnd TdiDecompile");
out_xd.pointer->pointer[out_xd.pointer->length - 1] = 0;
printf(out_xd.pointer->pointer);*/
 

  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  if(!xd.pointer)
    return NULL;


  ris = DescripToObject(env, xd.pointer);
   
  MdsFree1Dx(&xd, NULL);
  return ris;
}

JNIEXPORT void JNICALL Java_Database_putData
  (JNIEnv *env, jobject obj, jobject jnid, jobject jdata)
{
  int nid, status;
  jfieldID nid_fid; 
  jclass cls;
  struct descriptor *dsc;

  EMPTYXD(xd);

  cls = (*env)->GetObjectClass(env, jnid);
  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  dsc = ObjectToDescrip(env, jdata);

  /*status = TdiDecompile(dsc, &xd MDS_END_ARG);
printf("\ndopo di ObjTODesc %x %x %x %d\n", obj, dsc->pointer, xd.pointer, status);
xd.pointer->pointer[xd.pointer->length - 1] = 0;
printf(xd.pointer->pointer);
  */  
  if(!dsc)
    status = TreePutRecord(nid, (struct descriptor *)&xd, 0);
  else
    {
      status = TreePutRecord(nid, dsc, 0);
      FreeDescrip(dsc);
    }
  if(!(status & 1))
      RaiseException(env, MdsGetMsg(status));
}

JNIEXPORT jobject JNICALL Java_Database_getInfo
  (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);
  jmethodID constr;
  jvalue args[18];
  static  int nci_flags, nci_flags_len, time_inserted[2], time_len, conglomerate_nids_len, conglomerate_nids,
    owner_id, owner_len, dtype_len, class_len, length, length_len, usage_len, name_len, fullpath_len, minpath_len;
  static  char dtype, class, time_str[256], usage, name[16], fullpath[512], minpath[512];
  unsigned short asctime_len;
  struct descriptor time_dsc = {256, DTYPE_T, CLASS_S, time_str};

  struct nci_itm nci_list[] = 
  {{4, NciGET_FLAGS, &nci_flags,&nci_flags_len},
   {8, NciTIME_INSERTED, time_inserted, &time_len},
   {4, NciOWNER_ID,&owner_id, &owner_len},
   {1, NciCLASS, &class, &class_len},
   {1, NciDTYPE, &dtype, &dtype_len},
   {4, NciLENGTH, &length, &length_len},
   {1, NciUSAGE, &usage, &usage_len},
   {16, NciNODE_NAME, name, &name_len},
   {511, NciFULLPATH, fullpath, &fullpath_len},
   {511, NciMINPATH, minpath, &minpath_len},
  {4, NciNUMBER_OF_ELTS, &conglomerate_nids, &conglomerate_nids_len}, 
   {NciEND_OF_LIST, 0, 0, 0}};


  
  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  conglomerate_nids = 0;
  status = TreeGetNci(nid, nci_list);
  if(!(status & 1))
   {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
   }

  name[name_len] = 0;
  fullpath[fullpath_len] = 0;
  minpath[minpath_len] = 0;
  cls = (*env)->FindClass(env, "NodeInfo");
  constr = (*env)->GetStaticMethodID(env, cls, "getNodeInfo", "(ZZZZZZZZLjava/lang/String;IIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;I)LNodeInfo;");
  args[0].z = nci_flags & NciM_STATE;
  args[1].z = nci_flags & NciM_PARENT_STATE;
  args[2].z = nci_flags & NciM_SETUP_INFORMATION;
  args[3].z = nci_flags & NciM_WRITE_ONCE;
  args[4].z = nci_flags & NciM_COMPRESSIBLE;
  args[5].z = nci_flags & NciM_COMPRESS_ON_PUT;
  args[6].z = nci_flags & NciM_NO_WRITE_MODEL;
  args[7].z = nci_flags & NciM_NO_WRITE_SHOT;
  LibSysAscTim(&asctime_len, &time_dsc,time_inserted);
  time_str[asctime_len] = 0;
  args[8].l =  (*env)->NewStringUTF(env, time_str);
  args[9].i = owner_id;
  args[10].i = dtype;
  args[11].i = class;
  args[12].i = length;
  args[13].i = (int)usage;
  args[14].l = (*env)->NewStringUTF(env, name);
  args[15].l = (*env)->NewStringUTF(env, fullpath);
  args[16].l = (*env)->NewStringUTF(env, minpath);
  args[17].i = conglomerate_nids;
  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
}

JNIEXPORT void JNICALL Java_Database_setTags
  (JNIEnv *env, jobject obj, jobject jnid, jobjectArray jtags)
{
  int nid, status, n_tags, i;
  jobject jtag;
  const char *tag;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  n_tags = (*env)->GetArrayLength(env, jtags);
  status = TreeRemoveNodesTags(nid);
  /*  if(!(status & 1))
   {
      RaiseException(env, MdsGetMsg(status));
      return;
      }*/
  for(i = 0; i < n_tags; i++)
    {
      jtag = (*env)->GetObjectArrayElement(env, jtags, i);
      tag = (*env)->GetStringUTFChars(env, jtag, 0);
      status = TreeAddTag(nid, (char *)tag);
      (*env)->ReleaseStringUTFChars(env, jtag, tag);
      if(!(status & 1))
	{
	  RaiseException(env, MdsGetMsg(status));
	  return;
	}
    }
}
      
      
JNIEXPORT jobjectArray JNICALL Java_Database_getTags
  (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, n_tags, i;
  jobject jtag;
  jobjectArray jtags;
  
  char *tags[256];
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid),
    string_cls = (*env)->FindClass(env, "java/lang/String");
  int ctx = 0;

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  n_tags = 0;
  while(n_tags < 256 && (tags[n_tags] = TreeFindNodeTags(nid, &ctx)) )
  {
	 n_tags++;
	 if(ctx == -1)
		break;
  }
  jtags = (*env)->NewObjectArray(env, n_tags, string_cls, 0); 
  for(i = 0; i < n_tags; i++)
    {
      jtag = (*env)->NewStringUTF(env, tags[i]);
      //free(tags[i]);
      (*env)->SetObjectArrayElement(env, jtags, i, jtag);
    }
  return jtags;
}
      
JNIEXPORT void JNICALL Java_Database_renameNode
  (JNIEnv *env, jobject obj, jobject jnid, jstring jname)
{

  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);
  const char 

*name = (*env)->GetStringUTFChars(env, jname, 0);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  status = TreeRenameNode(nid, (char *)name);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if(!(status & 1))
      RaiseException(env, MdsGetMsg(status));
}
    
JNIEXPORT jobject JNICALL Java_Database_addNode
  (JNIEnv *env, jobject obj, jstring jname, jint usage)
{
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);
  int status, nid;
  jvalue args[1];
  jclass cls;
  jmethodID constr;

  status = TreeAddNode((char *)name, &nid, (char)usage);
  (*env)->ReleaseStringUTFChars(env, jname, name);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }

  cls = (*env)->FindClass(env, "NidData");
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  args[0].i = nid;
  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
}

JNIEXPORT jobjectArray JNICALL Java_Database_startDelete
  (JNIEnv *env, jobject obj, jobjectArray jnids)
{
  int nid, status, len, i, num_to_delete;
  jfieldID nid_fid;
  jmethodID constr;
  jvalue args[1];
  jobject jnid, jout_nids;
  jclass cls = (*env)->FindClass(env, "NidData");
  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");

  num_to_delete = 0;
  len = (*env)->GetArrayLength(env, jnids);

  for(i = 0; i < len; i++)
    {
      jnid = (*env)->GetObjectArrayElement(env, jnids, i);
      nid = (*env)->GetIntField(env, jnid, nid_fid);
      TreeDeleteNodeInitialize(nid, &num_to_delete, i==0);
    }
  num_to_delete -= len; 

  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  jout_nids = (*env)->NewObjectArray(env, num_to_delete, cls, 0);
  for(i = 0; i < num_to_delete; i++)
    {
      TreeDeleteNodeGetNid(&nid);
      args[0].i = nid;
      jnid = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
      (*env)->SetObjectArrayElement(env, jout_nids, i, jnid);
    }
  return jout_nids;
}
	

JNIEXPORT void JNICALL Java_Database_executeDelete
  (JNIEnv *env, jobject obj)
{
  TreeDeleteNodeExecute();
}


JNIEXPORT jobjectArray JNICALL Java_Database_getSons
  (JNIEnv *env, jobject obj, jobject jnid)
{
  
  int nid, status, i;
  jfieldID nid_fid;
  jobject jnids;
  jclass cls = (*env)->GetObjectClass(env, jnid);
  jmethodID constr;
  jvalue args[1];
  static int num_nids_len, num_nids, nids_len;
  int *nids;



  struct nci_itm nci_list1[] = 
  {{4, NciNUMBER_OF_CHILDREN, &num_nids, &num_nids_len},
   {NciEND_OF_LIST, 0, 0, 0}},

  nci_list2[] = 
  {{0, NciCHILDREN_NIDS, 0, &nids_len},
   {NciEND_OF_LIST, 0, 0, 0}};

//printf("\nStart getSons");
  
  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  status = TreeGetNci(nid, nci_list1);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  if(num_nids > 0)
  {
	nids = (int *)malloc(num_nids * sizeof(nid));
	nci_list2[0].buffer_length = sizeof(int) * num_nids;
	nci_list2[0].pointer = nids;
	status = TreeGetNci(nid, nci_list2);
	if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  } 
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  jnids = (*env)->NewObjectArray(env, num_nids, cls, 0);
  for(i = 0; i < num_nids; i++)
    {
      args[0].i = nids[i];
      jnid = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
      (*env)->SetObjectArrayElement(env, jnids, i, jnid);
    }
  if(num_nids > 0)
	free((char *)nids);

//printf("\nEnd getSons");
  return jnids;
}

JNIEXPORT jobjectArray JNICALL Java_Database_getMembers
  (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, status, i;
  jfieldID nid_fid;
  jobject jnids;
  jclass cls = (*env)->GetObjectClass(env, jnid);
  jmethodID constr;
  jvalue args[1];
  static int num_nids_len, num_nids, nids_len;
  int *nids;



  struct nci_itm nci_list1[] = 
  {{4, NciNUMBER_OF_MEMBERS, &num_nids, &num_nids_len},
   {NciEND_OF_LIST, 0, 0, 0}},

  nci_list2[] = 
  {{0, NciMEMBER_NIDS, 0, &nids_len},
   {NciEND_OF_LIST, 0, 0, 0}};

//printf("\nStart getMembers");

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  status = TreeGetNci(nid, nci_list1);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }


  if(num_nids > 0)
  {
	nids = (int *)malloc(num_nids * sizeof(nid));
	nci_list2[0].buffer_length = sizeof(int) * num_nids;
	nci_list2[0].pointer = nids;
	status = TreeGetNci(nid, nci_list2);
	if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  }
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  jnids = (*env)->NewObjectArray(env, num_nids, cls, 0);
  for(i = 0; i < num_nids; i++)
    {
      args[0].i = nids[i];
      jnid = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
      (*env)->SetObjectArrayElement(env, jnids, i, jnid);
    }
  if(num_nids > 0)free((char *)nids);

//printf("\nEnd getMembers");
  return jnids;
}

JNIEXPORT jboolean JNICALL Java_Database_isOn
  (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);

  status = TreeIsOn(nid);
  return (status & 1);
}
  
  
JNIEXPORT void JNICALL Java_Database_setOn
  (JNIEnv *env, jobject obj, jobject jnid, jboolean on)
{
  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);

  if(on)
     status = TreeTurnOn(nid);
  else
     status = TreeTurnOff(nid);
  if(!(status & 1))
  {
	printf("\nTreeTurnOn status: %d %s", status, MdsGetMsg(status));
      RaiseException(env, MdsGetMsg(status));
  }
}	
	
JNIEXPORT jobject JNICALL Java_Database_resolve
  (JNIEnv *env, jobject obj, jobject jpath_data)
{
  int nid, status;
  jfieldID path_fid;
  jclass cls = (*env)->GetObjectClass(env, jpath_data);
  jstring jpath;
  const char *path;
  jmethodID constr;
  jvalue args[1];

  path_fid = (*env)->GetFieldID(env, cls, "datum", "Ljava/lang/String;");
  jpath = (*env)->GetObjectField(env, jpath_data, path_fid);
  path = (*env)->GetStringUTFChars(env, jpath, 0);
  status = TreeFindNode((char *)path, &nid);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  
  cls = (*env)->FindClass(env, "NidData");
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  args[0].i = nid;
  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
}

JNIEXPORT void JNICALL Java_Database_setDefault
  (JNIEnv *env, jobject obj, jobject jnid)
{
  
  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);
  status = TreeSetDefaultNid(nid);
  if(!(status & 1))
      RaiseException(env, MdsGetMsg(status));
}


JNIEXPORT jobject JNICALL Java_Database_getDefault
  (JNIEnv *env, jobject obj)
{
  jclass cls;
  jmethodID constr;
  jvalue args[1];
  int status, nid;

  status = TreeGetDefaultNid(&nid);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  cls = (*env)->FindClass(env, "NidData");
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  args[0].i = nid;
  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
}


JNIEXPORT jobject JNICALL Java_Database_addDevice
  (JNIEnv *env, jobject obj, jstring jpath, jstring jmodel)
{
  int nid, status;
  const char *path = (*env)->GetStringUTFChars(env, jpath, 0);
  const char *model = (*env)->GetStringUTFChars(env, jmodel, 0);
  jclass cls;
  jmethodID constr;
  jvalue args[1];

  status = TreeAddConglom((char *)path, (char *)model, &nid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  (*env)->ReleaseStringUTFChars(env, jmodel, model);
  if(!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status));
      return NULL;
    }
  cls = (*env)->FindClass(env, "NidData");
  constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)LData;");
  args[0].i = nid;
  return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
}
  
  
static int doAction(int nid)
{
	extern int TdiEvaluate();
	int status;
	EMPTYXD(xd);
	EMPTYXD(xd1);
	struct descriptor_program *program_d_ptr;
	struct descriptor_method *method_d_ptr;
	struct descriptor_routine *routine_d_ptr;
	struct descriptor_r *curr_rec_ptr;
	struct descriptor *curr_d_ptr;
	char *command, *expression, *path;
	struct descriptor expr_d = {0, DTYPE_T, CLASS_S, 0};
	int method_nid, i;
	struct descriptor nid_d = {sizeof(int), DTYPE_NID, CLASS_S, (char *)&method_nid};
	char type = DTYPE_L;
	DESCRIPTOR_CALL(call_d, &type, 256, 0, 0);

	status = TreeGetRecord(nid, &xd);
	if(!(status & 1)) return status;
	if(!xd.pointer) return 0;
	curr_rec_ptr = (struct descriptor_r *)xd.pointer;
	if(curr_rec_ptr->dtype == DTYPE_ACTION)
		curr_rec_ptr = ((struct descriptor_action *)curr_rec_ptr)->task;
	if(!curr_rec_ptr) return 0;
	switch(curr_rec_ptr->dtype) {
	case DTYPE_PROGRAM:
		program_d_ptr = (struct descriptor_program *)xd.pointer;
		if(!program_d_ptr->program) {status = 0; break;}
		status = TdiEvaluate(program_d_ptr->program, &xd1 MDS_END_ARG);
		if(!(status & 1)) break;
		if(!xd1.pointer || xd1.pointer->dtype != DTYPE_T) 
		{
			status = 0; break;
		}
		command = malloc(xd1.pointer->length + 1);
		memcpy(command, xd1.pointer->pointer, xd1.pointer->length);
		command[xd1.pointer->length] = 0;
		MdsFree1Dx(&xd1, 0);
		expression = malloc(strlen(command) + 20);
		sprintf(expression, "spawn(\"%s\",,)", command);
		expr_d.length = strlen(expression);
		expr_d.pointer = expression;
		status = TdiCompile(&expr_d, &xd1 MDS_END_ARG);
		free(command);
		free(expression);
		if(status & 1) status = TdiEvaluate(&xd1, &xd1 MDS_END_ARG);
		break;
	case DTYPE_METHOD:
		method_d_ptr = (struct descriptor_method *)curr_rec_ptr;
		if(!method_d_ptr->object || !method_d_ptr->method)
		{
			status = 0; 
			break;
		}
		if(method_d_ptr->object->dtype == DTYPE_NID)
			status = TreeDoMethod(method_d_ptr->object, method_d_ptr->method MDS_END_ARG);
		else if(method_d_ptr->object->dtype == DTYPE_PATH)
		{
			path = malloc(method_d_ptr->object->length + 1);
			memcpy(path, method_d_ptr->object->pointer, method_d_ptr->object->length);
			path[method_d_ptr->object->length] = 0;
			status = TreeFindNode(path, &method_nid);
			free(path);
			if(status & 1) status = TreeDoMethod(&nid_d, method_d_ptr->method MDS_END_ARG);
		} else status = 0;
		break;
	case DTYPE_ROUTINE:
		routine_d_ptr = (struct descriptor_routine *)curr_rec_ptr;
		call_d.image = routine_d_ptr->image;
		call_d.routine = routine_d_ptr->routine;
		call_d.ndesc = routine_d_ptr->ndesc - 1;
		for(i = 0; i < routine_d_ptr->ndesc-3; i++)
			call_d.arguments[i] = routine_d_ptr->arguments[i];
		status = TdiEvaluate(&call_d, &xd1 MDS_END_ARG);
		MdsFree1Dx(&xd1, 0);
		break;
	default: status = 0;
	}
	MdsFree1Dx(&xd, 0);
	return status;
}

JNIEXPORT void JNICALL Java_Database_doAction
  (JNIEnv *env, jobject obj, jobject jnid)
{
  int nid, status;
  jfieldID nid_fid;
  jclass cls = (*env)->GetObjectClass(env, jnid);

  nid_fid = (*env)->GetFieldID(env, cls, "datum", "I");
  nid = (*env)->GetIntField(env, jnid, nid_fid);

  status = doAction(nid);
  if(!(status & 1))
      RaiseException(env, (status == 0)?"Cannot perform action":MdsGetMsg(status));
}
  
 
JNIEXPORT jobjectArray JNICALL Java_Database_getDeviceComponents
  (JNIEnv *env, jobject obj, jstring jname)
{
	const char *name = (*env)->GetStringUTFChars(env, jname, 0);
	int num_fields, i;
	jobjectArray jfields;
	jclass string_cls = (*env)->FindClass(env, "java/lang/String");
	jstring jfield; 

	char **fields = getDeviceFields(name, &num_fields);
	(*env)->ReleaseStringUTFChars(env, jname, name);
	if(fields == NULL) return NULL;

	jfields = (*env)->NewObjectArray(env, num_fields, string_cls, 0); 
	for(i = 0; i < num_fields; i++)
    {
		jfield = (*env)->NewStringUTF(env, fields[i]);
		TreeFree(fields[i]);
		(*env)->SetObjectArrayElement(env, jfields, i, jfield);
    }
	free((char *)fields);
	return jfields;
}




