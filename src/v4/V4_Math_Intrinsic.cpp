#include <random>
#include <vector>
#include "../../V4_Math_Intrinsic.h"

namespace V4 {

    // Genera Particelle
    Particles generateParticles(const int N) {
        // Pre-allochiamo la memoria esatta. Evita i push_back in un loop se sai già la dimensione!
        Particles particles;
        particles.x.resize(N);
        particles.vx.resize(N);

        particles.y.resize(N);
        particles.vy.resize(N);

        particles.z.resize(N);
        particles.vz.resize(N);

        // Motore randomico standard (Mersenne Twister)
        // Usiamo un SEED FISSO (es. 42) così ogni run genera le stesse esatte posizioni
        std::mt19937 gen(42);

        // Distribuzioni uniformi per posizioni e velocità
        std::uniform_real_distribution<float> distPos(-1000.0f, 1000.0f); // Spazio 3D
        std::uniform_real_distribution<float> distVel(-1.0f, 1.0f);       // Velocità iniziali

        for (int i = 0; i < N; ++i) {
            particles.x[i] = distPos(gen);
            particles.y[i] = distPos(gen);
            particles.z[i] = distPos(gen);

            particles.vx[i] = distVel(gen);
            particles.vy[i] = distVel(gen);
            particles.vz[i] = distVel(gen);
        }

        return particles;
    }

    // Calcola Nbody
    void computeNbody(Particles &particles) { // O come hai chiamato la struct SoA
        const int N = particles.x.size();

        for (int i = 0; i < N; ++i) {

            const float xi = particles.x[i];
            const float yi = particles.y[i];
            const float zi = particles.z[i];

            float acc_vx = 0.0f;
            float acc_vy = 0.0f;
            float acc_vz = 0.0f;

            for (int j = i + 1; j < N; ++j) {

                const float dx = particles.x[j] - xi;
                const float dy = particles.y[j] - yi;
                const float dz = particles.z[j] - zi;

                // 1. Calcoliamo la distanza al quadrato
                const float r2 = dx*dx + dy*dy + dz*dz + 0.001f; // eps

                // 2. Calcoliamo l'INVERSO della radice quadrata.
                // Scritto così, il compilatore capirà cosa vogliamo fare.
                const float invR = 1.0f / std::sqrt(r2);

                // 3. NIENTE PIU' DIVISIONI! Calcoliamo 1/r^3 con sole moltiplicazioni
                const float Fmag = invR * invR * invR;

                acc_vx += dx * Fmag;
                acc_vy += dy * Fmag;
                acc_vz += dz * Fmag;

                particles.vx[j] -= dx * Fmag;
                particles.vy[j] -= dy * Fmag;
                particles.vz[j] -= dz * Fmag;
            }

            particles.vx[i] += acc_vx;
            particles.vy[i] += acc_vy;
            particles.vz[i] += acc_vz;
        }
    }
}
