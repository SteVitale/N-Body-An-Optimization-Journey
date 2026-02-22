cmake_minimum_required(VERSION 3.28)
project(NBodyOptimization)

# Impostazioni C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(NBodyOptimization
        main.cpp
        V0_baseline.cpp
        V1_NewtonThirdLaw.cpp
        V2_StructOfArray.cpp
        V3_LoopHoisting.cpp
        V4_Math_Intrinsic.cpp
        V5_SIMD.cpp
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