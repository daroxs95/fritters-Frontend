#pragma once


#ifndef APP_VERSION
# define APP_VERSION "0.0.0"
#endif
#ifndef COMPILER_SPEC
# define COMPILER_SPEC "UNDEFINED"
#endif


#include <imgui/imgui.h>


void about(bool *aboutHandler)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImVec2 size = ImVec2(400,200);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size);
    if( ImGui::BeginPopupModal("About", aboutHandler) )
    {
        ImGui::Text("Fritters Frontend version %s", APP_VERSION);
        ImGui::Text("Compiled with %s", COMPILER_SPEC);
        ImGui::Text("\nCreated and maintained by Darián López Utra");
        ImGui::Text("Contact by: \n Telegram: @SunShineXS \n email: daroxs95@gmail.com");
        ImGui::EndPopup();//end the modal where the widget is being rendered
    };
    ImGui::OpenPopup("About");
}