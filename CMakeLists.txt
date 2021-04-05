cmake_minimum_required(VERSION 3.0.0)

project(frittersFrontend VERSION 0.0.1)

add_definitions( -DAPP_VERSION="${CMAKE_PROJECT_VERSION}" )

include("collector/collector.cmake")#need to be after project()

#set(FRESH_DOWNLOAD on)#this is for recloning collections on first time configuration, but is not recommended, cause is tampering the behavior of the cache variable with the same name

include(CTest)

enable_testing()

set (CMAKE_CXX_STANDARD 17)

add_executable(frittersFrontend 
              "main.cpp"
              "imgui/imgui/imgui.cpp"
              "imgui/imgui/imgui_draw.cpp"
              "imgui/imgui/imgui_demo.cpp"
              "imgui/imgui/imgui_widgets.cpp"
              "imgui/imgui/imgui_impl_dx9.cpp"
              "imgui/imgui/imgui_impl_win32.cpp"
              "imgui/imgui/imgui_stdlib.cpp"
              )

#collect( "https://github.com/PotatoBite/fritters" "v0.0.4" frittersFrontend )
collect( "E:/coding/Projects/github/PotatoBite/fritters" "v0.0.6" frittersFrontend )#development

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

target_link_libraries (frittersFrontend PRIVATE fritters_static d3d9 xinput)



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
    ""
)

#target_link_libraries( Fritters )

#preparing install step
set_target_properties(frittersFrontend PROPERTIES RESOURCE "${RequiredExtras}")

install(TARGETS frittersFrontend
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)