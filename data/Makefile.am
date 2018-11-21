SUBDIRS = icons	

EXTRA_DIST = \
	images/docklight.home.ico \
	docklight.menu.desktop \
	org.freedesktop.docklight.policy \
	docklight.template.desktop \
	docklight.ini \
	docklight.launcher.sh \
	$(NULL)

#CLEANFILES = \
#	docklight.menu.desktop \
#	$(NULL)

#/usr/share/polkit-1/actions
policydir = /usr/share/polkit-1/actions
policy_DATA = org.freedesktop.docklight.policy

#/usr/local/share
#applicationsdir = $(datadir)/applications
#applications_DATA = 4autotools/docklight.menu.desktop

applicationsdir = /usr/share/applications
applications_DATA = docklight.menu.desktop


#/docklight.data
docklightdatadir = $(bindir)/data
docklightdata_DATA = \
		docklight.ini \
		docklight.launcher.sh \
		docklight.template.desktop \
		$(NULL)


docklightimagesdir = $(bindir)/data/images
docklightimages_DATA = 	images/docklight.home.ico \
			images/*.png \
			$(NULL)

@INTLTOOL_DESKTOP_RULE@

#if test -f "/usr/local/bin/docklight"; then rm -f "/usr/local/bin/docklight"; fi
#ln -s  $(bindir)/docklight "/usr/local/bin/docklight"
install-exec-hook:
	if test ! -d $(bindir)/data; then mkdir $(bindir)/data; fi
	if test ! -d $(bindir)/data/attachments; then mkdir $(bindir)/data/attachments; fi
	chmod -R 0777 $(bindir)/data		
	
	
	

	
#if test -d $(prefix); then rm -fr $(prefix); fi	
uninstall-hook:
	if test -L "/usr/local/bin/docklight"; then rm "/usr/local/bin/docklight"; fi
	if test -d $(bindir)/data; then rm -fr $(bindir)/data; fi
	if test -d "/usr/lib/docklight" ; then rm -fr "/usr/lib/docklight"; fi	


#desktopdir = $(datadir)/applications
#desktop_in_files = docklight.menu.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
#@INTLTOOL_DESKTOP_RULE@


