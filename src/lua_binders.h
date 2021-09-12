#pragma once
//Basic lua bindings, i genuinely do not know much of this, so probably is very bad this way, but it works for now

#include <imgui/imgui.h>


#include <sol/sol.hpp>


int bindImGui2sol2( sol::state &lua )
{
    sol::table imgui = lua["imgui"].get_or_create<sol::table>();

    imgui["Text"]                                           = ImGui::Text;
    //imgui["CollapsingHeader"]                               = ImGui::CollapsingHeader;
    imgui["Begin"]                                          = ImGui::Begin;
    imgui["End"]                                            = ImGui::End;

    //imgui.set_function<bool(const char*, bool*, ImGuiTreeNodeFlags)>("CollapsingHeader", &ImGui::CollapsingHeader);

    return 0;
}