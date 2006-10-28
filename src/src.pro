# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/jam

HEADERS += svdrp.h \
           globalkeyhandler.h \
           menumain.h \
           menu.h \
           menuproguide.h \
           menutimers.h \
           menurecs.h \
           vdrrecs.h \
           tvrecplayer.h \
           controler.h \
           streamplayer.h \
           streamplayerxine.h \
           tv.h 
SOURCES += main.cpp \
           svdrp.cpp \
           globalkeyhandler.cpp \
           menumain.cpp \
           menu.cpp \
           menuproguide.cpp \
           menutimers.cpp \
           menurecs.cpp \
           vdrrecs.cpp \
           tvrecplayer.cpp \
           controler.cpp \
           streamplayer.cpp \
           streamplayerxine.cpp \
           tv.cpp 
LIBS += -lX11 \
`xine-config --libs`
TARGET = ../bin/jam
CONFIG += release \
warn_on \
qt \
thread
TEMPLATE = app
