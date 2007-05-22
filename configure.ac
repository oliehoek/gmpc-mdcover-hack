AC_INIT(src)
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([gmpc-mdcover], [0.15.1],qball@sarine.nl)

AC_PROG_CC
AM_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([gmpcmdca],[	glib-2.0 >= 2.10
	gobject-2.0 >= 2.4
	gtk+-2.0 >= 2.8
	gmodule-2.0
	libxml-2.0
	gthread-2.0
	libmpd >= 0.14.0
	gmpc	>= 0.15.0
])


AC_SUBST(gmpcmdca_LIBS)
AC_SUBST(gmpcmdca_CFLAGS)


AC_OUTPUT([Makefile src/Makefile])
