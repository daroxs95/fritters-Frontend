//#include <fritters/RC4.h>
#include <fritters/utils.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifndef APP_VERSION
# define APP_VERSION "0.0.0"
#endif
#ifndef COMPILER_SPEC
# define COMPILER_SPEC "UNDEFINED"
#endif

#include <easy_imgui/easy_imgui_sdl_gl3.h>
#include <easy_imgui/spdlog_helper.h>

#include <spdlog/spdlog.h>

#include "app.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF//this is not working to deactivate spdlog
using namespace std;

int main(void)
{
    auto logger = getMultiSinkLogger();
    logger.info("Starting app...");

    //create and fill state
    appState state;

    srand((unsigned) time(0));//for generating random strings
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    m_imgui_app("Fritters Playground", 
        [&](ImGuiIO &io, SDL_Window* sdl_window)mutable->void{
            app(state, clear_color, io, sdl_window);
        }, 
        clear_color,
        [&](ImGuiIO &io, SDL_Window* sdl_window)->void{
            //basic imgui
            ImGuiStyle& style = ImGui::GetStyle();

            io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 15.0f);
            //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,6.0f);
            style.FrameRounding = 6.0f;
            style.GrabRounding = style.FrameRounding;//make the slider get the same rounding

            //colors
            //theme dark mine
            ImGui::StyleColorsClassic();
            style.Colors[ImGuiCol_WindowBg]               = ImVec4(56.0 / 255.0, 54.0 / 255.0, 50.0 / 255.0, 1.0f);
            style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
            style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.17f, 0.17f, 0.17f, 0.39f);
            //theme light
            ImGui::StyleColorsLight();
            //style.FrameBorderSize = 1;

            //enable docking, not sure if needed
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            
            //plot
            ImPlot::CreateContext();
            ImPlot::GetStyle().AntiAliasedLines = true;
            ImPlot::GetStyle().Colormap = ImPlotColormap_Dark;

            SDL_MaximizeWindow(sdl_window);
        },
        [&]()->void{
            //plot
            ImPlot::DestroyContext();
        },
        true);



    logger.info("Graceful quiting app...\n");

    return 0;
}
