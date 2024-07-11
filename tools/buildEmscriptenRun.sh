#source ~/Apps/emsdk/emsdk_env.sh
rm -rf build
mkdir build
cd build
cp ../resources/test.wav .
emcmake cmake ../src/helloOAL
emmake make -j 16
ls
cp HelloOAL.data /srv/http/OAL/HelloOAL.data
cp HelloOAL.js /srv/http/OAL/HelloOAL.js
cp HelloOAL.wasm /srv/http/OAL/HelloOAL.wasm