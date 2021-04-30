set(ALL_TARGETS
    actions
    dependencies
    guards
    logging
    on_entry_exit
    plant_uml
    simple
    supervisor
    transitions
)

add_executable(actions
               actions.cpp)
add_executable(dependencies
               dependencies.cpp)
add_executable(guards
               guards.cpp)
add_executable(logging
               logging.cpp
               common/logger.h)
add_executable(on_entry_exit
               on_entry_exit.cpp)
add_executable(plant_uml
               plant_uml.cpp
               common/dump.h)
add_executable(simple
               simple.cpp)
add_executable(supervisor
               supervisor.cpp
               common/dump.h
               common/logger.h)
add_executable(transitions
               transitions.cpp)

foreach(target ${ALL_TARGETS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    target_link_libraries(${target} PRIVATE ${SANITIZER_FLAGS})
    target_include_directories(${target} PRIVATE include)
    target_link_libraries(${target} PRIVATE fmt::fmt spdlog::spdlog spdlog::spdlog_header_only)
endforeach()
