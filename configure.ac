AC_INIT([gmpc-mdcover], [0.17.99], [qball@sarine.nl])
m4_define([package_major_version], [0])
m4_define([package_minor_version], [17])
m4_define([package_micro_version], [99])
m4_define([package_version],[package_major_version.package_minor_version.package_micro_version])

AC_CONFIG_AUX_DIR([.])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_DEFINE(PLUGIN_MAJOR_VERSION, package_major_version, ["Major version of plugin"])
AC_DEFINE(PLUGIN_MINOR_VERSION, package_minor_version, ["Minor version of plugin"])
AC_DEFINE(PLUGIN_MICRO_VERSION, package_micro_version, ["Micro version of plugin"])

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
	libmpd >= 0.15.98
	gmpc	>= 0.15.98
])


AC_SUBST(gmpcmdca_LIBS)
AC_SUBST(gmpcmdca_CFLAGS)


AC_OUTPUT([Makefile src/Makefile])
