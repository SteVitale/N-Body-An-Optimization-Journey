#include <iostream>
#include <chrono>

#include "v0/V0_baseline.h"
#include "v1/V1_NewtonThirdLaw.h"
#include "v2/V2_StructOfArray.h"
#include "v3/V3_LoopHoisting.h"
#include "v4/V4_Math_Intrinsic.h"
#include "v5/V5_SIMD.h"
#include "v6/V6_Tiling.h"
#include "v7/V7_OpenMP.h"

#define NUM_PARTICLES 100000

// #define V_0
// #define V_1
// #define V_2
// #define V_3
// #define V_4
// #define V_5
// #define V_6
#define V_7

auto getTime() {
    return std::chrono::high_resolution_clock::now();
}
auto getDiffTime(auto Start) {
    return std::chrono::duration<double, std::milli>(getTime() - Start).count();
}

int main() {
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Numero Particelle: " << NUM_PARTICLES << std::endl;
    std::cout << "-----------------------------" << std::endl;

    /*
     *  COMPUTAZIONE V0
     */
#ifdef V_0
    auto particles_v0 = V0::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V0" << std::endl;
    const auto Start0 = getTime();
    V0::computeNbody(particles_v0);
    const auto End0 = getDiffTime(Start0);
    std::cout << "Fine computazione NBody V0" << std::endl;
    std::cout << "Tempo V0: " << End0 <<" ms" << "(Secondi: " << End0 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V1
     */
#ifdef V_1
    auto particles_v1 = V1::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V1" << std::endl;
    const auto Start1 = getTime();
    V1::computeNbody(particles_v1);
    const auto End1 = getDiffTime(Start1);
    std::cout << "Fine computazione NBody V1" << std::endl;
    std::cout << "Tempo V1: " << End1 <<" ms" << "(Secondi: " << End1 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V2
     */
#ifdef V_2
    auto particles_v2 = V2::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V2" << std::endl;
    const auto Start2 = getTime();
    V2::computeNbody(particles_v2);
    const auto End2 = getDiffTime(Start2);
    std::cout << "Fine computazione NBody V2" << std::endl;
    std::cout << "Tempo V2: " << End2 <<" ms" << "(Secondi: " << End2 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V3
     */
#ifdef V_3
    auto particles_v3 = V3::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V3" << std::endl;
    const auto Start3 = getTime();
    V3::computeNbody(particles_v3);
    const auto End3 = getDiffTime(Start3);
    std::cout << "Fine computazione NBody V3" << std::endl;
    std::cout << "Tempo V3: " << End3 <<" ms" << "(Secondi: " << End3 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V4
     */
#ifdef V_4
    auto particles_v4 = V4::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V4" << std::endl;
    const auto Start4 = getTime();
    V4::computeNbody(particles_v4);
    const auto End4 = getDiffTime(Start4);
    std::cout << "Fine computazione NBody V4" << std::endl;
    std::cout << "Tempo V4: " << End4 <<" ms" << "(Secondi: " << End4 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V5
     */
#ifdef V_5
    auto particles_v5 = V5::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V5" << std::endl;
    const auto Start5 = getTime();
    V5::computeNbody(particles_v5);
    const auto End5 = getDiffTime(Start5);
    std::cout << "Fine computazione NBody V5" << std::endl;
    std::cout << "Tempo V5: " << End5 <<" ms" << "(Secondi: " << End5 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V6
     */
#ifdef V_6
    auto particles_v6 = V6::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V6" << std::endl;
    const auto Start6 = getTime();
    V6::computeNbody(particles_v6);
    const auto End6 = getDiffTime(Start6);
    std::cout << "Fine computazione NBody V6" << std::endl;
    std::cout << "Tempo V6: " << End6 <<" ms" << "(Secondi: " << End6 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

    /*
     *  COMPUTAZIONE V7
     */
#ifdef V_7
    auto particles_v7 = V7::generateParticles(NUM_PARTICLES);
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Inizio computazione NBody V7" << std::endl;
    const auto Start7 = getTime();
    V7::computeNbody(particles_v7);
    const auto End7 = getDiffTime(Start7);
    std::cout << "Fine computazione NBody V7" << std::endl;
    std::cout << "Tempo V7: " << End7 <<" ms" << "(Secondi: " << End7 / 1000.0 << " s)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
#endif

/*
 * CI ASSICURIAMO CHE IL COMPILATORE -03 NON TOGLIE I LOOP
 */

#ifdef V_0
    std::cout << "Check: v0[0] = " << particles_v0[0].vx << "\n";
#endif
#ifdef V_1
    std::cout << "Check: v1[0] = " << particles_v1[0].vx << "\n";
#endif
#ifdef V_2
    std::cout << "Check: v2[0] = " << particles_v2.vx[0] << "\n";
#endif
#ifdef V_3
    std::cout << "Check: v3[0] = " << particles_v3.vx[0] << "\n";
#endif
#ifdef V_4
    std::cout << "Check: v4[0] = " << particles_v4.vx[0] << "\n";
#endif
#ifdef V_5
    std::cout << "Check: v5[0] = " << particles_v5.vx[0] << "\n";
#endif
#ifdef V_6
    std::cout << "Check: v6[0] = " << particles_v6.vx[0] << "\n";
#endif
#ifdef V_7
    std::cout << "Check: v7[0] = " << particles_v7.vx[0] << "\n";
#endif
    return 0;
}