SUBDIRS = source

lib_LTLIBRARIES=libmta_tr181.la
libmta_tr181_la_LDFLAGS=
libmta_tr181_la_SOURCES=
libmta_tr181_la_DEPENDENCIES= \
	source/TR-181/middle_layer_src/libCcspMtaAgent_middle_layer_src.la \
	source/TR-181/integration_src.shared/libCcspMtaAgent_integration_src_shared.la

libmta_tr181_la_LIBADD=$(libmta_tr181_la_DEPENDENCIES)
