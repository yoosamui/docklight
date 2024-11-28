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
docklight_CPPFLAGS +=  -s -DNDEBUG -Wall -Wextra -Werror -std=c++17 -pedantic

# standard
#docklight_CPPFLAGS += -DDEBUG -Wall -std=c++17 -ggdb


ALLDADDLIBS = $(GTKMM_LIBS)
#ALLDADDLIBS += $(LIBBAMF_PRIVATE_LIB)

docklight_LDADD = $(ALLDADDLIBS)

docklight_SOURCES =  \
utils/system.cpp \
utils/factory.cpp \
utils/wnck.cpp \
utils/struts.cpp \
utils/cairo.cpp \
utils/pixbuf.cpp \
components/titlewindow.cpp \
components/TransparentWindow.cpp \
components/ExplodesWindow.cpp \
components/coverwindow.cpp \
components/panel.cpp \
components/panelpreview.cpp \
components/dockmenu.cpp \
components/dockrender.cpp \
components/device.cpp \
components/config.cpp \
components/configfile.cpp \
components/dockitemcontainer.cpp \
components/dockitemprovider.cpp \
components/appobserver.cpp \
components/dockitem.cpp \
components/dockitemicon.cpp \
components/position.cpp \
appwindow.cpp \
appoptionsgroup.cpp \
main.cpp \
$(NULL)

