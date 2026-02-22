#ifndef NBODYOPTIMIZATION_V4_MATH_INTRINSIC_H
#define NBODYOPTIMIZATION_V4_MATH_INTRINSIC_H

#include <vector>

namespace V4 {
    // Epsilon per evitare sqrt(0)
    constexpr float eps = 0.001f;

    // Structure of Arrays
    struct Particles {
        std::vector<float> x, y, z;
        std::vector<float> vx, vy, vz;
    };

    // Genera Particelle
    Particles generateParticles(const int N);
    void computeNbody(Particles &particles);
}
#endif //NBODYOPTIMIZATION_V4_MATH_INTRINSIC_H