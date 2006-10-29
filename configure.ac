AC_INIT(src)
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([gmpcmdca], [0.0.1],qball@qballcow.nl)

AC_PROG_CC
AM_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([gmpcmdca],[	glib-2.0 >= 2.4
	gobject-2.0 >= 2.4
	gtk+-2.0 >= 2.4
	gmodule-2.0
	libxml-2.0
	gthread-2.0
	libmpd >= 0.12.0
	gmpc	>= 0.12.1
])


AC_SUBST(gmpcmdca_LIBS)
AC_SUBST(gmpcmdca_CFLAGS)


AC_OUTPUT([Makefile src/Makefile])
