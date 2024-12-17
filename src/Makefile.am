ACLOCAL_AMFLAGS = -I m4 --install
# program that will be installed in $bindir
bin_PROGRAMS = docklight

ALLCPPFLAGS = $(GTKMM_CFLAGS)
ALLCPPFLAGS += $(WNCK_CFLAGS)
ALLCPPFLAGS += -DPROGRAMNAME_LOCALEDIR=\""$(bindir)/docklight-5/locale"\"
ALLCPPFLAGS += -DWNCK_I_KNOW_THIS_IS_UNSTABLE  -Lbamf3 -I/usr/include/libbamf3/
#ALLCPPFLAGS += pkg-config --libs '-lX11 -lXcomposite -lXdamage -lXfixes -lXtst'
#pkg-config --libs fontconfig
docklight_CPPFLAGS = $(ALLCPPFLAGS)

# detailed
#docklight_CPPFLAGS += -Wall -Wextra -Werror -std=c++20 -pedantic

# release
#docklight_CPPFLAGS += O2 -s -DNDEBUG -Wall -Wextra -Werror -std=c++11 -pedantic
#docklight_CPPFLAGS +=  -s -DNDEBUG -Wall -Wextra -Werror -std=c++17 -pedantic

# standard
docklight_CPPFLAGS += -DDEBUG -Wall -std=c++17 -ggdb
#docklight_LDFLAGS = -lX11 -lXcomposite -lXdamage -lXfixes -lXtst


ALLDADDLIBS = $(GTKMM_LIBS)
ALLDADDLIBS +=  -lX11 -lXcomposite -lXdamage -lXrender -lXfixes -lXtst

docklight_LDADD = $(ALLDADDLIBS)

docklight_SOURCES =  \
utils/system.cpp \
utils/factory.cpp \
utils/wnck.cpp \
utils/struts.cpp \
utils/cairo.cpp \
utils/pixbuf.cpp \
utils/easing.cpp \
utils/themes.cpp \
components/animation.cpp \
components/titlewindow.cpp \
components/compositewindow.cpp \
components/animboomwindow.cpp \
components/dadwindow.cpp \
components/autohidemanager.cpp \
components/panelbase.cpp \
components/panel.cpp \
components/panelrender.cpp \
components/panelpreview.cpp \
components/dockmenu.cpp \
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

