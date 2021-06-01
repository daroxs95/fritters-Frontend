
#pragma once

#include <stb/stb_image_write.h>
#include <vector>



//All this is not needed if used inside something that has already defined opengl loader, but is here just in case
    #if !defined(IMGUI_IMPL_OPENGL_ES2) \
    && !defined(IMGUI_IMPL_OPENGL_ES3) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3) \
    && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

    // Try to detect GLES on matching platforms
    #if defined(__APPLE__)
    #include "TargetConditionals.h"
    #endif
    #if (defined(__APPLE__) && (TARGET_OS_IOS || TARGET_OS_TV)) || (defined(__ANDROID__))
    #define IMGUI_IMPL_OPENGL_ES3               // iOS, Android  -> GL ES 3, "#version 300 es"
    #elif defined(__EMSCRIPTEN__)
    #define IMGUI_IMPL_OPENGL_ES2               // Emscripten    -> GL ES 2, "#version 100"

    // Otherwise try to detect supported Desktop OpenGL loaders..
    #elif defined(__has_include)
    #if __has_include(<GL/glew.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GLEW
    #elif __has_include(<glad/glad.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GLAD
    #elif __has_include(<glad/gl.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GLAD2
    #elif __has_include(<GL/gl3w.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GL3W
    #elif __has_include(<glbinding/glbinding.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3
    #elif __has_include(<glbinding/Binding.h>)
        #define IMGUI_IMPL_OPENGL_LOADER_GLBINDING2
    #else
        #error "Cannot detect OpenGL loader!"
    #endif
    #else
        #define IMGUI_IMPL_OPENGL_LOADER_GL3W   // Default to GL3W embedded in our repository
    #endif

    #endif

    #if defined(IMGUI_IMPL_OPENGL_ES2)
    #include <GLES2/gl2.h>
    // About Desktop OpenGL function loaders:
    //  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
    //  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
    //  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    #include <GL/gl3w.h>            // Initialize with gl3wInit()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    #include <GL/glew.h>            // Initialize with glewInit()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    #include <glad/glad.h>          // Initialize with gladLoadGL()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    #include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    #define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
    #include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
    #include <glbinding/gl/gl.h>
    using namespace gl;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    #define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
    #include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
    #include <glbinding/gl/gl.h>
    using namespace gl;
    #else
    #include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    #endif
//END of "All this is not needed if used inside something that has already defined opengl loader, but is here just in case"





void saveImage(char* filepath, int width = 600, int height= 600, int xpos = 0, int ypos = 0) 
{
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(xpos, ypos, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);

    //FILE *f;
    //f = stbiw__fopen(filepath, "wb");
    //stbi_write_png_to_func(filepath, width, height, nrChannels, buffer.data(), stride)
}


//for SDL2 to take screenshots
void flip_surface(SDL_Surface* surface) 
{
    SDL_LockSurface(surface);
    
    int pitch = surface->pitch; // row size
    char* temp = new char[pitch]; // intermediate buffer
    char* pixels = (char*) surface->pixels;
    
    for(int i = 0; i < surface->h / 2; ++i) {
        // get pointers to the two rows to swap
        char* row1 = pixels + i * pitch;
        char* row2 = pixels + (surface->h - i - 1) * pitch;
        
        // swap rows
        memcpy(temp, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, temp, pitch);
    }
    
    delete[] temp;

    SDL_UnlockSurface(surface);
}

void saveImageSDL(char* filepath, int width = 600, int height= 600, int xpos = 0, int ypos = 0) 
{
    SDL_Surface * image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
    //glReadBuffer(GL_FRONT);//with this does not work
    glReadPixels(xpos, ypos, width, height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    flip_surface(image);
    SDL_SaveBMP(image, filepath);
    SDL_FreeSurface(image);
}