/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  +----------------------------------------------------------------------+
*/

#ifndef XMYSQLND_STMT_EXECUTION_STATE_H
#define XMYSQLND_STMT_EXECUTION_STATE_H

#include "xmysqlnd_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_xmysqlnd_stmt_execution_state XMYSQLND_STMT_EXECUTION_STATE;

typedef enum_func_status	(*func_xmysqlnd_stmt_execution_state__init)(XMYSQLND_STMT_EXECUTION_STATE * const result, MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_object_factory) *factory, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef size_t		(*func_xmysqlnd_stmt_execution_state__get_affected_items_count)(const XMYSQLND_STMT_EXECUTION_STATE * const state);
typedef size_t		(*func_xmysqlnd_stmt_execution_state__get_matched_items_count)(const XMYSQLND_STMT_EXECUTION_STATE * const state);
typedef size_t		(*func_xmysqlnd_stmt_execution_state__get_found_items_count)(const XMYSQLND_STMT_EXECUTION_STATE * const state);
typedef uint64_t	(*func_xmysqlnd_stmt_execution_state__get_last_insert_id)(const XMYSQLND_STMT_EXECUTION_STATE * const state);

typedef void		(*func_xmysqlnd_stmt_execution_state__set_affected_items_count)(XMYSQLND_STMT_EXECUTION_STATE * const state, const size_t value);
typedef void		(*func_xmysqlnd_stmt_execution_state__set_matched_items_count)(XMYSQLND_STMT_EXECUTION_STATE * const state, const size_t value);
typedef void		(*func_xmysqlnd_stmt_execution_state__set_found_items_count)(XMYSQLND_STMT_EXECUTION_STATE * const state, const size_t value);
typedef void		(*func_xmysqlnd_stmt_execution_state__set_last_insert_id)(XMYSQLND_STMT_EXECUTION_STATE * const state, const uint64_t value);

typedef void		(*func_xmysqlnd_stmt_execution_state__free_contents)(XMYSQLND_STMT_EXECUTION_STATE * const state);
typedef void		(*func_xmysqlnd_stmt_execution_state__dtor)(XMYSQLND_STMT_EXECUTION_STATE * const state);

MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_stmt_execution_state)
{
	func_xmysqlnd_stmt_execution_state__init init;
	func_xmysqlnd_stmt_execution_state__get_affected_items_count get_affected_items_count;
	func_xmysqlnd_stmt_execution_state__get_matched_items_count get_matched_items_count;
	func_xmysqlnd_stmt_execution_state__get_found_items_count get_found_items_count;
	func_xmysqlnd_stmt_execution_state__get_last_insert_id get_last_insert_id;

	func_xmysqlnd_stmt_execution_state__set_affected_items_count set_affected_items_count;
	func_xmysqlnd_stmt_execution_state__set_matched_items_count set_matched_items_count;
	func_xmysqlnd_stmt_execution_state__set_found_items_count set_found_items_count;
	func_xmysqlnd_stmt_execution_state__set_last_insert_id set_last_insert_id;

	func_xmysqlnd_stmt_execution_state__free_contents free_contents;
	func_xmysqlnd_stmt_execution_state__dtor dtor;
};


struct st_xmysqlnd_stmt_execution_state
{
	size_t items_affected;
	size_t items_matched;
	size_t items_found;
	uint64_t last_insert_id;
	/*UUID  last_document_id; */

	MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_stmt_execution_state) *m;
	zend_bool persistent;
};

PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(xmysqlnd_stmt_execution_state);
PHPAPI XMYSQLND_STMT_EXECUTION_STATE * xmysqlnd_stmt_execution_state_init(const zend_bool persistent, MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_object_factory) *object_factory, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
PHPAPI void xmysqlnd_stmt_execution_state_free(XMYSQLND_STMT_EXECUTION_STATE * const state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XMYSQLND_STMT_EXECUTION_STATE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */