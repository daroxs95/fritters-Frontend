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
#include <fritters/RC4.h>
#include <spdlog/spdlog.h>

#include "../random_password.h"
#include "../crypto.h"
#include "../imgui_helpers.h"
#include "../gl_helpers.h"
#include <easy_imgui/spdlog_helper.h>


#include "About.h"
#include "Help.h"


struct RC4PRGAsingleByteOutputProbability4eachValue
{
    long double realOcurrences[256];    //number of each byte ocurrence for some K (K = index)
    float occurrenceProbability[256];    //probability of each byte ocurrence for some K
};


//struct of base calculation type of probabilities after KSA in practice, any type of calculation needed experimentally
//should be doable using this
struct RC4calcInstanceInPractice
{
    char id[200];                                                               //id for each calculation
    bool isActive = true;
    long double realOccurrenceProbability[256][256];                          //holds probability of second_index to be on first_index at S_0(first state array of RC4), is float cause plothistogram does not support double
    float occurrenceProbability[256][256];                                     //holds probability of second_index to be on first_index at S_0(first state array of RC4), is float cause plothistogram does not support double
    std::function<std::string()> getPassword;                                   //function to know distribution of key(it generates keys with determined distribution)
    std::vector<RC4PRGAsingleByteOutputProbability4eachValue> PRGAoutputsProbabilities;
    std::vector<RC4PRGAsingleByteOutputProbability4eachValue> PRGAoutputsProbabilitiesS1eq0;
    std::vector<RC4PRGAsingleByteOutputProbability4eachValue> PRGAoutputsProbabilitiesS1neq0;
};

struct RC4calcInstanceTheoretical
{
    char id[100];
    float occurrenceProbability[256][256]; 
    std::function<double(uint8_t u, uint8_t v)> getProbability;
};
            
struct jArrayStruct
{
    float values[256];
    float isOdd[256];//set to float cause ImGui histogram does not support bool
};


