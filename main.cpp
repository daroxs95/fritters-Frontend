#include <fritters/RC4.h>
#include <fritters/utils.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>

//#include "imgui/easy_imgui_dx9.h"
//#include "imgui/easy_imgui_dx12.h"
#include "imgui/easy_imgui_dx11.h"
//#include "imgui/easy_imgui_sdl_gl3.h"
#include "imgui/implot/implot.h"
//#include "imgui/addons/imguiDock-master/imgui_dock.cpp"
//#include "imgui/addons/imguidock/imguidock.cpp"

#include "random_password.h"
#include "crypto.h"

//app functions
void RC4Analytics();
void RC4cipher();//RC4 cipher
void HexStringConverter();//hex-string converter

using namespace std;

struct app_state
{
};

static void app(app_state &app_state, ImVec4 &clear_color );

int main(void)
{
    /*
    {
        static std::ifstream passwords_file;

        static int number_of_passwords = 0;
        static float occurrence_probability[256][256];

        if (!passwords_file.is_open()) passwords_file.open("passwords.txt");

        std::list<std::string> passwords;
        {
            number_of_passwords = 0;

            std::string password;

            while (!passwords_file.eof())
            {
                std::getline(passwords_file, password);
                if (password != "")
                {
                    passwords.push_back(password);
                    number_of_passwords++;
                }
            }
            passwords_file.close();
        }

        static std::thread fillProbabilities(GetProbabilitiesRC4afterKSA, passwords, std::ref(occurrence_probability), number_of_passwords);
        //GetProbabilitiesRC4afterKSA(passwords, std::ref(occurrence_probability), number_of_passwords);
        fillProbabilities.join();
        cout << "calculate successfully";
    }
    */

    //create and fill state
    app_state state;

    srand((unsigned) time(0));//for generating random strings
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    m_imgui_app("Fritters Playground", 
        [&]()mutable->void{
            app(state, clear_color);
        }, 
        clear_color,
        [&](ImGuiIO &io)->void{
            //basic imgui
            ImGuiStyle& style = ImGui::GetStyle();

            ImGui::StyleColorsClassic();
            io.Fonts->AddFontFromFileTTF("imgui/fonts/Roboto-Medium.ttf", 15.0f);
            //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,6.0f);
            style.FrameRounding = 6.0f;
            style.GrabRounding = style.FrameRounding;//make the slider get the same rounding

            //colors
            style.Colors[ImGuiCol_WindowBg]               = ImVec4(56.0 / 255.0, 54.0 / 255.0, 50.0 / 255.0, 1.0f);
            style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
            style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.17f, 0.17f, 0.17f, 0.39f);
            
            //enable docking, not sure if needed
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            
            //plot
            ImPlot::CreateContext();
            ImPlot::GetStyle().AntiAliasedLines = true;

        },
        [&]()->void{
            //plot
            ImPlot::DestroyContext();
        },
        true);

    return 0;
}

static void app(app_state &state, ImVec4 &clear_color )
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
        RC4Analytics();

        ImPlot::ShowDemoWindow();

}

