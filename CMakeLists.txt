cmake_minimum_required(VERSION 3.28)
project(NBodyOptimization)

# Impostazioni C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(NBodyOptimization
        src/main.cpp
        src/v0/V0_baseline.cpp
        src/v1/V1_NewtonThirdLaw.cpp
        src/v2/V2_StructOfArray.cpp
        src/v3/V3_LoopHoisting.cpp
        src/v4/V4_Math_Intrinsic.cpp
        src/v5/V5_SIMD.cpp
        src/v6/V6_Tiling.cpp
        src/v7/V7_OpenMP.cpp
)

# FLAG DI COMPILAZIONE ESPLICITI PER GCC/CLANG
target_compile_options(NBodyOptimization PRIVATE
        -O3                 # Ottimizzazione massima
        -march=native       # Usa le istruzioni della tua CPU Windows passate a WSL
        -ffast-math         # necessario da V4 IN POI PER RSQRT
        -fopenmp
        -fopt-info-vec-optimized
        -fopt-info-vec-missed
        -Wall -Wextra       # Warning essenziali
        -Wpedantic          # Rispetta lo standard ISO C++ alla lettera
)

find_package(OpenMP REQUIRED)
target_link_libraries(NBodyOptimization PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(NBodyOptimization PRIVATE ${OpenMP_CXX_FLAGS})

# LINK TIME OPTIMIZATION (LTO)
# (Questo ti consiglio di tenerlo col check, perché a volte su WSL mancano i pacchetti binutils per LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
    set_property(TARGET NBodyOptimization PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()