void RC4Analytics(ImGuiIO &io, SDL_Window* window)                              //RC4 multicipher from file
{

            //menus
            static bool calcSarkar = true;
            static bool calcPracticeCustomKeys = true;
            static bool calcPracticeAllKeys = true;
            static bool calcBase  = true;
            static bool calcFromFile = true;

            static bool showAbout = false, showHelp = false;


            static std::ifstream passwords_file;
            static bool calculating = false;
            static int passwordsNumber = 0;                   
            static int passwordsNumberFile = 0;                   
            static float theoricProbabilities[256][256];          //calculated by theoretical formulae
            static float theoricProbabilitiesSarkar[256][256];   //calculated by theoretical formulae by Sarkar
            static long double mseBase[256];                                   //mean square error of base theoretical probability
            static long double mseSarkar[256];                                 //mean square error of Sarkar theoretical probability
            static int positions[] = {0};                                       //position to get probability at, for each histogram showed, size is of max histograms
            static char pathToPasswordsFile[ImGuiFs::MAX_PATH_BYTES] = "passwords.txt";
            
            //create a handler to logging, TODO set this handler as a singleton to whole app
            static auto logger = getMultiSinkLogger();

 
            static std::vector<RC4calcInstanceInPractice> practiceProbabilities;//cant decide if is better this way or as above
            practiceProbabilities.resize(7);


            static std::vector<jArrayStruct>        jArrays;
            static const int                        maxNumJArrays = 10000; 




            //PRGA RELATED
            static int outputBytesNumberPRGA = 50;
            static bool link2KSAkeys = true; //not in use, but will be



            //put this in some conditional to no execute it every time in the main loop
            static bool inited = false;
            if (!inited)
            {
                logger.info("jarray max size = {}", jArrays.max_size());
                strcpy(practiceProbabilities[0].id, "Uniformly distributed keys, fully random chars/digits");
                practiceProbabilities[0].getPassword = []()->std::string{
                    return getRandomString(32);
                };

                strcpy(practiceProbabilities[1].id, "Custom distributed keys, {1,0}");
                practiceProbabilities[1].getPassword = []()->std::string{
                    return getRandomStringCustomDistribution(2);
                };


                strcpy(practiceProbabilities[2].id, "Custom distributed keys, {1,0,ODD,EVEN,ODD,EVEN...} variable and even keylength, 3 <= keylength <= 32");
                practiceProbabilities[2].getPassword = []()->std::string{
                    int size = rand()%29 + 3;
                    if (size % 2 != 0) size++;
                    
                    return getRandomStringCustomDistribution(size);
                };

                strcpy(practiceProbabilities[3].id, "Custom distributed keys, {1,0,ODD,EVEN,ODD,EVEN...}, jarray fill, variable and even keylength, 2 <= keylength <= 32");
                practiceProbabilities[3].getPassword = []()->std::string{
                    int size = rand()%30 + 2;
                    if (size % 2 != 0) size++;
                    std::string a = getRandomStringCustomDistribution(size);

                    //need to store small number of jArrays, couse each struct is like 2kb and the ram gets eaten really fast,
                    // and eventually the app could crash, ie for 2*10^6 keys 
                    if(jArrays.size() < maxNumJArrays)
                    {
                        jArrays.resize(jArrays.size() + 1);

                        uint8_t temp[256];
                        RC4 cipher("password");
                        cipher.KSA(a,temp);
                        for (short i = 0; i < 256; i++)
                        {
                            if(temp[i] % 2 != 0) jArrays.back().isOdd[i] = true;
                            else jArrays.back().isOdd[i] = false;

                            jArrays.back().values[i] = static_cast<float>(temp[i]);
                        }
                    }
                    
                    return a;
                };

                strcpy(practiceProbabilities[4].id, "Custom distributed keys, {1,0,ODD,EVEN,ODD,EVEN...} fixed and even keylength, keylength = 6");
                practiceProbabilities[4].getPassword = []()->std::string{
                    return getRandomStringCustomDistribution(6);
                };

                strcpy(practiceProbabilities[5].id, "Custom distributed keys, {1,0,ODD,EVEN,ODD,EVEN...} fixed and even keylength, keylength = 8");
                practiceProbabilities[5].getPassword = []()->std::string{
                    return getRandomStringCustomDistribution(8);
                };

                strcpy(practiceProbabilities[6].id, "Custom distributed keys, {1,0,ODD,EVEN,ODD,EVEN...} fixed and even keylength, keylength = 256");
                practiceProbabilities[6].getPassword = []()->std::string{
                    return getRandomStringCustomDistribution(256);
                };

                inited = true;
            }


            
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
                    ImGui::MenuItem("Shows calcs by Sarkar/Mantin formulae",                        NULL, &calcSarkar);
                    for (auto item : practiceProbabilities)
                    {
                        ImGui::MenuItem(item.id,NULL, &item.isActive );//TODO: isActive not passing as pointer(can not be changed in the app)
                    }
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
                    ImGui::MenuItem("Help",  NULL, &showHelp);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (showAbout) about(&showAbout);
            if (showHelp)  help(&showHelp);



            if(calcFromFile)
            {         
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Number of passwords in file = %d", passwordsNumberFile);
                ImGui::SameLine();


                //another file browser
                if(ImGui::Button("Open"))
                {
                    nfdchar_t *outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
                        
                    if ( result == NFD_OKAY ) {
                        //printf("\n\tSuccessfully selected passwords file: %s \n", outPath);
                        logger.info("Successfully selected passwords file: {}", outPath);
                        
                        //puts(outPath);
                        strcpy(pathToPasswordsFile,outPath);
                        free(outPath);
                    }
                    else if ( result == NFD_CANCEL ) {
                        //printf("\n\tUser pressed cancel.\n");//not logging here, because is a weird thing lo keep track
                    }
                    else {
                        //printf("\n\tError: %s\n", NFD_GetError() );
                        logger.error("Error selecting file: {}", NFD_GetError() );
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

            //ImGui::PushItemWidth(ImGui::GetFontSize() * -13);
            ImGui::InputInt("Number of passwords to generate ",&passwordsNumber);

           
            if(!calculating)
            {   
                if (ImGui::Button("Calculate"))
                {   
                    //spdlog::info("Calculating!");
                    logger.info("Calculating!");

                    calculating = true;

                    std::thread t([&]()->void
                    {
                        std::list<std::string> passwords;
                        
                        srand((unsigned) time(0));//for generating random strings

                        jArrays.clear();
                        
                        if( calcFromFile )
                        {
                            passwordsNumberFile = 0;

                            std::string password;
                            //TODO apply some check for validity of file and crossplatform stuff with std::path/filesystem, and the closing need to be checked too
                            if(!passwords_file.is_open()) passwords_file.open(pathToPasswordsFile);
                            
                            while (! passwords_file.eof() )
                            {
                                std::getline(passwords_file, password);
                                if(password != "")
                                {
                                    passwords.push_back(password);                    
                                    passwordsNumberFile++;
                                }
                            }
                            passwords_file.close();
                        }

                        //set a default value for compatibility
                        if (passwordsNumber == 0) passwordsNumber = 500000;
                        

                        for (size_t j = 0; j < practiceProbabilities.size(); j++) //with for(auto i:practiceProbabilities ) not working 
                        {
                            InitArrayTo(practiceProbabilities[j].realOccurrenceProbability,(long double)0);
                            practiceProbabilities[j].PRGAoutputsProbabilities.clear();
                            practiceProbabilities[j].PRGAoutputsProbabilities.resize(outputBytesNumberPRGA);

                            for (size_t i = 0 ; i < passwordsNumber; i++)
                            {
                                auto outputPRGA = FillOcurrencesAfterKSAreturnPRGAstream(
                                                        practiceProbabilities[j].realOccurrenceProbability,
                                                        practiceProbabilities[j].getPassword(),
                                                        outputBytesNumberPRGA
                                                    );

                                for (size_t ii = 0; ii < outputPRGA.size(); ii++)
                                {
                                    practiceProbabilities[j].PRGAoutputsProbabilities[ii].realOcurrences[(uint8_t)outputPRGA[ii]]++;
                                }


                            }
                               
                            for (int i = 0; i < 256; i++)
                            {
                                arrayOccurrences2probabilities(
                                    practiceProbabilities[j].realOccurrenceProbability[i], 
                                    practiceProbabilities[j].occurrenceProbability[i],
                                    256,
                                    passwordsNumber
                                );
                            }

                            for( int i = 0 ; i < practiceProbabilities[j].PRGAoutputsProbabilities.size(); i++)
                            {
                                arrayOccurrences2probabilities(
                                    practiceProbabilities[j].PRGAoutputsProbabilities[i].realOcurrences,
                                    practiceProbabilities[j].PRGAoutputsProbabilities[i].occurrenceProbability,
                                    256,
                                    passwordsNumber
                                );
                            }
                            
                        }
                        
                        GetRealProbabilitiesRC4afterKSA(theoricProbabilities);
                        GetRealProbabilitiesRC4afterKSA_SARKAR(theoricProbabilitiesSarkar);
                        for (short i = 0; i < 256; i++)
                        {
                            mseBase[i] = calcMSE(theoricProbabilities[i],practiceProbabilities[0].occurrenceProbability[i],256);
                            mseSarkar[i] = calcMSE(theoricProbabilitiesSarkar[i],practiceProbabilities[0].occurrenceProbability[i],256);
                        }
                        
                        calculating = false;
                        
                        /*
                        temp
                        logger.info(practiceProbabilities[4].occurrenceProbability[0][1]);
                        logger.info(practiceProbabilities[5].occurrenceProbability[0][1]);
                        logger.info(practiceProbabilities[6].occurrenceProbability[0][1]);
                        end temp
                        */

                    });
                    t.detach();

                }

            }
            else
            {
                ImGui::Text("Calculating");
            }
            
            if(ImGui::CollapsingHeader("Probability of State Array's possible values(bytes) to be in position u in S(State Array) after KSA"))
            {
                //ImGui::PushItemWidth(ImGui::GetFontSize() * -1);// Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

                for (int i = 0; i < IM_ARRAYSIZE(positions); i++)
                {
                    ImGui::PushItemWidth(ImGui::GetFontSize() * -13);

                    ImGui::PushID(i);
                    ImGui::SliderInt("Value of u", &positions[i], 0, 255);//change way of adding label for something more performant
                    ImGui::PopID();//i thin this should be the whole for loop
                    ImGui::PopItemWidth();

                    ImGui::PushItemWidth(-1);

                    if(ImGui::CollapsingHeader("Practical probability of each value to be at position u"))
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
                                                    practiceProbabilities[ii].occurrenceProbability[positions[i]], 
                                                    IM_ARRAYSIZE(practiceProbabilities[ii].occurrenceProbability[positions[i]]), 
                                                    0, 
                                                    NULL, 
                                                    0.001f, 
                                                    get_max(practiceProbabilities[ii].occurrenceProbability[positions[i]],256), 
                                                    ImVec2(0,80)
                                                );

                                ImGui::TreePop();
                                ImGui::Separator();
                            }
                        }
                    }

                    if (ImGui::CollapsingHeader("j indexes for each analyzed key(index used in each iteration of KSA)"))
                    {
                        if (jArrays.size() > 0 )
                        {
                            static int positionJ;
                            ImGui::SliderInt("Value of u", &positionJ, 0, jArrays.size()-1);
                            ImGui::PlotHistogram("jArray values", 
                                                jArrays[positionJ].values, 
                                                IM_ARRAYSIZE(jArrays[positionJ].values), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                //get_max(jArrays[positionJ].values,256), 
                                                256,
                                                ImVec2(0,80)
                                            );
                            ImGui::PlotHistogram("jArray isOdd", 
                                                jArrays[positionJ].isOdd, 
                                                IM_ARRAYSIZE(jArrays[positionJ].isOdd), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                1, 
                                                ImVec2(0,80)
                                            );
                        }
                    }

                    if(calcBase)
                    {
                        if(ImGui::CollapsingHeader("Theorical probability of each value to be at position u"))
                        {
                            ImGui::PlotHistogram("", 
                                                theoricProbabilities[positions[i]], 
                                                IM_ARRAYSIZE(theoricProbabilities[positions[i]]), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                get_max(theoricProbabilities[positions[i]],256), 
                                                ImVec2(0,80)
                                            );
                            ImGui::Text( "Mean Square Error = %e", mseBase[positions[i]] );
                        }
                    }

                    if(calcSarkar)
                    {
                        if(ImGui::CollapsingHeader("Theorical probability of each value to be at position u, using Sarkar formulae"))
                        {
                            ImGui::PlotHistogram("", 
                                                theoricProbabilitiesSarkar[positions[i]], 
                                                IM_ARRAYSIZE(theoricProbabilitiesSarkar[positions[i]]), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                get_max(theoricProbabilitiesSarkar[positions[i]],256), 
                                                ImVec2(0,80)
                                            );
                            ImGui::Text("Mean Square Error = %e", mseSarkar[positions[i]]);
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
                    if (ImPlot::BeginPlot("Positions against its probability to end at position u(ie P(S[u]=x)","x","P(S[u]=x)") ) 
                    {

                        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                        
                        
                        if(calcPracticeAllKeys)
                        {           
                            for (size_t j = 0; j < practiceProbabilities.size(); j++)
                            {
                                ImPlot::PlotShaded(practiceProbabilities[j].id, practiceProbabilities[j].occurrenceProbability[positions[i]], 256, 0);
                                ImPlot::PlotLine(practiceProbabilities[j].id, practiceProbabilities[j].occurrenceProbability[positions[i]], 256);
                            }
                        }

                        if(calcBase)
                        {
                            ImPlot::PlotShaded("Probabilities after KSA theoretically", theoricProbabilities[positions[i]], 256, 0);
                            ImPlot::PlotLine("Probabilities after KSA theoretically", theoricProbabilities[positions[i]], 256);
                        }

                        if(calcSarkar)
                        {
                            ImPlot::PlotShaded("Probabilities after KSA theoretically by Sarkar formulae", theoricProbabilitiesSarkar[positions[i]], 256, 0);
                            ImPlot::PlotLine("Probabilities after KSA theoretically by Sarkar formulae", theoricProbabilitiesSarkar[positions[i]], 256);
                            ImPlot::PopStyleVar();
                        }


                        ImPlot::EndPlot();
                    }
                    
                    if (showPlotFullscreen)
                    {

                        if(ImGui::Button("Save image"))
                        {
                            ImVec2 temp                     = ImGui::GetItemRectSize();
                            ImVec2 winOffset2Widget         = ImGui::GetStyle().WindowPadding; //this is global, like any other imgui setting, find something else local if posible

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
                                //printf("\n\tSuccessfully selected output file: %s \n", outPath);
                                //puts(outPath);
                                //strcpy(pathToPasswordsFile,outPath);
                                if( saveImage(outPath,temp[0], temp[1],winOffset2Widget[0],ImGui::GetWindowSize()[1] - temp[1] -winOffset2Widget[1]) == 0)
                                {
                                    logger.info("Successfully saved image to: {}", outPath);
                                }
                                free(outPath);

                            }
                            else if ( result == NFD_CANCEL ) {
                                //printf("\n\tUser pressed cancel.\n");//not logging here, because is a weird thing lo keep track
                            }
                            else {
                                //printf("\n\tError: %s\n", NFD_GetError() );
                                logger.error("Error selecting file: {}", NFD_GetError() );
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
                    
                    ImGui::PopItemWidth();
                    
                }
            }

            if(ImGui::CollapsingHeader("Probability of some Ks(outputs of PRGA) to be each possible byte"))
            //if(ImGui::CollapsingHeader("Probability of each possible byte to appear in each of the first K(outputs of PRGA)"))
            {
                static int Kvalue = 0;
                ImGui::InputInt("Number of bytes of output to generate/analyze ",&outputBytesNumberPRGA);
                ImGui::Checkbox("Link to KSA keys", &link2KSAkeys);
                ImGui::SameLine();
                ImGui::SliderInt("Index K( byte output )", &Kvalue, 0, outputBytesNumberPRGA - 1);//change way of adding label for something more performant
                ImGui::PushItemWidth(-1);
                if(ImGui::CollapsingHeader("Practical probability of each value to be at output byte"))
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
                                                practiceProbabilities[ii].PRGAoutputsProbabilities[Kvalue].occurrenceProbability, 
                                                IM_ARRAYSIZE(practiceProbabilities[ii].PRGAoutputsProbabilities[Kvalue].occurrenceProbability), 
                                                0, 
                                                NULL, 
                                                0.001f, 
                                                get_max(practiceProbabilities[ii].PRGAoutputsProbabilities[Kvalue].occurrenceProbability,256), 
                                                ImVec2(0,80)
                                            );
                            ImGui::TreePop();
                            ImGui::Separator();
                        }
                    }
                }
                ImGui::PopItemWidth();
                
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
                            ImGui::Text("%f", occurrenceProbability[i][ii]);
                        }
                    }
                    ImGui::EndTable();
                }
            }*/
            

            ImGui::End();//vanilla
}
