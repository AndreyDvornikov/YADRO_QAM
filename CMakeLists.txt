cmake_minimum_required(VERSION 3.16)
project(QAM_Simulation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(qam_sim
    main.cpp
    src/modulator.cpp
    src/awgn.cpp
    src/demodulator.cpp
)

target_include_directories(qam_sim PRIVATE include)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(qam_sim PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions(qam_sim PRIVATE USE_OPENMP)
endif()