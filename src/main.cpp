#include "core/App.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Font.hpp"
#include "input/Input.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    App app;
    if (!app.initialize()) {
        std::cerr << "Failed to initialize App" << std::endl;
        return 1;
    }

    Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize Renderer" << std::endl;
        return 1;
    }

    Input input;
    if (!input.initialize()) {
        std::cerr << "Failed to initialize Input" << std::endl;
        return 1;
    }

    Font fontTitle;
    Font fontButton;

    if (!fontTitle.load("assets/fonts/Roboto-Regular.ttf", 48)) {
        std::cerr << "Failed to load title font" << std::endl;
    }

    if (!fontButton.load("assets/fonts/Roboto-Regular.ttf", 32)) {
        std::cerr << "Failed to load button font" << std::endl;
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};

    GLuint titleTexture = 0;
    int titleW = 0, titleH = 0;
    fontTitle.renderText("R36 ULTRA NATIVE APP", white, titleTexture, titleW, titleH);

    bool running = true;
    std::string lastActionStr = "NONE";
    GLuint actionTexture = 0;
    int actionW = 0, actionH = 0;

    // Center logical positions
    int logicalWidth = 720;
    int logicalHeight = 720;

    // Safe area margins
    int margin = 40;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                running = false;
            }

            input.update(event);

            if (input.getAction() != Action::NONE) {
                lastActionStr = input.getActionName();
                if (actionTexture) {
                    glDeleteTextures(1, &actionTexture);
                    actionTexture = 0;
                }
                fontButton.renderText("Pressed: " + lastActionStr, green, actionTexture, actionW, actionH);
            }
        }

        // Render
        renderer.clear(0.1f, 0.1f, 0.1f, 1.0f);

        if (titleTexture) {
            // Draw at center top
            int titleX = (logicalWidth - titleW) / 2;
            int titleY = margin + 50;
            fontTitle.drawText(titleTexture, titleX, titleY, titleW, titleH, 1.0f, 1.0f);
        }

        if (actionTexture) {
            // Draw below title
            int actionX = (logicalWidth - actionW) / 2;
            int actionY = logicalHeight / 2;
            fontButton.drawText(actionTexture, actionX, actionY, actionW, actionH, 1.0f, 1.0f);
        }

        renderer.present(app.getWindow());
        SDL_Delay(16); // ~60FPS
    }

    if (titleTexture) glDeleteTextures(1, &titleTexture);
    if (actionTexture) glDeleteTextures(1, &actionTexture);

    return 0;
}
