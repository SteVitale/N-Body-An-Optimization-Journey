#include <random>
#include <vector>
#include "../../V0_baseline.h"

namespace V0 {
    // Genera Particelle
    std::vector<Particle> generateParticles(const int N) {
        // Pre-allochiamo la memoria esatta. Evita i push_back in un loop se sai già la dimensione!
        std::vector<Particle> particles(N);

        // Motore randomico standard (Mersenne Twister)
        // Usiamo un SEED FISSO (es. 42) così ogni run genera le stesse esatte posizioni
        std::mt19937 gen(42);

        // Distribuzioni uniformi per posizioni e velocità
        std::uniform_real_distribution<float> distPos(-1000.0f, 1000.0f); // Spazio 3D
        std::uniform_real_distribution<float> distVel(-1.0f, 1.0f);       // Velocità iniziali

        for (int i = 0; i < N; ++i) {
            particles[i].x = distPos(gen);
            particles[i].y = distPos(gen);
            particles[i].z = distPos(gen);

            particles[i].vx = distVel(gen);
            particles[i].vy = distVel(gen);
            particles[i].vz = distVel(gen);
        }

        return particles;
    }

    // Calcola Nbody
    void computeNbody(std::vector<Particle> &particles) {
        const int N = particles.size();

        //Per ogni particella 'i' nell'universo...
        for (int i = 0; i < N; ++i) {

            // ... guardo tutte le altre particelle 'j' ...
            for (int j = 0; j < N; ++j) {

                // Una particella non attrae se stessa!
                if (i == j) continue;

                // 1. Calcolo la distanza parziale tra la particella 'i' e la particella 'j'
                const float dx = particles[j].x - particles[i].x;
                const float dy = particles[j].y - particles[i].y;
                const float dz = particles[j].z - particles[i].z;

                // 2. Calcolo r (distanza totale) usando dx, dy, dz
                const float r = std::sqrt( dx*dx + dy*dy + dz*dz + eps);

                // 3. Calcolo Fmag (la forza specifica TRA la particella i e la particella j)
                const float Fmag = 1.0f / (r * r * r);

                // 4. Aggiungo questo singolo contributo alla velocità della particella 'i'
                particles[i].vx += dx * Fmag;
                particles[i].vy += dy * Fmag;
                particles[i].vz += dz * Fmag;
            }
        }
    }
}
