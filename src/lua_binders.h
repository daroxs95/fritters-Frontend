#pragma once
//Basic lua bindings, i genuinely do not know much of this, so probably is very bad this way, but it works for now

#include <functional>

#include <imgui/imgui.h>


#include <sol/sol.hpp>


// C++ requires you to type out the same function body three times to obtain SFINAE-friendliness and 
// noexcept-correctness. That's unacceptable.
#define RETURNS(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__){ return __VA_ARGS__; }

// The name of overload sets can be legally used as part of a function call - we can use a macro to
// create a lambda for us that "lifts" the overload set into a function object.
#define LIFT(f) [](auto&&... xs) RETURNS(f(::std::forward<decltype(xs)>(xs)...))


int bindImGui2sol2( sol::state &lua )
{
    sol::table imgui = lua["imgui"].get_or_create<sol::table>();

    imgui["Text"]                                                           = ImGui::Text;
    imgui["Begin"]                                                          = ImGui::Begin;
    imgui["End"]                                                            = ImGui::End;
    
    std::function<bool(const char*, bool*, ImGuiTreeNodeFlags)> a           = LIFT(ImGui::CollapsingHeader);
    imgui["CollapsingHeader"]                                               = a;

    std::function<bool(const char*, ImGuiTreeNodeFlags)> b                  = LIFT(ImGui::CollapsingHeader);
    imgui["CollapsingHeaderSimple"]                                         = b;



    return 0;
}