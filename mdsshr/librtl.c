#if !(defined(__ALPHA) && defined(__VMS))
void __MB(){return;}
#endif
#if defined(WIN32)
#pragma warning (disable : 4100 4201 4115 4214 4514)
#include <process.h>
#include <windows.h>
#include <tchar.h>
#include <wtypes.h>
#include <winreg.h>
#else /* WIN32 */

#if defined(__osf__) || defined(__irix__) || defined(__sunos__)
#include <dlfcn.h>
#elif defined(__hpux)
#include <dl.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <librtl_messages.h>

#ifndef va_count
#define  va_count(narg) va_start(incrmtr, first); \
                        for (narg=1; (narg < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG); narg++)
#endif /* va_count */

typedef struct _VmList { void *ptr;
			 struct _VmList *next;
		       } VmList;

typedef struct _ZoneList { VmList *vm;
                           struct _ZoneList *next;
			 } ZoneList;

ZoneList *MdsZones=NULL;

int StrFree1Dx(struct descriptor *out);
int StrGet1Dx(unsigned short *len, struct descriptor *out);
int StrCopyDx(struct descriptor *out, struct descriptor *in);
int StrAppend(struct descriptor *out, struct descriptor *tail);

void MdsFree(void *ptr)
{
  free(ptr);
}

char *MdsDescrToCstring(struct descriptor *in)
{
  char *out = malloc(in->length + 1);
  memcpy(out,in->pointer,in->length);
  out[in->length] = 0;
  return out;
}

int LibSigToRet(){return 1;}

int StrConcat( struct descriptor *out, struct descriptor *first, ...)
{
  int i;
  int narg;
  va_list incrmtr;
  int status = StrCopyDx(out,first);
  if (status & 1)
  {
    va_count(narg);
    va_start(incrmtr,first);
    if (out->class == CLASS_D)
    {
      for (i=1;i<narg && (status & 1);i++)
        StrAppend(out,va_arg(incrmtr,struct descriptor *));
    }
    else if (out->class == CLASS_S)
    {
      struct descriptor temp = *out;
      struct descriptor *next;
      for (i=1,
           temp.length = (unsigned short)(out->length - first->length),
           temp.pointer = out->pointer + first->length;
           i<narg && (status & 1) && temp.length > 0;
           i++,
           temp.length = (unsigned short)(temp.length - next->length),
           temp.pointer += next->length)
      {
        next = va_arg(incrmtr,struct descriptor *);
        StrCopyDx(&temp,next);
      }
    }
    else
      status = 0;
  }
  return status;
}

int StrPosition(struct descriptor *source, struct descriptor *substring, int *start)
{
  char *source_c = MdsDescrToCstring(source);
  char *substring_c = MdsDescrToCstring(substring);
  char *search = source_c + ((start && *start > 0) ? (*start - 1) : 0);
  char *found = strstr(search,substring_c);
  int answer = found ? (found - source_c) + 1 : 0;
  free(source_c);
  free(substring_c);
  return answer;
}

int StrCopyR(struct descriptor *dest, unsigned short *len, char *source)
{
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  s.length = *len;
  s.pointer = source;
  return StrCopyDx(dest,&s);
}

int StrLenExtr(struct descriptor *dest, struct descriptor *source, int *start_in, int *len_in)
{
  unsigned short start = (unsigned short)((*start_in > 1) ? *start_in : 1);
  unsigned short len = (unsigned short)((*len_in > 0) ? *len_in : 0);
  unsigned short maxlen = (unsigned short)(source->length - start + 1);
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  s.length = (unsigned short)(len > maxlen ? maxlen : len);
  s.pointer = &source->pointer[start-1];
  return StrCopyDx(dest,&s);
}

int StrGet1Dx(unsigned short *len, struct descriptor *out)
{
  if (out->class != CLASS_D) return 0;
  if (out->length == *len) return 1;
  if (out->length && (out->pointer != NULL))
    free(out->pointer);
  out->length = *len;
  out->pointer = *len ? malloc(*len) : NULL;
  return 1;
}

#if defined(__alpha) && defined(__vms)
typedef __int64 _int64;
#elif defined(__osf__) || defined(__irix__) || defined(__sunos__)
typedef long _int64;
#elif defined(__hpux)
typedef long long _int64;
#endif

int LibEmul(int *m1, int *m2, int *add, _int64 *prod)
{
  _int64 m1_64 = *m1;
  _int64 m2_64 = *m2;
  _int64 add_64 = *add;
  *prod = m1_64 * m2_64 + add_64;
  return 1;
}

int LibSFree1Dd(struct descriptor *out)
{
  return StrFree1Dx(out);
}
  
int StrTrim(struct descriptor *out, struct descriptor *in, unsigned short *lenout)
{
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  unsigned short i;
  for (i=in->length;i>0;i--) if (in->pointer[i-1] != 32) break;
  s.length = i;
  s.pointer = in->pointer;
  if (lenout != NULL)
    *lenout = s.length;
  return StrCopyDx(out,&s);
}

