#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

// ============================================================
// 1. STRUKTURA REZULTATAMS SAUGOTI
// ============================================================

// Cia saugomas palyginimu ir sukeitimu skaicius.
// Kiekvienam algoritmo paleidimui sukuriamas naujas Stats objektas.
struct Stats {
    long long comparisons = 0;
    long long swaps = 0;
};

// ============================================================
// 2. SHELL SORT ALGORITMAS
// ============================================================

// Selo metodas rikiuoja elementus naudodamas tarpus.
// Is pradziu tarpas didelis, veliau jis mazinamas iki 1.
void shellSort(vector<int>& data, Stats& stats) {
    int n = data.size();

    for (int gap = n / 2; gap > 0; gap = gap / 2) {
        for (int i = gap; i < n; i++) {
            int j = i;

            // Lyginami elementai, esantys per gap tarpa.
            while (j >= gap) {
                stats.comparisons++;

                // Jei elementai netinkamoje tvarkoje, jie sukeiciami.
                if (data[j - gap] > data[j]) {
                    swap(data[j - gap], data[j]);
                    stats.swaps++;
                    j = j - gap;
                } else {
                    break;
                }
            }
        }
    }
}

// ============================================================
// 3. HEAP SORT ALGORITMAS
// ============================================================

// Pagalbine funkcija, kuri atkuria max-heap savybe.
// Max-heap reiskia, kad didziausias elementas turi buti virsuje.
void heapify(vector<int>& data, int size, int root, Stats& stats) {
    int largest = root;
    int left = 2 * root + 1;
    int right = 2 * root + 2;

    // Tikrinamas kairysis vaikas.
    if (left < size) {
        stats.comparisons++;

        if (data[left] > data[largest]) {
            largest = left;
        }
    }

    // Tikrinamas desinysis vaikas.
    if (right < size) {
        stats.comparisons++;

        if (data[right] > data[largest]) {
            largest = right;
        }
    }

    // Jei didziausias elementas nera saknyje, atliekamas sukeitimas.
    if (largest != root) {
        swap(data[root], data[largest]);
        stats.swaps++;

        heapify(data, size, largest, stats);
    }
}

// Piramidinis rikiavimas.
// Pirma sudaroma piramide, tada didziausi elementai keliami i masyvo gala.
void heapSort(vector<int>& data, Stats& stats) {
    int n = data.size();

    // Sudaroma pradine max-heap piramide.
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(data, n, i, stats);
    }

    // Didziausias elementas perkeliamas i nesurikiuotos dalies gala.
    for (int i = n - 1; i > 0; i--) {
        swap(data[0], data[i]);
        stats.swaps++;

        heapify(data, i, 0, stats);
    }
}

// ============================================================
// 4. DUOMENU GENERAVIMAS
// ============================================================

// Sukuriamas pradinis masyvas pagal pasirinkta duomenu tipa:
// 1) nesurikiuoti
// 2) atvirksciai surikiuoti
// 3) jau surikiuoti
vector<int> generateData(int size, string type) {
    vector<int> data;

    for (int i = 1; i <= size; i++) {
        data.push_back(i);
    }

    if (type == "nesurikiuoti") {
        mt19937 generator(12345);
        shuffle(data.begin(), data.end(), generator);
    }

    if (type == "atvirksciai") {
        reverse(data.begin(), data.end());
    }

    return data;
}

// ============================================================
// 5. LAIKO MATAVIMAS
// ============================================================

// Matuojamas tik rikiavimo laikas.
// Duomenu generavimas, kopijavimas ir spausdinimas cia nepatenka.
long long measureTime(vector<int>& data, string algorithm, Stats& stats) {
    auto start = steady_clock::now();

    if (algorithm == "Shell sort") {
        shellSort(data, stats);
    } else {
        heapSort(data, stats);
    }

    auto end = steady_clock::now();

    return duration_cast<microseconds>(end - start).count();
}

// ============================================================
// 6. VIENO ALGORITMO TESTAVIMAS
// ============================================================

// Funkcija testuoja viena pasirinkta algoritma.
// Jai perduodami jau sugeneruoti pradiniai duomenys.
void testAlgorithm(const vector<int>& originalData, int size, string type, string algorithm) {
    const int repeats = 5;

    long long totalTime = 0;
    long long totalComparisons = 0;
    long long totalSwaps = 0;

    for (int i = 0; i < repeats; i++) {
        // Kiekvienam pakartojimui naudojama nauja tu paciu duomenu kopija.
        vector<int> data = originalData;
        Stats stats;

        long long time = measureTime(data, algorithm, stats);

        // Patikrinama, ar algoritmas tikrai surikiavo duomenis.
        if (!is_sorted(data.begin(), data.end())) {
            cout << "Klaida: duomenys nesurikiuoti." << endl;
            return;
        }

        totalTime += time;
        totalComparisons += stats.comparisons;
        totalSwaps += stats.swaps;
    }

    // Isvedami 5 pakartojimu vidurkiai.
    cout << size << "\t"
         << type << "\t"
         << algorithm << "\t"
         << totalTime / repeats << "\t"
         << totalComparisons / repeats << "\t"
         << totalSwaps / repeats << endl;
}

// ============================================================
// 7. ABIEJU ALGORITMU PALYGINIMAS SU TAIS PACIAIS DUOMENIMIS
// ============================================================

// Cia sugeneruojamas vienas pradinis duomenu rinkinys.
// Tada tas pats rinkinys perduodamas Shell sort ir Heap sort algoritmams.
void runComparison(int size, string type) {
    vector<int> originalData = generateData(size, type);

    testAlgorithm(originalData, size, type, "Shell sort");
    testAlgorithm(originalData, size, type, "Heap sort");
}

// ============================================================
// 8. PAGRINDINE PROGRAMOS DALIS
// ============================================================

int main() {
    vector<int> sizes = {5000, 10000, 50000};

    vector<string> types = {
        "nesurikiuoti",
        "atvirksciai",
        "surikiuoti"
    };

    cout << "Dydis\tTipas\tAlgoritmas\tLaikas_us\tPalyginimai\tSukeitimai" << endl;

    // Paleidziami testai su visais dydziais ir visais duomenu tipais.
    for (int size : sizes) {
        for (string type : types) {
            runComparison(size, type);
        }
    }

    return 0;
}
