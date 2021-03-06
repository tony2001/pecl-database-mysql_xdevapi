/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_PHP_MYSQL_XDEVAPI_H
#define PHP_PHP_MYSQL_XDEVAPI_H

#define PHP_MYSQL_XDEVAPI_VERSION "8.0.3"
#define MYSQL_XDEVAPI_VERSION_ID 10000

#ifdef PHP_WIN32
#	ifdef PHP_MYSQL_XDEVAPI_EXPORTS
#		define PHP_MYSQL_XDEVAPI_API __declspec(dllexport)
#	elif defined(COMPILE_DL_MYSQL_XDEVAPI)
#		define PHP_MYSQL_XDEVAPI_API __declspec(dllimport)
#	else
#		define PHP_MYSQL_XDEVAPI_API PHPAPI
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MYSQL_XDEVAPI_API __attribute__ ((visibility("default")))
#else
#	define PHP_MYSQL_XDEVAPI_API PHPAPI
#endif

#endif /*PHP_PHP_MYSQL_XDEVAPI_H*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
