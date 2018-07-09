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
#include <globus_common.h>
#include <gssapi.h>
#include <globus_gss_assist.h>
#include <globus_gridmap_callout_error.h>
#include <stdlib.h>

#ifdef HAVE_STDARG_H
#undef HAVE_STDARG_H
#endif
#ifdef HAVE_GETADDRINFO
#undef HAVE_GETADDRINFO
#endif
#ifdef HAVE_GETPWUID
#undef HAVE_GETPWUID
#endif
#ifdef SIZEOF_LONG
#undef SIZEOF_LONG
#endif

#include <mdsdescrip.h>
#include <mds_stdarg.h>

extern int TdiExecute();
extern int MdsFree1Dx();

/**
 * @mainpage Roam Callout
 */

/**
 * @defgroup globus_gridmap_callout Globus Gridmap Callout
 */

/**
 * Globus Gridmap Callout Function
 * @ingroup globus_gridmap_callout
 */
/* @{ */
/**
 * Gridmap Authorization Callout Function
 *
 * This function provides a gridmap lookup in callout form.
 *
 * @param ap
 *        This function, like all functions using the Globus Callout API, is 
 *        passed parameter though the variable argument list facility. The
 *        actual arguments that are passed are:
 *
 *        - The GSS Security context established during service
 *          invocation. This parameter is of type gss_ctx_id_t.
 *        - The name of the service being invoced. This parameter should be
 *          passed as a NUL terminated string. If no service string is
 *          available a value of NULL should be passed in its stead. This
 *          parameter is of type char *
 *        - A NUL terminated string indicating the desired local identity. If
 *          no identity is desired NULL may be passed. In this case the first
 *          local identity that is found will be returned. This parameter is of
 *          type char *.
 *        - A pointer to a buffer. This buffer will contain the mapped (local)
 *          identity (NUL terminated string) upon successful return. This
 *          parameter is of type char *.
 *        - The length of the above mentioned buffer. This parameter is of type
 *          unsigned int.
 *
 * @return
 *        GLOBUS_SUCCESS upon success
 *        A globus result structure upon failure (needs to be defined better)
 */

int roam_check_access();

EXPORT globus_result_t roam_gridmap_callout(va_list ap)
{
  gss_ctx_id_t context;
  char *service;
  char *desired_identity;
  char *identity_buffer;
  char *local_identity;
  unsigned int buffer_length;
  globus_result_t result = GLOBUS_SUCCESS;
  gss_name_t peer;
  gss_buffer_desc peer_name_buffer;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  int rc;
  int initiator;

  context = va_arg(ap, gss_ctx_id_t);
  service = va_arg(ap, char *);
  desired_identity = va_arg(ap, char *);
  identity_buffer = va_arg(ap, char *);
  buffer_length = va_arg(ap, unsigned int);

  rc = globus_module_activate(GLOBUS_GSI_GSS_ASSIST_MODULE);

  /* check rc */

  rc = globus_module_activate(GLOBUS_GSI_GSSAPI_MODULE);

  /* check rc */

  rc = globus_module_activate(GLOBUS_GRIDMAP_CALLOUT_ERROR_MODULE);

  /* check rc */

  major_status = gss_inquire_context(&minor_status,
				     context,
				     GLOBUS_NULL,
				     GLOBUS_NULL,
				     GLOBUS_NULL,
				     GLOBUS_NULL, GLOBUS_NULL, &initiator, GLOBUS_NULL);

  if (GSS_ERROR(major_status)) {
    GLOBUS_GRIDMAP_CALLOUT_GSS_ERROR(result, major_status, minor_status);
    goto error;
  }

  major_status = gss_inquire_context(&minor_status,
				     context,
				     initiator ? GLOBUS_NULL : &peer,
				     initiator ? &peer : GLOBUS_NULL,
				     GLOBUS_NULL,
				     GLOBUS_NULL, GLOBUS_NULL, GLOBUS_NULL, GLOBUS_NULL);

  if (GSS_ERROR(major_status)) {
    GLOBUS_GRIDMAP_CALLOUT_GSS_ERROR(result, major_status, minor_status);
    goto error;
  }

  major_status = gss_display_name(&minor_status, peer, &peer_name_buffer, GLOBUS_NULL);

  if (GSS_ERROR(major_status)) {
    GLOBUS_GRIDMAP_CALLOUT_GSS_ERROR(result, major_status, minor_status);
    gss_release_name(&minor_status, &peer);
    goto error;
  }

  gss_release_name(&minor_status, &peer);

  if (desired_identity == NULL) {
    struct descriptor expression_d = { 0, DTYPE_T, CLASS_S, 0 };
    struct descriptor local_user_d = { 0, DTYPE_T, CLASS_D, 0 };
    int status;
    char *expression = malloc(strlen(service) + strlen(peer_name_buffer.value) + 23);
    if (expression) sprintf(expression, "_who=check_access_%s(\"%s\")", service, (char *)peer_name_buffer.value);
    expression_d.length = strlen(expression);
    expression_d.pointer = expression;
    status = TdiExecute(&expression_d, &local_user_d MDS_END_ARG);
    if (status & 1) {
      rc = GLOBUS_SUCCESS;
      local_identity =
	  strncpy((char *)malloc(local_user_d.length + 1), local_user_d.pointer,
		  local_user_d.length);
      local_identity[local_user_d.length] = 0;
      MdsFree1Dx(&local_user_d, 0);
    } else {
      rc = globus_gss_assist_gridmap(peer_name_buffer.value, &local_identity);
    }
    if (rc != 0) {
      GLOBUS_GRIDMAP_CALLOUT_ERROR(result,
				   GLOBUS_GRIDMAP_CALLOUT_LOOKUP_FAILED,
				   ("Could not map %s\n", peer_name_buffer.value));
      gss_release_buffer(&minor_status, &peer_name_buffer);
      goto error;
    }

    if (strlen(local_identity) + 1 > buffer_length) {
      GLOBUS_GRIDMAP_CALLOUT_ERROR(result,
				   GLOBUS_GRIDMAP_CALLOUT_BUFFER_TOO_SMALL,
				   ("Local identity length: %d Buffer length: %d\n",
				    strlen(local_identity), buffer_length));
    } else {
      strcpy(identity_buffer, local_identity);
    }
    free(local_identity);
  } else {
    GLOBUS_GRIDMAP_CALLOUT_ERROR(result,
				 GLOBUS_GRIDMAP_CALLOUT_LOOKUP_FAILED,
				 ("Could not map %s to %s\n",
				  peer_name_buffer.value, desired_identity));
  }

  gss_release_buffer(&minor_status, &peer_name_buffer);

 error:

  globus_module_deactivate(GLOBUS_GSI_GSSAPI_MODULE);
  globus_module_deactivate(GLOBUS_GSI_GSS_ASSIST_MODULE);
  return result;
}

/* @} */
