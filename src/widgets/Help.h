#pragma once

#include <imgui/imgui.h>

void help(bool *handler)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImVec2 size = ImVec2(400,200);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size);
    if( ImGui::BeginPopupModal("About", handler) )
    {
        ImGui::TextWrapped("- You are abe to zoom in some plot widgets with control + click, and save images of the plot graphics.");
        ImGui::EndPopup();//end the modal where the widget is being rendered
    };
    ImGui::OpenPopup("About");
}