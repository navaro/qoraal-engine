cmake_minimum_required(VERSION 3.16)
project(QoraalEngine LANGUAGES C)

option(CFG_ENGINE_REGISTRY_DISABLE "Disable the engine registry" OFF)

# --------------------
# 1) Fetch the Qoraal repo
# --------------------
include(FetchContent)

FetchContent_Declare(
    qoraal
    GIT_REPOSITORY https://github.com/navaro/qoraal.git
    GIT_TAG        main
    SOURCE_SUBDIR  .
)
FetchContent_MakeAvailable(qoraal)
include_directories("${qoraal_SOURCE_DIR}/include")

if(NOT CFG_ENGINE_REGISTRY_DISABLE)
    # Qoraal-Flash repo
    FetchContent_Declare(
        qoraal_flash
        GIT_REPOSITORY https://github.com/navaro/qoraal-flash.git
        GIT_TAG        main
        SOURCE_SUBDIR  .
    )
    FetchContent_MakeAvailable(qoraal_flash)
    include_directories("${qoraal_flash_SOURCE_DIR}/include")
endif()

# Include directories
include_directories(include)


if(CFG_ENGINE_REGISTRY_DISABLE)
    add_compile_definitions(CFG_ENGINE_REGISTRY_DISABLE=1)
endif()

# Add subdirectories
add_subdirectory(src)

# Option to build tests
option(BUILD_TOASTER "Build the test directory" OFF)

if(BUILD_TOASTER)
    add_subdirectory(test)
endif()

