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
/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "mdsplus/mdsconfig.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mdsplus.h"
#include <ipdesc.h>

/* If you declare any globals in php_mdsplus.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mdsplus)
*/

/* True global resources - no need for thread safety here */
static int le_mdsplus;

static char *mdsplus_error_msg = NULL;

/* {{{ mdsplus_functions[]
 *
 * Every user visible function must have an entry in mdsplus_functions[].
 */
function_entry mdsplus_functions[] = {
  PHP_FE(mdsplus_connect, NULL)
      PHP_FE(mdsplus_open, NULL)
      PHP_FE(mdsplus_close, NULL)
      PHP_FE(mdsplus_put, NULL)
      PHP_FE(mdsplus_value, NULL)
      PHP_FE(mdsplus_disconnect, NULL)
  PHP_FE(mdsplus_error, NULL) {NULL, NULL, NULL}	/* Must be the last line in mdsplus_functions[] */
};

/* }}} */

/* {{{ mdsplus_module_entry
 */
zend_module_entry mdsplus_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "mdsplus",
  mdsplus_functions,
  PHP_MINIT(mdsplus),
  PHP_MSHUTDOWN(mdsplus),
  PHP_RINIT(mdsplus),		/* Replace with NULL if there's nothing to do at request start */
  PHP_RSHUTDOWN(mdsplus),	/* Replace with NULL if there's nothing to do at request end */
  PHP_MINFO(mdsplus),
#if ZEND_MODULE_API_NO >= 20010901
  "0.1",			/* Replace with version number for your extension */
#endif
  STANDARD_MODULE_PROPERTIES
};

/* }}} */

#ifdef COMPILE_DL_MDSPLUS
ZEND_GET_MODULE(mdsplus)
#endif
/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mdsplus.global_value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_mdsplus_globals, mdsplus_globals)
    STD_PHP_INI_ENTRY("mdsplus.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mdsplus_globals, mdsplus_globals)
PHP_INI_END()
*/
/* }}} */
/* {{{ php_mdsplus_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mdsplus_init_globals(zend_mdsplus_globals *mdsplus_globals)
{
	mdsplus_globals->global_value = 0;
	mdsplus_globals->global_string = NULL;
}
*/
/* }}} */
static void mdsplus_replace_error(char *msg, int do_not_copy)
{
  if (mdsplus_error_msg)
    free(mdsplus_error_msg);
  if (do_not_copy)
    mdsplus_error_msg = msg;
  else
    mdsplus_error_msg = strcpy(malloc(strlen(msg) + 1), msg);
}

static char *mdsplus_translate_status(int socket, int status)
{
  char expression[100];
  struct descrip ans;
  int tmpstat;
  sprintf(expression, "getmsg(%d)", status);
  tmpstat = MdsValue(socket, expression, &ans, NULL);
  if (tmpstat & 1 && ans.ptr)
    return (char *)ans.ptr;
  else {
    char *msg = malloc(100);
    sprintf(msg, "Unable to determine error string, error status=%d", status);
    return msg;
  }
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mdsplus)
{
  /* If you have INI entries, uncomment these lines 
     ZEND_INIT_MODULE_GLOBALS(mdsplus, php_mdsplus_init_globals, NULL);
     REGISTER_INI_ENTRIES();
   */
  return SUCCESS;
}

/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mdsplus)
{
  /* uncomment this line if you have INI entries
     UNREGISTER_INI_ENTRIES();
   */
  return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mdsplus)
{
  return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mdsplus)
{
  return SUCCESS;
}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mdsplus)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "mdsplus support", "enabled");
  php_info_print_table_end();

  /* Remove comments if you have entries in php.ini
     DISPLAY_INI_ENTRIES();
   */
}

/* }}} */

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */

static int persistent = 0;
static char *lastHost = 0;
static int persistentConnection = -1;

