cmake_minimum_required(VERSION 3.0.0)

project(frittersFrontend VERSION 0.0.1)

add_definitions( -DAPP_VERSION="${CMAKE_PROJECT_VERSION}" )

include("collector/collector.cmake")#need to be after project()

#set(FRESH_DOWNLOAD on)#this is for recloning collections on first time configuration, but is not recommended, cause is tampering the behavior of the cache variable with the same name

include(CTest)

enable_testing()

set (CMAKE_CXX_STANDARD 17)

include_directories("imgui/imgui")

add_executable(frittersFrontend 
              "main.cpp"
              "imgui/imgui/imgui.cpp"
              "imgui/imgui/imgui_draw.cpp"
              "imgui/imgui/imgui_tables.cpp"#optional i think
              "imgui/imgui/imgui_widgets.cpp"
              #"imgui/imgui/imgui_impl_dx9.cpp"#optional
              "imgui/imgui/imgui_impl_dx11.cpp"#optional
              "imgui/imgui/imgui_impl_win32.cpp"#optional
              "imgui/imgui/imgui_stdlib.cpp"#optional
              "imgui/imgui/imgui_demo.cpp"#optional
              "imgui/implot/implot.cpp"#optional
              "imgui/implot/implot_items.cpp"#optional
              "imgui/implot/implot_demo.cpp"#optional
              )

#collect( "https://github.com/PotatoBite/fritters" "v0.0.4" frittersFrontend )
collect( "E:/coding/Projects/github/PotatoBite/fritters" "v0.0.10" frittersFrontend )#development
collect( "https://github.com/open-source-parsers/jsoncpp" "1.9.4" frittersFrontend)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

#target_link_libraries (frittersFrontend PRIVATE fritters_static d3d9 xinput Imm32 Dwmapi)#for d3d9
target_link_libraries (frittersFrontend PRIVATE fritters_static d3d11 d3dcompiler Imm32 Dwmapi jsoncpp_static)#for d3d9



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

#try to avoid this
foreach (_file ${RequiredExtras})
        configure_file (
      ${PROJECT_SOURCE_DIR}/${_file}
      ${PROJECT_BINARY_DIR}/bin/${_file}
      COPYONLY
    )
endforeach()

#preparing install step
set_target_properties(frittersFrontend PROPERTIES RESOURCE "${RequiredExtras}")

install(TARGETS frittersFrontend
    RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
    RESOURCE DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION  ${CMAKE_INSTALL_LIBDIR}
)