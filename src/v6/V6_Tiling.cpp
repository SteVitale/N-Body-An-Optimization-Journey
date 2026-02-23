#include <vector>
#include <random>
#include <algorithm>
#include "V6_Tiling.h"

namespace V6 {

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

        // 1. Loop esterno sui blocchi di I
        for (int i_start = 0; i_start < N; i_start += BLOCK_SIZE) {
            // Se N non è un multiplo perfetto di BLOCK_SIZE, ci fermiamo al limite esatto
            const int i_end = std::min(i_start + BLOCK_SIZE, N);

            // 2. Loop esterno sui blocchi di J
            for (int j_start = 0; j_start < N; j_start += BLOCK_SIZE) {
                const int j_end = std::min(j_start + BLOCK_SIZE, N);

                // 3. Loop interno sulle singole particelle 'i' del blocco corrente
                for (int i = i_start; i < i_end; ++i) {

                    const float xi = particles.x[i];
                    const float yi = particles.y[i];
                    const float zi = particles.z[i];

                    float acc_vx = 0.0f;
                    float acc_vy = 0.0f;
                    float acc_vz = 0.0f;

                    // 4. Loop vettorizzato SIMD sulle 'j' del blocco corrente
                    #pragma omp simd reduction(+:acc_vx, acc_vy, acc_vz)
                    for (int j = j_start; j < j_end; ++j) {

                        const float dx = particles.x[j] - xi;
                        const float dy = particles.y[j] - yi;
                        const float dz = particles.z[j] - zi;

                        const float r = std::sqrt(dx*dx + dy*dy + dz*dz + 0.001f);
                        const float Fmag = 1.0f / (r * r * r);

                        acc_vx += dx * Fmag;
                        acc_vy += dy * Fmag;
                        acc_vz += dz * Fmag;
                    }

                    // Scriviamo in memoria alla fine di ogni scontro tra blocchi.
                    // Verrà eseguito un po' più spesso rispetto alla V5, ma è un prezzo
                    // irrisorio da pagare per azzerare i Cache Miss!
                    particles.vx[i] += acc_vx;
                    particles.vy[i] += acc_vy;
                    particles.vz[i] += acc_vz;
                }
            }
        }
    }
}

