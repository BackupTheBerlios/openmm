# order of subdirectories matters here, hence no call to descend_directories()
set(subdirs
lib
plugin
app
test
)
foreach(dir ${subdirs})
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
message("Descend into directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir} ...")
add_subdirectory(${dir})
endif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
endforeach(dir ${subdirs})