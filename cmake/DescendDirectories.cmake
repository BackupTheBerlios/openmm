macro(descend_directories)

file(GLOB subdirs *)
foreach(dir ${subdirs})
if(IS_DIRECTORY ${dir})
message("Descend into directory ${dir} ...")
add_subdirectory(${dir})
endif(IS_DIRECTORY ${dir})
endforeach(dir)

endmacro()
