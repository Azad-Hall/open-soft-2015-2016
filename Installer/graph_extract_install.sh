#!/bin/bash

if ! sudo true; then
    echo "You must have root privileges to run this script."
    echo "Please run the installer as sudo -E ./graph_extract_instal.sh."
    exit 1
fi

if [ -z "$http_proxy" ]; then
    echo "Need http proxy to run the installer."
    echo "Please run the installer as: sudo -E ./graph_extract_instal.sh."
    exit 1
fi

set -e

echo "--- Installing Dependencies"

add-apt-repository -y ppa:ethereum/ethereum-qt
apt-get update || echo "--- Continuing Installation"

source dependencies.sh

echo "--- Installing GraphExtract"

rm -rf `pwd`/../build/
mkdir `pwd`/../build
cd `pwd`/../build
qmake `pwd`/../GUI/GUI.pro
make
touch Graph_Extract_run.sh
echo "#!/bin/sh" >> Graph_Extract_run.sh
echo "export LD_LIBRARY_PATH=`pwd`/qpdflib"  >> Graph_Extract_run.sh
echo "exec ./graphextractor/graphextractor" >> Graph_Extract_run.sh
chmod +x Graph_Extract_run.sh

echo "--- Installing Complete"