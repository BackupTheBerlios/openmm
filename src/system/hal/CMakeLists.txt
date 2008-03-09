SET( JAMM_SYSTEM_HAL_SRCS
    hal.cpp
    haldeviceevent.cpp
    halwatcher.cpp
)

ADD_LIBRARY( jamm-system-hal
    SHARED
    ${JAMM_SYSTEM_HAL_SRCS}
)

TARGET_LINK_LIBRARIES( jamm-system-hal
    jamm-system-dbus
)

# SET_TARGET_PROPERTIES( jamm-system-hal
#     PROPERTIES INSTALL_RPATH ${jammproj_SOURCE_DIR}/../jammlibs/dbus/src/.libs
# )

# ----------------------------------------------------------------------
# Libraries have been installed in:
#    /usr/local/lib
# 
# If you ever happen to want to link against installed libraries
# in a given directory, LIBDIR, you must either use libtool, and
# specify the full pathname of the library, or use the `-LLIBDIR'
# flag during linking and do at least one of the following:
#    - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
#      during execution
#    - add LIBDIR to the `LD_RUN_PATH' environment variable
#      during linking
#    - use the `-Wl,--rpath -Wl,LIBDIR' linker flag
#    - have your system administrator add LIBDIR to `/etc/ld.so.conf'
# 
# See any operating system documentation about shared libraries for
# more information, such as the ld(1) and ld.so(8) manual pages.
# ----------------------------------------------------------------------


INSTALL( TARGETS
    jamm-system-hal
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
