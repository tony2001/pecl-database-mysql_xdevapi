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
#ifndef MYSQLX_NODE_COLLECTION__MODIFY_H
#define MYSQLX_NODE_COLLECTION__MODIFY_H

namespace mysqlx {

namespace drv {

struct st_xmysqlnd_node_collection;
struct st_xmysqlnd_crud_collection_op__modify;

} // namespace drv

namespace devapi {

/* {{{ Collection_modify */
class Collection_modify : public util::custom_allocable
{
public:
	bool init(
		zval* object_zv,
		drv::st_xmysqlnd_node_collection* collection,
		const util::string_view& search_expression);
	~Collection_modify();

public:
	void sort(
		zval* sort_expr,
		int num_of_expr,
		zval* return_value);
	void limit(
		zend_long rows,
		zval* return_value);
	void skip(
		zend_long position,
		zval* return_value);
	void bind(
		HashTable* bind_variables,
		zval* return_value);

	enum class Operation
	{
		Set,
		Replace,
		Array_insert,
		Array_append
	};

	void add_operation(
		Operation operation,
		const util::string_view& path,
		const bool is_document,
		zval* value,
		zval* return_value);

	void set(
		const util::string_view& path,
		const bool is_document,
		zval* value,
		zval* return_value);
	void unset(
		zval* variables,
		int num_of_variables,
		zval* return_value);
	void replace(
		const util::string_view& path,
		zval* value,
		zval* return_value);
	void merge(
		const util::string_view& document_contents,
		zval* return_value);
	void patch(
		const util::string_view& document_contents,
		zval* return_value);

	void arrayInsert(
		const util::string_view& path,
		zval* value,
		zval* return_value);
	void arrayAppend(
		const util::string_view& path,
		zval* value,
		zval* return_value);
	void arrayDelete(
		const util::string_view& array_index_path,
		zval* return_value);

	void execute(zval* return_value);

private:
	zval* object_zv{nullptr};
	drv::st_xmysqlnd_node_collection* collection{nullptr};
	drv::st_xmysqlnd_crud_collection_op__modify* modify_op{nullptr};

};
/* }}} */


void mysqlx_new_node_collection__modify(
	zval* return_value,
	const util::string_view& search_expression,
	drv::st_xmysqlnd_node_collection* collection);
void mysqlx_register_node_collection__modify_class(INIT_FUNC_ARGS, zend_object_handlers * mysqlx_std_object_handlers);
void mysqlx_unregister_node_collection__modify_class(SHUTDOWN_FUNC_ARGS);

} // namespace devapi

} // namespace mysqlx

#endif /* MYSQLX_NODE_COLLECTION__MODIFY_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
