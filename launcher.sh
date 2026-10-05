#!/bin/bash

# EmulationStation Launcher for R36 Ultra App

APP_NAME="r36ultra-app"
INSTALL_DIR="/opt/$APP_NAME"
EXECUTABLE="$INSTALL_DIR/$APP_NAME"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: $EXECUTABLE not found. Please run install.sh first."
    sleep 3
    exit 1
fi

echo "Launching $APP_NAME..."

# Change directory so it can find assets/
cd "$INSTALL_DIR"

# Launch the app
./$APP_NAME

# The app exits cleanly, return control to ES.
exit $?
