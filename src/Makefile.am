ACLOCAL_AMFLAGS = -I m4 --install
# program that will be installed in $bindir
bin_PROGRAMS = docklight

ALLCPPFLAGS = $(GTKMM_CFLAGS)
ALLCPPFLAGS += $(WNCK_CFLAGS)
ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""$(bindir)/docklight-5/locale"\"
ALLCPPFLAGS += -DWNCK_I_KNOW_THIS_IS_UNSTABLE  -Lbamf3 -I/usr/include/libbamf3/
docklight_CPPFLAGS = $(ALLCPPFLAGS)

# detailed
#docklight_CPPFLAGS += -Wall -Wextra -Werror -std=c++20 -pedantic

# release
#docklight_CPPFLAGS += O2 -s -DNDEBUG -Wall -Wextra -Werror -std=c++11 -pedantic

# standard
docklight_CPPFLAGS += -Wall -std=c++11
#-Lbamf3 -I/usr/include/libbamf3/
#docklight_CPPFLAGS += $(pkg-config --libs libbamf3)

#BAPIS = \
#    libbamf3.deps \
#    libbamf3.vapi \
#    $(NULL)
#LIBADD += -L<LIBDIR> -lbamf3

#EXTRA_DIST = $(BAPIS)

ALLDADDLIBS = $(GTKMM_LIBS)
#ALLDADDLIBS += $(LIBBAMF_PRIVATE_LIB)

docklight_LDADD = $(ALLDADDLIBS)

docklight_SOURCES =  \
appwindow.cpp \
appoptionsgroup.cpp \
utils/pixbuf.cpp \
utils/factory.cpp \
components/TransparentWindow.cpp \
components/ExplodesWindow.cpp \
components/panel.cpp \
components/dockrender.cpp \
components/device.cpp \
components/config.cpp \
components/dockitemcontainer.cpp \
components/appupdater.cpp \
components/dockitem.cpp \
components/position.cpp \
utils/system.cpp \
main.cpp \
$(NULL)

