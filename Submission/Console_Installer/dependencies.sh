#!/bin/bash

function install_dependency {
    echo "--- Installing dependency: $1"
    sudo -E apt-get -y install $1
}

install_dependency build-essential
install_dependency libopencv-dev
install_dependency tesseract-ocr
install_dependency tesseract-ocr-eng
install_dependency tesseract-ocr-equ
install_dependency unpaper
install_dependency imagemagick
