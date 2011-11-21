cmake_minimum_required(VERSION 2.6.2)

# check basic dependency Poco here, because resgen is build standalone before everything
find_package(POCO REQUIRED)
include_directories(${POCO_INCLUDE_DIRS})

add_executable(resgen
main.cpp
)

target_link_libraries(resgen
${POCO_UTIL}
${POCO_XML}
${POCO_FOUNDATION}
)

if(NOT RESGEN)
set(RESGEN
${CMAKE_CURRENT_BINARY_DIR}/resgen
CACHE INTERNAL resgen
)
endif(NOT RESGEN)

install(TARGETS resgen
RUNTIME DESTINATION bin
)
