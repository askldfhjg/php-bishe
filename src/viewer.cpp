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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_viewer.h"
#include <winsock2.h>
#pragma  comment(lib, "Ws2_32.lib")


ZEND_DECLARE_MODULE_GLOBALS(viewer)


/* True global resources - no need for thread safety here */
static int le_viewer;

/* {{{ viewer_functions[]
 *
 * Every user visible function must have an entry in viewer_functions[].
 */
const zend_function_entry viewer_functions[] = {
	PHP_FE(confirm_viewer_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(certsocket_add,	NULL)
	PHP_FE(certsocket_send,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in viewer_functions[] */
};
/* }}} */

/* {{{ viewer_module_entry
 */
zend_module_entry viewer_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"viewer",
	viewer_functions,
	PHP_MINIT(viewer),
	PHP_MSHUTDOWN(viewer),
	PHP_RINIT(viewer),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(viewer),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(viewer),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_VIEWER
ZEND_GET_MODULE(viewer)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("viewer.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_viewer_globals, viewer_globals)
    STD_PHP_INI_ENTRY("viewer.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_viewer_globals, viewer_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_viewer_init_globals
 */
 /*Uncomment this function if you have INI entries */
static void php_viewer_init_globals(zend_viewer_globals *viewer_globals)
{
	viewer_globals->string = NULL;
}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(viewer)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(viewer)
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
PHP_RINIT_FUNCTION(viewer)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(viewer)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(viewer)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Certsocket Support", "enabled");
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
/* {{{ proto string confirm_viewer_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_viewer_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "viewer", arg);
	RETURN_STRINGL(strg, len, 0);
}

/* Sets addr by hostname, or by ip in string form (AF_INET)  */
static int php_set_inet_addr(struct sockaddr_in *sin, char *string, php_socket *php_sock TSRMLS_DC) /* {{{ */
{
	struct in_addr tmp;
	struct hostent *host_entry;

	if (inet_aton(string, &tmp)) {
		sin->sin_addr.s_addr = tmp.s_addr;
	} 
	else {
		memcpy(&(sin->sin_addr.s_addr), host_entry->h_addr_list[0], host_entry->h_length);
	}

	return 1;
}
/* }}} */

/*int encode()
{
	Value var;
	var["test"]="fuck";
	FastWriter writer;
	//return writer.write(var);
	return 1;
}  */

PHP_FUNCTION(certsocket_send)
{	
	char *addr = NULL;
	char *str = NULL;
	int addr_len ,str_len;
	struct sockaddr_in	sin;
	long port;
	int	retval;
	php_socket	*php_sock = (php_socket*)emalloc(sizeof(php_socket));

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &addr, &addr_len, &str , &str_len ,&port) == FAILURE) {
		efree(php_sock);
		return;
	}

	php_sock->bsd_socket = socket(AF_INET, SOCK_STREAM, 6);
	php_sock->type = AF_INET;
	php_sock->error = 0;
	php_sock->blocking = 1;

	sin.sin_family = AF_INET;
	sin.sin_port   = htons((unsigned short int)port);
	if (! php_set_inet_addr(&sin,addr, php_sock TSRMLS_CC)) {
		RETURN_FALSE;
	}
	retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
	retval = send(php_sock->bsd_socket, str, str_len, 0);
	closesocket(php_sock->bsd_socket);
	efree(php_sock);
	RETURN_LONG(str_len);
}

PHP_FUNCTION(certsocket_add)
{
	char *key=NULL;
	int key_len,val_len;
	char *val=NULL;
	char *result;
	int result_length;
	struct sockaddr_in	sin;
	long port=9527;
	int	retval;
	php_socket	*php_sock = (php_socket*)emalloc(sizeof(php_socket));
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &val , &val_len) == FAILURE) {
		return;
	}
	result_length=key_len+val_len+10+1;
	result = (char *) emalloc(result_length);
	VIEWER_G(string) = (char *) emalloc(7);
	//memcpy(result, VIEWER_G(string), strlen(VIEWER_G(string)));
	strcat(VIEWER_G(string),"dfdf");
	/*strcat(result,key);
	strcat(result,"\":\"");
	strcat(result,val);
	strcat(result,"\"");
	erealloc(VIEWER_G(string),result_length);
	VIEWER_G(string)=estrdup(result); 
	//RETURN_TRUE;  */
	php_sock->bsd_socket = socket(AF_INET, SOCK_STREAM, 6);
	php_sock->type = AF_INET;
	php_sock->error = 0;
	php_sock->blocking = 1;

	sin.sin_family = AF_INET;
	sin.sin_port   = htons((unsigned short int)port);
	if (! php_set_inet_addr(&sin,"127.0.0.1", php_sock TSRMLS_CC)) {
		RETURN_FALSE;
	}
	retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
	retval = send(php_sock->bsd_socket, VIEWER_G(string), strlen(VIEWER_G(string)), 0);
	closesocket(php_sock->bsd_socket);
	efree(php_sock);
	RETURN_TRUE;  
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
