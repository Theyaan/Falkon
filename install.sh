#!/bin/bash

# R36 Ultra Falkon Integration Install Script
set -e

APP_NAME="falkon-for-arkos"
INSTALL_DIR="/opt/falkon-for-arkos"
ES_TOOLS_DIR="/roms/tools"

echo "Installing Falkon integration..."

# Create installation directory
sudo mkdir -p "$INSTALL_DIR"
sudo mkdir -p "$ES_TOOLS_DIR"

# Copy binary
sudo cp build/falkon-for-arkos "$INSTALL_DIR/"

# Set permissions
sudo chmod +x "$INSTALL_DIR/falkon-for-arkos"

# Copy Falkon Launcher to tools
if [ -f "falkon.sh" ]; then
    sudo cp falkon.sh "$ES_TOOLS_DIR/Falkon.sh"
    sudo chmod +x "$ES_TOOLS_DIR/Falkon.sh"
    echo "Falkon integration installed to $ES_TOOLS_DIR/Falkon.sh"
fi

echo "Installation complete."
