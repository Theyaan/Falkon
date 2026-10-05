#include "Input.hpp"
#include <iostream>

Input::Input() : m_controller(nullptr), m_currentAction(Action::NONE) {
}

Input::~Input() {
    if (m_controller) {
        SDL_GameControllerClose(m_controller);
        m_controller = nullptr;
    }
}

bool Input::initialize() {
    if (SDL_NumJoysticks() > 0) {
        if (SDL_IsGameController(0)) {
            m_controller = SDL_GameControllerOpen(0);
            if (m_controller) {
                std::cout << "Controller connected: " << SDL_GameControllerName(m_controller) << std::endl;
            } else {
                std::cerr << "Could not open game controller: " << SDL_GetError() << std::endl;
            }
        }
    }

    // Optional: enable events if not already
    SDL_GameControllerEventState(SDL_ENABLE);
    return true;
}

void Input::update(SDL_Event& event) {
    m_currentAction = Action::NONE; // Reset action per frame unless repeating is implemented

    if (event.type == SDL_CONTROLLERBUTTONDOWN) {
        m_currentAction = mapButton(event.cbutton.button);
    } else if (event.type == SDL_CONTROLLERAXISMOTION) {
        // Deadzone check
        if (abs(event.caxis.value) > 8000) {
            m_currentAction = mapAxis(event.caxis.axis, event.caxis.value);
        }
    } else if (event.type == SDL_KEYDOWN) {
        // Fallback to keyboard for testing on desktop
        switch (event.key.keysym.sym) {
            case SDLK_UP: m_currentAction = Action::UP; break;
            case SDLK_DOWN: m_currentAction = Action::DOWN; break;
            case SDLK_LEFT: m_currentAction = Action::LEFT; break;
            case SDLK_RIGHT: m_currentAction = Action::RIGHT; break;
            case SDLK_RETURN:
            case SDLK_x: m_currentAction = Action::ACCEPT; break; // X as A
            case SDLK_z: m_currentAction = Action::BACK; break; // Z as B
            case SDLK_ESCAPE: m_currentAction = Action::START; break;
        }
    }
}

Action Input::mapButton(Uint8 button) {
    switch (button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return Action::UP;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return Action::DOWN;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return Action::LEFT;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return Action::RIGHT;
        case SDL_CONTROLLER_BUTTON_A: return Action::ACCEPT;
        case SDL_CONTROLLER_BUTTON_B: return Action::BACK;
        case SDL_CONTROLLER_BUTTON_START: return Action::START;
        case SDL_CONTROLLER_BUTTON_BACK: return Action::SELECT;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return Action::PAGE_UP;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Action::PAGE_DOWN;
        default: return Action::NONE;
    }
}

Action Input::mapAxis(Uint8 axis, Sint16 value) {
    if (axis == SDL_CONTROLLER_AXIS_LEFTY) {
        return value < 0 ? Action::UP : Action::DOWN;
    } else if (axis == SDL_CONTROLLER_AXIS_LEFTX) {
        return value < 0 ? Action::LEFT : Action::RIGHT;
    }
    return Action::NONE;
}

std::string Input::getActionName() const {
    switch (m_currentAction) {
        case Action::UP: return "UP";
        case Action::DOWN: return "DOWN";
        case Action::LEFT: return "LEFT";
        case Action::RIGHT: return "RIGHT";
        case Action::ACCEPT: return "ACCEPT (A)";
        case Action::BACK: return "BACK (B)";
        case Action::MENU: return "MENU";
        case Action::START: return "START";
        case Action::SELECT: return "SELECT";
        case Action::PAGE_UP: return "PAGE_UP (L1)";
        case Action::PAGE_DOWN: return "PAGE_DOWN (R1)";
        case Action::NONE: return "NONE";
        default: return "UNKNOWN";
    }
}
