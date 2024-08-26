ACLOCAL_AMFLAGS = -I m4 --install
# program that will be installed in $bindir
bin_PROGRAMS = docklight

ALLCPPFLAGS = $(GTKMM_CFLAGS)
ALLCPPFLAGS += $(WNCK_CFLAGS)
ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""$(bindir)/docklight-5/locale"\"
docklight_CPPFLAGS = $(ALLCPPFLAGS)


# detailed
docklight_CPPFLAGS += -Wall -Wextra -Werror -std=c++11 -pedantic

# release
#docklight_CPPFLAGS += O2 -s -DNDEBUG -Wall -Wextra -Werror -std=c++11 -pedantic

# standard
#docklight_CPPFLAGS += -Wall -std=c++11


ALLDADDLIBS = $(GTKMM_LIBS)
docklight_LDADD = $(ALLDADDLIBS)

docklight_SOURCES =  \
appwindow.cpp \
appoptionsgroup.cpp \
components/TransparentWindow.cpp \
components/ExplodesWindow.cpp \
components/panel.cpp \
components/device.cpp \
components/config.cpp \
utils/system.cpp \
main.cpp \
$(NULL)

