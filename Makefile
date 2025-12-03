ifeq ($(OS),Windows_NT)
	CMAKE = cmake .. -DCFG_ENGINE_REGISTRY_ENABLE=ON -DBUILD_ENGINE_TESTS=ON -G "MinGW Makefiles"
else
	CMAKE = cmake .. -DCFG_ENGINE_REGISTRY_ENABLE=ON -DBUILD_ENGINE_TESTS=ON
endif
MKDIR = mkdir -p build
EXECUTABLE = ./build/test/toaster ./test/toaster.e
RM = rm -rf

.PHONY: all build run clean

all: build run

build:
	$(MKDIR)
	cd build && $(CMAKE) && cmake --build .

run:
	cd $(CURDIR) && $(EXECUTABLE)

clean:
	$(RM) build
