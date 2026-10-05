#include "App.hpp"
#include <SDL2/SDL.h>
#include <iostream>

App::App() : m_running(false), m_window(nullptr), m_glContext(nullptr) {
    m_displayInfo = {720, 720, 1.0f, 1.0f, 1.0f};
}

App::~App() {
    quit();
}

bool App::initialize() {
    // We prioritize KMSDRM if available, but SDL will pick the best available.
    // Setting environment variable might be too strict if we want it to run on desktop for dev,
    // so we just let SDL choose and we print what it chose.

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "SDL video driver: " << SDL_GetCurrentVideoDriver() << std::endl;

    // Use OpenGL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Get display resolution
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        std::cerr << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError() << std::endl;
        // fallback
        dm.w = 720;
        dm.h = 720;
    }

    m_displayInfo.width = dm.w;
    m_displayInfo.height = dm.h;
    m_displayInfo.scaleX = dm.w / 720.0f;
    m_displayInfo.scaleY = dm.h / 720.0f;
    m_displayInfo.aspectRatio = static_cast<float>(dm.w) / dm.h;

    std::cout << "Display: " << dm.w << "x" << dm.h << std::endl;

    // Create window
    m_window = SDL_CreateWindow(
        "R36 Ultra Native App",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        dm.w,
        dm.h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Enable VSync
    SDL_GL_SetSwapInterval(1);

    m_running = true;
    return true;
}

void App::run() {
    // In later steps, we will inject Input and Renderer, and this will become the main loop
    while (m_running) {
        processInput();
        update();
        render();
    }
}

void App::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_running = false;
        }
    }
}

void App::update() {
    // Application state update
}

void App::render() {
    // Clear screen with a background color to show it's working
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(m_window);
}

void App::quit() {
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}
