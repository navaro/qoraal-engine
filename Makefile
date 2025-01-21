# -------------------------
#     Minimal Makefile
# -------------------------

# Git clone info
QORAAL_REPO := https://github.com/navaro/qoraal.git
QORAAL_DIR  := qoraal

# Build output
BUILD_DIR   := build
TARGET_EXEC := qoraal-engine

# Compiler flags (adjust as needed)
CC      := gcc
CFLAGS  := -O0 -g
LDFLAGS := -lpthread --static -Xlinker -Map=output.map -T engine.ld
CPPFLAGS := -Iinclude -I$(QORAAL_DIR)/include -DCFG_OS_POSIX -MMD -MP

# ------------------------------------------------------------------------------
# 1) All local engine sources in ./src/
# ------------------------------------------------------------------------------
ENGINE_SRCS := \
  src/parts/toaster.c \
  src/parts/debug.c \
  src/parts/console.c \
  src/parts/engine.c \
  src/parts/parts.c \
  src/tool/lex.c \
  src/tool/machine.c \
  src/tool/collection.c \
  src/tool/parse.c \
  src/engine.c \
  src/port/engine_posix.c \
  src/starter.c

# ------------------------------------------------------------------------------
# 2) All sources in the cloned Qoraal repo, ./qoraal/src/
# ------------------------------------------------------------------------------
QORAAL_SRCS := \
  qoraal/src/debug.c \
  qoraal/src/os_mlock.c \
  qoraal/src/os_posix.c \
  qoraal/src/os.c \
  qoraal/src/qoraal.c \
  qoraal/src/svc/svc_events.c \
  qoraal/src/svc/svc_logger.c \
  qoraal/src/svc/svc_services.c \
  qoraal/src/svc/svc_shell.c \
  qoraal/src/svc/svc_tasks.c \
  qoraal/src/svc/svc_threads.c \
  qoraal/src/svc/svc_wdt.c \
  qoraal/src/common/cbuffer.c \
  qoraal/src/common/dictionary.c \
  qoraal/src/common/lists.c \
  qoraal/src/common/mlog.c \
  qoraal/src/common/rtclib.c \
  qoraal/src/common/strsub.c \
  qoraal/src/qshell/posixcmd.c \
  qoraal/src/qshell/servicescmd.c

# ------------------------------------------------------------------------------
# 3) Optional test sources in ./test
# ------------------------------------------------------------------------------
TEST_SRCS := \
  test/main.c

# All sources
ALL_SRCS := $(ENGINE_SRCS) $(QORAAL_SRCS) $(TEST_SRCS)

# All objects => transform "foo.c" → "build/foo.o"
ALL_OBJS := $(ALL_SRCS:%.c=$(BUILD_DIR)/%.o)
ALL_DEPS := $(ALL_OBJS:.o=.d)

# ------------------------------------------------------------------------------
# Default target: clone, then build
# ------------------------------------------------------------------------------
.PHONY: all
all: clone $(BUILD_DIR)/$(TARGET_EXEC)

# ------------------------------------------------------------------------------
# Clone or say "already exists"
# ------------------------------------------------------------------------------
.PHONY: clone
clone:
	git clone $(QORAAL_REPO) $(QORAAL_DIR) || echo "qoraal repository already exists!"

# ------------------------------------------------------------------------------
# Link final exe from all objects
# ------------------------------------------------------------------------------
$(BUILD_DIR)/$(TARGET_EXEC): $(ALL_OBJS)
	mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# ------------------------------------------------------------------------------
# Single pattern rule for ANY .c => build/*.o
#    - The forward slashes must match exactly how you list them in ALL_SRCS
# ------------------------------------------------------------------------------
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Clean everything
# ------------------------------------------------------------------------------
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(QORAAL_DIR)

# ------------------------------------------------------------------------------
# Include the .d files for incremental builds
# ------------------------------------------------------------------------------
-include $(ALL_DEPS)
