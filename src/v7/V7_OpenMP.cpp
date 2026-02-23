#include "V7_OpenMP.h"

#include <vector>
#include <random>

namespace V7 {

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

        // QUESTA E' LA MAGIA.
        // Chiediamo a OpenMP di parallelizzare il ciclo esterno (le particelle 'i').
        // 'schedule(static)' dice: dividi le 100.000 iterazioni in blocchi uguali
        // e assegnale ai core in modo fisso all'inizio del loop.
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < N; ++i) {

            const float xi = particles.x[i];
            const float yi = particles.y[i];
            const float zi = particles.z[i];

            float acc_vx = 0.0f;
            float acc_vy = 0.0f;
            float acc_vz = 0.0f;

            // Il SIMD continua a lavorare sul ciclo interno DENTRO ogni singolo core!
            #pragma omp simd reduction(+:acc_vx, acc_vy, acc_vz)
            for (int j = 0; j < N; ++j) {

                const float dx = particles.x[j] - xi;
                const float dy = particles.y[j] - yi;
                const float dz = particles.z[j] - zi;

                const float r = std::sqrt(dx*dx + dy*dy + dz*dz + 0.001f);
                const float Fmag = 1.0f / (r * r * r);

                acc_vx += dx * Fmag;
                acc_vy += dy * Fmag;
                acc_vz += dz * Fmag;
            }

            // Siccome ogni thread sta lavorando su una 'i' diversa,
            // nessuno scriverà mai sulla stessa cella di memoria contemporaneamente.
            // Zero conflitti (Race Conditions), zero lock!
            particles.vx[i] += acc_vx;
            particles.vy[i] += acc_vy;
            particles.vz[i] += acc_vz;
        }
    }
}