int StrCopyDx(struct descriptor *out, struct descriptor *in)
{
  if (out->class == CLASS_D && (in->length != out->length))
    StrGet1Dx(&in->length, out);
  if (out->length && out->pointer != NULL)
    memcpy(out->pointer, in->pointer, out->length > in->length ?
	   in->length : out->length);
  if (out->length > in->length)
    memset(out->pointer+in->length, 32, out->length - in->length);
  return 1;
}

int StrCompare(struct descriptor *str1, struct descriptor *str2)
{
  char *str1c = MdsDescrToCstring(str1);
  char *str2c = MdsDescrToCstring(str2);
  int ans;
  ans = strcmp(str1c,str2c);
  free(str1c);
  free(str2c);
  return ans;
}

int StrUpcase(struct descriptor *out, struct descriptor *in)
{
  struct descriptor s = {0,DTYPE_T,CLASS_D,0};
  int i;
  StrCopyDx(&s,in);
  for (i=0; i<s.length && i<s.length; i++)
    s.pointer[i] = (char)toupper(in->pointer[i]);
  StrCopyDx(out,&s);
  StrFree1Dx(&s);
  return 1;
}

int StrRight(struct descriptor *out, struct descriptor *in, unsigned short *start) {
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  s.length = (unsigned short)((int)in->length - *start + 1);
  s.pointer = in->pointer+(*start-1);
  return StrCopyDx(out,&s);
}

int LibCreateVmZone(ZoneList **zone)
{
  ZoneList *list;
  *zone = malloc(sizeof(ZoneList));
  (*zone)->vm = NULL;
  (*zone)->next = NULL;
  if (MdsZones == NULL)
    MdsZones = *zone;
  else {
    for (list = MdsZones; list->next; list = list->next);
    list->next = *zone;
  }
  return (*zone != NULL);
}
    
int LibResetVmZone(ZoneList **zone)
{
  VmList *list;
  while ((list = zone ? (*zone ? (*zone)->vm : NULL) : NULL) != NULL)
    LibFreeVm(0, &list->ptr, zone);
  return 1;
}

int LibFreeVm(int *len, void **vm, ZoneList **zone)
{
  VmList *list = NULL;
  if (zone != NULL) {
    VmList *prev;
    for (prev = NULL, list = (*zone)->vm; 
         list && (list->ptr != *vm); prev=list, list = list->next);
    if (list) {
      if (prev)
        prev->next = list->next;
      else
	(*zone)->vm = list->next;
    }
  }
  free(*vm);
  if (list) 
    free(list);
  return 1;
}

int LibGetVm(int *len, void **vm, ZoneList **zone)
{
  VmList *list = malloc(sizeof(VmList));

  *vm = list->ptr = malloc(*len);
  list->next = NULL;
  if (zone != NULL) {
    if ((*zone)->vm) {
      VmList *ptr;
      for (ptr = (*zone)->vm; ptr->next; ptr = ptr->next);
      ptr->next = list;
    } else (*zone)->vm = list;
  }
  return (*vm != NULL);
}

int LibEstablish()
{
  return 1;
}

int LibSysAscTim(unsigned short *len, struct descriptor *str, unsigned int *time_in, unsigned int flags)
{
  time_t bintim;
  char *time_str;
  char time_out[23];
  unsigned short slen=sizeof(time_out);
  if (time_in)
  {
    unsigned int tmp = (time_in[0] >> 24) | (time_in[1] << 8);
    bintim = (time_t)((double)tmp * 1.6777216 - 3.5067168e+09); 
  }
  else
    bintim = time(0);
  time_str = ctime(&bintim);
  time_out[0]  = time_str[8];
  time_out[1]  = time_str[9];
  time_out[2]  = '-';
  time_out[3]  = time_str[4];
  time_out[4]  = (char)(time_str[5] & 0xdf);
  time_out[5]  = (char)(time_str[6] & 0xdf);
  time_out[6]  = '-';
  time_out[7]  = time_str[20];
  time_out[8]  = time_str[21];
  time_out[9]  = time_str[22];
  time_out[10] = time_str[23];
  time_out[11] = ' ';
  time_out[12] = time_str[11];
  time_out[13] = time_str[12];
  time_out[14] = time_str[13];
  time_out[15] = time_str[14];
  time_out[16] = time_str[15];
  time_out[17] = time_str[16];
  time_out[18] = time_str[17];
  time_out[19] = time_str[18];
  time_out[20] = '.';
  time_out[21] = '0';
  time_out[22] = '0';
  StrCopyR(str,&slen,time_out);
  if (len) *len = slen;
  return 1;
}

int LibGetDvi(int *code, void *dummy1, struct descriptor *device, int *ans, struct descriptor *ans_string, int *len)
{
  *ans = 132;
  return 1;
}