PHP_FUNCTION(mdsplus_connect)
{
  char *arg = NULL;
  int arg_len, len;
  int handle;
  int persistent;
  long zarg_len;
  long zpersistent;

  mdsplus_replace_error(0, 1);
  persistent = 0;
  if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s|l", &arg, &zarg_len, &zpersistent) ==
      FAILURE) {
    RETURN_FALSE;
  }
  arg_len = (int)zarg_len;
  persistent = (int)zpersistent;
  handle = (!persistent || lastHost == 0 || persistentConnection == -1
	    || strcmp(arg, lastHost) != 0) ? ConnectToMds(arg) : persistentConnection;
  if (handle != -1) {
    if (persistent) {
      if (lastHost != 0) {
	free(lastHost);
	lastHost = 0;
      }
      lastHost = strcpy(malloc(strlen(arg) + 1), arg);
      persistentConnection = handle;
    }
    RETURN_LONG(handle);
  } else {
    char *error =
	strcpy(malloc(arg_len + strlen("Error connecting to host: ") + 1),
	       "Error connecting to host: ");
    strcat(error, arg);
    mdsplus_replace_error(error, 1);
    RETURN_FALSE;
  }
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(mdsplus_disconnect)
{
  int socket;
  long int zsocket;
  if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "l", &zsocket) == FAILURE) {
    return;
  }
  socket = (int)zsocket;
  mdsplus_replace_error(0, 1);
  if (socket == persistentConnection)
    persistentConnection = -1;
  DisconnectFromMds(socket);
  RETURN_TRUE;
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(mdsplus_open)
{
  char *tree = NULL;
  int arg_len, len;
  long zarg_len;
  int shot;
  long zshot;
  int socket;
  long zsocket;
  int status;

  if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "lsl", &zsocket, &tree, &zarg_len, &zshot) ==
      FAILURE) {
    return;
  }
  socket = (int)zsocket;
  arg_len = (int)zarg_len;
  shot = (int)zshot;
  mdsplus_replace_error(0, 1);
  status = MdsOpen(socket, tree, shot);
  if (status & 1) {
    RETURN_TRUE;
  } else {
    mdsplus_replace_error(mdsplus_translate_status(socket, status), 1);
    RETURN_FALSE;
  }
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(mdsplus_close)
{
  int socket;
  long zsocket;
  int status;

  if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "l", &zsocket) == FAILURE) {
    return;
  }
  socket = (int)zsocket;
  mdsplus_replace_error(0, 1);
  status = MdsClose(socket);
  if (status & 1) {
    RETURN_TRUE;
  } else {
    char *msg;
    mdsplus_replace_error(msg = mdsplus_translate_status(socket, status), 1);
    RETURN_FALSE;
  }
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */

static void MakeArray(zval * arr, struct descrip *ans, int dim, int *index)
{
  int i;
  array_init(arr);
  if (dim == 0) {
    for (i = 0; i < ans->dims[dim]; i++) {
      switch (ans->dtype) {
      case DTYPE_UCHAR:
	add_next_index_long(arr, (int)((unsigned char *)ans->ptr)[*index]);
	break;
      case DTYPE_USHORT:
	add_next_index_long(arr, (int)((unsigned short *)ans->ptr)[*index]);
	break;
      case DTYPE_ULONG:
	add_next_index_long(arr, (int)((int *)ans->ptr)[*index]);
	break;
      case DTYPE_CHAR:
	add_next_index_long(arr, (int)((char *)ans->ptr)[*index]);
	break;
      case DTYPE_SHORT:
	add_next_index_long(arr, (int)((short *)ans->ptr)[*index]);
	break;
      case DTYPE_LONG:
	add_next_index_long(arr, ((int *)ans->ptr)[*index]);
	break;
      case DTYPE_FLOAT:
	add_next_index_double(arr, (double)((float *)ans->ptr)[*index]);
	break;
      case DTYPE_DOUBLE:
	add_next_index_double(arr, ((double *)ans->ptr)[*index]);
	break;
      case DTYPE_CSTRING:{
	  char *string =
	      strncpy(emalloc(ans->length + 1), ((char *)ans->ptr) + (*index * ans->length),
		      ans->length);
	  string[ans->length] = 0;
	  add_next_index_string(arr, string, 0);
	  break;
	}
      }
      *index = *index + 1;
    }
  } else {
    for (i = 0; i < ans->dims[dim]; i++) {
      zval *arr2;
      MAKE_STD_ZVAL(arr2);
      MakeArray(arr2, ans, dim - 1, index);
      add_next_index_zval(arr, arr2);
    }
  }
}

