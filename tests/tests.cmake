#TESTS
### Add some usable files to test
set (TestRequiredExtras
)
set (TESTS_FOLDER ${CMAKE_BINARY_DIR}/tests)

foreach (_file ${TestRequiredExtras})
        configure_file ( ${_file} ${_file} COPYONLY)
endforeach()

#test
add_executable(test_lua_sol2
               tests/lua_sol2.cpp
)
if(WIN32)
        add_dependencies( test_lua_sol2 _Lua)
        include_directories (${_Lua_DIR}/include)
        target_link_directories ( test_lua_sol2 PRIVATE ${_Lua_DIR}/lib)
endif()

target_link_libraries (test_lua_sol2 PRIVATE lua )#for sdl_opengl3 by find package

set_target_properties(test_lua_sol2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TESTS_FOLDER} )
#target_include_directories(test_lua_sol2
#    PRIVATE
#    src
#)
add_test(NAME test_lua_sol2 COMMAND test_lua_sol2 WORKING_DIRECTORY ${TESTS_FOLDER})