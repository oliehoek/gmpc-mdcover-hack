#bin_PROGRAMS=osdplugin_la
libdir=${prefix}/share/gmpc/plugins/

lib_LTLIBRARIES = mdcaplugin.la

mdcaplugin_la_SOURCES=	\
	plugin.c 


mdcaplugin_la_LIBADD= @gmpcmdca_LIBS@ 
mdcaplugin_la_CFLAGS= @gmpcmdca_CFLAGS@  
mdcaplugin_la_LDFLAGS= -module -avoid-version 

install: 
	@$(MAKE)
	mkdir -p ${DESTDIR}${libdir}
	if test -f .libs/mdcaplugin.so; then cp .libs/mdcaplugin.so ${DESTDIR}${libdir}; fi
	if test -f .libs/mdcaplugin.dll; then cp .libs/mdcaplugin.dll ${DESTDIR}${libdir}; fi
