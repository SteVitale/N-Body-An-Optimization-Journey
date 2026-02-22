#ifndef NBODYOPTIMIZATION_V1_NEWTONTHIRDLAW_H
#define NBODYOPTIMIZATION_V1_NEWTONTHIRDLAW_H

#include <vector>
namespace V1 {
    // Epsilon per evitare sqrt(0)
    constexpr float eps = 0.001f;

    // Array of Structures
    struct Particle {
        float x, y, z;
        float vx, vy, vz;
    };

    std::vector<Particle> generateParticles(const int N);
    void computeNbody(std::vector<Particle> &particles);
}

#endif //NBODYOPTIMIZATION_V1_NEWTONTHIRDLAW_H