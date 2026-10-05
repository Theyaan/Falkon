#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

struct DisplayInfo {
    int width;
    int height;
    float scaleX;
    float scaleY;
    float aspectRatio;
};

class App {
public:
    App();
    ~App();

    bool initialize();
    void run();
    void quit();

    SDL_Window* getWindow() const { return m_window; }
    DisplayInfo getDisplayInfo() const { return m_displayInfo; }

private:
    bool m_running;
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    DisplayInfo m_displayInfo;

    void processInput();
    void update();
    void render();
};

#endif // APP_HPP
