lib_LTLIBRARIES = libgtrtstQ4.la

libgtrtstQ4_la_LDFLAGS = \
		-no-undefined \
		-version-info 2:1:1 \
		-export-dynamic \
		-export-symbols @srcdir@/gtrtstQ4.exp \
		-module

INCLUDES = -I@top_srcdir@/ini \
           $(INCLUDES_COMMON) \
           $(QT_CXXFLAGS) \
           -DTHEPREFIX=\"@prefix@\" \
	   -DSYSTEM_FILE_PATH=\"@sysconfdir@\" \
	   -DDEFLIB_PATH=\"@libdir@\" \
	   -DSHLIBEXT=\"@SHLIBEXT@\" \
	   $(LTDLINCL)

libgtrtstQ4_la_SOURCES = \
	szLogPrintf.cpp \
	szMessageBox.cpp

EXTRA_DIST = \
	README

