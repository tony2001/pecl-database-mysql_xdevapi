/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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
#include <php.h>
#undef ERROR
#include <zend_exceptions.h>		/* for throwing "not implemented" */
#include <ext/mysqlnd/mysqlnd.h>
#include <ext/mysqlnd/mysqlnd_debug.h>
#include <ext/mysqlnd/mysqlnd_alloc.h>
#include <xmysqlnd/xmysqlnd.h>
#include <xmysqlnd/xmysqlnd_node_session.h>
#include <xmysqlnd/xmysqlnd_node_schema.h>
#include <xmysqlnd/xmysqlnd_node_table.h>
#include <xmysqlnd/xmysqlnd_node_stmt.h>
#include <xmysqlnd/xmysqlnd_crud_table_commands.h>
#include "php_mysqlx.h"
#include "mysqlx_class_properties.h"
#include "mysqlx_executable.h"
#include "mysqlx_crud_operation_bindable.h"
#include "mysqlx_crud_operation_limitable.h"
#include "mysqlx_crud_operation_sortable.h"
#include "mysqlx_class_properties.h"
#include "mysqlx_exception.h"
#include "mysqlx_executable.h"
#include "mysqlx_expression.h"
#include "mysqlx_node_sql_statement.h"
#include "mysqlx_node_table__select.h"

static zend_class_entry *mysqlx_node_table__select_class_entry;

#define DONT_ALLOW_NULL 0
#define NO_PASS_BY_REF 0


ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__where, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(NO_PASS_BY_REF, projection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__group_by, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(NO_PASS_BY_REF, sort_expr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__having, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(NO_PASS_BY_REF, sort_expr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__orderby, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(NO_PASS_BY_REF, sort_expr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__limit, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_TYPE_INFO(NO_PASS_BY_REF, rows, IS_LONG, DONT_ALLOW_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__offset, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_TYPE_INFO(NO_PASS_BY_REF, position, IS_LONG, DONT_ALLOW_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__bind, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_TYPE_INFO(NO_PASS_BY_REF, placeholder_values, IS_ARRAY, DONT_ALLOW_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqlx_node_table__select__execute, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


struct st_mysqlx_node_table__select
{
	XMYSQLND_CRUD_TABLE_OP__SELECT * crud_op;
	XMYSQLND_NODE_TABLE * table;
};


#define MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(_to, _from) \
{ \
	const struct st_mysqlx_object * const mysqlx_object = Z_MYSQLX_P((_from)); \
	(_to) = (struct st_mysqlx_node_table__select *) mysqlx_object->ptr; \
	if (!(_to) || !(_to)->table) { \
		php_error_docref(NULL, E_WARNING, "invalid object of class %s", ZSTR_VAL(mysqlx_object->zo.ce->name)); \
		DBG_VOID_RETURN; \
	} \
} \


/* {{{ mysqlx_node_table__select::__construct */
static
PHP_METHOD(mysqlx_node_table__select, __construct)
{
}
/* }}} */


/* {{{ mysqlx_node_table__select::where */
static
PHP_METHOD(mysqlx_node_table__select, where)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	MYSQLND_CSTRING where_expr = {NULL, 0};

	DBG_ENTER("mysqlx_node_table__select::where");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",
												&object_zv, mysqlx_node_table__select_class_entry,
												&(where_expr.s), &(where_expr.l)))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->table && where_expr.s && where_expr.l)
	{
		if (PASS == xmysqlnd_crud_table_select__set_criteria(object->crud_op, where_expr))
		{
			ZVAL_COPY(return_value, object_zv);
		}
	}
}
/* }}} */


#define ADD_SORT 1
#define ADD_GROUPING 2

/* {{{ mysqlx_node_table__select__add_sort_or_grouping */
static void
mysqlx_node_table__select__add_sort_or_grouping(INTERNAL_FUNCTION_PARAMETERS, const unsigned int op_type)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	zval * sort_expr = NULL;

	DBG_ENTER("mysqlx_node_table__select__add_sort_or_grouping");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz",
												&object_zv, mysqlx_node_table__select_class_entry,
												&sort_expr))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && sort_expr) {
		switch (Z_TYPE_P(sort_expr)) {
			case IS_STRING: {
				const MYSQLND_CSTRING sort_expr_str = { Z_STRVAL_P(sort_expr), Z_STRLEN_P(sort_expr) };
				if (ADD_SORT == op_type) {
					if (PASS == xmysqlnd_crud_table_select__add_orderby(object->crud_op, sort_expr_str)) {
						ZVAL_COPY(return_value, object_zv);
					}
				} else if (ADD_GROUPING == op_type) {
					if (PASS == xmysqlnd_crud_table_select__add_grouping(object->crud_op, sort_expr_str)) {
						ZVAL_COPY(return_value, object_zv);
					}
				}
				break;
			}
			case IS_ARRAY: {
				zval * entry;
				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(sort_expr), entry) {
					enum_func_status ret = FAIL;
					const MYSQLND_CSTRING sort_expr_str = { Z_STRVAL_P(entry), Z_STRLEN_P(entry) };
					if (Z_TYPE_P(entry) != IS_STRING) {
						static const unsigned int errcode = 10003;
						static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
						static const MYSQLND_CSTRING errmsg = { "Parameter must be an array of strings", sizeof("Parameter must be an array of strings") - 1 };
						mysqlx_new_exception(errcode, sqlstate, errmsg);
						goto end;
					}
					if (ADD_SORT == op_type) {
						ret = xmysqlnd_crud_table_select__add_orderby(object->crud_op, sort_expr_str);
					} else if (ADD_GROUPING == op_type) {
						ret = xmysqlnd_crud_table_select__add_grouping(object->crud_op, sort_expr_str);
					}
					if (FAIL == ret) {
						static const unsigned int errcode = 10004;
						static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
						static const MYSQLND_CSTRING errmsg = { "Error while adding a sort expression", sizeof("Error while adding a sort expression") - 1 };
						mysqlx_new_exception(errcode, sqlstate, errmsg);
						goto end;
					}
				} ZEND_HASH_FOREACH_END();
				ZVAL_COPY(return_value, object_zv);
				break;
			}
			/* fall-through */
			default: {
				static const unsigned int errcode = 10005;
				static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
				static const MYSQLND_CSTRING errmsg = { "Parameter must be a string or array of strings", sizeof("Parameter must be a string or array of strings") - 1 };
				mysqlx_new_exception(errcode, sqlstate, errmsg);
			}			
		}
	}