void RC4Analytics()//RC4 multicipher from file
{
            static std::ifstream passwords_file;
            static bool calculating = false;
            static int number_of_passwords = 0;
            static float occurrence_probability[256][256];//holds probability of second_index to be on first_index at S_0(first state array of RC4), is float cause plothistogram does not support double
            static float occurrence_probability_theoretical[256][256];//calculated by theoretical formulae
            static int positions[] = {0};//position to get probability at, for each histogram showed, size is of max histograms
            
            enum class CalculateFrom
            {
                file, generated
            };

            static CalculateFrom calculateFrom = CalculateFrom::file;


            //TODO apply some check for validity of file and crossplatform stuff with std::path/filesystem, and the closing need to be checked too
            if(!passwords_file.is_open()) passwords_file.open("passwords.txt");
            
            ImGui::Begin("RC4 S_0 analysis");//vanilla
            
            ImGui::Text("RC4 each possible value's probability to be in position u at first S(State Array) after KSA");

            if (ImGui::RadioButton("From file", calculateFrom == CalculateFrom::file)) { calculateFrom = CalculateFrom::file;} ImGui::SameLine();
            if (ImGui::RadioButton("From generated", calculateFrom == CalculateFrom::generated)) { calculateFrom = CalculateFrom::generated;} ImGui::SameLine();

            if(!calculating)
            {   
                if (ImGui::Button("Calculate"))
                {   
                    calculating = true;

                    std::thread t([&]()->void
                    {
                        std::list<std::string> passwords;
                        
                        srand((unsigned) time(0));//for generating random strings

                        if(calculateFrom == CalculateFrom::file)
                        {
                            number_of_passwords = 0;

                            std::string password;

                            while (! passwords_file.eof() )
                            {
                                std::getline(passwords_file, password);
                                if(password != "")
                                {
                                    passwords.push_back(password);                    
                                    number_of_passwords++;
                                }
                            }
                            passwords_file.close();
                        }
                        else if(calculateFrom == CalculateFrom::generated)
                        {
                            //set a default value for compatibility
                            if (number_of_passwords == 0) number_of_passwords = 500000;

                            for (size_t i = 0; i < number_of_passwords; i++)
                            {
                                passwords.push_back(getRandomString(32));
                            }
                        }
                        
                        GetProbabilitiesRC4afterKSA(passwords, occurrence_probability, number_of_passwords);
                        GetRealProbabilitiesRC4afterKSA(occurrence_probability_theoretical);
                        calculating = false;
                    });
                    t.detach();
                }

            }
            else
            {
                ImGui::Text("Calculating");
            }
            
            ImGui::SameLine();
            if(calculateFrom == CalculateFrom::generated)
            {
                ImGui::InputInt("Number of passwords to generate ",&number_of_passwords);
            }
            else if(calculateFrom == CalculateFrom::file)
            {
                ImGui::Text("Number of passwords analysed = %d", number_of_passwords);
            }

            //ImGui::PushItemWidth(ImGui::GetFontSize() * -1);// Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.
            ImGui::PushItemWidth(-1);// Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

            for (int i = 0; i < IM_ARRAYSIZE(positions); i++)
            {
                ImGui::Text("Value of u");
                ImGui::SameLine();
                ImGui::SliderInt(std::to_string(i).c_str(), &positions[i], 0, 255);
                //ImGui::InputInt(std::to_string(i).c_str(), &positions[i] );//change way of adding label for something more performant

                if(ImGui::CollapsingHeader("Practical probability of each value to be at position u"))
                {
                    //check if needed a label for correct behavior, widgets with same labels apparently share properties, like focus
                    ImGui::PlotHistogram("", occurrence_probability[positions[i]], IM_ARRAYSIZE(occurrence_probability[positions[i]]), 0, NULL, 0.001f, get_max(occurrence_probability[positions[i]],256), ImVec2(0,80));
                }

                if(ImGui::CollapsingHeader("Theorical probability of each value to be at position u"))
                {
                    ImGui::PlotHistogram("", occurrence_probability_theoretical[positions[i]], IM_ARRAYSIZE(occurrence_probability_theoretical[positions[i]]), 0, NULL, 0.001f, get_max(occurrence_probability_theoretical[positions[i]],256), ImVec2(0,80));
                }

                ImPlot::SetNextPlotLimits(0,256,0,0.008);
                if (ImPlot::BeginPlot("Positions(v) against its probability to be at position u(ie P(S[u]=x)","x","P(S[u]=x)")) {

                    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                    ImPlot::PlotShaded("Probabilities after KSA in practice", occurrence_probability[positions[i]], 256, 0);
                    ImPlot::PlotLine("Probabilities after KSA in practice", occurrence_probability[positions[i]], 256);
                                    

                    ImPlot::PlotShaded("Probabilities after KSA theoretically", occurrence_probability_theoretical[positions[i]], 256, 0);
                    ImPlot::PlotLine("Probabilities after KSA theoretically", occurrence_probability_theoretical[positions[i]], 256);
                    ImPlot::PopStyleVar();

                    ImPlot::EndPlot();
                }
            }


            //table TODO if in mood
            /*if (ImGui::CollapsingHeader("Window options"))
            {
                if (ImGui::BeginTable("split", 256))
                {
                    for (short i = 0; i < 256; i++)
                    {
                        ImGui::TableNextColumn();
                        for (short ii = 0; ii < 256; ii++)
                        {
                            ImGui::Text("%f", occurrence_probability[i][ii]);
                        }
                    }
                    ImGui::EndTable();
                }
            }*/

            ImGui::End();//vanilla
}

