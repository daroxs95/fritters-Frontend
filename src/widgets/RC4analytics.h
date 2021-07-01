#pragma once

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

#include <implot/implot.h>
#include <addons/imguifilesystem/imguifilesystem.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <nfd.h>
#include <SDL2/SDL.h>

#include "../random_password.h"
#include "../crypto.h"
#include "../imgui_helpers.h"
#include "../gl_helpers.h"

#include "About.h"
#include "Help.h"


void RC4Analytics(ImGuiIO &io, SDL_Window* window)                              //RC4 multicipher from file
{

            //menus
            static bool calcSarkar = true;
            static bool calcPracticeCustomKeys = true;
            static bool calcPracticeAllKeys = true;
            static bool calcBase  = true;

            static bool showAbout = false, showHelp = false;


            static std::ifstream passwords_file;
            static bool calculating = false;
            static int number_of_passwords = 0;                   
            static float occurrence_probability_theoretical[256][256];          //calculated by theoretical formulae
            static float occurrence_probability_theoretical_Sarkar[256][256];   //calculated by theoretical formulae by Sarkar
            static long double mse_base[256];                                   //mean square error of base theoretical probability
            static long double mse_Sarkar[256];                                 //mean square error of Sarkar theoretical probability
            static int positions[] = {0};                                       //position to get probability at, for each histogram showed, size is of max histograms
            static char pathToPasswordsFile[ImGuiFs::MAX_PATH_BYTES] = "passwords.txt";
            
            //struct of base calculation type of probabilities after KSA in practice, any type of calculation needed experimentally
            //should be doable using this
            struct RC4_calc_instance_in_practice
            {
                char id[100];                                                    //id for each calculation
                float occurrence_probability[256][256];                         //holds probability of second_index to be on first_index at S_0(first state array of RC4), is float cause plothistogram does not support double
                std::function<std::string()> getPassword;                       //function to know distribution of key(it generates keys with determined distribution)
            };
            
            struct RC4_calc_instance_theoretical
            {
                char id[100];
                float occurrence_probability[256][256]; 
                std::function<double(uint8_t u, uint8_t v)> getProbability;
            };

            /*
            //this looks ugly, need to refactor
            static std::vector<RC4_calc_instance_in_practice> practiceProbabilities = { 
                RC4_calc_instance_in_practice(),
                RC4_calc_instance_in_practice() 
                };
            */
            static std::vector<RC4_calc_instance_in_practice> practiceProbabilities;//cant decide if is better this way or as above
            practiceProbabilities.resize(2);

            //put this in some conditional to no execute it every time in the main loop
            static bool inited = false;
            if (!inited)
            {
                strcpy(practiceProbabilities[0].id, "Full random password");
                practiceProbabilities[0].getPassword = []()->std::string{
                    return getRandomString(32);
                };

                strcpy(practiceProbabilities[1].id, "Full random password custom distribution");
                practiceProbabilities[1].getPassword = []()->std::string{
                    return getRandomStringCustomDistribution(32);
                };
                inited = true;
            }



            enum class CalculateFrom
            {
                file, generated
            };
            static CalculateFrom calculateFrom = CalculateFrom::file;


            
            if (!ImGui::Begin("RC4 S_0 analysis", NULL, ImGuiWindowFlags_MenuBar))
            {
                // Early out if the window is collapsed, as an optimization.
                ImGui::End();
                return;
            }

            // Menu Bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Probabilities"))
                {
                    ImGui::MenuItem("Shows calcs by Goutam Paul,Subhamoy Maitra, Rohit Srivastava", NULL, &calcBase);
                    ImGui::MenuItem("Shows calcs by Sarkar/Mantin formulae", NULL, &calcSarkar);
                    ImGui::MenuItem("Shows calcs experimentally using uniformly distributed keys", NULL, &calcPracticeAllKeys);
                    ImGui::MenuItem("Shows calcs experimentally using custom distributed keys", NULL, &calcPracticeCustomKeys);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools"))
                {
                    // ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
                    // ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
                    // ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    // ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
                    // ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
                    ImGui::MenuItem("About", NULL, &showAbout);
                    ImGui::MenuItem("Help", NULL, &showHelp);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (showAbout) about(&showAbout);
            if (showHelp)  help(&showHelp);

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
                            //TODO apply some check for validity of file and crossplatform stuff with std::path/filesystem, and the closing need to be checked too
                            if(!passwords_file.is_open()) passwords_file.open(pathToPasswordsFile);
                            
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
                        

                        for (size_t j = 0; j < practiceProbabilities.size(); j++) //with for(auto i:practiceProbabilities ) not working 
                        {
                            for (size_t i = 0 ; i < number_of_passwords; i++)
                            {
                                FillOcurrencesafterKSA(practiceProbabilities[j].occurrence_probability,
                                                        practiceProbabilities[j].getPassword() 
                                                    );
                            }
                               
                            for (int i = 0; i < 256; i++)
                            {
                                arrayOccurrences2probabilities(
                                    practiceProbabilities[j].occurrence_probability[i], 
                                    practiceProbabilities[j].occurrence_probability[i],
                                    256,
                                    number_of_passwords
                                    );
                            }
                        }
                        
                        GetRealProbabilitiesRC4afterKSA(occurrence_probability_theoretical);
                        GetRealProbabilitiesRC4afterKSA_SARKAR(occurrence_probability_theoretical_Sarkar);
                        for (short i = 0; i < 256; i++)
                        {
                            mse_base[i] = calcMSE(occurrence_probability_theoretical[i],practiceProbabilities[0].occurrence_probability[i],256);
                            mse_Sarkar[i] = calcMSE(occurrence_probability_theoretical_Sarkar[i],practiceProbabilities[0].occurrence_probability[i],256);
                        }
                        
                        calculating = false;
                    });
                    t.detach();
                }

            }
            else
            {
                ImGui::Text("Calculating");
            }
            
            //ImGui::SameLine();
            if(calculateFrom == CalculateFrom::generated)
            {
                ImGui::PushItemWidth(ImGui::GetFontSize() * -13);
                ImGui::InputInt("Number of passwords to generate ",&number_of_passwords);
            }
            else if(calculateFrom == CalculateFrom::file)
            {
                ImGui::Text("Number of passwords analysed = %d", number_of_passwords);



                //another file browser
                if(ImGui::Button("Open"))
                {
                    nfdchar_t *outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
                        
                    if ( result == NFD_OKAY ) {
                        printf("\n\tSuccessfully selected passwords file: %s \n", outPath);
                        //puts(outPath);
                        strcpy(pathToPasswordsFile,outPath);
                        free(outPath);
                    }
                    else if ( result == NFD_CANCEL ) {
                        printf("\n\tUser pressed cancel.\n");
                    }
                    else {
                        printf("\n\tError: %s\n", NFD_GetError() );
                    }
                }


                
                // File browser: DO NOT DELETE is for fallback if NFD does not work
                /*
                const bool browseButtonPressed = ImGui::Button("...");// we need a trigger boolean variable
                static ImGuiFs::Dialog dlg;// one per dialog (and must be static)
                const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed);// see other dialog types and the full list of arguments for advanced usage
                // If you want to copy the (valid) returned path somewhere, you can use something like:
                if (strlen(dlg.getChosenPath())>0) {
                    strcpy(pathToPasswordsFile,dlg.getChosenPath());
                }
                */

                ImGui::SameLine();
                HelpMarker("Select a file containing a list of passwords, and they must be splitted by newline");
                ImGui::SameLine();
                ImGui::Text("Chosen file: \"%s\"",pathToPasswordsFile);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(pathToPasswordsFile);

            }

            //ImGui::PushItemWidth(ImGui::GetFontSize() * -1);// Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

            for (int i = 0; i < IM_ARRAYSIZE(positions); i++)
            {
                ImGui::PushItemWidth(ImGui::GetFontSize() * -13);

                ImGui::PushID(i);
                ImGui::SliderInt("Value of u", &positions[i], 0, 255);//change way of adding label for something more performant
                ImGui::PopID();//i thin this should be the whole for loop

                ImGui::PushItemWidth(-1);// Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.
                

                if(ImGui::CollapsingHeader("Practical probability of each value to be at position u with uniformly distributed keys"))
                {
                    //drawing ImGui::PlotHistogram for each item in vector of structs RC4_calc_instance_in_practice
                    for (size_t ii = 0; ii < practiceProbabilities.size(); ii++)
                    {
                        //"Practical probability of each value to be at position u with uniformly distributed keys"
                        //if(ImGui::TreeNode(practiceProbabilities[ii].id))
                        if(ImGui::TreeNodeEx(practiceProbabilities[ii].id))
                        {
                            //check if needed a label for correct behavior, widgets with same labels apparently share properties, like focus
                            ImGui::PlotHistogram("", 
                                                practiceProbabilities[ii].occurrence_probability[positions[i]], 
                                                IM_ARRAYSIZE(practiceProbabilities[ii].occurrence_probability[positions[i]]), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                get_max(practiceProbabilities[ii].occurrence_probability[positions[i]],256), 
                                                ImVec2(0,80)
                                            );

                            ImGui::TreePop();
                            ImGui::Separator();
                        }
                    }
                }

                if(calcBase)
                {
                    if(ImGui::CollapsingHeader("Theorical probability of each value to be at position u"))
                    {
                        ImGui::PlotHistogram("", 
                                            occurrence_probability_theoretical[positions[i]], 
                                            IM_ARRAYSIZE(occurrence_probability_theoretical[positions[i]]), 
                                            0, 
                                            NULL, 
                                            0.001f, 
                                            get_max(occurrence_probability_theoretical[positions[i]],256), 
                                            ImVec2(0,80)
                                        );
                        ImGui::Text( "Mean Square Error = %e", mse_base[positions[i]] );
                    }
                }

                if(calcSarkar)
                {
                    if(ImGui::CollapsingHeader("Theorical probability of each value to be at position u, using Sarkar formulae"))
                    {
                        ImGui::PlotHistogram("", 
                                            occurrence_probability_theoretical_Sarkar[positions[i]], 
                                            IM_ARRAYSIZE(occurrence_probability_theoretical_Sarkar[positions[i]]), 
                                            0, 
                                            NULL, 
                                            0.001f, 
                                            get_max(occurrence_probability_theoretical_Sarkar[positions[i]],256), 
                                            ImVec2(0,80)
                                        );
                        ImGui::Text("Mean Square Error = %e", mse_Sarkar[positions[i]]);
                    }
                }

                //this is to make the widget zoomable/show it in a modal
                static bool showPlotFullscreen = false;
                if (showPlotFullscreen) 
                {
                    ImGui::OpenPopup("Fullscreen");
                }


                if (showPlotFullscreen) 
                {
                    // Always center this window when appearing, preparing the modal to render the widget on
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImVec2 size = ImGui::GetMainViewport()->Size;
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                    ImGui::SetNextWindowSize(size);
                    ImGui::BeginPopupModal("Fullscreen",NULL, ImGuiWindowFlags_NoTitleBar);//render the widget in a modal
                }
                ImPlot::SetNextPlotLimits(0,256,0,0.008);
                if (ImPlot::BeginPlot("Positions against its probability to end at position u(ie P(S[u]=x)","x","P(S[u]=x)")) 
                {

                    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                    
                    if(calcPracticeAllKeys)
                    {           
                        ImPlot::PlotShaded("Probabilities after KSA in practice new", practiceProbabilities[0].occurrence_probability[positions[i]], 256, 0);
                        ImPlot::PlotLine("Probabilities after KSA in practice new", practiceProbabilities[0].occurrence_probability[positions[i]], 256);
                    }

                    if(calcBase)
                    {
                        ImPlot::PlotShaded("Probabilities after KSA theoretically", occurrence_probability_theoretical[positions[i]], 256, 0);
                        ImPlot::PlotLine("Probabilities after KSA theoretically", occurrence_probability_theoretical[positions[i]], 256);
                    }

                    if(calcSarkar)
                    {
                        ImPlot::PlotShaded("Probabilities after KSA theoretically by Sarkar formulae", occurrence_probability_theoretical_Sarkar[positions[i]], 256, 0);
                        ImPlot::PlotLine("Probabilities after KSA theoretically by Sarkar formulae", occurrence_probability_theoretical_Sarkar[positions[i]], 256);
                        ImPlot::PopStyleVar();
                    }


                    ImPlot::EndPlot();
                }
                
                if (showPlotFullscreen)
                {
                    ImVec2 temp = ImGui::GetItemRectSize();

                    if(ImGui::Button("Save image"))
                    {
                        /*
                        SDL_Renderer *r_copie=SDL_GetRenderer(window);
                        SDL_Surface *sshot = SDL_CreateRGBSurface(0, 800, 800, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                        SDL_LockSurface(sshot);
                        SDL_RenderReadPixels(r_copie, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
                        SDL_SaveBMP(sshot, "screenshot.bmp");
                        SDL_UnlockSurface(sshot);
                        SDL_FreeSurface(sshot);
                        */

                        //saveImage("temp.png",temp[0], temp[1],1,ImGui::GetWindowSize()[1] - temp[1] -1);//there are 1pixel of diference for the border, need to generalize that better

                        
                        nfdchar_t *outPath = NULL;
                        nfdresult_t result = NFD_SaveDialog("png", NULL, &outPath );//the filterlist if splitted by comma shows the extensions together but like OR, and if splitted by ; shows each ext independently
                            
                        if ( result == NFD_OKAY ) {
                            printf("\n\tSuccessfully selected output file: %s \n", outPath);
                            //puts(outPath);
                            //strcpy(pathToPasswordsFile,outPath);
                            saveImage(outPath,temp[0], temp[1],1,ImGui::GetWindowSize()[1] - temp[1] -1);//there are 1pixel of diference for the border, need to generalize that better
                            free(outPath);

                        }
                        else if ( result == NFD_CANCEL ) {
                            printf("\n\tUser pressed cancel.\n");
                        }
                        else {
                            printf("\n\tError: %s\n", NFD_GetError() );
                        }
                        
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Close", ImVec2(120, 0))) 
                    { 
                        //ImGui::CloseCurrentPopup(); //not needed for now
                        showPlotFullscreen = false;
                        //ImGui::EndPopup();
                    }

                    ImGui::EndPopup();//end the modal where the widget is being rendered
                }



                //This is a context menu(control click) to change the view of the widget to a modal, need to be after the EndPopup() from the modal containing the widget, because the assignation being done here of bool variable holding if the widget is currently rendered in modal form or embebed in the windows.
                //need to make this optimal, ie: without the use of BeginPopupContextItem()
                if (!showPlotFullscreen && 
                    io.KeyCtrl &&
                    ImGui::BeginPopupContextItem("Positions against its probability to end at position u(ie P(S[u]=x)",
                    ImGuiPopupFlags_MouseButtonLeft))
                {
                        ImGui::CloseCurrentPopup();
                        showPlotFullscreen = true;
                    ImGui::EndPopup();
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
