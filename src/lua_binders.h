#pragma once
//Basic lua bindings, i genuinely do not know much of this, so probably is very bad this way, but it works for now

#include <functional>

#include <imgui/imgui.h>


#include <sol/sol.hpp>

#include "crypto.h"
#include "widgets/RC4analyticsConfig.h"


bool mCollapsingHeader(const char* label, bool* p_visible, ImGuiTreeNodeFlags flags)
{
    return ImGui::CollapsingHeader(label,p_visible,flags);
}

bool mCollapsingHeaderSimple(const char* label, ImGuiTreeNodeFlags flags = 0)
{
    return ImGui::CollapsingHeader(label, flags);
}

bool mButtonSimple(const char* label)
{
    return ImGui::Button(label);
}

bool mTreeNodeSimple(const char* label)
{
    return ImGui::TreeNode(label);
}

bool mMenuItemSimple(const char* label, const char* shortcut, bool* p_selected, bool enabled = true)
{
    return ImGui::MenuItem(label, shortcut, p_selected, enabled);
}


int bindImGui2sol2( sol::state &lua )
{
    sol::table imgui = lua["imgui"].get_or_create<sol::table>();

    imgui["Text"]                                                           = ImGui::Text;
    imgui["Begin"]                                                          = ImGui::Begin;
    imgui["End"]                                                            = ImGui::End;
    imgui["CollapsingHeader"]                                               = mCollapsingHeader;
    imgui["CollapsingHeaderSimple"]                                         = mCollapsingHeaderSimple;
    imgui["Button"]                                                         = mButtonSimple;
    //imgui["Button2"]                                                        = ImGui::Button;

    imgui["TreeNode"]                                                       = mTreeNodeSimple;
    imgui["TreePop"]                                                        = ImGui::TreePop;

    imgui["BeginMenuBar"]                                                   = ImGui::BeginMenuBar;
    imgui["EndMenuBar"]                                                     = ImGui::EndMenuBar;
    imgui["BeginMenu"]                                                      = ImGui::BeginMenu;
    imgui["EndMenu"]                                                        = ImGui::EndMenu;
    imgui["MenuItem"]                                                       = mMenuItemSimple;


    return 0;
}


int bindCryptoExperimentsStructs2sol2( sol::state &lua )
{
    sol::usertype<RC4calcInstanceInPractice> RC4calcInstanceInPractice_type = 
        lua.new_usertype<RC4calcInstanceInPractice>("RC4calcInstanceInPractice",
		sol::constructors<RC4calcInstanceInPractice()>());
    
    RC4calcInstanceInPractice_type["id"]                                    = &RC4calcInstanceInPractice::id;
    RC4calcInstanceInPractice_type["experimentsNumber"]                     = &RC4calcInstanceInPractice::experimentsNumber;
    RC4calcInstanceInPractice_type["getOccurrenceProbability"]              = &RC4calcInstanceInPractice::getOccurrenceProbability;
    RC4calcInstanceInPractice_type["isActive"]                              = &RC4calcInstanceInPractice::isActive;
    RC4calcInstanceInPractice_type["PRGAoutputsProbabilities"]                              = &RC4calcInstanceInPractice::PRGAoutputsProbabilities;


    return 0;
}


int bindConfigStructs2sol2( sol::state &lua )
{
    sol::usertype<Config> Config_type = 
        lua.new_usertype<Config>("Config", sol::constructors<Config()>());
 
    Config_type["plotHeight"]                                    = &Config::plotHeight;

    return 0;
}