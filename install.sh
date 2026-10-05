#!/bin/bash

# R36 Ultra App Install Script
set -e

APP_NAME="r36ultra-app"
INSTALL_DIR="/opt/$APP_NAME"
ES_PORTS_DIR="/roms/ports" # Typical ArkOS ports directory

echo "Installing $APP_NAME..."

# Create installation directory
sudo mkdir -p "$INSTALL_DIR"

# Copy binary and assets
sudo cp -r build/$APP_NAME "$INSTALL_DIR/"
sudo cp -r assets "$INSTALL_DIR/"

# Set permissions
sudo chmod +x "$INSTALL_DIR/$APP_NAME"

# Copy launcher to EmulationStation ports directory
if [ -d "$ES_PORTS_DIR" ]; then
    sudo cp launcher.sh "$ES_PORTS_DIR/$APP_NAME.sh"
    sudo chmod +x "$ES_PORTS_DIR/$APP_NAME.sh"
    echo "Launcher installed to $ES_PORTS_DIR/$APP_NAME.sh"
else
    echo "Warning: $ES_PORTS_DIR not found. You may need to copy launcher.sh manually."
fi

echo "Installation complete. Application installed to $INSTALL_DIR."
