/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifndef PHP_VIEWER_H
#define PHP_VIEWER_H

extern zend_module_entry viewer_module_entry;
#define phpext_viewer_ptr &viewer_module_entry

#ifdef PHP_WIN32
#include <winsock.h>
#define PHP_VIEWER_API __declspec(dllexport)
typedef SOCKET PHP_SOCKET;
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_VIEWER_API __attribute__ ((visibility("default")))
#else
#	define PHP_VIEWER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(viewer);
PHP_MSHUTDOWN_FUNCTION(viewer);
PHP_RINIT_FUNCTION(viewer);
PHP_RSHUTDOWN_FUNCTION(viewer);
PHP_MINFO_FUNCTION(viewer);


PHP_FUNCTION(confirm_viewer_compiled);	/* For testing, remove later. */
PHP_FUNCTION(certsocket_add);
PHP_FUNCTION(certsocket_send);
PHP_FUNCTION(certsocket_init);


typedef struct {
	PHP_SOCKET bsd_socket;
	int		type;
	int		error;
	int		blocking;
} php_socket;

#define php_sockets_le_socket_name "Viewer"
/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
*/

ZEND_BEGIN_MODULE_GLOBALS(viewer)
	int len;
ZEND_END_MODULE_GLOBALS(viewer)


/* In every utility function you add that needs to use variables 
   in php_viewer_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as VIEWER_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define VIEWER_G(v) TSRMG(viewer_globals_id, zend_viewer_globals *, v)
#else
#define VIEWER_G(v) (viewer_globals.v)
#endif


#endif	/* PHP_VIEWER_H */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
