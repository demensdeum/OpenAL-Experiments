rm -rf build
mkdir build
cd build
cmake ../src/helloOAL
make
cp ../resources/test.wav .
./HelloOAL