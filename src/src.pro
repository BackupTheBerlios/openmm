# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/jam

HEADERS += jam.h \
           jamstreamplayer.h \
           jamstreamplayerxine.h \
           jamtv.h \
           svdrp.h \
           globalkeyhandler.h \
           menumain.h \
           menu.h \
           menuproguide.h 
SOURCES += jam.cpp \
           main.cpp \
           jamstreamplayer.cpp \
           jamstreamplayerxine.cpp \
           jamtv.cpp \
           svdrp.cpp \
           globalkeyhandler.cpp \
           menumain.cpp \
           menu.cpp \
           menuproguide.cpp 
LIBS += -lX11 \
`xine-config --libs`
TARGET = ../bin/jam
CONFIG += release \
warn_on \
qt \
thread
TEMPLATE = app
