#include <random>
#include <vector>
#include "../../V2_StructOfArray.h"
/*
 *
 * Nella V1 (AoS): x, y, z sono attaccati in memoria (12 byte in totale). Quando la CPU chiede x,
 * la RAM le manda un'intera Cache Line da 64 byte che contiene già y e z pronti per essere usati.
 * Un singolo viaggio in RAM, 3 dati utili.
 *
 *
 * Nella V2 (SoA): I tuoi dati sono spalmati su 3 vettori diversi. Quando la CPU calcola dx, dy, dz,
 * deve mantenere attivi 3 flussi di lettura separati dalla RAM, saltando da un array all'altro.
 * Questo stressa il prefetcher hardware della CPU e causa continui Cache Misses.
 *
 *
 */

namespace V2 {

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

        //Per ogni particella 'i' nell'universo...
        for (int i = 0; i < N; ++i) {

            // ... guardo tutte le altre particelle 'j' ...
            for (int j = i+1; j < N; ++j) {

                // Una particella non attrae se stessa!
                //if (i == j) continue;

                // 1. Calcolo la distanza parziale tra la particella 'i' e la particella 'j'
                const float dx = particles.x[j] - particles.x[i];
                const float dy = particles.y[j] - particles.y[i];
                const float dz = particles.z[j] - particles.z[i];

                // 2. Calcolo r (distanza totale) usando dx, dy, dz
                const float r = std::sqrt( dx*dx + dy*dy + dz*dz + eps);

                // 3. Calcolo Fmag (la forza specifica TRA la particella i e la particella j)
                const float Fmag = 1.0f / (r * r * r);

                // 4.1 Aggiungo questo singolo contributo alla velocità della particella 'i'
                particles.vx[i] += dx * Fmag;
                particles.vy[i] += dy * Fmag;
                particles.vz[i] += dz * Fmag;

                // 4.2 Aggiorno la particella j (NewtonThirdLaw Applicata)
                particles.vx[j] -= dx * Fmag;
                particles.vy[j] -= dy * Fmag;
                particles.vz[j] -= dz * Fmag;
            }
        }
    }
}