int StrAppend(struct descriptor *out, struct descriptor *tail)
{
  if (tail->length != 0 && tail->pointer != NULL)
  {
    struct descriptor new = {0,DTYPE_T,CLASS_D,0};
    unsigned short len = (unsigned short)(out->length + tail->length);
    StrGet1Dx(&len,&new);
    memcpy(new.pointer, out->pointer, out->length);
    memcpy(new.pointer + out->length, tail->pointer, tail->length);
    StrFree1Dx(out);
    *out = new;
  }
  return 1;
}
    
int StrFree1Dx(struct descriptor *out)
{
  if (out->class == CLASS_D) {
    if (out->pointer)
      free(out->pointer);
    out->pointer = NULL;
    out->length = 0;
  }
  return 1;
}

int StrFindFirstNotInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    size_t tmp;
    tmp = strspn(src, s);
    ans = (int)((tmp == strlen(src)) ? 0 : (tmp + 1));
    free(src);
    free(s);
  }
  return ans;
}

int StrFindFirstInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    char *tmp;
    tmp = (char *)strpbrk(src, s);
    ans = tmp == NULL ? 0 : tmp - src + 1;
    free(src);
    free(s);
  }
  return ans;
}

struct node { void   *left;
              void   *right;
              short bal;};

struct bbtree_info { struct node  *currentnode;
                     char         *keyname;
		     int          (*compare_routine)();
		     int 	  (*alloc_routine)();
		     struct node  *new_node;
		     int           foundintree;
		     int           controlflags;
		     void         *user_context; };

static int MdsInsertTree();

int LibInsertTree(struct node **treehead,char *symbol_ptr, int *control_flags, int (*compare_rtn)(),
                    int (*alloc_rtn)(), struct node **blockaddr, void *user_data)
{
  struct bbtree_info bbtree;
  bbtree.currentnode = *treehead;
  bbtree.keyname = symbol_ptr;
  bbtree.compare_routine = compare_rtn;
  bbtree.alloc_routine = alloc_rtn;
  bbtree.new_node = NULL;
  bbtree.foundintree = 0;
  bbtree.controlflags = *control_flags;
  bbtree.user_context = user_data;
  MdsInsertTree(&bbtree);
  if (bbtree.foundintree == 1)
    *treehead = bbtree.currentnode;
  *blockaddr = bbtree.new_node;
  return bbtree.foundintree;
}

static int MdsInsertTree(struct bbtree_info *bbtree_ptr)
{

#define currentNode (bbtree_ptr->currentnode)
#define ALLOCATE    (*(bbtree_ptr->alloc_routine))
/*
#define left_of(node_ptr) (node_ptr->left ? (struct node *)((int)(node_ptr) + node_ptr->left) : 0)
#define right_of(node_ptr) (node_ptr->right ? (struct node *)((int)(node_ptr) + node_ptr->right) : 0)
#define offset_of(node_ptr, offnode_ptr) (offnode_ptr ? ((int)(offnode_ptr)-(int)(node_ptr)) : 0)
*/
#define left_of(node_ptr) node_ptr->left
#define right_of(node_ptr) node_ptr->right
#define offset_of(node_ptr, offnode_ptr) offnode_ptr

    struct node *save_current;
    int in_balance;
    struct node *down_left;
    struct node *down_right;

    if (currentNode == 0)
    {
	if (!(ALLOCATE(bbtree_ptr->keyname, &save_current, bbtree_ptr->user_context) & 1)) return 0;
	currentNode = save_current;
	currentNode->left = 0;
	currentNode->right = 0;
	currentNode->bal = 0;
	bbtree_ptr->new_node = save_current;
	bbtree_ptr->foundintree = 1;
	return 0;
    }
    save_current = currentNode;
    if ((in_balance = (*(bbtree_ptr->compare_routine))(bbtree_ptr->keyname, currentNode, bbtree_ptr->user_context)) <= 0)
    {
	if ( (in_balance == 0) && (!(bbtree_ptr->controlflags & 1)) )
	{
	    bbtree_ptr->new_node = save_current;
	    bbtree_ptr->foundintree = 3;
	    return 1;
	}
	currentNode = left_of(currentNode);
	in_balance = MdsInsertTree(bbtree_ptr);
	if ( (bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0) ) return 1;
	down_left = currentNode;
	currentNode = save_current;
	currentNode->left = offset_of(currentNode,down_left);
	if (in_balance) return 1;
	else
	{
	    currentNode->bal--;
	    if (currentNode->bal == 0) return 1;
	    else
	    {
		if (currentNode->bal & 1) return 0;
		down_left = left_of(currentNode);
		if (down_left->bal < 0)
		{
		    currentNode->left = offset_of(currentNode,right_of(down_left));
		    down_left->right = offset_of(down_left,currentNode);
		    currentNode->bal = 0;
		    currentNode = down_left;
		    currentNode->bal = 0;
		    return 1;
		}
		else
		{
		    down_right = right_of(down_left);
		    down_left->right = offset_of(down_left,left_of(down_right));
		    down_right->left = offset_of(down_right,down_left);
		    currentNode->left = offset_of(currentNode,right_of(down_right));
		    down_right->right = offset_of(down_right,currentNode);
		    currentNode->bal = 0;
		    down_left->bal = 0;
		    if (down_right->bal > 0) down_left->bal = -1;
		    else if (down_right->bal < 0) currentNode->bal = 1;
		    currentNode = down_right;
		    currentNode->bal = 0;
		    return 1;
		}
	    }
	}
    }
    else
    {
	currentNode = right_of(currentNode);
	in_balance = MdsInsertTree(bbtree_ptr);
	if ( (bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0) ) return 1;
	down_right = currentNode;
	currentNode = save_current;
	currentNode->right = offset_of(currentNode,down_right);
	if (in_balance)
	    return 1;
	else
	{
	    currentNode->bal++;
	    if (currentNode->bal == 0)
		return 1;
	    else
	    {
		if (currentNode->bal & 1) return 0;
		down_right = right_of(currentNode);
		if (down_right->bal > 0)
		{
		    currentNode->right = offset_of(currentNode,left_of(down_right));
		    down_right->left = offset_of(down_right,currentNode);
		    currentNode->bal = 0;
		    currentNode = down_right;
		    currentNode->bal = 0;
		    return 1;
		}
		else
		{
		    down_left = left_of(down_right);
		    down_right->left = offset_of(down_right,right_of(down_left));
		    down_left->right = offset_of(down_left,down_right);
		    currentNode->right = offset_of(currentNode,left_of(down_left));
		    down_left->left = offset_of(down_left,currentNode);
		    currentNode->bal = 0;
		    down_right->bal = 0;
		    if (down_left->bal < 0) down_right->bal = 1;
		    else if (down_left->bal > 0) currentNode->bal = -1;
		    currentNode = down_left;
		    currentNode->bal = 0;
		    return 1;
		}
	    }
	}
    }
}
#undef currentNode

