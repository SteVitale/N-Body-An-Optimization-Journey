# Direct N-Body Simulation: An Optimization Journey

Questo progetto esplora le tecniche di ottimizzazione in C++ (High Performance Computing) applicate al classico problema della simulazione a N-corpi (Direct N-Body). L'obiettivo non è la precisione fisica, ma misurare e abbattere i tempi di calcolo di un algoritmo computazionalmente intensivo sfruttando al massimo l'architettura della CPU (Cache, SIMD, Branch Predictor).

---

## 1. Descrizione del Problema
Il problema N-Body consiste nel calcolare le forze di interazione (gravitazionali o elettrostatiche) che ogni particella in un sistema esercita su tutte le altre. Nella versione "Direct", l'interazione viene calcolata esplicitamente per ogni singola coppia.



## 2. Il Calcolo Matematico
Per ogni coppia di particelle $i$ e $j$, calcoliamo le componenti della distanza:
$$dx = x_j - x_i$$
$$dy = y_j - y_i$$
$$dz = z_j - z_i$$

Calcoliamo poi la distanza totale al quadrato. Viene aggiunto un fattore di softening $\epsilon^2$ per evitare divisioni per zero nel caso in cui due particelle collidano o si sovrappongano:
$$r^2 = dx^2 + dy^2 + dz^2 + \epsilon^2$$

Ricaviamo la grandezza scalare della forza (incorporando le masse e la costante gravitazionale in un valore unitario per stressare solo il calcolo geometrico):
$$F_{mag} = \frac{1}{(r^2 \cdot \sqrt{r^2})}$$

Infine, aggiorniamo la velocità della particella $i$ lungo i tre assi, proiettando la forza vettorialmente:
$$v_{xi} = v_{xi} + dx \cdot F_{mag}$$
$$v_{yi} = v_{yi} + dy \cdot F_{mag}$$
$$v_{zi} = v_{zi} + dz \cdot F_{mag}$$

---

## 3. Perché la versione Naive è "Intrattabile"
Un'implementazione ingenua (Baseline) soffre di tre colli di bottiglia catastrofici:

* **Complessità Algoritmica $O(N^2)$:** Un sistema di sole 100.000 particelle richiede 10 miliardi di interazioni per un singolo passo temporale.
* **Inquinamento della Cache (AoS):** Utilizzare un classico Array of Structures (es. `struct Particle { float x, y, z, vx, vy, vz; }`) costringe la CPU a caricare in Cache dati non necessari simultaneamente, saturando la banda della RAM e causando continui Cache Miss.
* **Latenza Matematica:** Le operazioni di radice quadrata (`sqrt`) e divisione sono le istruzioni hardware più costose per l'ALU, richiedendo svariati cicli di clock rispetto a una semplice moltiplicazione.

---

## 4. Roadmap di Ottimizzazione
L'evoluzione del codice seguirà questi step rigorosi, misurando i tempi di esecuzione a ogni passaggio:

1. **V0 - Baseline (AoS):** Implementazione ingenua $O(N^2)$ in C++ standard.
2. **V1 - Newton's Third Law:** Sfruttamento dell'azione e reazione ($F_{ij} = -F_{ji}$) per dimezzare il numero di interazioni matematiche.
3. **V2 - Data-Oriented Design (SoA):** Ristrutturazione della memoria in Structure of Arrays per massimizzare i Cache Hit.
4. **V3 - Loop Hoisting:** Spostiamo tutto ciò che riguarda 'i' nei registri ultra-veloci della CPU (variabili locali), e aggiorniamo la memoria di i una volta sola alla fine.
5. **V4 - Math & Intrinsics:** Sostituzione di `sqrt` e divisioni con approssimazioni hardware rapide ed eliminazione delle sottoespressioni comuni (CSE).
6. **V5 - Auto-Vectorization (SIMD):** Preparazione dei loop per permettere al compilatore di usare istruzioni AVX/AVX2.
7. **V6 - Cache Blocking (Tiling):** Suddivisione del calcolo in blocchi dimensionati per rimanere all'interno della Cache L1/L2.

# Cronistoria delle Ottimizzazioni: Da Naive a HPC

Questo documento traccia l'evoluzione del nostro solver N-Body. Ogni versione applica una specifica tecnica di ottimizzazione (algoritmica, di memoria o matematica), dimostrando come la "simpatia meccanica" verso l'hardware possa abbattere drasticamente i tempi di esecuzione.

I test sono stati eseguiti su un sistema di $N = 100.000$ particelle.

---

### V0: La Baseline (Approccio Ingenuo)
* **Tecnica:** Nessuna. Doppio ciclo `for` annidato ($O(N^2)$), layout di memoria AoS (Array of Structures), matematica standard.
* **Problema:** La CPU fa calcoli ridondanti, la Cache L1 è inquinata da dati inutili (`vx, vy, vz` caricati insieme a `x, y, z`), e le istruzioni matematiche pesanti (`sqrt`, `/`) bloccano l'ALU.
* **Tempo misurato:** ~41.42 secondi.