end:
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::orderby() */
static
PHP_METHOD(mysqlx_node_table__select, orderby)
{
	DBG_ENTER("mysqlx_node_table__select::orderby");
	mysqlx_node_table__select__add_sort_or_grouping(INTERNAL_FUNCTION_PARAM_PASSTHRU, ADD_SORT);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::groupBy() */
static
PHP_METHOD(mysqlx_node_table__select, groupBy)
{
	DBG_ENTER("mysqlx_node_table__select::groupBy");
	mysqlx_node_table__select__add_sort_or_grouping(INTERNAL_FUNCTION_PARAM_PASSTHRU, ADD_GROUPING);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::having() */
static
PHP_METHOD(mysqlx_node_table__select, having)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	MYSQLND_CSTRING search_condition = {NULL, 0};

	DBG_ENTER("mysqlx_node_table__select::having");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",
												&object_zv, mysqlx_node_table__select_class_entry,
												&(search_condition.s), &(search_condition.l)))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && object->table) {
		if (PASS == xmysqlnd_crud_table_select__set_having(object->crud_op, search_condition)) {
			ZVAL_COPY(return_value, object_zv);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::limit() */
static
PHP_METHOD(mysqlx_node_table__select, limit)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	zend_long rows;

	DBG_ENTER("mysqlx_node_table__select::limit");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol",
												&object_zv, mysqlx_node_table__select_class_entry,
												&rows))
	{
		DBG_VOID_RETURN;
	}

	if (rows < 0) {
		static const unsigned int errcode = 10006;
		static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
		static const MYSQLND_CSTRING errmsg = { "Parameter must be a non-negative value", sizeof("Parameter must be a non-negative value") - 1 };
		mysqlx_new_exception(errcode, sqlstate, errmsg);	
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && object->table) {
		if (PASS == xmysqlnd_crud_table_select__set_limit(object->crud_op, rows)) {
			ZVAL_COPY(return_value, object_zv);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::offset() */
static
PHP_METHOD(mysqlx_node_table__select, offset)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	zend_long position;

	DBG_ENTER("mysqlx_node_table__select::offset");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol",
												&object_zv, mysqlx_node_table__select_class_entry,
												&position))
	{
		DBG_VOID_RETURN;
	}

	if (position < 0) {
		static const unsigned int errcode = 10006;
		static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
		static const MYSQLND_CSTRING errmsg = { "Parameter must be a non-negative value", sizeof("Parameter must be a non-negative value") - 1 };
		mysqlx_new_exception(errcode, sqlstate, errmsg);	
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && object->table) {
		if (PASS == xmysqlnd_crud_table_select__set_offset(object->crud_op, position)) {
			ZVAL_COPY(return_value, object_zv);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::bind() */
static
PHP_METHOD(mysqlx_node_table__select, bind)
{
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;
	HashTable * bind_variables;

	DBG_ENTER("mysqlx_node_table__select::bind");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oh",
												&object_zv, mysqlx_node_table__select_class_entry,
												&bind_variables))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && object->table) {
		zend_string * key;
		zval * val;
		ZEND_HASH_FOREACH_STR_KEY_VAL(bind_variables, key, val) {
			if (key) {
				const MYSQLND_CSTRING variable = { ZSTR_VAL(key), ZSTR_LEN(key) };
				if (FAIL == xmysqlnd_crud_table_select__bind_value(object->crud_op, variable, val)) {
					static const unsigned int errcode = 10005;
					static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
					static const MYSQLND_CSTRING errmsg = { "Error while binding a variable", sizeof("Error while binding a variable") - 1 };
					mysqlx_new_exception(errcode, sqlstate, errmsg);
					goto end;
				}
			}
		} ZEND_HASH_FOREACH_END();
	}