int LibLookupTree(struct node **treehead, int *symbolstring, int (*compare_rtn)(), struct node **blockaddr)
{
  int ch_result;
  struct node *currentnode = *treehead;
  while (currentnode != 0)
  {
    if ( (ch_result = (*compare_rtn)(symbolstring,currentnode)) == 0)
    {
      *blockaddr = currentnode;
      return 1;
    }
    else if (ch_result < 0)
      currentnode = left_of(currentnode);
    else
      currentnode = right_of(currentnode);
  }
  return LibKEYNOTFOU;
}

static int MdsTraverseTree(int (*user_rtn)(), void *user_data, struct node *currentnode);

int LibTraverseTree(struct node **treehead, int (*user_rtn)(), void *user_data)
{ 
    return MdsTraverseTree(user_rtn, user_data, *treehead);
}

static int MdsTraverseTree(int (*user_rtn)(), void *user_data, struct node *currentnode)
{ 
    struct node *right_subtree;
    int status;
    if (currentnode == 0) return 1;
    if (left_of(currentnode))
    {
	status = MdsTraverseTree(user_rtn,user_data,left_of(currentnode));
	if (!(status & 1)) return status;
    }
    right_subtree = right_of(currentnode);
    status = (*user_rtn)(currentnode,user_data);
    if (!(status & 1)) return status;
    if (right_subtree)
    {
	status = MdsTraverseTree(user_rtn,user_data,right_subtree);
	if (!(status & 1)) return status;
    }
    return 1;
}

int StrCaseBlindCompare(struct descriptor *one, struct descriptor *two)
{
  int ans;
  char *one_c = MdsDescrToCstring(one);
  char *two_c = MdsDescrToCstring(two);
  int i;
  for (i=0;i<one->length;i++)
    one_c[i] = (char)toupper(one_c[i]);
  for (i=0;i<two->length;i++)
    two_c[i] = (char)toupper(two_c[i]);
  ans = strcmp(one_c,two_c);
  free(one_c);
  free(two_c);
  return ans;
}
unsigned int StrMatchWild(struct descriptor *candidate, struct descriptor *pattern)
{
  struct descr {
     int length;
     char *ptr;
  };
  struct descr cand;
  struct descr scand;
  struct descr pat;
  struct descr spat;
  char pc;
  int true;
  true = 1;
  cand.length = candidate->length;
  cand.ptr = candidate->pointer;
  scand = cand;
  pat.length = pattern->length;
  pat.ptr = pattern->pointer;
  spat = pat;
  scand.length = 0;

  while (true) {
    if (--pat.length < 0)
    {
      if (cand.length == 0)
        return StrMATCH;
      else 
      {
        if (--scand.length < 0)
          return StrNOMATCH;
        else {
          scand.ptr++;
          cand = scand;
          pat = spat;
        }
      }
    }
    else 
    {
      if ((pc = *pat.ptr++) == '*')
      {
        if (pat.length == 0) return StrMATCH;
        scand = cand;
        spat = pat;
      }
      else
      {
        if (--cand.length < 0) return StrNOMATCH;
        if (*cand.ptr++ != pc)
        {
          if (pc != '%')
          {
            if (--scand.length < 0)
              return StrNOMATCH;
            else {
              scand.ptr++;
              cand = scand;
              pat = spat;
            }
          }
        }
      } 
    }   
  }
}

