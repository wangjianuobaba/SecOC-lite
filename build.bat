rem use visual studio to open project

rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build .
rem start .sln