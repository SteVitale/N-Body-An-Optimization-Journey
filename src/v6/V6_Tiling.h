#ifndef NBODYOPTIMIZATION_V6_TILING_H
#define NBODYOPTIMIZATION_V6_TILING_H

#include <vector>

namespace V6 {
    // Epsilon per evitare sqrt(0)
    constexpr float eps = 0.001f;

    // 256 è un numero magico tipico per i float in L1.
    // Puoi provare a cambiarlo (es. 128, 512) per fare tuning hardware!
    constexpr int BLOCK_SIZE = 4096;


    // Structure of Arrays
    struct Particles {
        std::vector<float> x, y, z;
        std::vector<float> vx, vy, vz;
    };

    // Genera Particelle
    Particles generateParticles(const int N);
    void computeNbody(Particles &particles);
}
#endif //NBODYOPTIMIZATION_V6_TILING_H