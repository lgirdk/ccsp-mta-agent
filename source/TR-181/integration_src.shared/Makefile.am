AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_
AM_LDFLAGS = -lccsp_common
AM_LDFLAGS += -lhal_mta

AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = i686-linux-gnu

noinst_LTLIBRARIES = libCcspMtaAgent_integration_src_shared.la
libCcspMtaAgent_integration_src_shared_la_CPPFLAGS = -I$(top_srcdir)/source/TR-181/board_sbapi -I$(top_srcdir)/source/TR-181/middle_layer_src -I$(top_srcdir)/source/TR-181/include -I$(top_srcdir)/source/Custom $(CPPFLAGS)
libCcspMtaAgent_integration_src_shared_la_SOURCES = cosa_x_cisco_com_mta_apis.c
libCcspMtaAgent_integration_src_shared_la_LDFLAGS = -lccsp_common -lhal_mta
