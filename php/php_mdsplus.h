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

#ifndef PHP_MDSPLUS_H
#define PHP_MDSPLUS_H

extern zend_module_entry mdsplus_module_entry;
#define phpext_mdsplus_ptr &mdsplus_module_entry

#ifdef PHP_WIN32
#define PHP_MDSPLUS_API __declspec(dllexport)
#else
#define PHP_MDSPLUS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(mdsplus);
PHP_MSHUTDOWN_FUNCTION(mdsplus);
PHP_RINIT_FUNCTION(mdsplus);
PHP_RSHUTDOWN_FUNCTION(mdsplus);
PHP_MINFO_FUNCTION(mdsplus);

PHP_FUNCTION(mdsplus_connect);
PHP_FUNCTION(mdsplus_open);
PHP_FUNCTION(mdsplus_close);
PHP_FUNCTION(mdsplus_put);
PHP_FUNCTION(mdsplus_value);
PHP_FUNCTION(mdsplus_disconnect);
PHP_FUNCTION(mdsplus_error);

/*
        Declare any global variables you may need between the BEGIN
        and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(mdsplus)
        long  global_value;
        char *global_string;
ZEND_END_MODULE_GLOBALS(mdsplus)
*/

/* In every utility function you add that needs to use variables
   in php_mdsplus_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MDSPLUS_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MDSPLUS_G(v) TSRMG(mdsplus_globals_id, zend_mdsplus_globals *, v)
#else
#define MDSPLUS_G(v) (mdsplus_globals.v)
#endif

#endif /* PHP_MDSPLUS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
