SUBDIRS = icons	

policydir = /usr/share/polkit-1/actions
policy_DATA = \
	org.freedesktop.docklight.policy \
	$(NULL)

applicationsdir = /usr/share/applications
applications_DATA = \
	docklight.desktop \
	$(NULL)


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


install-data-local:
	
	if test -f $(srcdir)/docklight.sh; then cp $(srcdir)/docklight.sh $(bindir); fi
	if test -f $(bindir)/docklight.sh; then chmod u+x $(bindir)/docklight.sh; fi
	if test -f $(bindir)/docklight.sh; then mv $(bindir)/docklight.sh /bin; fi
	if test -f /bin/docklight.sh && test ! -L /bin/docklight; then ln -s /bin/docklight.sh /bin/docklight; fi
	

install-exec-hook:

	mkdir -p $(bindir)/docklight-5 
	if test -f $(bindir)/docklight; then mv $(bindir)/docklight  $(bindir)/docklight-5; fi
	
uninstall-hook:

	if test -d $(bindir)/docklight-5; then rm -fdr $(bindir)/docklight-5; fi
	if test -f /bin/docklight.sh; then rm -f /bin/docklight.sh; fi
	if test	-L /bin/docklight; then rm /bin/docklight; fi

