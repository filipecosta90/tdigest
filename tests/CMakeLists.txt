
if (BUILD_BENCHMARK)
    if (UNIX)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -g -ggdb -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2  -g -ggdb -fno-omit-frame-pointer")
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
        add_subdirectory(vendor/google/benchmark)
        include_directories(vendor/google/benchmark/include)
        add_executable(histogram_benchmark histogram_benchmark.cpp)
        target_link_libraries(histogram_benchmark tdigest benchmark::benchmark)
    else()
        message(WARNING
              "google.benchmark - microbenchmarks disabled on WIN32 platforms")
    endif()
endif()

add_executable(td_test td_test.c minunit.h)
target_link_libraries(td_test tdigest m)
enable_testing()
add_test(td_test td_test)
