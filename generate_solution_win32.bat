mkdir build
cd build
del CMakeCache.txt
set http_proxy=http://a.pugachev%40securitycode.ru:Ondrey1995!@proxy1.securitycode.ru:4343
cmake -G "Visual Studio 14 2015" .. -DMSVC_RUNTIME=dynamic
cd ..