MKDIR = mkdir -p build
EXECUTABLE = ./build/test/toaster ./test/toaster.e
RM = rm -rf

.PHONY: all build run clean

all: build run

build:
	$(MKDIR)
	cd build && cmake .. -DCFG_ENGINE_REGISTRY_ENABLE=ON -DBUILD_TOASTER=ON && cmake --build .

run:
	cd $(CURDIR) && $(EXECUTABLE)

clean:
	$(RM) build
