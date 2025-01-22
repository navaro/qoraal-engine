mkdir build
cd build
cmake .. -DBUILD_TOASTER=ON 
cmake --build .
cd ..
./build/test/toaster ./test/toaster.e

