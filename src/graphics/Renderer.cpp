#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer() : m_initialized(false) {
}

Renderer::~Renderer() {
}

bool Renderer::initialize() {
    printGLInfo();
    m_initialized = true;
    return true;
}

void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::present(SDL_Window* window) {
    SDL_GL_SwapWindow(window);
}

void Renderer::printGLInfo() {
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);

    std::cout << "OpenGL ES Vendor: " << (vendor ? reinterpret_cast<const char*>(vendor) : "Unknown") << std::endl;
    std::cout << "OpenGL ES Renderer: " << (renderer ? reinterpret_cast<const char*>(renderer) : "Unknown") << std::endl;
    std::cout << "OpenGL ES Version: " << (version ? reinterpret_cast<const char*>(version) : "Unknown") << std::endl;
}
