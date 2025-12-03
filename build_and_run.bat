mkdir build
cd build
cmake .. -DCFG_ENGINE_REGISTRY_ENABLE=ON -DBUILD_TESTS=ON -G "MinGW Makefiles" 
cmake --build .
cd ..
.\build\test\toaster .\test\toaster.e
