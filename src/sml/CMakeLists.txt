set(ALL_TARGETS
    simple
)

add_executable(simple
    simple.cpp
)

foreach(target ${ALL_TARGETS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    target_link_libraries(${target} PRIVATE ${SANITIZER_FLAGS})
endforeach()

target_include_directories(simple PRIVATE include)
target_link_libraries(simple PRIVATE fmt::fmt spdlog::spdlog spdlog::spdlog_header_only)