---

### V1: Ottimizzazione Algoritmica (Terzo Principio di Newton)
* **Tecnica:** Sfruttamento dell'azione e reazione ($F_{ij} = -F_{ji}$). Il loop interno parte da `j = i + 1`, calcolando solo il "triangolo superiore" delle interazioni.
* **Risultato:** Le operazioni matematiche vengono letteralmente dimezzate.
* **Tempo misurato:** ~22.15 secondi (Speedup di ~2.1x).

---

### V2: Il Paradosso del Data-Oriented Design (SoA)
* **Tecnica:** Ristrutturazione della memoria in SoA (Structure of Arrays). I dati sono separati in array contigui di sole `x`, sole `y`, ecc.
* **Il Problema (Cache Trashing):** In modo controintuitivo, le prestazioni sono **peggiorate**. Perché? Nel codice scalare, calcolare le distanze su 3 array separati costringe la CPU a mantenere aperti 3 flussi di lettura dalla RAM simultaneamente. Inoltre, ad ogni singola iterazione di `j`, il programma continuava a leggere `x[i]` (che non cambia mai) e a scrivere in memoria su `vx[i]`, saturando la banda della RAM e causando continui *Cache Miss*.
* **Tempo misurato:** ~26.25 secondi (Rallentamento).

---

### V3: Loop Hoisting & Accumulatori (Il Fix per il SoA)
* **Tecnica:** Loop Invariant Code Motion (Hoisting) e utilizzo dei registri locali.
* **Risultato:** Abbiamo spostato la lettura delle coordinate della particella `i` fuori dal loop interno, salvandole nei velocissimi registri della CPU. Abbiamo inoltre creato degli accumulatori locali (`acc_vx`, ecc.) per sommare le forze internamente, scrivendo il risultato finale in memoria (sulla RAM lenta) **una sola volta** alla fine del ciclo.
* **Tempo misurato:** ~17.73 secondi (Abbiamo superato la V1 e sbloccato il vero potenziale del SoA).

---

### V4: Ottimizzazione Matematica e Intrinsics Hardware
* **Tecnica:** Sostituzione della divisione e della radice quadrata con l'Inverso della Radice Quadrata (`rsqrt`) moltiplicato per se stesso.
* **Dettaglio:** Le operazioni `std::sqrt` e `/` costano tra i 10 e i 15 cicli di clock ciascuna. Una moltiplicazione costa ~1 ciclo. Calcolando $invR = 1.0f / \sqrt{r^2}$ e ricavando la forza tramite $F_{mag} = invR \cdot invR \cdot invR$, eliminiamo del tutto le divisioni logiche nel loop interno.
* **Il ruolo del Compilatore:** Questa tecnica brilla solo sbloccando le approssimazioni hardware del compilatore tramite il flag `-ffast-math`. Senza di esso, il compilatore deve rispettare il rigoroso standard IEEE-754 e non utilizzerà le velocissime istruzioni assembly dedicate (`rsqrtps` su architetture x86).
* **Motivo del risultato inatteso:** L'ottimizzazione V4 (rsqrt) su CPU moderne può risultare controproducente rispetto alla V3. Le moderne ALU dispongono di unità di calcolo per sqrt talmente ottimizzate che l'overhead delle istruzioni di approssimazione introdotte da -ffast-math risulta maggiore del calcolo esatto. 
* **Tempo misurato:** ~20.8 secondi (Maggiore di V4)*

---

### V5: Il Trionfo dell'Hardware sull'Algoritmo (Auto-Vectorization SIMD)
* **Tecnica:** Vettorizzazione SIMD forzata tramite `#pragma omp simd` e ritorno alla complessità algoritmica $O(N^2)$.
* **Il Paradosso (Perché V1 bloccava V5):** L'ottimizzazione matematica della V1 (Terzo Principio di Newton) aveva introdotto un loop "triangolare" (`j = i + 1`) e scritture sparse in memoria (`p.vx[j] -= ...`). I compilatori odiano i loop irregolari e disabilitano le ottimizzazioni SIMD (AVX/AVX2) se notano scritture che possono creare dipendenze o corrompere i dati (Scatter Writes).
* **La Soluzione:** Per sbloccare i registri vettoriali a 256-bit (che calcolano 8 particelle per singolo ciclo di clock), abbiamo dovuto **rinunciare al Terzo Principio di Newton**. Siamo tornati a un doppio ciclo completo (`j = 0; j < N`), raddoppiando letteralmente le operazioni logiche da fare. Tuttavia:
    1. Il loop è tornato a essere un rettangolo perfetto, prevedibile per il compilatore.
    2. Abbiamo rimosso completamente i salti condizionali (Branchless Programming): non c'è nessun `if(i == j) continue;`. Quando `i == j`, la distanza `dx` è 0, e il contributo alla forza aggiunge un innocuo `+0` all'accumulatore.
    3. Il loop interno ora esegue *solo* letture contigue e somme su accumulatori locali (`reduction`). Nessuna scrittura in memoria lenta.
