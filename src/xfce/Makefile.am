AM_CPPFLAGS =                                    \
	-I$(top_srcdir)                              \
	-DG_LOG_DOMAIN=\"xfce4-fortune-fish-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"        \
	$(PLATFORM_CPPFLAGS)

plugin_LTLIBRARIES =                \
	libxfce4fortunefish.la

plugindir =	$(libdir)/xfce4/panel/plugins

pixmapsdir = $(pkgdatadir)/pixmaps

libxfce4fortunefish_la_SOURCES =	\
	../fortune-fish.c               \
	../fortune-fish-config.c        \
	../fortune-fish-settings.c      \
	xfce4-fortune-fish-plugin.c

libxfce4fortunefish_la_CFLAGS =     \
	-I$(top_srcdir)/src             \
	-DDATADIR=\"$(datadir)\"        \
	-DPKGDATADIR=\"$(pkgdatadir)\"  \
	-DPIXMAPSDIR=\"$(pixmapsdir)\"  \
	-DLOCALEDIR=\"$(localedir)\"    \
	-DXFCE_PLUGIN                   \
	$(LIBXFCE4PANEL_CFLAGS)         \
	$(LIBXFCE4UTIL_CFLAGS)          \
	$(LIBXFCE4UI_CFLAGS)            \
	$(GTK_CFLAGS)                   \
	$(GLIB_CFLAGS)                  \
	$(PLATFORM_CPPFLAGS)            \
	$(PLATFORM_CFLAGS)

libxfce4fortunefish_la_LDFLAGS = \
       -avoid-version            \
       -module                   \
       -no-undefined             \
       -export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
       $(PLATFORM_LDFLAGS)

libxfce4fortunefish_la_LIBADD =             \
	$(LIBXFCE4PANEL_LIBS)                   \
	$(LIBXFCE4UI_LIBS)                      \
	$(LIBXFCE4UTIL_LIBS)                    \
	$(GTK_LIBS)                             \
	$(GLIB_LIBS)

#
# Desktop file
#
desktopdir = $(datadir)/xfce4/panel/plugins
desktop_in_in_files = xfce4-fortune-fish-plugin.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =                                                            \
        $(desktop_in_in_files)

CLEANFILES =                                                            \
        $(desktop_in_files)                                             \
        $(desktop_DATA)

%.desktop.in: %.desktop.in.in
	sed -e "s,\@libdir\@,$(libdir),g" < $< > $@
