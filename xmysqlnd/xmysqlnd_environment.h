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
  | Authors: Darek Slusarczyk <marines@php.net>                          |
  +----------------------------------------------------------------------+
*/
#ifndef XMYSQLND_ENVIRONMENT_H
#define XMYSQLND_ENVIRONMENT_H

#include "util/strings.h"

namespace mysqlx {

namespace drv {

/* {{{ Environment */
struct Environment
{
	enum class Variable
	{
		Mysql_port,
		Mysqlx_port
	};

	static util::string get_as_string(Variable var);
	static int get_as_int(Variable var);

};
/* }}} */

} // namespace drv

} // namespace mysqlx

#endif /* XMYSQLND_ENVIRONMENT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
