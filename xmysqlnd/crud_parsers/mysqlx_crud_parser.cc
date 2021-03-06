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
  | Authors: Filip Janiszewski <fjanisze@php.net>                        |
  +----------------------------------------------------------------------+
*/

#include "mysqlx_crud_parser.h"

namespace mysqlx {
namespace devapi {
namespace parser {

/* {{{ mysqlx::devapi::parser::Args_conv::Args_conv */
Args_conv::Args_conv( std::vector<std::string>& placeholders ) :
	placeholders{ placeholders }
{}
/* }}} */


/* {{{ mysqlx::devapi::parser::Args_conv::conv_placeholder */
unsigned Args_conv::conv_placeholder( const cdk::protocol::mysqlx::string& parm )
{
	const unsigned next = placeholders.size();
	placeholders.push_back( parm );
	return next;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::parse */
Mysqlx::Expr::Expr* parse( const std::string& expression,
						   const bool doc_datamodel,
						   std::vector< std::string >& placeholders )
{
	Args_conv args_conv( placeholders );
	::parser::Expression_parser expr( doc_datamodel ? ::parser::Parser_mode::DOCUMENT :
									  ::parser::Parser_mode::TABLE,
									  expression.c_str() );

	Mysqlx::Expr::Expr* pb_expr = new Mysqlx::Expr::Expr;
	cdk::protocol::mysqlx::Expr_builder eb( *pb_expr, &args_conv );

	cdk::mysqlx::Expr_converter conv;

	conv.reset( expr );

	conv.process( eb );

	return pb_expr;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::parse */
Mysqlx::Expr::Expr* parse( const std::string& expression,
						   const bool doc_datamodel )
{
	std::vector< std::string > placeholders;
	Mysqlx::Expr::Expr* expr = parse( expression, doc_datamodel, placeholders );
	if ( false == placeholders.empty() ) {
		/*
		 * The expression shouldn't contain placeholders, something
		 * must be wrong better return nullptr
		 */
		delete expr;
		expr = nullptr;
	}
	return expr;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Expr_builder::Expr_builder */
Expr_builder::Expr_builder( Mysqlx::Expr::Expr& msg,
							cdk::protocol::mysqlx::Args_conv* conv )
{
	reset( msg, conv );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_builder::reset */
void Order_builder::reset( Builder_base::Message& msg,
						   cdk::protocol::mysqlx::Args_conv* conv )
{
	Builder_base::reset( msg, conv );
	expr_builder.reset( *msg.mutable_expr(), conv );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_builder::sort_key */
Order_builder::Expr_prc* Order_builder::sort_key( cdk::api::Sort_direction::value dir )
{
	m_msg->set_direction( dir == cdk::api::Sort_direction::ASC ?
						  Message::ASC : Message::DESC );
	return &expr_builder;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by_item::Order_by_item */
Order_by_item::Order_by_item( const char* expr,
							  cdk::Sort_direction::value sort_direction,
							  ::parser::Parser_mode::value mode ) :
	parser_mode( mode ),
	expression( expr ),
	sort_direction( sort_direction )
{}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by_item::process */
void Order_by_item::process( cdk::Expression::Processor& prc ) const
{
	::parser::Expression_parser parser( parser_mode, expression );
	parser.process( prc );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by_item::direction */
cdk::Sort_direction::value Order_by_item::direction() const
{
	return sort_direction;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::Order_by */
Order_by::Order_by( ::parser::Parser_mode::value mode ) : parser_mode( mode )
{}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::add_item */
void Order_by::add_item( const char* expr,
						 cdk::Sort_direction::value sort_direction )
{
	item_list.push_back( Order_by_item( expr, sort_direction, parser_mode ) );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::clear */
void Order_by::clear()
{
	item_list.clear();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::count */
uint32_t Order_by::count() const
{
	return item_list.size();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::process */
void Order_by::process( Processor& prc ) const
{
	prc.list_begin();
	for ( Order_item_list::const_iterator it = item_list.begin();
			it != item_list.end(); ++it ) {
		Processor* pprc = &prc;
		if ( pprc ) {
			Processor::Element_prc* list_el_ptr = prc.list_el();
			if ( list_el_ptr ) {
				it->process_if( list_el_ptr->sort_key( it->direction() ) );
			}
		}
	}
	prc.list_end();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Order_by::get_direction */
cdk::Sort_direction::value Order_by::get_direction( uint32_t pos ) const
{
	return item_list[pos].direction();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Expr_to_doc_prc_converter::Expr_to_doc_prc_converter */
Expr_to_doc_prc_converter::Doc_prc* Expr_to_doc_prc_converter::doc()
{
	return m_proc;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Expr_to_doc_prc_converter::scalar */
Expr_to_doc_prc_converter::Scalar_prc* Expr_to_doc_prc_converter::scalar()
{
	throw std::runtime_error( "Document expected" );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Expr_to_doc_prc_converter::arr */
Expr_to_doc_prc_converter::List_prc* Expr_to_doc_prc_converter::arr()
{
	throw std::runtime_error( "Document expected" );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_builder::reset */
void Projection_builder::reset( Projection_builder::Message& msg,
								cdk::protocol::mysqlx::Args_conv* conv )
{
	Builder_base::reset( msg, conv );
	expression_builder.reset( *msg.mutable_source(), conv );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_builder::expr */
Projection_builder::Expr_prc* Projection_builder::expr()
{
	return &expression_builder;
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_builder::alias */
void Projection_builder::alias( const cdk::foundation::string& a )
{
	m_msg->set_alias( a );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::Projection_list */
Projection_list::Projection_list( bool doc_datamodel )
{
	if ( doc_datamodel ) {
		parser_mode = ::parser::Parser_mode::DOCUMENT;
	} else {
		parser_mode = ::parser::Parser_mode::TABLE;
	}
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::process */
void Projection_list::process( cdk::Projection::Processor& prc ) const
{
	prc.list_begin();
	for ( Proj_vec::const_iterator it = values.begin();
			it != values.end(); ++it ) {
		::parser::Projection_parser parser( parser_mode, *it );
		cdk::Projection::Processor::Element_prc* eprc = prc.list_el();
		if ( eprc ) {
			parser.process( *eprc );
		}
	}
	prc.list_end();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::add_value */
void Projection_list::add_value( const char* val )
{
	values.push_back( val );
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::clear */
void Projection_list::clear()
{
	values.clear();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::count */
uint32_t Projection_list::count() const
{
	return values.size();
}
/* }}} */


/* {{{ mysqlx::devapi::parser::Projection_list::process */
void Projection_list::process( cdk::Expression::Document::Processor& prc ) const
{
	::parser::Expression_parser parser( ::parser::Parser_mode::DOCUMENT, values[0] );
	cdk::Expr_conv_base<Expr_to_doc_prc_converter,
		cdk::Expression,
		cdk::Expression::Document> spec;
	spec.reset( parser );
	spec.process( prc );
}
/* }}} */


} //mysqlx::devapi::parser
} //mysqlx::devapi
} //mysqlx