* **Risultato:** Anche se il programma esegue il doppio delle operazioni matematiche rispetto alla V3, l'hardware le esegue 8 volte più in fretta elaborando blocchi contigui di memoria.
* **Tempo misurato:** ~6.6 secondi. (Uno Speedup di **7x** rispetto alla baseline iniziale, eseguito interamente in Single-Thread).

---

### V6: Cache Blocking (Tiling) e Memory Tuning
* **Tecnica:** Suddivisione dello spazio di iterazione in blocchi (Loop Tiling) per mantenere i dati all'interno della Cache L1/L2 e mitigare il "Memory Wall".

* **Il Problema (In vista del Multi-Threading):** Anche se la V5 esegue i calcoli a velocità impressionante grazie al SIMD, gli array delle posizioni (`x, y, z`) per 100.000 particelle occupano circa 1.2 MB. Questo supera ampiamente la capacità della Cache L1 (tipicamente 32-64 KB per core). Ad ogni iterazione, la CPU deve sfrattare dati vecchi per caricare i nuovi, stressando il bus della RAM. Se attivassimo il multi-threading ora, i core finirebbero per litigare per la banda passante della memoria.
* **La Soluzione e il Tuning:** Abbiamo trasformato il doppio ciclo in un quadruplo ciclo, calcolando le interazioni tra "piastrelle" (Tiles) di particelle. Abbiamo eseguito un tuning empirico per trovare il *sweet spot* della dimensione del blocco (`BLOCK_SIZE`).
  * Con `BLOCK_SIZE = 256`, il programma era leggermente più lento (~6.3s) a causa dell'overhead eccessivo dei cicli e delle troppe scritture intermedie in memoria.
  * Con `BLOCK_SIZE = 4096`, abbiamo centrato l'obiettivo. 4096 `float` occupano circa 16 KB per array (totale ~48 KB per posizioni 3D), una dimensione che si incastra perfettamente tra la Cache L1 e la velocissima Cache L2 del processore, azzerando i viaggi inutili verso la RAM.
* **Risultato:** Abbiamo stabilizzato le performance annullando il collo di bottiglia della memoria. Il codice è ora strutturalmente pronto per essere distribuito su più core senza far collassare il memory bus.
* **Tempo misurato:** ~6.0 secondi.

---

### V7: Il Gran Finale - Multi-Threading con OpenMP
* **Tecnica:** Parallelizzazione Shared Memory su più core tramite direttive OpenMP (`#pragma omp parallel for schedule(static)`).
  [Image of multi-core CPU architecture memory hierarchy]
* **L'Architettura:** Fino alla V6, avevamo ottimizzato tutto il possibile per un **singolo core** (Memoria SoA, Registri Locali, Vettorizzazione SIMD e Tiling). Con la V7, abbiamo sguinzagliato tutti i core logici e fisici del processore.
* **Perché funziona perfettamente:** Abbiamo applicato la parallelizzazione sul **ciclo esterno** (quello delle particelle `i`). Questo garantisce che ogni thread (core) prenda in carico un pacchetto indipendente di particelle. Nessun core cercherà mai di scrivere sulla stessa cella di memoria (`p.vx[i]`) contemporaneamente a un altro core. Questo elimina alla radice il rischio di *Race Conditions* e la necessità di usare lenti meccanismi di sincronizzazione (lock/mutex).
* **La Sinergia Definitiva:** 1. Il ciclo esterno divide il carico sui core (Thread-Level Parallelism).
  2. Il ciclo interno di ogni core macina i dati a blocchi di 8 tramite AVX (Data-Level Parallelism).
  3. Il SoA e il Tiling garantiscono che la RAM riesca ad alimentare tutti i core senza intasare il bus di memoria.
* **Risultato Finale:** La barriera del secondo è stata infranta. Il tempo di calcolo scala quasi linearmente con il numero di core a disposizione, portando l'uso della CPU al 100%.

* **Tempo misurato:** ~0.96 secondi. (**Speedup finale rispetto alla Baseline V0: ~45x**).

---

## 🛠️ How to Build and Run

### Prerequisites
* **CMake** (version 3.10 or higher)
* A C++ compiler that supports **C++20** (GCC or Clang)
* **OpenMP** (for multithreading in V7)

### Build Instructions
This project uses CMake. To achieve the benchmarked execution times, it is **strictly required** to compile the project in `Release` mode. If compiled in `Debug` mode, the Auto-Vectorization (SIMD) and optimization flags (`-O3`) will be disabled, resulting in drastically slower execution.

Open your terminal, clone the repository, and run the following commands:

```bash
# 1. Clone the repository and enter the directory
git clone thisRepo
cd thisRepo

# 2. Create a build directory and enter it
mkdir build && cd build

# 3. Configure the project in Release mode
cmake -DCMAKE_BUILD_TYPE=Release ..

# 4. Compile the project using all available CPU cores
make -j $(nproc)
```

### Run Instructions
Once the compilation is complete, run the executable:
```bash
./NBodyOptimization
```
