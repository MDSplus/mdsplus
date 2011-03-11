
#include "mdsip_connections.h"
#include <STATICdef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

STATIC_ROUTINE int MdsLoginVMS(int id, char *username, char *password)
{
  struct descrip loginget_arg;
  struct descrip loginpwd_arg;
  struct descrip pwd_arg;
  struct descrip ansarg;
  int status;
  ansarg.ptr = 0;
  status = MdsValue(id, LOGINREQUEST, MakeDescrip(&loginget_arg,DTYPE_CSTRING,0,0,LOGINGETP1), &ansarg, NULL);
  if (status & 1)
  {
    if (ansarg.ptr && ansarg.dtype == DTYPE_CHAR)
    {
      char alg = *(char *)ansarg.ptr;
      free(ansarg.ptr);
      status = MdsValue(id, LOGINREQUEST, MakeDescrip(&loginget_arg,DTYPE_CSTRING,0,0,LOGINGETP2), &ansarg, NULL);
      if (status & 1)
      {
        if (ansarg.ptr && ansarg.dtype == DTYPE_SHORT)
        {
          struct dscr { short length; char dtype; char class; void *pointer;};
          int hash[2];
          int i;
          struct dscr hashd = {8, 9, 1, 0};
          struct dscr pwdd = {0,14,1,0};
          struct dscr userd = {0,14,1,0};
          short salt = *(short *)ansarg.ptr;
          free(ansarg.ptr);
          ansarg.ptr = 0;
          hashd.pointer = hash;
          pwdd.length = strlen(password);
          pwdd.pointer = strcpy((char *)malloc(pwdd.length+1),password);
          userd.length = strlen(username);
          userd.pointer = strcpy((char *)malloc(userd.length+1),username);
          for (i=0;i<pwdd.length;i++)
            ((char *)pwdd.pointer)[i] = __toupper(((char *)pwdd.pointer)[i]);
          for (i=0;i<userd.length;i++)
            ((char *)userd.pointer)[i] = __toupper(((char *)userd.pointer)[i]);
          status = Lgihpwd(&hashd,&pwdd,alg,salt,&userd);
          if (status & 1)
          {
            int two = 2;
            status = MdsValue(id, LOGINREQUEST, MakeDescrip(&loginpwd_arg,DTYPE_CSTRING,0,0,LOGINPWD),
                                                  MakeDescrip(&pwd_arg,DTYPE_LONG,1,&two,hash), &ansarg, NULL);
          }
          else
            printf("Login error: Error checking password\n");
        }
        else
        {
          if (ansarg.ptr)
          {
            free(ansarg.ptr);
            ansarg.ptr = 0;
          }
          printf("Login error: Unrecognized response from server\n");
          status = 0;
        }
      }
    }
    else
    {
      if (ansarg.ptr)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
      }
      printf("Login error: Unrecognized response from server\n");
      status = 0;
    }
  }
  if (!(status & 1) && ansarg.ptr) printf("Login error: %s\n",(char *)ansarg.ptr);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int MdsLogin(int id, char *username, char *password)
{
  struct descrip loginuser_arg;
  struct descrip userarg;
  struct descrip ansarg;
  int status;
  ansarg.ptr = 0;
  status = MdsValue(id, LOGINREQUEST, MakeDescrip(&loginuser_arg,DTYPE_CSTRING,0,0,LOGINUSER),
                                        MakeDescrip(&userarg,DTYPE_CSTRING,0,0,username), &ansarg, NULL);
  if (status & 1)
  {
    int typelen = strlen(LOGINVMS);
    if (ansarg.dtype == DTYPE_CSTRING && ansarg.length == typelen && ansarg.ptr)
    {
      if (strncmp(ansarg.ptr,LOGINVMS,typelen)==0)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
        status = MdsLoginVMS(id, username, password);
      }
      else
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
        printf("Login error: Unrecognized response from server\n");
        status = 0;
      }
    }
    else
    {
      if (ansarg.ptr)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
      }
      printf("Login error: Unrecognized response from server\n");
      status = 0;
    }
  }
  if (!(status & 1) && ansarg.ptr) printf("Login error: %s\n",(char *)ansarg.ptr);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
