
# rm -rf "`pwd`/../build/"
mkdir -p "`pwd`/../build_console"
# chmod o+rw `pwd`/../build_console
cd "`pwd`/../build_console"

echo "--- Installing the console application"

cmake "`pwd`/../Console/"
make -j4
cp "`pwd`/../Installer/run.sh" "`pwd`/"
chmod +x run.sh


# find . -type f -name '*.o' -exec rm {} +
# find . -type f -name '*.cpp' -exec rm {} +
# find . -type f -name '*.c' -exec rm {} +
# find . -type f -name 'Makefile' -exec rm {} +

echo "--- Installation Complete"