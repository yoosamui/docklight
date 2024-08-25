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
docklightdatadir = $(bindir)/docklight-5/data
docklightdata_DATA = \
		docklight.ini \
		docklight.launcher.sh \
		docklight.template.desktop \
		$(NULL)


docklightimagesdir = $(bindir)/docklight-5/data/images
docklightimages_DATA = 	\
			images/docklight.home.ico \
			images/explodes.svg \
			images/*.png \
			$(NULL)

@INTLTOOL_DESKTOP_RULE@

#if test -f "/usr/local/bin/docklight"; then rm -f "/usr/local/bin/docklight"; fi
#ln -s  $(bindir)/docklight "/usr/local/bin/docklight"
install-exec-hook:
	if test ! -d $(bindir)/docklight-5; then mkdir $(bindir)/docklight-5; fi
	mv $(bindir)/docklight  $(bindir)/docklight-5
	
#	if test ! -d $(bindir)/DOCKLIGHT/data; then mkdir $(bindir)/DOCKLIGHT/data; fi
#if test ! -d $(bindir)/DOCKLIGHT/data/attachments; then mkdir $(bindir)/DOCKLIGHT/data/attachments; fi
#	chmod -R 0777 $(bindir)/DOCKLIGHT/data		
	
	
	

	
#if test -d $(prefix); then rm -fr $(prefix); fi	
uninstall-hook:
	if test -d $(bindir)/docklight-5; then rm -fdr $(bindir)/docklight-5; fi
# if test -d $(bindir)/DOCKLIGHT/data; then rm -fr $(bindir)/DOCKLIGHT/data; fi
#	if test -d "/usr/lib/DOCKLIGHT" ; then rm -fr "/usr/lib/DOCKLIGHT"; fi	


#desktopdir = $(datadir)/applications
#desktop_in_files = docklight.menu.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
#@INTLTOOL_DESKTOP_RULE@


