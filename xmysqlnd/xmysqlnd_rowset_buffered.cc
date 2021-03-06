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
#include "php_api.h"
extern "C" {
#include <ext/mysqlnd/mysqlnd.h>
#include <ext/mysqlnd/mysqlnd_debug.h>
}
#include "xmysqlnd.h"
#include "xmysqlnd_driver.h"
#include "xmysqlnd_node_session.h"
#include "xmysqlnd_node_stmt.h"
#include "xmysqlnd_node_stmt_result.h"
#include "xmysqlnd_node_stmt_result_meta.h"
#include "xmysqlnd_rowset_buffered.h"

namespace mysqlx {

namespace drv {

/* {{{ xmysqlnd_rowset_buffered::init */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, init)(XMYSQLND_ROWSET_BUFFERED * const result,
												const MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_object_factory) * const factory,
												XMYSQLND_NODE_STMT * const stmt,
												MYSQLND_STATS * const stats,
												MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::init");
	result->stmt = stmt->data->m.get_reference(stmt);
	DBG_RETURN(result->stmt? PASS:FAIL);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::next */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, next)(XMYSQLND_ROWSET_BUFFERED * const result,
												MYSQLND_STATS * const stats,
												MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::next");
	if (result->row_cursor >= result->row_count) {
		DBG_RETURN(FAIL);
	}
	++result->row_cursor;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::fetch_current */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_current)(XMYSQLND_ROWSET_BUFFERED * const result,
														 zval * row,
														 MYSQLND_STATS * const stats,
														 MYSQLND_ERROR_INFO * const error_info)
{
	enum_func_status ret{FAIL};
	DBG_ENTER("xmysqlnd_rowset_buffered::fetch_current");
	ret = result->m.fetch_one(result, result->row_cursor, row, stats, error_info);
	DBG_INF_FMT("%s", PASS == ret? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */

/*!!!
  For Collection.find() this method should not call array_init_size() but ZVAL_STRINGL(row).
  Or just a new method is needed, that fetches only the first column. A flag may be needed to
  flatten to not use an array in case of just one column.
*/

/* {{{ xmysqlnd_rowset_buffered::fetch_one */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_one)(XMYSQLND_ROWSET_BUFFERED * const result,
													 const size_t row_cursor,
													 zval * row,
													 MYSQLND_STATS * const stats,
													 MYSQLND_ERROR_INFO * const error_info)
{
	const unsigned int field_count = result->meta->m->get_field_count(result->meta);
	const size_t row_count = result->row_count;
	DBG_ENTER("xmysqlnd_rowset_buffered::fetch_one");
	if (row_cursor >= row_count || !result->rows[row_cursor]) {
		DBG_RETURN(FAIL);
	}
	array_init_size(row, field_count);
	if (field_count) {
		zval * const row_cursor_zv = result->rows[row_cursor];
		unsigned int col = 0;
		for (;col < field_count; ++col) {
			const XMYSQLND_RESULT_FIELD_META * field_meta = result->meta->m->get_field(result->meta, col);
			zval * const zv = &row_cursor_zv[col];

			Z_TRY_ADDREF_P(zv);

			if (field_meta->zend_hash_key.is_numeric == FALSE) {
				zend_hash_update(Z_ARRVAL_P(row), field_meta->zend_hash_key.sname, zv);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(row), field_meta->zend_hash_key.key, zv);
			}
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::fetch_one_c */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_one_c)(XMYSQLND_ROWSET_BUFFERED * const result,
													   const size_t row_cursor,
													   zval ** row,
													   const zend_bool duplicate,
													   MYSQLND_STATS * const stats,
													   MYSQLND_ERROR_INFO * const error_info)
{
	const unsigned int field_count = result->meta->m->get_field_count(result->meta);
	const size_t row_count = result->row_count;
	DBG_ENTER("xmysqlnd_rowset_buffered::fetch_one_c");
	if (row_cursor >= row_count || !result->rows[row_cursor]) {
		DBG_RETURN(FAIL);
	}
	if (field_count &&
		(*row = static_cast<zval*>(mnd_ecalloc(field_count, sizeof(zval)))))
	{
		const zval * const row_cursor_zv = result->rows[row_cursor];
		unsigned int col = 0;
		for (;col < field_count; ++col) {
			const zval * const from = &row_cursor_zv[col];
			zval * to = &(*row)[col];
			if (duplicate) {
				ZVAL_DUP(to, from);
			} else {
				ZVAL_COPY_VALUE(to, from);
			}
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::fetch_all */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_all)(XMYSQLND_ROWSET_BUFFERED * const result,
													 zval * set,
													 MYSQLND_STATS * const stats,
													 MYSQLND_ERROR_INFO * const error_info)
{
	const size_t row_count = result->row_count;
	size_t row_cursor{0};
	DBG_ENTER("xmysqlnd_rowset_buffered::fetch_all");
	array_init_size(set, row_count);
	for (;row_cursor < row_count; ++row_cursor) {
		zval row;
		ZVAL_UNDEF(&row);
		if (PASS == result->m.fetch_one(result, row_cursor, &row, stats, error_info)) {
			zend_hash_next_index_insert(Z_ARRVAL_P(set), &row);
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::fetch_all_c */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_all_c)(XMYSQLND_ROWSET_BUFFERED * const result,
													   zval ** set,
													   const zend_bool duplicate,
													   MYSQLND_STATS * const stats,
													   MYSQLND_ERROR_INFO * const error_info)
{
	const unsigned int field_count = result->meta->m->get_field_count(result->meta);
	const size_t row_count = result->row_count;
	DBG_ENTER("xmysqlnd_rowset_buffered::fetch_all_c");
	DBG_INF_FMT("dupli=%s", duplicate? "YES":"NO");
	DBG_INF_FMT("rows =%u  cols=%u", (uint) row_count, (uint) field_count);
	DBG_INF_FMT("cells=%u", (uint) (row_count * field_count));
	if ((*set = static_cast<zval*>(mnd_ecalloc(row_count * field_count, sizeof(zval))))) {
		size_t row{0};
		for (;row < row_count; ++row) {
			const zval * const from_row_zv = result->rows[row];
			const size_t offset = row * field_count;
			unsigned int col = 0;
			for (;col < field_count; ++col) {
				const zval * const from = &from_row_zv[col];
				zval * const to = &(*set)[offset + col];
				if (duplicate) {
					ZVAL_DUP(to, from);
				} else {
					ZVAL_COPY_VALUE(to, from);
				}
			}
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::rewind */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, rewind)(XMYSQLND_ROWSET_BUFFERED * const result)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::rewind");
	result->row_cursor = 0;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::eof */
static zend_bool
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, eof)(const XMYSQLND_ROWSET_BUFFERED * const result)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::eof");
	DBG_INF_FMT("%s", result->row_cursor >= result->row_count? "TRUE":"FALSE");
	DBG_RETURN(result->row_cursor >= result->row_count? TRUE:FALSE);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::create_row */
static zval *
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, create_row)(XMYSQLND_ROWSET_BUFFERED * const result,
													  const XMYSQLND_NODE_STMT_RESULT_META * const meta,
													  MYSQLND_STATS * const stats,
													  MYSQLND_ERROR_INFO * const error_info)
{
	const unsigned int column_count = meta->m->get_field_count(meta);
	zval * row = static_cast<zval*>(mnd_pecalloc(column_count, sizeof(zval), result->persistent));
	DBG_ENTER("xmysqlnd_rowset_buffered::create_row");
	DBG_RETURN(row);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::destroy_row */
static void
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, destroy_row)(XMYSQLND_ROWSET_BUFFERED * const result,
													   zval * row,
													   MYSQLND_STATS * const stats,
													   MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::destroy_row");
	if (row) {
		mnd_pefree(row, result->persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::add_row */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, add_row)(XMYSQLND_ROWSET_BUFFERED * const result, zval * row, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::add_row");
	DBG_INF_FMT("row=%p", row);

	if (!result->rows || result->rows_allocated == result->row_count) {
		result->rows_allocated = ((result->rows_allocated + 2) * 5)/ 3;
		result->rows = static_cast<zval**>(mnd_perealloc(result->rows, result->rows_allocated * sizeof(zval*), result->persistent));
	}
	if (row) {
		result->rows[result->row_count++] = row;
	}
	DBG_INF_FMT("row_count=%u  rows_allocated=%u", (uint) result->row_count, (uint) result->rows_allocated);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::get_row_count */
static size_t
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, get_row_count)(const XMYSQLND_ROWSET_BUFFERED * const result)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::get_row_count");
	DBG_RETURN(result->row_count);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::attach_meta */
static enum_func_status
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, attach_meta)(XMYSQLND_ROWSET_BUFFERED * const result,
													   XMYSQLND_NODE_STMT_RESULT_META * const meta,
													   MYSQLND_STATS * const stats,
													   MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::attach_meta");
	if (meta) {
		if (result->meta) {
			xmysqlnd_node_stmt_result_meta_free(result->meta, stats, error_info);
		}
		result->meta = meta;
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::free_rows_contents */
static void
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_rows_contents)(XMYSQLND_ROWSET_BUFFERED * const result,
															  MYSQLND_STATS * stats,
															  MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::free_rows_contents");
	DBG_INF_FMT("rows=%p  meta=%p", result->rows, result->meta);

	if (result->rows && result->meta) {
		const unsigned int col_count = result->meta->m->get_field_count(result->meta);

		DBG_INF_FMT("Freeing %u rows with %u columns each", result->row_count, col_count);

		for (unsigned int row{0}; row < result->row_count; ++row) {
			for (unsigned int col{0}; col < col_count; ++col) {
				zval_ptr_dtor(&(result->rows[row][col]));
			}
			result->m.destroy_row(result, result->rows[row], stats, error_info);
			result->rows[row] = nullptr;
		}
		result->row_count = 0;
		result->row_cursor = 0;
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::free_rows */
static void
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_rows)(XMYSQLND_ROWSET_BUFFERED * const result,
													 MYSQLND_STATS * stats,
													 MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::free_rows");
	DBG_INF_FMT("rows=%p  meta=%p", result->rows, result->meta);

	if (result->rows) {
		const zend_bool pers = result->persistent;

		result->m.free_rows_contents(result, stats, error_info);

		mnd_pefree(result->rows, pers);
		result->rows = nullptr;

		result->rows_allocated = 0;
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::free_contents */
static void
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_contents)(XMYSQLND_ROWSET_BUFFERED * const result,
														 MYSQLND_STATS * stats,
														 MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::free_contents");

	result->m.free_rows(result, stats, error_info);
	if (result->meta) {
		result->meta = nullptr;
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered::dtor */
static void
XMYSQLND_METHOD(xmysqlnd_rowset_buffered, dtor)(XMYSQLND_ROWSET_BUFFERED * const result,
												MYSQLND_STATS * stats,
												MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered::dtor");
	if (result) {
		result->m.free_contents(result, stats, error_info);
		if (result->stmt) {
			result->stmt->data->m.free_reference(result->stmt, stats, error_info);
		}

		mnd_pefree(result, result->persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


static
MYSQLND_CLASS_METHODS_START(xmysqlnd_rowset_buffered)
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, init),

	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, next),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_current),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_one),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_one_c),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_all),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, fetch_all_c),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, rewind),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, eof),

	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, create_row),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, destroy_row),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, add_row),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, get_row_count),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_rows_contents),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_rows),

	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, attach_meta),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, free_contents),
	XMYSQLND_METHOD(xmysqlnd_rowset_buffered, dtor),
