# INFO: We're writing into srcdir, not builddir!

$(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx.proto
ifdef MYSQL_XDEVAPI_PROTOBUF_INCLUDES
	$(MYSQL_XDEVAPI_PROTOC) -I$(MYSQL_XDEVAPI_PROTOBUF_INCLUDES) --cpp_out $(srcdir)/xmysqlnd/proto_gen/ --proto_path $(srcdir)/xmysqlnd/proto_def/ $(srcdir)/xmysqlnd/proto_def/mysql*.proto
else
	$(MYSQL_XDEVAPI_PROTOC) --cpp_out $(srcdir)/xmysqlnd/proto_gen/ --proto_path $(srcdir)/xmysqlnd/proto_def/ $(srcdir)/xmysqlnd/proto_def/mysql*.proto
endif

$(srcdir)/xmysqlnd/proto_gen/mysqlx_connection.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_connection.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_crud.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_crud.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_datatypes.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_datatypes.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_expect.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_expect.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_expr.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_expr.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_notice.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_notice.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_resultset.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_resultset.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_session.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_session.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/proto_gen/mysqlx_sql.pb.cc: $(srcdir)/xmysqlnd/proto_def/mysqlx_sql.proto $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc


# set dependencies on protobuf generated sources, to avoid build
# errors due to lack of *.pb.h headers (caused by parallel compilation race)
$(srcdir)/%.cc: $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/%.cc: $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/messages/%.cc: $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc

$(srcdir)/xmysqlnd/crud_parsers/%.cc: $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc


BUILT_SOURCES = $(srcdir)/xmysqlnd/proto_gen/mysqlx_connection.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_datatypes.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_expr.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_session.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_crud.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_expect.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_notice.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_resultset.pb.cc \
	 $(srcdir)/xmysqlnd/proto_gen/mysqlx_sql.pb.cc
