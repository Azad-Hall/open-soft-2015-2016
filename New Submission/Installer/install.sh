
# rm -rf "`pwd`/../build/"
mkdir -p "`pwd`/../build"
# chmod o+rw `pwd`/../build
cd "`pwd`/../build"

echo "--- Installing the console application"

cmake "`pwd`/../Console/"
make -j4
cp "`pwd`/../Installer/run.sh" "`pwd`/"
chmod +x run.sh

echo "--- Installing Graph Extractor"

qmake "`pwd`/../GUI/GUI.pro"
make -j4

touch Graph_Extractor_run.sh
echo "DIR=\"`dirname \"\$(readlink -f \"$0\")\"`\"" >> Graph_Extractor_run.sh
echo "cd \"\$DIR\"" >> Graph_Extractor_run.sh
echo "#!/bin/sh" >> Graph_Extractor_run.sh
echo "export LD_LIBRARY_PATH=\"`pwd`/qpdflib:`pwd`/qcustomplot\"" >> Graph_Extractor_run.sh
echo "exec \"`pwd`/graphextractor/graphextractor\"" >> Graph_Extractor_run.sh
chmod +x Graph_Extractor_run.sh

# find . -type f -name '*.o' -exec rm {} +
# find . -type f -name '*.cpp' -exec rm {} +
# find . -type f -name '*.c' -exec rm {} +
# find . -type f -name 'Makefile' -exec rm {} +

echo "--- Installation Complete"