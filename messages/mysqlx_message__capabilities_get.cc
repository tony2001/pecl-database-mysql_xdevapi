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
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  +----------------------------------------------------------------------+
*/
#include "php_api.h"
extern "C" {
#include <ext/mysqlnd/mysqlnd.h>
#include <ext/mysqlnd/mysqlnd_debug.h>
#include <ext/mysqlnd/mysqlnd_alloc.h>
#include <ext/mysqlnd/mysqlnd_statistics.h>
}
#include "xmysqlnd/xmysqlnd.h"
#include "xmysqlnd/xmysqlnd_node_session.h"
#include "xmysqlnd/xmysqlnd_zval2any.h"
#include "php_mysqlx.h"
#include "mysqlx_class_properties.h"
#include "mysqlx_node_session.h"
#include "mysqlx_node_connection.h"
#include "mysqlx_node_pfc.h"

#include "xmysqlnd/xmysqlnd_wireprotocol.h"

#include "xmysqlnd/proto_gen/mysqlx.pb.h"
#include "xmysqlnd/proto_gen/mysqlx_connection.pb.h"
#include "mysqlx_message__error.h"

#include "util/object.h"

namespace mysqlx {

namespace devapi {

namespace msg {

using namespace drv;

zend_class_entry *mysqlx_message__capabilities_get_class_entry;

struct st_mysqlx_message__capabilities_get
{
	struct st_xmysqlnd_msg__capabilities_get msg;
	zend_bool persistent;
};

#define MYSQLX_FETCH_MESSAGE__CAPABILITIES_GET__FROM_ZVAL(_to, _from) \
{ \
	st_mysqlx_object* mysqlx_object = Z_MYSQLX_P((_from)); \
	(_to) = (st_mysqlx_message__capabilities_get*) mysqlx_object->ptr; \
	if (!(_to)) { \
		php_error_docref(nullptr, E_WARNING, "invalid object or resource %s", ZSTR_VAL(mysqlx_object->zo.ce->name)); \
		RETVAL_NULL(); \
		DBG_VOID_RETURN; \
	} \
} \


ZEND_BEGIN_ARG_INFO_EX(mysqlx_message__capabilities_get__send, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_TYPE_INFO(0, node_pfc, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, node_connection, IS_OBJECT, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(mysqlx_message__capabilities_get__read_response, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_TYPE_INFO(0, node_pfc, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, node_connection, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

/* {{{ proto bool mysqlx_message__capabilities_get::send(object messsage, object pfc, object connection) */
MYSQL_XDEVAPI_PHP_METHOD(mysqlx_message__capabilities_get, send)
{
	zval * object_zv;
	zval * codec_zv;
	zval * connection_zv;
	st_mysqlx_message__capabilities_get* object;
	st_mysqlx_node_connection* connection;
	st_mysqlx_node_pfc* codec;
	enum_func_status ret{FAIL};

	DBG_ENTER("mysqlx_message__capabilities_get::send");
	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OOO",
												&object_zv, mysqlx_message__capabilities_get_class_entry,
												&codec_zv, mysqlx_node_pfc_class_entry,
												&connection_zv, mysqlx_node_connection_class_entry))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_MESSAGE__CAPABILITIES_GET__FROM_ZVAL(object, object_zv);
	MYSQLX_FETCH_NODE_PFC_FROM_ZVAL(codec, codec_zv);
	MYSQLX_FETCH_NODE_CONNECTION_FROM_ZVAL(connection, connection_zv);

	const XMYSQLND_L3_IO io = {connection->vio, codec->pfc};
	const struct st_xmysqlnd_message_factory msg_factory = xmysqlnd_get_message_factory(&io, connection->stats, connection->error_info);
	object->msg = msg_factory.get__capabilities_get(&msg_factory);

	ret = object->msg.send_request(&object->msg);

	RETVAL_BOOL(ret == PASS);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ proto long mysqlx_message__capabilities_get::read_response(object messsage, object pfc, object connection) */
MYSQL_XDEVAPI_PHP_METHOD(mysqlx_message__capabilities_get, read_response)
{
	zval * object_zv;
	zval * codec_zv;
	zval * connection_zv;
	st_mysqlx_message__capabilities_get* object;
	st_mysqlx_node_connection* connection;
	st_mysqlx_node_pfc* codec;
	size_t ret{0};

	DBG_ENTER("mysqlx_message__capabilities_get::read_response");
	if (FAILURE == zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OOO",
												&object_zv, mysqlx_message__capabilities_get_class_entry,
												&codec_zv, mysqlx_node_pfc_class_entry,
												&connection_zv, mysqlx_node_connection_class_entry))
	{
		DBG_VOID_RETURN;
	}

	MYSQLX_FETCH_MESSAGE__CAPABILITIES_GET__FROM_ZVAL(object, object_zv);
	MYSQLX_FETCH_NODE_PFC_FROM_ZVAL(codec, codec_zv);
	MYSQLX_FETCH_NODE_CONNECTION_FROM_ZVAL(connection, connection_zv);

	RETVAL_FALSE;

	ret = object->msg.read_response(&object->msg, return_value);
	if (FAIL == ret) {
		mysqlx_new_message__error(return_value, connection->error_info->error, connection->error_info->sqlstate, connection->error_info->error_no);
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlx_message__capabilities_get_methods[] */
static const zend_function_entry mysqlx_message__capabilities_get_methods[] = {
	PHP_ME(mysqlx_message__capabilities_get, send,				mysqlx_message__capabilities_get__send,				ZEND_ACC_PUBLIC)
	PHP_ME(mysqlx_message__capabilities_get, read_response,		mysqlx_message__capabilities_get__read_response,	ZEND_ACC_PUBLIC)
	{nullptr, nullptr, nullptr}
};
/* }}} */


static zend_object_handlers mysqlx_object_message__capabilities_get_handlers;
static HashTable mysqlx_message__capabilities_get_properties;


/* {{{ mysqlx_message__capabilities_get_free_storage */
static void
mysqlx_message__capabilities_get_free_storage(zend_object * object)
{
	st_mysqlx_object* mysqlx_object = mysqlx_fetch_object_from_zo(object);
	st_mysqlx_message__capabilities_get* message = (st_mysqlx_message__capabilities_get*) mysqlx_object->ptr;

	if (message) {
		mnd_pefree(message, message->persistent);
	}
	mysqlx_object_free_storage(object);
}
/* }}} */


/* {{{ php_mysqlx_message__capabilities_get_object_allocator */
static zend_object *
php_mysqlx_message__capabilities_get_object_allocator(zend_class_entry * class_type)
{
	const zend_bool persistent = FALSE;
	st_mysqlx_object* mysqlx_object = (st_mysqlx_object*) mnd_pecalloc(1, sizeof(struct st_mysqlx_object) + zend_object_properties_size(class_type), persistent);
	st_mysqlx_message__capabilities_get* message = (st_mysqlx_message__capabilities_get*) mnd_pecalloc(1, sizeof(struct st_mysqlx_message__capabilities_get), persistent);

	DBG_ENTER("php_mysqlx_message__capabilities_get_object_allocator");
	if ( mysqlx_object && message ) {
		mysqlx_object->ptr = message;

		message->persistent = persistent;
		zend_object_std_init(&mysqlx_object->zo, class_type);
		object_properties_init(&mysqlx_object->zo, class_type);

		mysqlx_object->zo.handlers = &mysqlx_object_message__capabilities_get_handlers;
		mysqlx_object->properties = &mysqlx_message__capabilities_get_properties;

		DBG_RETURN(&mysqlx_object->zo);
	}
	if (mysqlx_object) {
		mnd_pefree(mysqlx_object, persistent);
	}
	if (message) {
		mnd_pefree(message, persistent);
	}
	DBG_RETURN(nullptr);
}
/* }}} */


/* {{{ mysqlx_register_message__capabilities_get_class */
void
mysqlx_register_message__capabilities_get_class(INIT_FUNC_ARGS, zend_object_handlers * mysqlx_std_object_handlers)
{
	mysqlx_object_message__capabilities_get_handlers = *mysqlx_std_object_handlers;
	mysqlx_object_message__capabilities_get_handlers.free_obj = mysqlx_message__capabilities_get_free_storage;

	{
		zend_class_entry tmp_ce;
		INIT_CLASS_ENTRY(tmp_ce, "mysqlx_message__capabilities_get", mysqlx_message__capabilities_get_methods);
//		INIT_NS_CLASS_ENTRY(tmp_ce, "mysql_xdevapi", "node_pfc", mysqlx_message__capabilities_get_methods);
		tmp_ce.create_object = php_mysqlx_message__capabilities_get_object_allocator;
		mysqlx_message__capabilities_get_class_entry = zend_register_internal_class(&tmp_ce);
	}

	zend_hash_init(&mysqlx_message__capabilities_get_properties, 0, nullptr, mysqlx_free_property_cb, 1);
}
/* }}} */


/* {{{ mysqlx_unregister_message__capabilities_get_class */
void
mysqlx_unregister_message__capabilities_get_class(SHUTDOWN_FUNC_ARGS)
{
	zend_hash_destroy(&mysqlx_message__capabilities_get_properties);
}
/* }}} */

} // namespace msg

} // namespace devapi

} // namespace mysqlx

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
