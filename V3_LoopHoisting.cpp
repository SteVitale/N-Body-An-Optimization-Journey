#include <random>
#include <vector>
#include "V3_LoopHoisting.h"
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
 * SOLUZIONE:
 * Applichiamo una tecnica chiamata Loop Invariant Code Motion (Hoisting) combinata con gli Accumulatori Locali.
 * Spostiamo tutto ciò che riguarda i nei registri ultra-veloci della CPU (variabili locali),
 * e aggiorniamo la memoria di i una volta sola alla fine.
 *
 */

namespace V3 {

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
    void computeNbody(Particles &p) {
        const int N = p.x.size();

        for (int i = 0; i < N; ++i) {

            // 1. HOISTING: Leggo le posizioni di 'i' UNA SOLA VOLTA dalla RAM
            // e le metto nei registri veloci della CPU.
            const float xi = p.x[i];
            const float yi = p.y[i];
            const float zi = p.z[i];

            // 2. ACCUMULATORI: Non scrivo in memoria p.vx[i] ad ogni iterazione!
            // Accumulo le forze in variabili locali veloci.
            float acc_vx = 0.0f;
            float acc_vy = 0.0f;
            float acc_vz = 0.0f;

            for (int j = i + 1; j < N; ++j) {

                // Uso xi, yi, zi (velocissimi, sono nei registri!)
                const float dx = p.x[j] - xi;
                const float dy = p.y[j] - yi;
                const float dz = p.z[j] - zi;

                const float r = std::sqrt(dx*dx + dy*dy + dz*dz + eps);
                const float Fmag = 1.0f / (r * r * r);

                // Aggiorno gli accumulatori locali (velocissimo)
                acc_vx += dx * Fmag;
                acc_vy += dy * Fmag;
                acc_vz += dz * Fmag;

                // La particella j la devo per forza scrivere in memoria
                // perché cambia ad ogni iterazione (Scatter Write)
                p.vx[j] -= dx * Fmag;
                p.vy[j] -= dy * Fmag;
                p.vz[j] -= dz * Fmag;
            }

            // 3. Scrivo il risultato accumulato nella memoria di 'i' UNA SOLA VOLTA!
            p.vx[i] += acc_vx;
            p.vy[i] += acc_vy;
            p.vz[i] += acc_vz;
        }
    }
}

