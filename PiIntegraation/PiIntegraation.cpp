/**
 * @file pi_integral.cpp
 * @brief Obliczanie liczby PI metodą całkowania numerycznego przy użyciu wielowątkowości.
 *
 * Program implementuje metodę prostokątów do obliczania przybliżonej wartości liczby PI.
 * Obliczenia są równoległe, dzięki czemu można zwiększyć ich wydajność na maszynach wielordzeniowych.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

/**
 * @brief Funkcja obliczająca wartość funkcji f(x) = 4 / (1 + x^2).
 *
 * Funkcja ta jest podstawą do obliczania wartości liczby PI metodą całkowania
 * w przedziale [0, 1].
 *
 * @param x Punkt, w którym funkcja ma być obliczona.
 * @return Wartość funkcji f(x).
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Funkcja obliczająca wartość całki w zadanym przedziale.
 *
 * Funkcja używa metody prostokątów do przybliżenia wartości całki w przedziale
 * \p [start, end], dzieląc ten przedział na \p steps kroków.
 *
 * @param start Początek przedziału całkowania.
 * @param end Koniec przedziału całkowania.
 * @param steps Liczba kroków podziału w przedziale.
 * @param stepSize Rozmiar jednego kroku (delta x).
 * @param result Zmienna wynikowa, do której zostanie zapisany wynik obliczeń dla danego przedziału.
 */
void calculatePartialIntegral(double start, double end, long long steps, double stepSize, double& result) {
    double sum = 0.0; ///< Zmienna przechowująca sumę wyników funkcji w przedziale.
    for (long long i = 0; i < steps; ++i) {
        double x = start + i * stepSize + stepSize / 2.0; ///< Środek prostokąta w każdym kroku.
        sum += f(x) * stepSize; ///< Dodanie wartości funkcji w środku prostokąta pomnożonej przez rozmiar kroku.
    }
    result = sum; ///< Przypisanie wyniku całkowania w przedziale do zmiennej \p result.
}

/**
 * @brief Funkcja główna programu.
 *
 * Funkcja sterująca, która wykonuje obliczenia liczby PI metodą całkowania
 * numerycznego. Obsługuje wielowątkowość i zapisuje wyniki do pliku w formacie CSV.
 *
 * @return Zwraca 0, jeśli program zakończył się poprawnie.
 */
int main() {
    // Parametry testowe
    vector<long long> stepCounts = { 100000000, 1000000000, 3000000000 }; ///< Liczby podziałów (ilość kroków dla całkowania).
    int maxThreads = 50; ///< Maksymalna liczba wątków do testowania równoległych obliczeń.

    // Otwórz plik do zapisu wyników
    ofstream outputFile("results.csv"); ///< Obiekt do zapisu wyników w formacie CSV.
    if (!outputFile.is_open()) {
        cerr << "Nie można otworzyć pliku results.csv do zapisu." << endl;
        return 1;
    }
    outputFile << "Liczba krokow,Liczba watków,Czas (s),Przyblizona liczba PI\n"; ///< Nagłówek pliku CSV.

    // Iteracja przez liczby kroków
    for (long long steps : stepCounts) {
        double stepSize = 1.0 / static_cast<double>(steps); ///< Długość kroku całkowania.

        // Iteracja przez liczbę wątków
        for (int numThreads = 1; numThreads <= maxThreads; ++numThreads) {
            vector<thread> threads; ///< Wektor przechowujący obiekty wątków.
            vector<double> partialResults(numThreads, 0.0); ///< Wektor przechowujący wyniki obliczeń z poszczególnych wątków.

            auto startTime = chrono::high_resolution_clock::now(); ///< Rejestracja czasu rozpoczęcia obliczeń.

            // Podział pracy na wątki
            long long stepsPerThread = steps / numThreads; ///< Liczba kroków przypadających na każdy wątek.
            for (int i = 0; i < numThreads; ++i) {
                double start = i * stepsPerThread * stepSize; ///< Początek zakresu dla bieżącego wątku.
                double end = (i + 1) * stepsPerThread * stepSize; ///< Koniec zakresu dla bieżącego wątku.
                threads.emplace_back(calculatePartialIntegral, start, end, stepsPerThread, stepSize, ref(partialResults[i]));
            }

            // Czekanie na zakończenie wszystkich wątków
            for (auto& t : threads) {
                t.join();
            }

            // Sumowanie wyników częściowych
            double pi = 0.0;
            for (double result : partialResults) {
                pi += result;
            }

            auto endTime = chrono::high_resolution_clock::now(); ///< Zapisanie czasu zakończenia obliczeń.
            chrono::duration<double> duration = endTime - startTime; ///< Czas trwania obliczeń w sekundach.

            // Zapis wyników do pliku
            outputFile << steps << "," << numThreads << "," << duration.count() << "," << pi << "\n";

            // Wyświetlanie informacji na konsoli (opcjonalne)
            cout << "Liczba kroków: " << steps << ", Wątki: " << numThreads
                << ", Czas: " << duration.count() << "s, PI: " << pi << endl;
        }
    }

    outputFile.close(); ///< Zamknij plik wyników.
    cout << "Wyniki zapisane do pliku results.csv" << endl;

    return 0; ///< Zwraca 0, jeśli program zakończył się poprawnie.
}
