cmake_minimum_required(VERSION 3.0.0)

project(frittersFrontend VERSION 0.0.1)

add_definitions( -DAPP_VERSION="${CMAKE_PROJECT_VERSION}" )

#set(CMAKE_EXE_LINKER_FLAGS "-mconsole -v")#console mode app// in gcc throws error, in msvc warning

include("collector/collector.cmake")#need to be after project()
include(FetchContent)

#set(FRESH_DOWNLOAD on)#this is for recloning collections on first time configuration, but is not recommended, cause is tampering the behavior of the cache variable with the same name

include(CTest)

enable_testing()

set (CMAKE_CXX_STANDARD 17)

include_directories("imgui")
include_directories("3rdParty/NFD/include")
include_directories("3rdParty")
include_directories("imgui/imgui/libs/gl3w")#only for use the opengl loader gl3w that resides inside source, can be retireved 

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #set(GUI_TYPE WIN32)#i this this is not needed at least for sdl
else()
    add_definitions( -DSDL_MAIN_HANDLED )
endif()

#selecting platform specific files for NFD
if (WIN32)#Windows settings
    list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_win.cpp")
elseif(UNIX AND NOT APPLE)#for linux only
    list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_gtk.c")
endif ()
list(APPEND NFD_SOURCES "3rdParty/NFD/nfd_common.c")


add_executable(frittersFrontend ${GUI_TYPE}
              "src/main.cpp"
              "imgui/imgui/imgui.cpp"
              "imgui/imgui/imgui_draw.cpp"
              "imgui/imgui/imgui_tables.cpp"#optional i think
              "imgui/imgui/imgui_widgets.cpp"
              ${NFD_SOURCES}#optional for native file dialog
              #"imgui/imgui/imgui_impl_dx12.cpp"#optional
              #"imgui/imgui/imgui_impl_dx9.cpp"#optional
              #"imgui/imgui/imgui_impl_dx11.cpp"#optional
              #"imgui/imgui/imgui_impl_win32.cpp"#optional
              "imgui/imgui/imgui_impl_sdl.cpp"#optional
              #"imgui/imgui/imgui_impl_glfw.cpp"#optional
              "imgui/imgui/libs/gl3w/GL/gl3w.c"#optional, in the end will be used something external, with collector
              #${COLLECTOR_INSTALLS}/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/share/gl3w/GL/gl3w.c#optional, not working, need to put it after created
              "imgui/imgui/imgui_impl_opengl3.cpp"#optional
              #"imgui/imgui/imgui_impl_vulkan.cpp"#optional
              "imgui/imgui/imgui_stdlib.cpp"#optional
              "imgui/imgui/imgui_demo.cpp"#optional
              "imgui/implot/implot.cpp"#optional
              "imgui/implot/implot_items.cpp"#optional
              "imgui/implot/implot_demo.cpp"#optional
              "imgui/addons/imguifilesystem/imguifilesystem.cpp"#optional
              )

#collect( "https://github.com/PotatoBite/fritters" "v0.0.11" frittersFrontend )
collect( "E:/coding/Projects/PotatoBite/fritters" "v0.0.12" frittersFrontend )#development

#collect( "D:/collector/_SDL-03e69d826e7dd77f343b1140fa34b9e803725f52" "release-2.0.14" frittersFrontend )
#collect( "https://github.com/skaslev/gl3w" "" frittersFrontend )#not needed for now, but is important to start using it
#collect( "https://github.com/glfw/glfw" "3.3.4" frittersFrontend )#not needed for now, but is important to start using it

#collect( "https://github.com/Rezonality/zep" "v0.1.12" frittersFrontend )


if (WIN32)#Windows settings
    list(APPEND CMAKE_PREFIX_PATH ${COLLECTOR_INSTALLS}/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/cmake)#need to put this in the collecor module, right now is for finding SDL2
elseif(UNIX AND NOT APPLE)#for linux only
    list(APPEND CMAKE_PREFIX_PATH ${COLLECTOR_INSTALLS}/${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/lib/cmake/SDL2)#need to put this in the collecor module, right now is for finding SDL2, this is for linux
endif ()



# SDL2 with fetch content
# FetchContent_Declare(
#     SDL2#can not have same names (ie: _SDL2) as collector cause cmake internally uses something similar
#   GIT_REPOSITORY "D:/collector/_SDL-03e69d826e7dd77f343b1140fa34b9e803725f52"
#   GIT_TAG        "release-2.0.14"         
# )
# FetchContent_MakeAvailable(SDL2)
# #FetchContent_GetProperties(SDL2)
# # if(NOT googletest_POPULATED)
# #   FetchContent_Populate(SDL2)
# #   add_subdirectory(${SDL2_SOURCE_DIR} ${SDL2_BINARY_DIR})
# #   execute_process(COMMAND "cmake -GNinja ." WORKING_DIRECTORY ${mariadb-connector-c_SOURCE_DIR})
# # endif()

# include_directories(${SDL2_SOURCE_DIR}/include)
# message("asdasdasd " ${SDL2_SOURCE_DIR} )
# message("asdasdasd " ${SDL2_BINARY_DIR} )
# list(APPEND CMAKE_PREFIX_PATH ${SDL2_SOURCE_DIR})#need to put this in the collecor module, right now is for finding SDL2




#find_package(SDL2 REQUIRED)#need to check this after the collection are installed, at least in this moment, if not used this way linker throws some errors


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
        #target_link_libraries (frittersFrontend PRIVATE fritters_static d3d9 xinput Imm32 Dwmapi jsoncpp_static)#for d3d9
        #target_link_libraries (frittersFrontend PRIVATE fritters_static d3d11 d3dcompiler Imm32 Dwmapi)#for d3d11
        #target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2::SDL2main SDL2::SDL2-static d3d11 d3dcompiler shell32)#for d3d11 sdl
        #target_link_libraries (frittersFrontend PRIVATE fritters_static d3d12 d3dcompiler Imm32 Dwmapi dxgi jsoncpp_static)#for d3d12
        #target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2::SDL2main SDL2::SDL2-static ${Vulkan_LIBRARY} shell32 )#for sdl_vulkan
        #target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2main SDL2-staticd opengl32 shell32 Imm32)#for sdl_opengl3
        target_link_libraries (frittersFrontend PRIVATE fritters_static SDL2::SDL2main SDL2::SDL2-static opengl32 shell32 )#for sdl_opengl3 by find package
        #target_link_libraries (frittersFrontend PRIVATE fritters_static glfw3 gdi32 opengl32 shell32 Imm32)#for glfw_opengl3
    elseif(UNIX AND NOT APPLE)#for linux only
        #target_link_libraries (frittersFrontend PRIVATE fritters_static GL glfw3 X11 ${CMAKE_DL_LIBS} pthread)#for glfw_opengl3
        #target_link_libraries (frittersFrontend PRIVATE fritters_static GL SDL2::SDL2main SDL2::SDL2 ${CMAKE_DL_LIBS} pthread)#for sdl_opengl3 by find package
        target_link_libraries (frittersFrontend PRIVATE fritters_static GL SDL2 ${CMAKE_DL_LIBS} pthread)#for sdl_opengl3 by find package
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
    "passwords.txt"
    "imgui/fonts/Roboto-Medium.ttf"
)
get_target_property(BINARY_EXECUTABLE_OUTPUT frittersFrontend RUNTIME_OUTPUT_DIRECTORY)
foreach (_file ${RequiredExtras})
    list( APPEND extraFiles COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/${_file}" "${BINARY_EXECUTABLE_OUTPUT}/$<CONFIG>/")
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
