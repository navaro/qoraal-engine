mkdir build
cd build
cmake .. -DBUILD_TOASTER=ON -G "MinGW Makefiles" 
cmake --build .
cd ..
.\build\test\toaster .\test\toaster.e
