#pragma once
//Basic lua bindings, i genuinely do not know much of this, so probably is very bad this way, but it works for now

#include <functional>

#include <imgui/imgui.h>
#include <sol/sol.hpp>
#include <easy_imgui/spdlog_helper.h>
#include <implot/implot.h>

#include "crypto.h"
#include "widgets/RC4analyticsConfig.h"
#include "data_types/common.h"


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

int mInputIntSimple(const char* label, int v, const int &step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0)
{
    ImGui::InputInt(label, &v, step, step_fast, 0);
    return v;
}

void mLoggerInfo(const char* text)
{
    getMultiSinkLogger().info(text);
}
void mLoggerWarn(const char* text)
{
    getMultiSinkLogger().warn(text);
}
void mLoggerError(const char* text)
{
    getMultiSinkLogger().error(text);
}

void mImPlotSimpleBeginPlot(const char* title_id,
                          const char* x_label       = NULL,
                          const char* y_label       = NULL,
                          const float& width        = -1,
                          const float& height       = 0
                          )
{
    ImPlot::BeginPlot(title_id,x_label,y_label,ImVec2(width,height));
}


int bindLogger2sol2 (sol::state &lua)
{
    sol::table logger = lua["logger"].get_or_create<sol::table>();

    logger["info"]      = mLoggerInfo;
    logger["warn"]      = mLoggerWarn;
    logger["error"]     = mLoggerError;

    return 0;
}


int bindImPlotr2sol2 (sol::state &lua)
{
    sol::table implot = lua["implot"].get_or_create<sol::table>();

    implot["BeginPlot"]      = mImPlotSimpleBeginPlot;
    implot["EndPlot"]      = ImPlot::EndPlot;
    //implot["PlotShaded"]      = template <> void ImPlot::PlotShaded<float>{};

    return 0;
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
    imgui["SameLine"]                                                       = ImGui::SameLine;
    imgui["InputInt"]                                                       = mInputIntSimple;


    imgui["TreeNode"]                                                       = mTreeNodeSimple;
    imgui["TreePop"]                                                        = ImGui::TreePop;

    imgui["BeginMenuBar"]                                                   = ImGui::BeginMenuBar;
    imgui["EndMenuBar"]                                                     = ImGui::EndMenuBar;
    imgui["BeginMenu"]                                                      = ImGui::BeginMenu;
    imgui["EndMenu"]                                                        = ImGui::EndMenu;
    imgui["MenuItem"]                                                       = mMenuItemSimple;
    imgui["SetClipboardText"]                                               = ImGui::SetClipboardText;

    return 0;
}


int bindCryptoExperimentsStructs2sol2( sol::state &lua )
{

    sol::usertype<Single256ArrayOutputProb> Single256ArrayOutputProb_type = 
        lua.new_usertype<Single256ArrayOutputProb>("Single256ArrayOutputProb",
		sol::constructors<Single256ArrayOutputProb()>());
    Single256ArrayOutputProb_type["getOccurrenceProbability"]               =&Single256ArrayOutputProb::getOccurrenceProbability;



    sol::usertype<RC4calcInstanceInPractice> RC4calcInstanceInPractice_type = 
        lua.new_usertype<RC4calcInstanceInPractice>("RC4calcInstanceInPractice",
		sol::constructors<RC4calcInstanceInPractice()>());
    
    RC4calcInstanceInPractice_type["id"]                                    = &RC4calcInstanceInPractice::id;
    RC4calcInstanceInPractice_type["experimentsNumber"]                     = &RC4calcInstanceInPractice::experimentsNumber;
    RC4calcInstanceInPractice_type["getOccurrenceProbability"]              = &RC4calcInstanceInPractice::getOccurrenceProbability;
    RC4calcInstanceInPractice_type["isActive"]                              = &RC4calcInstanceInPractice::isActive;
    RC4calcInstanceInPractice_type["PRGAoutputsProbabilities"]              = &RC4calcInstanceInPractice::PRGAoutputsProbabilities;
    RC4calcInstanceInPractice_type["PRGAoutputsProbabilitiesS1eq0"]         = &RC4calcInstanceInPractice::PRGAoutputsProbabilitiesS1eq0;
    RC4calcInstanceInPractice_type["PRGAoutputsProbabilitiesS1neq0"]        = &RC4calcInstanceInPractice::PRGAoutputsProbabilitiesS1neq0;

    
    return 0;
}


int bindConfigStructs2sol2( sol::state &lua )
{
    sol::usertype<Config> Config_type = 
        lua.new_usertype<Config>("Config", sol::constructors<Config()>());
 
    Config_type["plotHeight"]                                    = &Config::plotHeight;

    return 0;
}


int bindJarraysStructs2sol2( sol::state &lua )
{
    sol::usertype<jArrayStruct> jArrayStruct_type = 
        lua.new_usertype<jArrayStruct>("jArrayStruct", sol::constructors<jArrayStruct()>());
 
    jArrayStruct_type["getValue"]   = &jArrayStruct::getValue;
    jArrayStruct_type["getIsOdd"]   = &jArrayStruct::getIsOdd;

    return 0;
}