#ifndef MAX
#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) < (b) ? (a) : (b)
#endif

int StrElement(struct descriptor *dest, int *num, struct descriptor *delim, struct descriptor *src)
{
  char *src_ptr = src->pointer;
  char *se_ptr = src_ptr+src->length;
  char *e_ptr;
  unsigned short len;
  int cnt;
  int status;
  if (delim->length != 1) return StrINVDELIM;
  for (cnt=0; (cnt<*num)&&(src_ptr<=se_ptr); src_ptr++)
    if (*src_ptr==*delim->pointer) cnt++;
  if (cnt < *num) return StrNOELEM;
  for(e_ptr=src_ptr; src_ptr<=se_ptr; src_ptr++)
    if (*src_ptr == *delim->pointer) break;
  len = (unsigned short)(src_ptr-e_ptr);
  status = StrCopyR(dest, &len, e_ptr);
  return status;
}

int StrTranslate(struct descriptor *dest, 
                  struct descriptor *src, 
                  struct descriptor *tran,
                  struct descriptor *match)
{
  char *dst = (char *)malloc(src->length);
  int i;
  int status;
  for (i=0;i<src->length;i++)
  {
    int j;
    int next = 1;
    for (j=0;next && j<match->length;j += next)
      next = (match->pointer[j] != src->pointer[i]) ? 1 : 0;
    dst[i] = (char)(next ? src->pointer[i] : ((j < tran->length) ? tran->pointer[j] : ' '));
  }   
  status = StrCopyR(dest, &src->length, dst);
  free(dst);
  return status;
}

int StrReplace(struct descriptor *dest, struct descriptor *src, int *start_idx, int *end_idx, struct descriptor *rep)
{
  int status;
  int start;
  int end;
  unsigned short int dstlen;
  char *dst;
  char *sptr;
  char *dptr;
  start = MAX(1,MIN(*start_idx, src->length));
  end = MAX(1,MIN(*end_idx, src->length));
  dstlen = (unsigned short) (start-1+rep->length+src->length-end+1);
  dst = (char *)malloc(dstlen);
  for (sptr=src->pointer, dptr=dst; (dptr-dst) < (start-1) ;*dptr++=*sptr++) ;
  for (sptr=rep->pointer; (sptr-rep->pointer) < rep->length ;*dptr++=*sptr++) ; 
  for (sptr=src->pointer+end; (sptr-src->pointer) < src->length ;*dptr++=*sptr++) ;
  status = StrCopyR(dest, &dstlen, dst);
  free(dst);
  return status;
}

#if  defined(_MSC_VER)
int LibSpawn(struct descriptor *cmd, struct descriptor *in, struct descriptor *out)
{
  char *cmd_c = MdsDescrToCstring(cmd);
  int status = _spawnlp(_P_WAIT, cmd_c, cmd_c, NULL);
  free(cmd_c);
  return (status == 0);
}

int LibWait(float *secs)
{
  int msec = (int)(*secs*1000.);
  Sleep(msec);
  return 1;
}

