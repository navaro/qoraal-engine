MKDIR = mkdir -p build
EXECUTABLE = ./build/src/tictactoe
RM = rm -rf

.PHONY: all build run clean

all: build run

build:
	$(MKDIR)
	cd build && cmake .. -DBUILD_TOASTER=ON && cmake --build .

run:
	cd $(CURDIR) && $(EXECUTABLE)

clean:
	$(RM) build
