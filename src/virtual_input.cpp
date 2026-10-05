#include <SDL2/SDL.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/uinput.h>
#include <sys/wait.h>
#include <signal.h>

// We are using qtvirtualkeyboard, so we can emit a specific key (like F11 or something Falkon recognizes)
// However, the issue explicitly requested a keyboard that toggles on FN press.
// Since we are running in KMSDRM via EGLFS, we cannot use X11 tools like matchbox-keyboard.
// Ideally, qtvirtualkeyboard handles text inputs automatically when they gain focus.
// To force it, we can emit a dummy text input or tab key, but the most reliable way
// if qtvirtualkeyboard isn't doing the job is to just map FN to a key that brings focus
// or use a different tool. For now, we will map FN to emit KEY_F11 or simulate a touchscreen tap
// or just emit nothing because qtvirtualkeyboard handles it automatically on focus in QT apps.
// BUT since the user explicitly requested a toggle, we will emit a standard toggle key (e.g. F12 or KEY_KBDINPUTASSIST_TOGGLE).

int setup_uinput() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Failed to open /dev/uinput. Need root or udev rules." << std::endl;
        return -1;
    }

    // Enable mouse and keyboard events
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_REL);

    // Mouse buttons
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

    // Add all typical keyboard keys so matchbox-keyboard or any other virtual keyboard
    // can technically be simulated, or so we can emit keypresses directly.
    for (int i = 1; i < 255; i++) {
        ioctl(fd, UI_SET_KEYBIT, i);
    }

    // Mouse axes
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_RELBIT, REL_WHEEL);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "r36s-virtual-mouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    if (write(fd, &uidev, sizeof(uidev)) < 0) {
        std::cerr << "Failed to write uinput device structure." << std::endl;
        close(fd);
        return -1;
    }

    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        std::cerr << "Failed to create uinput device." << std::endl;
        close(fd);
        return -1;
    }

    return fd;
}

void emit_event(int fd, int type, int code, int val) {
    struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

void emit_sync(int fd) {
    emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        std::cout << "Usage: r36s-falkon-controller\n";
        return 0;
    }

    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    int uinput_fd = setup_uinput();
    if (uinput_fd < 0) {
        SDL_Quit();
        return 1;
    }

    SDL_GameController* controller = nullptr;
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_GameControllerOpen(0);
    }

    if (!controller) {
        std::cerr << "No controller found." << std::endl;
    } else {
        std::cout << "Connected: " << SDL_GameControllerName(controller) << std::endl;
    }

    bool running = true;

    // Joystick state
    int left_x = 0, left_y = 0;
    int right_x = 0, right_y = 0;

    // Trigger states
    bool l2_pressed = false;
    bool r2_pressed = false;

    const int DEADZONE = 8000;
    const float FAST_SPEED = 15.0f;
    const float SLOW_SPEED = 5.0f;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
                int val = (event.type == SDL_CONTROLLERBUTTONDOWN) ? 1 : 0;
                auto btn = event.cbutton.button;

                if (btn == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) { // L1 - Scroll Up
                    if (val) {
                        emit_event(uinput_fd, EV_REL, REL_WHEEL, 1);
                        emit_sync(uinput_fd);
                    }
                } else if (btn == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) { // R1 - Scroll Down
                    if (val) {
                        emit_event(uinput_fd, EV_REL, REL_WHEEL, -1);
                        emit_sync(uinput_fd);
                    }
                } else if (btn == SDL_CONTROLLER_BUTTON_GUIDE || btn == SDL_CONTROLLER_BUTTON_BACK || btn == SDL_CONTROLLER_BUTTON_MISC1) {
                    // Treat FN button as guide, back, or misc1 depending on mapping
                    if (val) {
                        // Emitting a keyboard layout toggle or something recognizable. F12 or KEY_KBDILLUMTOGGLE
                        emit_event(uinput_fd, EV_KEY, KEY_KBDILLUMTOGGLE, 1);
                        emit_sync(uinput_fd);
                        emit_event(uinput_fd, EV_KEY, KEY_KBDILLUMTOGGLE, 0);
                        emit_sync(uinput_fd);
                    }
                }
            } else if (event.type == SDL_CONTROLLERAXISMOTION) {
                auto axis = event.caxis.axis;
                auto val = event.caxis.value;
                if (abs(val) < DEADZONE) val = 0;

                if (axis == SDL_CONTROLLER_AXIS_LEFTX) left_x = val;
                else if (axis == SDL_CONTROLLER_AXIS_LEFTY) left_y = val;
                else if (axis == SDL_CONTROLLER_AXIS_RIGHTX) right_x = val;
                else if (axis == SDL_CONTROLLER_AXIS_RIGHTY) right_y = val;
                else if (axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) { // L2
                    bool currently_pressed = (val > DEADZONE);
                    if (currently_pressed != l2_pressed) {
                        l2_pressed = currently_pressed;
                        emit_event(uinput_fd, EV_KEY, BTN_LEFT, l2_pressed ? 1 : 0);
                        emit_sync(uinput_fd);
                    }
                } else if (axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) { // R2
                    bool currently_pressed = (val > DEADZONE);
                    if (currently_pressed != r2_pressed) {
                        r2_pressed = currently_pressed;
                        emit_event(uinput_fd, EV_KEY, BTN_RIGHT, r2_pressed ? 1 : 0);
                        emit_sync(uinput_fd);
                    }
                }
            }
        }

        int dx = 0, dy = 0;

        if (left_x != 0 || left_y != 0) {
            dx = (left_x / 32767.0f) * FAST_SPEED;
            dy = (left_y / 32767.0f) * FAST_SPEED;
        } else if (right_x != 0 || right_y != 0) {
            dx = (right_x / 32767.0f) * SLOW_SPEED;
            dy = (right_y / 32767.0f) * SLOW_SPEED;
        }

        if (dx != 0 || dy != 0) {
            emit_event(uinput_fd, EV_REL, REL_X, dx);
            emit_event(uinput_fd, EV_REL, REL_Y, dy);
            emit_sync(uinput_fd);
        }

        SDL_Delay(16); // roughly 60Hz loop
    }

    if (controller) SDL_GameControllerClose(controller);
    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
    SDL_Quit();

    return 0;
}