static char *GetRegistryPath(char *pathname)
{
  HKEY regkey1,regkey2,regkey3;
  unsigned char *path = NULL;
  if ( (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE",0,KEY_READ,&regkey1) == ERROR_SUCCESS) &&
       (RegOpenKeyEx(regkey1,"MIT",0,KEY_READ,&regkey2) == ERROR_SUCCESS) &&
       (RegOpenKeyEx(regkey2,"MDSplus",0,KEY_READ,&regkey3) == ERROR_SUCCESS) )
  {
    unsigned long valtype;
    unsigned long valsize;
    if (RegQueryValueEx(regkey3,pathname,0,&valtype,NULL,&valsize) == ERROR_SUCCESS)
    {
	  int plen;
	  valsize += 2;
      path = malloc(valsize);
      RegQueryValueEx(regkey3,pathname,0,&valtype,path,&valsize);
	  plen = strlen(path);
	  if (path[plen-1] != '\\')
	  {
		  path[plen++] = '\\';
		  path[plen] = '\0';
	  }
    }
  }
  RegCloseKey(regkey1);
  RegCloseKey(regkey2);
  RegCloseKey(regkey3);
  return (char *)path;
}

typedef struct findFileCtx { HANDLE hand;
		             char *path;
                             char *pathstart;
                             char *pathend;
                             char *name;
		             char *searchname;
                           } FindFileCtx;

static int FindNextInPath(FindFileCtx *ffctx,WIN32_FIND_DATA *fdata)
{
  while((ffctx->hand == INVALID_HANDLE_VALUE) && (ffctx->path < ffctx->pathend))
  {
    char filename[512];
    strcpy(filename,ffctx->path);
    strcat(filename,ffctx->name);
    ffctx->hand = FindFirstFile(filename, fdata);
    if (ffctx->hand == INVALID_HANDLE_VALUE)
    {
      ffctx->path += strlen(ffctx->path);
      while ((ffctx->path < ffctx->pathend) && (*ffctx->path == '\0')) ffctx->path++;
    }
  }
  return (ffctx->hand != INVALID_HANDLE_VALUE);
}

int LibFindFile(struct descriptor *filespec, struct descriptor *result, void **ctx)
{
  BOOL status = FALSE;
  WIN32_FIND_DATA fdata;
  FindFileCtx *ffctx = *(FindFileCtx **)ctx;
  if (ffctx == NULL)
  {
    ffctx = malloc(sizeof(FindFileCtx));
    ffctx->searchname = MdsDescrToCstring(filespec);
    ffctx->hand = FindFirstFile(ffctx->searchname, &fdata);
    ffctx->pathstart = NULL;
    if (ffctx->hand == INVALID_HANDLE_VALUE)
    {
      ffctx->name=strchr(ffctx->searchname,':');
      if (ffctx->name != NULL)
      {
        *ffctx->name = '\0';
        ffctx->name++;
        ffctx->path = ffctx->pathstart = GetRegistryPath(ffctx->searchname);
        if (ffctx->path != NULL)
        {
          char *delim;
          ffctx->pathend = ffctx->path + strlen(ffctx->path);
          while((delim=strrchr(ffctx->path,';')) != NULL) *delim='\0';
          status = FindNextInPath(ffctx,&fdata);
        }
      }
    }
    else
    {
      char *delim;
      status = TRUE;
      ffctx->path = ffctx->searchname;
      if ((delim=strrchr(ffctx->path,'\\')) != NULL)
        *(delim + 1) = '\0';
    }
    if (status == TRUE)
      *ctx = (void *)ffctx;
  }
  else
  {
    status = FindNextFile(ffctx->hand, &fdata);
    if (!status)
    {
      FindClose(ffctx->hand);
      status = FindNextInPath(ffctx,&fdata);
    }
  }
  if (status) {
    struct descriptor pathdsc = {0,DTYPE_T,CLASS_S,0};
    struct descriptor namedsc = {0,DTYPE_T,CLASS_S,0};
    pathdsc.length = (unsigned short)strlen(ffctx->path);
    pathdsc.pointer = ffctx->path;
    namedsc.length = (unsigned short)strlen(fdata.cFileName);
    namedsc.pointer = fdata.cFileName;
	StrCopyDx(result, &pathdsc);
    StrAppend(result,&namedsc);
  }
  else
  {
    if (ffctx->pathstart) free(ffctx->pathstart);
    free(ffctx->searchname);
    free(ffctx);
    *ctx = NULL;
  }
  return (int)status;
}

int LibFindFileRecurseCaseBlind(struct descriptor *filespec, struct descriptor *result, void **ctx)
{
  return LibFindFile(filespec, result, ctx);
}

int LibFindFileEnd(void **ctx)
{
  FindFileCtx *ffctx = *(FindFileCtx **)ctx;
  if (ffctx != NULL)
  {
    FindClose(ffctx->hand);
    if (ffctx->pathstart) free(ffctx->pathstart);
    free(ffctx->searchname);
    free(ffctx);
    *ctx = NULL;
  }
  return 1;
}

int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, FARPROC *symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  HINSTANCE handle;
  *symbol_value = (FARPROC)0;
  handle = LoadLibrary(c_filename);
  if (handle != 0)
  {
    char *c_symbol = MdsDescrToCstring(symbol);
    *symbol_value = GetProcAddress(handle, c_symbol);
    free(c_symbol);
  }
  free(c_filename);
  if (*symbol_value == (FARPROC)0)
    return LibKEYNOTFOU;
  else
    return 1;  
}

unsigned int LibCallg(void **arglist, FARPROC *routine)
{
  int a_idx;

  unsigned int retval;
  for (a_idx=*(int *)arglist; a_idx > 0; a_idx--)
  {

	  void *arg = arglist[a_idx];
    __asm mov eax, arg

	__asm push eax
  }
  __asm call routine
  __asm mov retval, eax
  for (a_idx=*(int *)arglist; a_idx > 0; a_idx--) __asm pop eax

  return retval;
}
#elif defined(__osf__) || defined (__hpux) || defined(__irix__) || defined(__sunos__)
unsigned int LibCallg(void **arglist, unsigned int (*routine)())
{
  switch (*(int *)arglist & 0xff)
  {
    case 0:  return (*routine)();
    case 1:  return (*routine)(arglist[1]);
    case 2:  return (*routine)(arglist[1],arglist[2]);
    case 3:  return (*routine)(arglist[1],arglist[2],arglist[3]);
    case 4:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4]);
    case 5:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5]);
    case 6:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6]);
    case 7:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7]);
    case 8:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8]);
    case 9:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9]);
    case 10: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10]);
    case 11: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11]);
    case 12: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12]);
    case 13: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13]);
    case 14: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14]);
    case 15: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15]);
    case 16: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16]);
    case 17: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17]);
    case 18: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18]);
    case 19: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19]);
    case 20: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20]);
    case 21: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21]);
    case 22: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22]);
    case 23: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23]);
    case 24: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24]);
    case 25: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25]);
    case 26: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26]);
    case 27: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27]);
    case 28: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28]);
    case 29: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29]);
    case 30: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30]);
    case 31: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30],arglist[31]);
    case 32: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30],arglist[31],arglist[32]);
    default: printf("Error - currently no more than 32 arguments supported on external calls\n");
  }
  return 0;
}

