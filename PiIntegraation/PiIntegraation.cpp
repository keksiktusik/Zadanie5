#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

// Funkcja do obliczania wartości funkcji f(x) = 4 / (1 + x^2)
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Funkcja obliczająca całkę na zadanym przedziale
void calculatePartialIntegral(double start, double end, int steps, double stepSize, double& result) {
    double sum = 0.0;
    for (int i = 0; i < steps; ++i) {
        double x = start + i * stepSize + stepSize / 2.0; // Środek prostokąta
        sum += f(x) * stepSize;
    }
    result = sum;
}
int main()
{
    // Parametry testowe
    vector<long long> stepCounts = { 100000000, 1000000000, 3000000000 }; // Liczby podziałów
    int maxThreads = 50; // Maksymalna liczba wątków

    // Otwórz plik do zapisu wyników
    ofstream outputFile("results.csv");
    outputFile << "Liczba krokow,Liczba watków,Czas (s),Przyblizona liczba PI\n";

    // Iteracja przez liczby kroków
    for (long long steps : stepCounts) {
        double stepSize = 1.0 / steps;

        // Iteracja przez liczbę wątków
        for (int numThreads = 1; numThreads <= maxThreads; ++numThreads) {
            vector<thread> threads;
            vector<double> partialResults(numThreads, 0.0);

            auto startTime = chrono::high_resolution_clock::now(); // Start pomiaru czasu

            // Podział pracy na wątki
            long long stepsPerThread = steps / numThreads;
            for (int i = 0; i < numThreads; ++i) {
                double start = i * stepsPerThread * stepSize;
                double end = (i + 1) * stepsPerThread * stepSize;
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

            auto endTime = chrono::high_resolution_clock::now(); // Koniec pomiaru czasu
            chrono::duration<double> duration = endTime - startTime;

            // Zapis wyników do pliku
            outputFile << steps << "," << numThreads << "," << duration.count() << "," << pi << "\n";

            // Wyświetlanie informacji na konsoli (opcjonalne)
            cout << "Liczba kroków: " << steps << ", Wątki: " << numThreads
                << ", Czas: " << duration.count() << "s, PI: " << pi << endl;
        }
    }

    outputFile.close(); // Zamknij plik
    cout << "Wyniki zapisane do pliku results.csv" << endl;

    return 0;
}