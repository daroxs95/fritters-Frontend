#pragma once

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

#include <implot/implot.h>
#include <addons/imguifilesystem/imguifilesystem.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <nfd.h>
#include <SDL2/SDL.h>


#include "random_password.h"
#include "crypto.h"
#include "imgui_helpers.h"
#include "gl_helpers.h"

//widgets
#include "widgets/RC4analytics.h"
#include "widgets/HexStringConverter.h"
#include "widgets/RC4cipher.h"


struct app_state
{
};


static void app(app_state &state, ImVec4 &clear_color , ImGuiIO &io, SDL_Window* window)
{       
        /*
        //basic example
        {
            static float f = 10.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        */
        ImGui::DockSpaceOverViewport();//not needed but useful
        RC4cipher();
        HexStringConverter();
        RC4Analytics(io, window);

        ImPlot::ShowDemoWindow();

        //std::cout<< getRandomStringCustomDistribution(32) << std::endl;

}
