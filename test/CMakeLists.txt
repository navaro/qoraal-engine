set(TEST_SRCS
    platform/platform.c
    toaster.c
    main.c
)

add_executable(toaster ${TEST_SRCS})

# If you want debugging turned off, change -O0 -g or remove them
target_link_libraries(toaster PRIVATE qoraal qoraal-flash qoraal-engine pthread)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal-engine PRIVATE -O0 -g)
target_compile_options(qoraal-flash PRIVATE -O0 -g)
target_compile_options(toaster PRIVATE -O0 -g)

target_link_options(toaster PRIVATE
    "-Wl,--whole-archive"
    "$<TARGET_FILE:qoraal-engine>"
    "$<TARGET_FILE:qoraal-flash>"
    "-Wl,--no-whole-archive"
)

# Generate a map file
set_target_properties(toaster PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")