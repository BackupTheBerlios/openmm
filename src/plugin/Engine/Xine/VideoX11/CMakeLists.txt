add_library(ommxinevideo-x11
SHARED
X11VideoXine.cpp
)

target_link_libaries(ommxinevideo-x11
ommengine-xine
X11
)

install(TARGETS ommxinevideo-x11
LIBRARY DESTINATION lib/omm
)
