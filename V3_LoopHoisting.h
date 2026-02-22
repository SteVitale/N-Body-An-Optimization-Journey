#ifndef NBODYOPTIMIZATION_V3_LOOPHOISTING_H
#define NBODYOPTIMIZATION_V3_LOOPHOISTING_H

#include <vector>

namespace V3 {
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
#endif //NBODYOPTIMIZATION_V3_LOOPHOISTING_H