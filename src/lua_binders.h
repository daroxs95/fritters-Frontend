#pragma once
//Basic lua bindings, i genuinely do not know much of this, so probably is very bad this way, but it works for now

#include <functional>

#include <imgui/imgui.h>


#include <sol/sol.hpp>

bool mCollapsingHeader(const char* label, bool* p_visible, ImGuiTreeNodeFlags flags)
{
    return ImGui::CollapsingHeader(label,p_visible,flags);
}

bool mCollapsingHeaderSimple(const char* label, ImGuiTreeNodeFlags flags)
{
    return ImGui::CollapsingHeader(label, flags);
}

int bindImGui2sol2( sol::state &lua )
{
    sol::table imgui = lua["imgui"].get_or_create<sol::table>();

    imgui["Text"]                                                           = ImGui::Text;
    imgui["Begin"]                                                          = ImGui::Begin;
    imgui["End"]                                                            = ImGui::End;
    imgui["CollapsingHeader"]                                               = mCollapsingHeader;
    imgui["CollapsingHeaderSimple"]                                         = mCollapsingHeaderSimple;

    return 0;
}