PHP_FUNCTION(mdsplus_value)
{
  char *expression = NULL;
  int expression_len, len;
  int socket;
  long zsocket;
  long zexpression_len;
  struct descrip ans;
  int status;
  char *error = NULL;
  int num_args = ZEND_NUM_ARGS();
  int i;
  zval **args[252];
  ans.ptr = 0;
  if (num_args < 2 || num_args > 252)
    WRONG_PARAM_COUNT;

  if (zend_parse_parameters(2 TSRMLS_CC, "ls!", &zsocket, &expression, &zexpression_len) == FAILURE)
    return;
  socket = (int)zsocket;
  expression_len = (int)zexpression_len;
  mdsplus_replace_error(0, 1);
  if (num_args == 2)
    status = MdsValue(socket, expression, &ans, NULL);
  else {
    int dims[7] = { 0, 0, 0, 0, 0, 0, 0 };
    if (zend_get_parameters_array_ex(num_args, args) != SUCCESS)
      WRONG_PARAM_COUNT;
    status =
	SendArg(socket, 0, DTYPE_CSTRING, num_args - 1, strlen(expression), 0, dims, expression);
    for (i = 2; (status & 1) && i < num_args; i++) {
      switch (Z_TYPE_P(*args[i])) {
      case IS_LONG:
	status =
	    SendArg(socket, i - 1, DTYPE_LONG, num_args - 1, sizeof(int), 0, dims,
		    (void *)&Z_LVAL_P(*args[i]));
	break;
      case IS_DOUBLE:
	status =
	    SendArg(socket, i - 1, DTYPE_DOUBLE, num_args - 1, sizeof(double), 0, dims,
		    (void *)&Z_DVAL_P(*args[i]));
	break;
      case IS_STRING:
	status =
	    SendArg(socket, i - 1, DTYPE_CSTRING, num_args - 1, Z_STRLEN_P(*args[i]), 0, dims,
		    Z_STRVAL_P(*args[i]));
	break;
      default:
	mdsplus_replace_error("invalid argument type, must be long, double or string only", 0);
	RETURN_FALSE;
      }
      if (!(status & 1)) {
	mdsplus_replace_error("error sending argument to server", 0);
	RETURN_FALSE;
	break;
      }
    }
    if (!(status & 1)) {
      mdsplus_replace_error("error sending argument to server", 0);
      RETURN_FALSE;
    } else {
      short len;
      int numbytes;
      void *dptr;
      void *mem = 0;
      status =
		  GetAnswerInfoTS(socket, &ans.dtype, &len, &ans.ndims, &ans.dims, &numbytes, &dptr, &mem);
      ans.length = len;
      if (numbytes) {
	if (ans.dtype == DTYPE_CSTRING) {
	  ans.ptr = malloc(numbytes + 1);
	  ((char *)ans.ptr)[numbytes] = 0;
	} else if (numbytes > 0)
	  ans.ptr = malloc(numbytes);
	if (numbytes > 0)
	  memcpy(ans.ptr, dptr, numbytes);
      } else
	ans.ptr = NULL;
      if (mem)
	free(mem);
    }

  }
  if (status & 1) {
    if (ans.ndims == 0) {
      switch (ans.dtype) {
      case DTYPE_UCHAR:
	RETURN_LONG((int)*(unsigned char *)ans.ptr);
	break;
      case DTYPE_USHORT:
	RETURN_LONG((int)*(unsigned short *)ans.ptr);
	break;
      case DTYPE_ULONG:
	RETURN_LONG((int)*(int *)ans.ptr);
	break;
      case DTYPE_CHAR:
	RETURN_LONG((int)*(char *)ans.ptr);
	break;
      case DTYPE_SHORT:
	RETURN_LONG((int)*(short *)ans.ptr);
	break;
      case DTYPE_LONG:
	RETURN_LONG(*(int *)ans.ptr);
	break;
      case DTYPE_FLOAT:
	RETURN_DOUBLE((double)*(float *)ans.ptr);
	break;
      case DTYPE_DOUBLE:
	RETURN_DOUBLE(*(double *)ans.ptr);
	break;
      case DTYPE_CSTRING:
	RETURN_STRING(ans.ptr ? ans.ptr : strcpy(malloc(1), ""), 1);
	break;
      default:
	{
	  char error[128];
	  sprintf(error, "expression '%s' returned unsupported data type: %d", expression,
		  ans.dtype);
	  mdsplus_replace_error(error, 0);
	  RETVAL_FALSE;
	  break;
	}
      }
    } else {
      int index = 0;
      switch (ans.dtype) {
      case DTYPE_UCHAR:
      case DTYPE_USHORT:
      case DTYPE_ULONG:
      case DTYPE_CHAR:
      case DTYPE_SHORT:
      case DTYPE_LONG:
      case DTYPE_FLOAT:
      case DTYPE_DOUBLE:
      case DTYPE_CSTRING:
	break;
      default:
	{
	  char error[128];
	  sprintf(error, "expression '%s' returned unsupported data type: %d", expression,
		  ans.dtype);
	  mdsplus_replace_error(error, 0);
	  RETVAL_FALSE;
	  if (ans.ptr)
	    free(ans.ptr);
	  return;
	}
      }
      MakeArray(return_value, &ans, ans.ndims - 1, &index);
    }
  } else {
    char *error;
    if (ans.ptr) {
      error = malloc(strlen(ans.ptr) + strlen(expression) + 128);
      sprintf(error, "expression '%s' evaluation failed with the following error: %s", expression,
	      ans.ptr);
    } else {
      error = malloc(strlen(expression) + 128);
      sprintf(error, "expression '%s' evaluation failed with status return %d", expression, status);
    }
    mdsplus_replace_error(error, 1);
    RETVAL_FALSE;
  }
  if (ans.ptr)
    free(ans.ptr);
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(mdsplus_put)
{
  char *node = NULL;
  int node_len;
  char *expression = NULL;
  int expression_len, len;
  int socket;
  long zsocket;
  long znode_len;
  long zexpression_len;
  struct descrip ans;
  int status;
  int num_args = ZEND_NUM_ARGS();
  int i;
  zval **args[252];
  ans.ptr = 0;
  if (num_args < 3 || num_args > 252)
    WRONG_PARAM_COUNT;

  if (zend_parse_parameters
      (3 TSRMLS_CC, "lss!", &zsocket, &node, &znode_len, &expression, &zexpression_len) == FAILURE)
    return;
  socket = (int)zsocket;
  node_len = (int)znode_len;
  expression_len = (int)zexpression_len;
  mdsplus_replace_error(0, 1);
  if (num_args == 3)
    status = MdsPut(socket, node, expression, &ans, NULL);
  else {
    int dims[7] = { 0, 0, 0, 0, 0, 0, 0 };
    char putexp[512];
    if (zend_get_parameters_array_ex(num_args, args) != SUCCESS)
      WRONG_PARAM_COUNT;
    strcpy(putexp, "TreePut(");
    for (i = 0; i < num_args - 2; i++)
      strcat(putexp, "$,");
    strcat(putexp, "$)");
    status = SendArg(socket, 0, DTYPE_CSTRING, num_args, strlen(putexp), 0, dims, putexp);
    status = SendArg(socket, 1, DTYPE_CSTRING, num_args, strlen(node), 0, dims, node);
    status = SendArg(socket, 2, DTYPE_CSTRING, num_args, strlen(expression), 0, dims, expression);
    for (i = 3; (status & 1) && i < num_args; i++) {
      switch (Z_TYPE_P(*args[i])) {
      case IS_LONG:
	status =
	    SendArg(socket, i, DTYPE_LONG, num_args, sizeof(int), 0, dims,
		    (void *)&Z_LVAL_P(*args[i]));
	break;
      case IS_DOUBLE:
	status =
	    SendArg(socket, i, DTYPE_DOUBLE, num_args, sizeof(double), 0, dims,
		    (void *)&Z_DVAL_P(*args[i]));
	break;
      case IS_STRING:
	status =
	    SendArg(socket, i, DTYPE_CSTRING, num_args, Z_STRLEN_P(*args[i]), 0, dims,
		    Z_STRVAL_P(*args[i]));
	break;
      default:
	mdsplus_replace_error("invalid argument type, must be long, double or string only", 0);
	status = 0;
	goto done_mdsplus_put;
	break;
      }
      if (!(status & 1)) {
	mdsplus_replace_error("error sending argument to server", 0);
	goto done_mdsplus_put;
      }
    }
    if (!(status & 1)) {
      mdsplus_replace_error("error sending argument to server", 0);
      goto done_mdsplus_put;
    } else {
      short len;
      int numbytes;
      void *dptr;
      void *mem = 0;
      status =
		  GetAnswerInfoTS(socket, &ans.dtype, &len, &ans.ndims, &ans.dims, &numbytes, &dptr, &mem);
      ans.length = len;
      if (numbytes) {
	if (ans.dtype == DTYPE_CSTRING) {
	  ans.ptr = malloc(numbytes + 1);
	  ((char *)ans.ptr)[numbytes] = 0;
	} else if (numbytes > 0)
	  ans.ptr = malloc(numbytes);
	if (numbytes > 0)
	  memcpy(ans.ptr, dptr, numbytes);
      } else
	ans.ptr = NULL;
      if (mem)
	free(mem);
    }

  }
  if (status & 1) {
    if (ans.ptr && (*(int *)ans.ptr & 1)) {
      goto done_mdsplus_put;
    } else {
      mdsplus_replace_error(mdsplus_translate_status(socket, *(int *)ans.ptr), 1);
      status = 0;
      goto done_mdsplus_put;
    }
  } else {
    char *error;
    if (ans.ptr) {
      error = malloc(strlen(ans.ptr) + strlen(expression) + 128);
      sprintf(error, "expression '%s' evaluation failed with the following error: %s", expression,
	      ans.ptr);
    } else {
      error = malloc(strlen(expression) + 128);
      sprintf(error, "expression '%s' evaluation failed with status return %d", expression, status);
    }
    mdsplus_replace_error(error, 1);
  }
 done_mdsplus_put:
  if (ans.ptr)
    free(ans.ptr);
  if (status & 1) {
    RETURN_TRUE;
  } else {
    RETURN_FALSE;
  }
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mdsplus_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(mdsplus_error)
{
  if (mdsplus_error_msg) {
    RETVAL_STRING(mdsplus_error_msg, 1);
  } else {
    RETURN_FALSE;
  }
  return;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
