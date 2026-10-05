#!/bin/bash

# EmulationStation Launcher for Falkon on ArkOS (R36S)

APP_DIR="/opt/r36ultra-app"
CONTROLLER_BIN="$APP_DIR/r36s-falkon-controller"

if [ ! -f "$CONTROLLER_BIN" ]; then
    echo "Error: $CONTROLLER_BIN not found. Please run install.sh first."
    sleep 3
    exit 1
fi

echo "Launching Falkon..."

# Enable Qt Virtual Keyboard since we are running natively via EGLFS without X11
export QT_IM_MODULE=qtvirtualkeyboard
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_ALWAYS_SET_MODE=1

# Start the virtual mouse/keyboard daemon in the background with sudo to ensure uinput access
# We use sudo -b to run it in background and we will kill it by name since killing the sudo process pid won't kill the child reliable.
sudo "$CONTROLLER_BIN" &
sleep 1

# Launch Falkon
falkon

# Once Falkon exits, kill the controller daemon
sudo killall r36s-falkon-controller

exit 0
