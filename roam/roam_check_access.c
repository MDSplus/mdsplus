/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <mdsdescrip.h>
#include <mdsshr.h>
#ifdef XIO
#include "globus_common.h"
#include "gssapi.h"
#include "globus_gss_assist.h"
#include "globus_gridmap_callout_error.h"
#include <stdlib.h>
#include "globus_xio.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"
#include "globus_xio_http.h"
#define LINE_LEN 1024
#else
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

static char *urlencode(char *in)
{
  char *ans = 0;
  int i;
  int j = 0;
  int inlen = strlen(in);
  ans = malloc(inlen * 3);
  for (i = 0, j = 0; i < inlen; i++) {
    if ((in[i] >= '0' && in[i] <= '9') ||
	(in[i] >= 'A' && in[i] <= 'Z') || (in[i] >= 'a' && in[i] <= 'z')) {
      ans[j++] = in[i];
    } else if (in[i] == ' ') {
      ans[j++] = '+';
    } else {
      ans[j++] = '%';
      sprintf(&ans[j], "%2X", (unsigned int)in[i]);
      j += 2;
    }
  }
  ans[j] = 0;
  return ans;
}

#ifdef XIO
EXPORT int roam_check_access(char *host, int https, char *resource, char *permit, char *dn,
		      struct descriptor_xd *aux)
{
  globus_xio_driver_t tcp_driver;
  globus_xio_driver_t gsi_driver;
  globus_xio_driver_t http_driver;
  globus_xio_stack_t stack;
  globus_xio_handle_t xio_handle;
  globus_xio_attr_t attr;
  globus_result_t res;
  char line[LINE_LEN];
  globus_size_t nbytes;
  char *url;
  char *ans = 0;
  int status = GLOBUS_FAILURE;

  url = malloc(strlen(host) + 30 + (strlen(resource) + strlen(permit) + strlen(dn)) * 3);
  strcpy(url, https ? "https://" : "http://");
  strcat(url, host);
  strcat(url, "/%2fcheck_access_aux.php?");
  strcat(url, "rname=");
  strcat(url, urlencode(resource));
  strcat(url, "&");
  strcat(url, "permission=");
  strcat(url, urlencode(permit));
  strcat(url, "&");
  strcat(url, "uname=");
  strcat(url, urlencode(dn));
  printf("url=%s\n", url);
  globus_module_activate(GLOBUS_XIO_MODULE);
  globus_xio_stack_init(&stack, NULL);
  globus_xio_driver_load("tcp", &tcp_driver);
  globus_xio_stack_push_driver(stack, tcp_driver);
  if (https) {
    globus_xio_driver_load("gsi", &gsi_driver);
    globus_xio_stack_push_driver(stack, gsi_driver);
  }
  globus_xio_driver_load("http", &http_driver);
  globus_xio_stack_push_driver(stack, http_driver);
  globus_xio_attr_init(&attr);
  if (https) {
    globus_xio_attr_cntl(attr, gsi_driver, GLOBUS_XIO_GSI_SET_SSL_COMPATIBLE, GLOBUS_TRUE);
  }
  res = globus_xio_handle_create(&xio_handle, stack);
  //  test_res(res);
  res = globus_xio_open(xio_handle, url, attr);
  //test_res(res);
  while (res == GLOBUS_SUCCESS) {
    nbytes = 0;
    res = globus_xio_read(xio_handle, line, LINE_LEN, 1, &nbytes, NULL);
    if (nbytes > 0) {
      if (ans) {
	int newlen = strlen(ans) + nbytes;
	ans = strncat(realloc(ans, newlen + 1), line, nbytes);
	ans[newlen] = 0;
      } else {
	ans = strncpy(malloc(nbytes + 1), line, nbytes);
	ans[nbytes] = 0;
      }
    }
    if (res)
      break;
  }
  if (ans && strlen(ans) > 4) {
    if (strncmp(ans, "yes", 3) == 0) {
      struct descriptor accnt = { 0, DTYPE_T, CLASS_S, 0 };
      status = GLOBUS_SUCCESS;
      accnt.pointer = malloc(strlen(ans) - 4);
      if (strlen(ans) > 5)
	strncpy(accnt.pointer, &ans[4], strlen(ans) - 4);
      accnt.pointer =[strlen(ans) - 5] = 0;
      accnt.length = strlen(accnt.pointer);
      MdsCopyXdDx(&accnt, aux);
      free(accnt.pointer);
    }
  }
  if (ans)
    free(ans);
  globus_xio_attr_destroy(attr);
  globus_xio_close(xio_handle, NULL);
  return status;
}
#else

struct _buf {
  size_t size;
  char *ptr;
};

static size_t callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  char *ptr = (char *)contents;
  struct _buf *buf = (struct _buf *)userp;
  if (buf->size == 0) {
    buf->ptr = malloc(size * nmemb + 1);
    memcpy(buf->ptr, ptr, size * nmemb);
  } else {
    buf->ptr = realloc(buf->ptr, buf->size + size * nmemb + 1);
    memcpy(buf->ptr + buf->size, ptr, size * nmemb);
  }
  buf->size += size * nmemb;
  buf->ptr[buf->size] = 0;
  return size * nmemb;
}

EXPORT int roam_check_access(char *host, int https, char *resource, char *permit, char *dn,
		      struct descriptor_xd *aux)
{
  CURL *ctx = curl_easy_init();
  CURLcode status;
  struct _buf buf = { 0, 0 };
  char *url = malloc(strlen(host) + 100 + (strlen(resource) + strlen(permit) + strlen(dn)) * 3);
  char *res = urlencode(resource);
  char *perm = urlencode(permit);
  char *uname = urlencode(dn);
  sprintf(url, "http%s://%s/roam.php?check_access&rname=%s&permission=%s&uname=%s",
	  https ? "s" : "", host, res, perm, uname);
  free(res);
  free(perm);
  free(uname);
  curl_easy_setopt(ctx, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(ctx, CURLOPT_TIMEOUT, 15);
  curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, callback);
  curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &buf);
  curl_easy_setopt(ctx, CURLOPT_URL, url);
  curl_easy_setopt(ctx, CURLOPT_CAPATH, "/etc/grid-security/certificates");
  status = curl_easy_perform(ctx);
  curl_easy_cleanup(ctx);
  if (status == 0 && buf.size >= 4) {
    if (strncmp(buf.ptr, "yes", 3) == 0) {
      if (buf.size > 4) {
	struct descriptor accnt = { 0, DTYPE_T, CLASS_S, 0 };
	accnt.pointer = malloc(buf.size - 4);
	if (buf.size > 5)
	  strncpy(accnt.pointer, &buf.ptr[4], buf.size - 4);
	accnt.pointer[buf.size - 4] = 0;
	accnt.length = strlen(accnt.pointer);
	MdsCopyDxXd(&accnt, aux);
      }
    } else
      status = -2;
  } else {
    status = -1;
  }
  if (buf.ptr)
    free(buf.ptr);
  return status;
}
#endif