#if defined(__osf__) || defined(__irix__) || defined(__sunos__)
int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, void **symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  char *full_filename = malloc(strlen(c_filename) + 10);
  void *handle;
  *symbol_value = NULL;
  strcpy(full_filename,"lib");
  strcat(full_filename,c_filename);
  strcat(full_filename,".so");
  handle = dlopen(full_filename,RTLD_LAZY);
  if (handle != NULL)
  {
    char *c_symbol = MdsDescrToCstring(symbol);
    *symbol_value = dlsym(handle,c_symbol);
    free(c_symbol);
  }
  free(c_filename);
  free(full_filename);
  if (*symbol_value == NULL)
    return LibKEYNOTFOU;
  else
    return 1;  
}

#else /* __osf__ */
int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, void **symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  char *full_filename = malloc(strlen(c_filename) + 10);
  shl_t handle;
  *symbol_value = NULL;
  strcpy(full_filename,"lib");
  strcat(full_filename,c_filename);
  strcat(full_filename,".sl");
  handle = shl_load(full_filename,BIND_DEFERRED | BIND_NOSTART | DYNAMIC_PATH,0);
  if (handle != NULL)
  {
    char *c_symbol = MdsDescrToCstring(symbol);
    int s = shl_findsym(&handle,c_symbol,0,symbol_value);
    free(c_symbol);
  }
  free(c_filename);
  free(full_filename);
  if (*symbol_value == NULL)
    return LibKEYNOTFOU;
  else
    return 1;  
}
#endif
typedef struct {
  char *env;
  char *file;
  struct descriptor wild_descr;
  char **env_strs;
  int  num_env;
  int  next_index;
  int  next_dir_index; /* index intor env_strs to put the next directory to search */
  DIR  *dir_ptr;
} FindFileCtx;


static int FindFile(struct descriptor *filespec, struct descriptor *result, int **ctx, int recursively, int caseBlind);
static int FindFileStart(struct descriptor *filespec, FindFileCtx **ctx, int caseBlind);
static int FindFileEnd(FindFileCtx *ctx);
static char *FindNextFile(FindFileCtx *ctx, int recursively, int caseBlind);

extern int LibFindFile(struct descriptor *filespec, struct descriptor *result, int **ctx)
{
  return FindFile(filespec, result, ctx, 0, 0);
}
extern int LibFindFileRecurseCaseBlind(struct descriptor *filespec, struct descriptor *result, int **ctx)
{
  return FindFile(filespec, result, ctx, 1, 1);
}
static int FindFile(struct descriptor *filespec, struct descriptor *result, int **ctx, int recursively, int caseBlind)
{
  unsigned int status;
  char *ans;
  if (*ctx == 0) {
    status = FindFileStart(filespec, (FindFileCtx **)ctx, caseBlind);
    if ((status&1) ==0)
      return status;
  }
  ans = FindNextFile((FindFileCtx *)*ctx, recursively, caseBlind);
  if (ans != 0) {
    static struct descriptor ansd = {0, DTYPE_T, CLASS_S,0};
    ansd.length = strlen(ans);
    ansd.pointer = ans;
    StrCopyDx(result,&ansd);
    free(ans);
    status = 1;
  }
  else {
    status = 0;
    LibFindFileEnd(ctx);    
  }
  return status;
}

extern int LibFindFileEnd(int **ctx)
{
  int status = FindFileEnd((FindFileCtx *)*ctx);
  if (status) *ctx=NULL;
  return status;
}

static int FindFileEnd(FindFileCtx *ctx)
{
  int i;
  if (ctx != NULL) {
    if (ctx->dir_ptr) {
      closedir(ctx->dir_ptr);
      ctx->dir_ptr = 0;
    }
    free (ctx->env);
    free (ctx->file);
    for (i=0; i<ctx->num_env; i++)
      free(ctx->env_strs[i]);
    free(ctx->env_strs);
    free(ctx);
  }
  return 1;
}

#define CSTRING_FROM_DESCRIPTOR(cstring, descr)\
  cstring=strncpy(malloc(descr->length+1),descr->pointer,descr->length);\
  cstring[descr->length] = '\0';

