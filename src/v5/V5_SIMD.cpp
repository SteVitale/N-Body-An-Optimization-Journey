#include <vector>
#include <random>
#include "V5_SIMD.h"

namespace V5 {

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
    void computeNbody(Particles &particles) {
        const int N = particles.x.size();

        for (int i = 0; i < N; ++i) {

            const float xi = particles.x[i];
            const float yi = particles.y[i];
            const float zi = particles.z[i];

            float acc_vx = 0.0f;
            float acc_vy = 0.0f;
            float acc_vz = 0.0f;

            // Diciamo a GCC: "Vettorizza questo loop e somma i risultati
            // finali in questi tre accumulatori (reduction)"
            #pragma omp simd reduction(+:acc_vx, acc_vy, acc_vz)
            for (int j = 0; j < N; ++j) { // ATTENZIONE: Tornati a j=0! Niente più Newton.

                const float dx = particles.x[j] - xi;
                const float dy = particles.y[j] - yi;
                const float dz = particles.z[j] - zi;

                // NIENTE IF (i==j). Se i==j, dx,dy,dz sono 0.
                // eps evita la divisione per zero. dx * Fmag farà +0 all'accumulatore!
                const float r = std::sqrt(dx*dx + dy*dy + dz*dz + 0.001f);
                const float Fmag = 1.0f / (r * r * r);

                acc_vx += dx * Fmag;
                acc_vy += dy * Fmag;
                acc_vz += dz * Fmag;

                // Niente più scritture su p.vx[j]!
            }

            particles.vx[i] += acc_vx;
            particles.vy[i] += acc_vy;
            particles.vz[i] += acc_vz;
        }
    }
}

