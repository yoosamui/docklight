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

ALLDADDLIBS = $(GTKMM_LIBS)
#ALLDADDLIBS +=$(WNCK_LIBS)
docklight_LDADD = $(ALLDADDLIBS)
#docklight_LDADD += -llibz


#  flags for the C compiler & linker
AM_CPPFLAGS = -g -Wall   -Wno-reorder -std=c++11  
#-pthread
#-pedantic  -lpthread 

# list of source files needed to generate target (docklight)
docklight_SOURCES =  \
main.cpp \
DockWindow.cpp \
DockWindow.h \
AppUpdater.cpp \
AppUpdater.h \
Screen.cpp \
Screen.h \
Struts.cpp \
Struts.h \
Animations.h \
AppWindow.cpp\
AppWindow.h \
DockPanel.cpp \
DockPanel.h \
DockItem.cpp \
DockItem.h \
WindowControl.cpp \
WindowControl.h \
IconLoader.cpp \
IconLoader.h \
Launcher.cpp \
Launcher.h \
SessionWindow.cpp \
SessionWindow.h \
MoReader.cpp \
MoReader.h \
Utilities.cpp \
Utilities.h \
Configuration.cpp \
Configuration.h \
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

