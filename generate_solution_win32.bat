mkdir build
cd build
del CMakeCache.txt
cmake -G "Visual Studio 14 2015" .. -DMSVC_RUNTIME=dynamic
cd ..