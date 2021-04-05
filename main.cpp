#include "imgui/easy_imgui_dx9.h"
#include <fritters/RC4.h>
#include <fritters/utils.h>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

struct app_state
{

};

std::string strtohex(std::string data)
{
    std::stringstream res;

    for (size_t i = 0; i <  data.size(); ++i)
    {
        std::cout << std::hex << std::setw(2) << static_cast<unsigned>(data[i]) << std::endl;
    }
    return res.str();
}

static void app(app_state &app_state, ImVec4 &clear_color );

int main(void)
{
    //create and fill state
    app_state state;

    std::cout<< "This is the text to cipher"<<std::endl << string2hexstring("This is the text to cipher")
    <<std::endl<< hexstring2asciistring(string2hexstring("This is the text to cipher"));
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    m_imgui_app("Fritters Playground", 
        [&]()mutable->void{
            app(state, clear_color);
        }, 
        clear_color,
        [](ImGuiIO &io)->void{
            ImGui::StyleColorsClassic();
            //io.Fonts->AddFontFromFileTTF("imgui/fonts/Roboto-Medium.ttf", 16.0f);
        },
        false);

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

            //not working
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

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

        //hex-string converter
        {
            ImGui::Begin("Hex-string converter");
            static string ascii_string = "This is the string to convert to hex-string";
            static string hex_string = "This is the string to convert to hex-string";

            if (ImGui::InputTextMultiline("ascii-string", &ascii_string, ImVec2(0,50)))
            {
                hex_string = string2hexstring(ascii_string);
            }

            if (ImGui::InputTextMultiline("hex-string", &hex_string, ImVec2(0,50)))
            {
                ascii_string = hexstring2asciistring(hex_string);
            }

            ImGui::End();
        }

        //RC4 cipher
        {
            ImGui::Begin("RC4 cipher");
            static string plaintext = "This is the plaintext";
            static string password;
            static string ciphertext = "This will be the ciphertext";
            //static string ciphertext = "This will be the ciphertext";

            if(ImGui::InputTextWithHint("","password", &password))
            {
                if(password.length() > 0)
                {
                    RC4 cipher(password);
                    ciphertext = plaintext;
                    cipher.inplaceCipher(ciphertext);
                    ciphertext = string2hexstring(ciphertext);
                }
            }

            ImGui::InputTextWithHint("","First State Array", &password);

            if (ImGui::InputTextMultiline("plaintext", &plaintext, ImVec2(0,60)))
            {
                if(password.length() > 0)
                {
                    RC4 cipher(password);
                    ciphertext = plaintext;
                    cipher.inplaceCipher(ciphertext);
                    ciphertext = string2hexstring(ciphertext);
                }
            }

            if (ImGui::InputTextMultiline("ciphertext", &ciphertext, ImVec2(0,60)))
            {
                if(password.length() > 0)
                {
                    RC4 decipher(password);
                    plaintext = hexstring2asciistring(ciphertext);
                    decipher.inplaceCipher(plaintext);
                }
            }

            ImGui::End();
        }
}