MYSQLND_CLASS_METHODS_END;


PHP_MYSQL_XDEVAPI_API MYSQLND_CLASS_METHODS_INSTANCE_DEFINE(xmysqlnd_rowset_buffered);

/* {{{ xmysqlnd_rowset_buffered_create */
PHP_MYSQL_XDEVAPI_API XMYSQLND_ROWSET_BUFFERED *
xmysqlnd_rowset_buffered_create(XMYSQLND_NODE_STMT * stmt,
								const zend_bool persistent,
								const MYSQLND_CLASS_METHODS_TYPE(xmysqlnd_object_factory) * const object_factory,
								MYSQLND_STATS * stats,
								MYSQLND_ERROR_INFO * error_info)
{
	XMYSQLND_ROWSET_BUFFERED* result{nullptr};
	DBG_ENTER("xmysqlnd_rowset_buffered_create");
	result = object_factory->get_rowset_buffered(object_factory, stmt, persistent, stats, error_info);
	DBG_RETURN(result);
}
/* }}} */


/* {{{ xmysqlnd_rowset_buffered_free */
PHP_MYSQL_XDEVAPI_API void
xmysqlnd_rowset_buffered_free(XMYSQLND_ROWSET_BUFFERED * const result, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("xmysqlnd_rowset_buffered_free");
	DBG_INF_FMT("result=%p", result);
	if (result) {
		result->m.dtor(result, stats, error_info);
	}
	DBG_VOID_RETURN;
}
/* }}} */

} // namespace drv

} // namespace mysqlx

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
