#!/bin/bash
if [ "$EUID" -ne 0 ]; then
    echo "You must have root privileges to install the libraries."
    echo "Please run the installer as sudo -E ./graph_extract_install.sh."
    exit 1
fi

if [ -z "$http_proxy" ]; then
    echo "Need http proxy to run the installer. Set the http proxy and restart the terminal."
    echo "Please run the installer as: sudo -E ./graph_extract_install.sh."
    exit 1
fi

if [ -z "$https_proxy" ]; then
    echo "Need https proxy to run the installer. Set the https proxy and restart the terminal."
    echo "Please run the installer as: sudo -E ./graph_extract_install.sh."
    exit 1
fi

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

echo "--- Installing Dependencies"

apt-get update || echo "--- Continuing Installation"

source dependencies.sh

chmod +x install.sh
sudo -u $SUDO_USER ./install.sh