static int FindFileStart(struct descriptor *filespec, FindFileCtx **ctx, int caseBlind)
{
  FindFileCtx *lctx;
  char *fspec;
  char *colon;
  *ctx = (FindFileCtx *)malloc(sizeof(FindFileCtx));
  lctx = *ctx;

  CSTRING_FROM_DESCRIPTOR(fspec, filespec)

  lctx->next_index = lctx->next_dir_index = 0;
  colon = (char *)index(fspec, ':');
  if (colon == 0) {
    lctx->env = 0;
    colon = fspec-1;
  }
  else {
    lctx->env = strncpy(malloc(colon-fspec+1),fspec,colon-fspec);
    lctx->env[colon-fspec] = '\0';
  }
  if (strlen(colon+1)==0) {
    if (lctx->env) free(lctx->env);
    free(fspec);
    return 0;    
  }
  else {
    lctx->file = malloc(strlen(colon+1));
    strcpy(lctx->file, colon+1);
    lctx->wild_descr.length = strlen(colon+1);
    lctx->wild_descr.pointer=lctx->file;
    lctx->wild_descr.dtype = DTYPE_T;
    lctx->wild_descr.class = CLASS_S;
    if (caseBlind)
      StrUpcase(&lctx->wild_descr,&lctx->wild_descr);
    free(fspec);
  }
  if (lctx->env != 0) {
    int num = 0;
    char *env;
    char *semi;
    env = getenv(lctx->env);
    if (env != 0) {
      if (env[strlen(env)-1] != ';') {
        char *tmp = malloc(strlen(env)+2);
        strcpy(tmp, env);
	strcat(tmp, ";");
        env = tmp;
      }
      else {
	char *tmp = malloc(strlen(env)+1);
        strcpy(tmp, env);
        env = tmp;
      }
      for(semi=(char *)index(env, ';'); semi!= 0; num++, semi=(char *)index(semi+1, ';'));
      if (num > 0) {
        char *ptr;
        int i;
        lctx->num_env = num;
        lctx->env_strs = (char **)malloc(num*sizeof(char *));
        for (ptr=env,i=0; i<num; i++) {
          char *cptr;
          int len = ((cptr= (char *)index(ptr, ';'))==0) ? strlen(ptr) : cptr-ptr; 
	  lctx->env_strs[i] = strncpy(malloc(len+1),ptr,len);
          lctx->env_strs[i][len] = '\0';
          ptr=cptr+1;
	}
      }
      free(env);
    }
  }
  lctx->dir_ptr = 0;
  return 1;
}

static char *FindNextFile(FindFileCtx *ctx, int recursively, int caseBlind)
{
  char *ans;
  struct dirent *dp;

  int found = 0;
  while (! found) {
    while (ctx->dir_ptr==0)
      if (ctx->next_index < ctx->num_env) {
        ctx->dir_ptr=opendir(ctx->env_strs[ctx->next_index++]);
	ctx->next_dir_index = ctx->next_index;
      }
      else
        return 0;
    dp = readdir(ctx->dir_ptr);
    if (dp != NULL) {
      struct descriptor upname = {0,DTYPE_T,CLASS_D,0};
      DESCRIPTOR_FROM_CSTRING(filename, dp->d_name)
      if (caseBlind)
	{
        StrUpcase(&upname,&filename);
      }
      else {
	StrCopyDx(&upname,&filename);
      }
      found = StrMatchWild(&upname, &ctx->wild_descr)&1;
      StrFree1Dx(&upname);
      if (recursively) {
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
          char *tmp_dirname;
	  DIR *tmp_dir;
	  tmp_dirname = malloc(strlen(ctx->env_strs[ctx->next_index-1])+2+strlen(dp->d_name));
          strcpy(tmp_dirname, ctx->env_strs[ctx->next_index-1]);
          strcat(tmp_dirname, "/");
	  strcat(tmp_dirname, dp->d_name);
	  tmp_dir = opendir(tmp_dirname);
	  if (tmp_dir != NULL) {
	    int i;
	    char **tmp;
	    closedir(tmp_dir);
	    ctx->env_strs = realloc(ctx->env_strs, sizeof(char*)*(ctx->num_env+1));
	    for(i=ctx->num_env-1; i>=ctx->next_dir_index; i--)
	      ctx->env_strs[i+1] = ctx->env_strs[i];
	    ctx->env_strs[ctx->next_dir_index] = tmp_dirname;
	    ctx->num_env++;
	    ctx->next_dir_index++;
	  }
          else
	    free(tmp_dirname);
	}
      }
    }
    else {
     closedir(ctx->dir_ptr);
     ctx->dir_ptr = NULL;
    }
  }
  ans = malloc(strlen(ctx->env_strs[ctx->next_index-1])+1+strlen(dp->d_name)+1);
  strcpy(ans, ctx->env_strs[ctx->next_index-1]);
  strcat(ans, "/");
  strcat(ans, dp->d_name);
  return ans;
}

int LibSpawn(struct descriptor *cmd, struct descriptor *in, struct descriptor *out)
{
  printf("LibSpawn not yet supported\n");
  return 0;
}

int LibWait(float *secs)
{
  sleep((unsigned int)*secs);
  return 1;
}
#endif
