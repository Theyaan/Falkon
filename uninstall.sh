#!/bin/bash

# R36 Ultra App Uninstall Script
set -e

APP_NAME="r36ultra-app"
INSTALL_DIR="/opt/$APP_NAME"
ES_PORTS_DIR="/roms/ports"

echo "Uninstalling $APP_NAME..."

# Remove installation directory
if [ -d "$INSTALL_DIR" ]; then
    sudo rm -rf "$INSTALL_DIR"
    echo "Removed $INSTALL_DIR"
else
    echo "Installation directory $INSTALL_DIR not found."
fi

# Remove launcher
if [ -f "$ES_PORTS_DIR/$APP_NAME.sh" ]; then
    sudo rm "$ES_PORTS_DIR/$APP_NAME.sh"
    echo "Removed $ES_PORTS_DIR/$APP_NAME.sh"
else
    echo "Launcher script not found in $ES_PORTS_DIR."
fi

echo "Uninstallation complete."