end:
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto mixed mysqlx_node_table__select::execute() */
static
PHP_METHOD(mysqlx_node_table__select, execute)
{
	zend_long flags = 0;
	struct st_mysqlx_node_table__select * object;
	zval * object_zv;

	DBG_ENTER("mysqlx_node_table__select::execute");

	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
												&object_zv, mysqlx_node_table__select_class_entry))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_NODE_TABLE_FROM_ZVAL(object, object_zv);

	RETVAL_FALSE;

	if (object->crud_op && object->table) {
		if (FALSE == xmysqlnd_crud_table_select__is_initialized(object->crud_op)) {
			static const unsigned int errcode = 10008;
			static const MYSQLND_CSTRING sqlstate = { "HY000", sizeof("HY000") - 1 };
			static const MYSQLND_CSTRING errmsg = { "Find not completely initialized", sizeof("Find not completely initialized") - 1 };
			mysqlx_new_exception(errcode, sqlstate, errmsg);
		} else {
			XMYSQLND_NODE_STMT * stmt = object->table->data->m.select(object->table, object->crud_op);
			{
				if (stmt) {
					zval stmt_zv;
					ZVAL_UNDEF(&stmt_zv);
					mysqlx_new_node_stmt(&stmt_zv, stmt);
					if (Z_TYPE(stmt_zv) == IS_NULL) {
						xmysqlnd_node_stmt_free(stmt, NULL, NULL);		
					}
					if (Z_TYPE(stmt_zv) == IS_OBJECT) {
						zval zv;
						ZVAL_UNDEF(&zv);
						mysqlx_node_statement_execute_read_response(Z_MYSQLX_P(&stmt_zv), flags, MYSQLX_RESULT_ROW, &zv);

						ZVAL_COPY(return_value, &zv);
					}
					zval_ptr_dtor(&stmt_zv);
				}
			}
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlx_node_table__select_methods[] */
static const zend_function_entry mysqlx_node_table__select_methods[] = {
	PHP_ME(mysqlx_node_table__select, __construct,	NULL,											ZEND_ACC_PRIVATE)

	PHP_ME(mysqlx_node_table__select, where,	arginfo_mysqlx_node_table__select__where,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, groupBy,	arginfo_mysqlx_node_table__select__group_by,	ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, having,	arginfo_mysqlx_node_table__select__having,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, bind,		arginfo_mysqlx_node_table__select__bind,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, orderby,	arginfo_mysqlx_node_table__select__orderby,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, limit,	arginfo_mysqlx_node_table__select__limit,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, offset,	arginfo_mysqlx_node_table__select__offset,		ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_node_table__select, execute,	arginfo_mysqlx_node_table__select__execute,		ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};
/* }}} */

#if 0
/* {{{ mysqlx_node_table__select_property__name */
static zval *
mysqlx_node_table__select_property__name(const struct st_mysqlx_object * obj, zval * return_value)
{
	const struct st_mysqlx_node_table__select * object = (const struct st_mysqlx_node_table__select *) (obj->ptr);
	DBG_ENTER("mysqlx_node_table__select_property__name");
	if (object->table && object->table->data->table_name.s) {
		ZVAL_STRINGL(return_value, object->table->data->table_name.s, object->table->data->table_name.l);
	} else {
		/*
		  This means EG(uninitialized_value). If we return just return_value, this is an UNDEF-ed value
		  and ISSET will say 'true' while for EG(unin) it is false.
		  In short:
		  return NULL; -> isset()===false, value is NULL
		  return return_value; (without doing ZVAL_XXX)-> isset()===true, value is NULL
		*/
		return_value = NULL;
	}
	DBG_RETURN(return_value);
}
/* }}} */
#endif

static zend_object_handlers mysqlx_object_node_table__select_handlers;
static HashTable mysqlx_node_table__select_properties;

const struct st_mysqlx_property_entry mysqlx_node_table__select_property_entries[] =
{
#if 0
	{{"name",	sizeof("name") - 1}, mysqlx_node_table__select_property__name,	NULL},
#endif
	{{NULL,	0}, NULL, NULL}
};

/* {{{ mysqlx_node_table__select_free_storage */
static void
mysqlx_node_table__select_free_storage(zend_object * object)
{
	struct st_mysqlx_object * mysqlx_object = mysqlx_fetch_object_from_zo(object);
	struct st_mysqlx_node_table__select * inner_obj = (struct st_mysqlx_node_table__select *) mysqlx_object->ptr;

	if (inner_obj) {
		if (inner_obj->table) {
			xmysqlnd_node_table_free(inner_obj->table, NULL, NULL);
			inner_obj->table = NULL;
		}
		mnd_efree(inner_obj);
	}
	mysqlx_object_free_storage(object); 
}
/* }}} */


/* {{{ php_mysqlx_node_table__select_object_allocator */
static zend_object *
php_mysqlx_node_table__select_object_allocator(zend_class_entry * class_type)
{
	struct st_mysqlx_object * mysqlx_object = mnd_ecalloc(1, sizeof(struct st_mysqlx_object) + zend_object_properties_size(class_type));
	struct st_mysqlx_node_table__select * object = mnd_ecalloc(1, sizeof(struct st_mysqlx_node_table__select));

	DBG_ENTER("php_mysqlx_node_table__select_object_allocator");
	if (!mysqlx_object || !object) {
		DBG_RETURN(NULL);	
	}
	mysqlx_object->ptr = object;

	zend_object_std_init(&mysqlx_object->zo, class_type);
	object_properties_init(&mysqlx_object->zo, class_type);

	mysqlx_object->zo.handlers = &mysqlx_object_node_table__select_handlers;
	mysqlx_object->properties = &mysqlx_node_table__select_properties;


	DBG_RETURN(&mysqlx_object->zo);
}
/* }}} */


/* {{{ mysqlx_register_node_table__select_class */
void
mysqlx_register_node_table__select_class(INIT_FUNC_ARGS, zend_object_handlers * mysqlx_std_object_handlers)
{
	mysqlx_object_node_table__select_handlers = *mysqlx_std_object_handlers;
	mysqlx_object_node_table__select_handlers.free_obj = mysqlx_node_table__select_free_storage;

	{
		zend_class_entry tmp_ce;
		INIT_NS_CLASS_ENTRY(tmp_ce, "Mysqlx", "NodeTableSelect", mysqlx_node_table__select_methods);
		tmp_ce.create_object = php_mysqlx_node_table__select_object_allocator;
		mysqlx_node_table__select_class_entry = zend_register_internal_class(&tmp_ce);
		zend_class_implements(mysqlx_node_table__select_class_entry, 1, mysqlx_executable_interface_entry);
	}

	zend_hash_init(&mysqlx_node_table__select_properties, 0, NULL, mysqlx_free_property_cb, 1);

	/* Add name + getter + setter to the hash table with the properties for the class */
	mysqlx_add_properties(&mysqlx_node_table__select_properties, mysqlx_node_table__select_property_entries);
#if 0
	/* The following is needed for the Reflection API */
	zend_declare_property_null(mysqlx_node_table__select_class_entry, "name",	sizeof("name") - 1,	ZEND_ACC_PUBLIC);
#endif
}
/* }}} */


/* {{{ mysqlx_unregister_node_table__select_class */
void
mysqlx_unregister_node_table__select_class(SHUTDOWN_FUNC_ARGS)
{
	zend_hash_destroy(&mysqlx_node_table__select_properties);
}
/* }}} */


/* {{{ mysqlx_new_node_table__select */
void
mysqlx_new_node_table__select(zval * return_value, XMYSQLND_NODE_TABLE * table, const zend_bool clone, zval * columns)
{
	DBG_ENTER("mysqlx_new_node_table__select");

	if (SUCCESS == object_init_ex(return_value, mysqlx_node_table__select_class_entry) && IS_OBJECT == Z_TYPE_P(return_value)) {
		const struct st_mysqlx_object * const mysqlx_object = Z_MYSQLX_P(return_value);
		struct st_mysqlx_node_table__select * const object = (struct st_mysqlx_node_table__select *) mysqlx_object->ptr;
		if (object) {
			object->table = clone? table->data->m.get_reference(table) : table;
			object->crud_op = xmysqlnd_crud_table_select__create(
				mnd_str2c(object->table->data->schema->data->schema_name),
				mnd_str2c(object->table->data->table_name),
				columns);
		} else {
			php_error_docref(NULL, E_WARNING, "invalid object of class %s", ZSTR_VAL(mysqlx_object->zo.ce->name));
			zval_ptr_dtor(return_value);
			ZVAL_NULL(return_value);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
