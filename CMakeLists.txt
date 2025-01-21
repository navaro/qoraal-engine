cmake_minimum_required(VERSION 3.16)
project(QoraalEngine LANGUAGES C)

# --------------------
# 1) Fetch the Qoraal repo
# --------------------
include(FetchContent)

FetchContent_Declare(
    qoraal
    GIT_REPOSITORY https://github.com/navaro/qoraal.git
    GIT_TAG        main  # or whatever branch/commit you need
    SOURCE_SUBDIR  .
)
FetchContent_MakeAvailable(qoraal)

# ------------------------------------------
# 2) List your local engine sources in src/
# ------------------------------------------
set(ENGINE_SOURCES
    src/parts/toaster.c
    src/parts/debug.c
    src/parts/console.c
    src/parts/engine.c
    src/parts/parts.c
    src/tool/lex.c
    src/tool/machine.c
    src/tool/collection.c
    src/tool/parse.c
    src/engine.c
    src/port/engine_qoraal.c
    src/starter.c
)

# ------------------------------------------
# 3) List the Qoraal sources.
#    These come from the fetched repo
#    in _deps/qoraal-src/ inside build/
# ------------------------------------------
set(QORAAL_SOURCES
    # Because of how FetchContent works,
    # Qoraal's source root will be: 
    #   ${qoraal_SOURCE_DIR}/
    # We'll refer to them directly:
    ${qoraal_SOURCE_DIR}/src/debug.c
    ${qoraal_SOURCE_DIR}/src/os_mlock.c
    ${qoraal_SOURCE_DIR}/src/os_posix.c
    ${qoraal_SOURCE_DIR}/src/os.c
    ${qoraal_SOURCE_DIR}/src/qoraal.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_events.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_logger.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_services.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_shell.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_tasks.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_threads.c
    ${qoraal_SOURCE_DIR}/src/svc/svc_wdt.c
    ${qoraal_SOURCE_DIR}/src/common/cbuffer.c
    ${qoraal_SOURCE_DIR}/src/common/dictionary.c
    ${qoraal_SOURCE_DIR}/src/common/lists.c
    ${qoraal_SOURCE_DIR}/src/common/mlog.c
    ${qoraal_SOURCE_DIR}/src/common/rtclib.c
    ${qoraal_SOURCE_DIR}/src/common/strsub.c
    ${qoraal_SOURCE_DIR}/src/qshell/posixcmd.c
    ${qoraal_SOURCE_DIR}/src/qshell/servicescmd.c
)

# ------------------------------------------
# 4) Optional test sources
# ------------------------------------------
set(TEST_SOURCES
    test/main.c
)

# ------------------------------------------
# 5) Create one executable with everything
# ------------------------------------------
add_executable(qoraal-engine
    ${ENGINE_SOURCES}
    ${QORAAL_SOURCES}
    ${TEST_SOURCES}
)

# ------------------------------------------
# 6) Tell the compiler about includes/defines
# ------------------------------------------
target_compile_definitions(qoraal-engine PRIVATE CFG_OS_POSIX)
target_include_directories(qoraal-engine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"      # your local includes
    "${qoraal_SOURCE_DIR}/include"            # Qoraal's includes
)

# If you want debugging turned off, tweak -O0 -g or remove them
target_compile_options(qoraal-engine PRIVATE -O0 -g)

# ------------------------------------------
# 7) Link with pthread, use engine.ld, static
# ------------------------------------------
# Add a map file + script for linking:
#   -Xlinker -Map=output.map
#   -T engine.ld
#   --static
# On some toolchains you might need a different syntax, but let's try:
target_link_options(qoraal-engine PRIVATE
    "-Xlinker" "-Map=output.map"
    "-T" "${CMAKE_CURRENT_SOURCE_DIR}/engine.ld"
    "--static"
)

# Link with pthread.
# You can do find_package(Threads REQUIRED), but if that fails,
# we can just do:
target_link_libraries(qoraal-engine PRIVATE pthread)
