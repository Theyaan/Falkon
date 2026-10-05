#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL.h>
#include <string>

enum class Action {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ACCEPT,
    BACK,
    MENU,
    START,
    SELECT,
    PAGE_UP,
    PAGE_DOWN
};

class Input {
public:
    Input();
    ~Input();

    bool initialize();
    void update(SDL_Event& event);

    Action getAction() const { return m_currentAction; }
    std::string getActionName() const;

private:
    SDL_GameController* m_controller;
    Action m_currentAction;

    Action mapButton(Uint8 button);
    Action mapAxis(Uint8 axis, Sint16 value);
};

#endif // INPUT_HPP
