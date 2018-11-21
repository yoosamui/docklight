SUBDIRS = src po data 
		
# Add INTLTOOL_FILES to the EXTRA_DIST list of files.
# This ensures that when you do a make dist, 
# these commands will be included in the source tarball.
# Define INTLTOOL_FILES as:	

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in



DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update \
                 po/.intltool-merge-cache	

# Depending on the types of files that contain translatable strings, add code such as
#desktopdir = $(datadir)/applications
#desktop_in_files = programname.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
#@INTLTOOL_DESKTOP_RULE@


		
#dist_doc_DATA = README
#DIST_SUBDIRS = data
# this works - it deletes testSubDir in root
#clean-local:
#	-rm -rf testSubDir

