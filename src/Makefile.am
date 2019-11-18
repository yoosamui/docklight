ACLOCAL_AMFLAGS = -I m4 --install
bin_PROGRAMS = docklight


#ALLCPPFLAGS = -pthread -L /lib64
ALLCPPFLAGS = $(GTKMM_CFLAGS)
ALLCPPFLAGS += $(WNCK_CFLAGS)

#This macro will be used when you initialize gettext in your source code.
#ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""$(datadir)/locale"\"
#ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""$(prefix)/share/locale"\"
ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""/usr/lib/docklight/share/locale"\"


docklight_CPPFLAGS = $(ALLCPPFLAGS)
#docklight_CPPFLAGS += -Wall -Wextra -Werror -std=c++11 -pedantic

docklight_CPPFLAGS += -Wall -std=c++11

ALLDADDLIBS = $(GTKMM_LIBS)
#ALLDADDLIBS +=$(WNCK_LIBS)
docklight_LDADD = $(ALLDADDLIBS)
#docklight_LDADD += -llibz
#ALLCPPFLAGS +=  ${CXXFLAGS="-pedantic"}
#AM_CPPFLAGS = $(CXXFLAGS="-g")

#  flags for the C compiler & linker
#AM_CPPFLAGS = -g -Wall -Wno-reorder -std=c++11 -pedantic
#CFLAGS+=" -std=c11"
#-pthread
#-pedantic  -lpthread
# list of source files needed to generate target (docklight)


docklight_SOURCES =  \
common.h \
utils/string.cpp \
utils/string.h \
utils/file.cpp \
utils/file.h \
utils/system.cpp \
utils/system.h \
utils/pixbuf.cpp \
utils/pixbuf.h \
utils/position.cpp \
utils/position.h \
utils/launcher.cpp \
utils/launcher.h \
utils/moreader.cpp \
utils/moreader.h \
utils/cairo.cpp \
utils/cairo.h \
utils/easing.cpp \
utils/easing.h \
utils/wnck.cpp \
utils/wnck.h \
utils/desktopfile.cpp \
utils/desktopfile.h \
components/arguments.cpp \
components/arguments.h \
components/panelrender.cpp \
components/panelrender.h \
components/panelbase.cpp \
components/panelbase.h \
components/panel.cpp \
components/panel.h \
components/panelpreview.cpp \
components/panelpreview.h \
components/dockmenu.cpp \
components/dockmenu.h \
components/config.cpp \
components/config.h \
components/device.cpp \
components/device.h \
components/appupdater.cpp \
components/appupdater.h \
components/dockitem.cpp \
components/dockitem.h \
components/autohide.cpp \
components/autohide.h \
components/launcherwindow.cpp \
components/launcherwindow.h \
components/titlewindow.cpp \
components/titlewindow.h \
appwindow.cpp \
appwindow.h \
main.cpp \
$(NULL)

# this works - it deletes testSubDir in src
#clean-local:
#	-rm -rf testSubDir

# the above install-data-local example for installing /etc/afile would be better replaced by
# (DONT WORK FOR ME!)
#sysconf_DATA = $(top_srcdir)/docklight.ini
#dist_sysconf_DATA = $(top_srcdir)/docklight.ini




install-exec-hook:
	if test -f "/usr/local/bin/docklight"; then rm -f "/usr/local/bin/docklight"; fi
	ln -s  $(bindir)/docklight "/usr/local/bin/docklight"

uninstall-hook:
	if test  -f "/usr/local/bin/docklight"; then rm -f "/usr/local/bin/docklight"; fi

# this works
#install-exec-hook:
#	cp docklight.ini $(DESTDIR)$(bindir)
#	cp docklight.ico $(DESTDIR)$(bindir)
#	cp logo.png $(DESTDIR)$(bindir)
#	cp createlauncher.sh $(DESTDIR)$(bindir)
#
#	mkdir $(DESTDIR)$(bindir)/data

#if test -d ${pkgdatadir}/applications; then rmdir ${pkgdatadir}/applications; fi
#if test -d ${pkgdatadir}; then rmdir ${pkgdatadir}; fi
#uninstall-hook:
#	rm -rf $(bindir)
##	rm -rf $(prefix)

#	rm -f $(DESTDIR)$(bindir)/docklight.ico
#	rm -f $(DESTDIR)$(bindir)/logo.png
#	rm -f $(DESTDIR)$(bindir)/createlauncher.sh
#
#	rm -fr $(DESTDIR)$(bindir)/data

# It is possible to extend this mechanism by defining an install-exec-local or install-data-local rule.
# If these rules exist, they will be run at ‘make install’ time.
# These rules can do almost anything; care is required.


#myprgdir = $(sysconfdir)/myprg/
#dist_myprg_DATA =


#install-data-local:
#    $(MKDIR_P) $(DESTDIR)$(sysconfdir)/myprg/foo

