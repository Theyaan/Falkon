#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <string>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void clear(float r, float g, float b, float a);
    void present(SDL_Window* window);

    void printGLInfo();
private:
    bool m_initialized;
};

#endif // RENDERER_HPP
