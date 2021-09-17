cmake_minimum_required(VERSION 3.0.0)

project(frittersFrontend VERSION 0.0.3)

add_definitions( -DAPP_VERSION="${CMAKE_PROJECT_VERSION}" )
add_definitions( -DCOMPILER_SPEC="${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}" )
add_definitions( -DAPP_NAME="${PROJECT_NAME}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(WIN32)
        message(INFO " detected GNU compiler in windows, passing flag -mconsole")
        set(CMAKE_EXE_LINKER_FLAGS "-mconsole")#console mode app// unrecognized by msvc
    endif()
endif()

#set(CMAKE_EXE_LINKER_FLAGS "-v")#verbosity in linking call// unrecognized by msvc
set(FRESH_DOWNLOAD OFF)

include("collector/collector.cmake")#need to be after project()

#set(FRESH_DOWNLOAD on)#this is for recloning collections on first time configuration, but is not recommended, cause is tampering the behavior of the cache variable with the same name

include(CTest)

enable_testing()

set (CMAKE_CXX_STANDARD 17)

include_directories("3rdParty/imgui")
include_directories("3rdParty/NFD/include")
include_directories("3rdParty")
include_directories("3rdParty/imgui/imgui/libs/gl3w")#only for use the opengl loader gl3w that resides inside source, can be retireved 


#fix use of SDL, not fully tested on all platforms compilers, just on windows with MSVC and mingw
add_definitions( -DSDL_MAIN_HANDLED )



#selecting platform specific files for NFD
if (WIN32)#Windows settings
    list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_win.cpp")
elseif(UNIX AND NOT APPLE)#for linux only
    list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_gtk.c")
endif ()
list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_common.c")




#filling dearimgui basic required sources
set(IMGUI_BASIC_SOURCES "3rdParty/imgui/imgui/imgui.cpp"
                        "3rdParty/imgui/imgui/imgui_draw.cpp"
                        "3rdParty/imgui/imgui/imgui_tables.cpp"#optional i think
                        "3rdParty/imgui/imgui/imgui_widgets.cpp"
                        )

#filling dearimgui rendering backend necessary files
set(IMGUI_RENDERING_BACKEND_SOURCES "3rdParty/imgui/imgui/imgui_impl_sdl.cpp"
                        "3rdParty/imgui/imgui/imgui_impl_opengl3.cpp"
                        "3rdParty/imgui/imgui/imgui_tables.cpp"#optional i think
                        "3rdParty/imgui/imgui/imgui_widgets.cpp"
                        )




add_executable(frittersFrontend
            "src/main.cpp"
            ${IMGUI_BASIC_SOURCES}
            ${IMGUI_RENDERING_BACKEND_SOURCES}
            ${NFD_SOURCES}                                                  #optional for native file dialog
            "3rdParty/imgui/imgui/libs/gl3w/GL/gl3w.c"                      #optional, in the end will be used something external, with collector, using fectch_content and target_sources
            "3rdParty/imgui/imgui/imgui_stdlib.cpp"                         #optional
            "3rdParty/imgui/imgui/imgui_demo.cpp"                           #optional
            "3rdParty/imgui/implot/implot.cpp"                              #optional
            "3rdParty/imgui/implot/implot_items.cpp"                        #optional
            "3rdParty/imgui/implot/implot_demo.cpp"                         #optional
            "3rdParty/imgui/addons/imguifilesystem/imguifilesystem.cpp"     #optional
            #"3rdParty/imgui_lua_bindings/imgui_lua_bindings.cpp"            #optional
            )



collect     ( "https://github.com/PotatoBite/fritters"      "v0.0.15"          frittersFrontend )
collect     ( "https://github.com/gabime/spdlog"            "v1.8.5"           frittersFrontend )
collect     ( "https://github.com/ThePhD/sol2"              "v3.2.2"           frittersFrontend )

#temporal solution for lua
collect     ( "https://github.com/daroxs95/Lua"          "v5.4.2-install"   frittersFrontend )
add_dependencies( _Lua _spdlog)#temporal hack, without this linux build fails in _spdlog due cmake error: unknown flag -pthread
list(APPEND EXTRA_LIBS "lua")


if (WIN32)#Windows settings
    list(APPEND CMAKE_PREFIX_PATH ${COLLECTOR_INSTALLS}/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/${CMAKE_BUILD_TYPE}/cmake)#need to put this in the collecor module, right now is for finding SDL2
elseif(UNIX AND NOT APPLE)#for linux only
    #for NFD only
    # Use the package PkgConfig to detect GTK+ headers/library files
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    add_compile_options(${GTK3_CFLAGS_OTHER})
    target_include_directories(frittersFrontend PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_directories(frittersFrontend PRIVATE ${GTK3_LIBRARY_DIRS})

    list(APPEND CMAKE_PREFIX_PATH ${COLLECTOR_INSTALLS}/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/${CMAKE_BUILD_TYPE}/lib/cmake/SDL2)#need to put this in the collector module, right now is for finding SDL2, this is for linux
endif ()




find_package(SDL2)#need to check this after the collection are installed, at least in this moment, if not used this way linker throws some errors
if(NOT ${SDL2_FOUND})
    collect( "https://github.com/libsdl-org/SDL" "release-2.0.14" frittersFrontend )
    #rerun cmake in initial build
    #will update cmakecache/project files on first build
    #so you may have to reload project after first build
    message("SDL2 not found")
    add_custom_target(Rescan ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS _SDL)
else()
    #Rescan becomes a dummy target after first build
    #this prevents cmake from rebuilding cache/projects on subsequent builds
    add_custom_target(Rescan)
endif()
add_dependencies(frittersFrontend Rescan)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)

if(${SDL2_FOUND})
    if (WIN32)       #Windows settings
        target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2::SDL2main SDL2::SDL2-static opengl32 shell32 ${EXTRA_LIBS} )#for sdl_opengl3 by find package
    elseif(UNIX AND NOT APPLE)#for linux only
        target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2::SDL2main SDL2::SDL2-static GL ${CMAKE_DL_LIBS} pthread ${GTK3_LIBRARIES} ${EXTRA_LIBS})#for sdl_opengl3 by find package
        #target_compile_definitions(frittersFrontend PRIVATE ${GTK3_CFLAGS_OTHER})
    endif ()
endif()

include(GNUInstallDirs)

set_target_properties(frittersFrontend
                      PROPERTIES 
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)




### Add some usable files to project build folder
set (RequiredExtras
    "assets/passwords.txt"
    "assets/imgui.ini"
    "3rdParty/imgui/fonts/Roboto-Medium.ttf"
    "scripts/lua/main.lua"
)
get_target_property(BINARY_EXECUTABLE_OUTPUT frittersFrontend RUNTIME_OUTPUT_DIRECTORY)
foreach (_file ${RequiredExtras})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        list( APPEND extraFiles COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/${_file}" "${BINARY_EXECUTABLE_OUTPUT}/$<CONFIG>/")
    else()
        list( APPEND extraFiles COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/${_file}" "${BINARY_EXECUTABLE_OUTPUT}/")
    endif()
endforeach()
add_custom_command(TARGET frittersFrontend POST_BUILD
                    ${extraFiles}
)


#preparing install step
set_target_properties(frittersFrontend PROPERTIES RESOURCE "${RequiredExtras}")

install(TARGETS frittersFrontend
    RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
    RESOURCE DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION  ${CMAKE_INSTALL_LIBDIR}
)


#add tests
include("tests/tests.cmake")

#generate doc
include("docs/docs.cmake")