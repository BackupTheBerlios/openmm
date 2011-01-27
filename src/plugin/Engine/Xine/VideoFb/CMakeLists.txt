add_library(ommxinevideo-fb
SHARED
FbVideoXine.cpp
)

target_link_libraries(ommxinevideo-fb
ommengine-xine
)

install(TARGETS ommxinevideo-fb
LIBRARY DESTINATION lib/omm
)
