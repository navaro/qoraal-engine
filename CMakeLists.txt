cmake_minimum_required(VERSION 3.16)
project(QoraalEngine LANGUAGES C)

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

# Qoraal-Flash repo
FetchContent_Declare(
    qoraal_flash
    GIT_REPOSITORY https://github.com/navaro/qoraal-flash.git
    GIT_TAG        main
    SOURCE_SUBDIR  .
)
FetchContent_MakeAvailable(qoraal_flash)

# Include directories
include_directories(include)
include_directories("${qoraal_SOURCE_DIR}/include")
include_directories("${qoraal_flash_SOURCE_DIR}/include")

# Add subdirectories
add_subdirectory(src)

# Option to build tests
option(BUILD_TOASTER "Build the test directory" OFF)

if(BUILD_TOASTER)
    add_subdirectory(test)
endif()

