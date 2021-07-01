#pragma once

#include <thread>//TODO implement calculations in some thread
#include <stdio.h>
#include <stdlib.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>


#include "../crypto.h"
#include "../imgui_helpers.h"


void HexStringConverter()//hex-string converter
{
            ImGui::Begin("Hex-string converter");

            static std::string ascii_string = "This is the string to convert to hex-string";
            static std::string hex_string   = "This is the string to convert to hex-string";

            ImGui::PushItemWidth(ImGui::GetFontSize() * -10);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

            if (ImGui::InputTextMultiline("ascii-string", &ascii_string, ImVec2(0,50)))
            {
                hex_string = string2hexstring(ascii_string);
            }

            if (ImGui::InputTextMultiline("hex-string", &hex_string, ImVec2(0,50), ImGuiInputTextFlags_CharsHexadecimal))
            {
                ascii_string = hexstring2asciistring(hex_string);
            }

            ImGui::End();//vanilla
}
