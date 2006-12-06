# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/jam

LIBS += -L/home/jb/devel/cc/jam/src/stuff/qt-embedded-free-3.3.7/lib \
        -lqte-mt \
        -lX11 \
        `/usr/local/bin/xine-config \
        --libs` 
QMAKE_CXXFLAGS_RELEASE += -fno-exceptions \
                          -fno-rtti 
DEFINES = QWS 
TARGET = ../bin/jam 
CONFIG += release \
          warn_on \
          qt \
          thread 
TEMPLATE = app 
HEADERS += globalkeyhandler.h \
           menu.h \
           controler.h \
           streamplayer.h \
           streamplayerxine.h \
           mediaplayer.h \
           tvplayer.h \
           tvchannel.h \
           tvtimer.h \
           list.h \
           listbrowser.h \
           tv.h \
           module.h \
           page.h \
           tvrecplayer.h \
           title.h \
           mrl.h \
           listproxy.h \
           listmanager.h \
           vdr.h \
           vdrrequest.h \
           tvprogram.h \
           popupmenu.h \
           tvtimerpopup.h \
           titlefilter.h \
           tvprogramfilter.h \
           tvrec.h \
           tvrecpopup.h \
           listcomposer.h \
           titlepair.h \
           tvprogrambrowser.h \
           tvprogrampopup.h 
SOURCES += main.cpp \
           globalkeyhandler.cpp \
           menu.cpp \
           controler.cpp \
           streamplayer.cpp \
           streamplayerxine.cpp \
           mediaplayer.cpp \
           tvplayer.cpp \
           tvchannel.cpp \
           tvtimer.cpp \
           list.cpp \
           listbrowser.cpp \
           tv.cpp \
           module.cpp \
           page.cpp \
           tvrecplayer.cpp \
           title.cpp \
           mrl.cpp \
           listproxy.cpp \
           listmanager.cpp \
           vdr.cpp \
           vdrrequest.cpp \
           tvprogram.cpp \
           popupmenu.cpp \
           tvtimerpopup.cpp \
           titlefilter.cpp \
           tvprogramfilter.cpp \
           tvrec.cpp \
           tvrecpopup.cpp \
           listcomposer.cpp \
           titlepair.cpp \
           tvprogrambrowser.cpp \
           tvprogrampopup.cpp 