void RC4cipher()//RC4 cipher
{
            ImGui::Begin("RC4 cipher");//vanilla

            static string plaintext = "This is the plaintext";
            static string password;
            static string ciphertext = "This will be the ciphertext";
            static string hex_ciphertext = "This will be the ciphertext";

            static string *S0_array;
            static string dec_S0_array = "This is First State Array S[0]";
            static string hex_S0_array = "This is First State Array S[0]";

            static uint8_t tmepS0array[256];
            static std::stringstream tempStream;

            enum class Bases
            {
                hex, dec
            };

            static Bases showInBase = Bases::dec;

            if(showInBase == Bases::dec)
            {
                S0_array = &dec_S0_array;
            }
            else if (showInBase == Bases::hex)
            {
                S0_array = &hex_S0_array;
            }
            

            ImGui::PushItemWidth(ImGui::GetFontSize() * -10);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.



            if(ImGui::InputTextWithHint("","password", &password))
            {
                if(password.length() > 0)
                {
                    RC4 cipher(password);
                    
                    cipher.getStateArray(tmepS0array);//get the S[0] array
                    
                    //convert S[0] array to string
                    tempStream.str(std::string());//clearing the stream
                    for (size_t i = 0; i < 256; i++)
                    {
                        tempStream << tmepS0array[i];
                    }
                    hex_S0_array = string2hexstring(tempStream.str());
                    
                    tempStream.str(std::string());//clearing the stream
                    for (size_t i = 0; i < 256; i++)
                    {
                        tempStream << (unsigned int)tmepS0array[i] << " ";
                    }
                    dec_S0_array = tempStream.str();

                    ciphertext = plaintext;
                    cipher.inplaceCipher(ciphertext);
                    ciphertext = string2hexstring(ciphertext);
                }
            }

            if (ImGui::RadioButton("Hex", showInBase == Bases::hex)) { showInBase = Bases::hex;} ImGui::SameLine();
            if (ImGui::RadioButton("Dec", showInBase == Bases::dec)) { showInBase = Bases::dec;} ImGui::SameLine();

            ImGui::InputTextMultiline("First State Array", S0_array, ImVec2(0,60),ImGuiInputTextFlags_ReadOnly);

            if (ImGui::InputTextMultiline("plaintext", &plaintext, ImVec2(0,60)))
            {
                if(password.length() > 0)
                {
                    RC4 cipher(password);
                    cipher.getStateArray(tmepS0array);//get the S[0] array
                    ciphertext = plaintext;
                    cipher.inplaceCipher(ciphertext);
                    ciphertext = string2hexstring(ciphertext);
                }
            }

            if (ImGui::InputTextMultiline("ciphertext", &ciphertext, ImVec2(0,60),ImGuiInputTextFlags_CharsHexadecimal))
            {
                if(password.length() > 0)
                {
                    RC4 decipher(password);
                    plaintext = hexstring2asciistring(ciphertext);
                    decipher.inplaceCipher(plaintext);
                }
            }

            ImGui::End();//vanilla
}

void HexStringConverter()//hex-string converter
{
            ImGui::Begin("Hex-string converter");

            static string ascii_string = "This is the string to convert to hex-string";
            static string hex_string = "This is the string to convert to hex-string";

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