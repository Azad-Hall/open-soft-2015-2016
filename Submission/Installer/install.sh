
rm -rf `pwd`/../build/
mkdir `pwd`/../build
# chmod o+rw `pwd`/../build
cd `pwd`/../build

echo "--- Installing the console application"

cmake `pwd`/../Console/
make
cp `pwd`/../Installer/run.sh `pwd`/
chmod +x run.sh

echo "--- Installing Graph Extractor"

qmake `pwd`/../GUI/GUI.pro
make

touch Graph_Extractor_run.sh
echo "#!/bin/sh" >> Graph_Extractor_run.sh
echo "export LD_LIBRARY_PATH=`pwd`/qpdflib"  >> Graph_Extractor_run.sh
echo "exec `pwd`/graphextractor/graphextractor" >> Graph_Extractor_run.sh
chmod +x Graph_Extractor_run.sh

find . -type f -name '*.o' -exec rm {} +
find . -type f -name '*.cpp' -exec rm {} +
find . -type f -name '*.c' -exec rm {} +
find . -type f -name 'Makefile' -exec rm {} +

echo "--- Installation Complete"