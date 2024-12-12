#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>

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
int main(){
{
    int numThreads, numSteps;
    cout << "Podaj liczbe wątków: ";
    cin >> numThreads;
    cout << "Podaj liczbe kroków (np. 100000000): ";
    cin >> numSteps;

    double stepSize = 1.0 / numSteps; // Rozmiar kroku
    vector<thread> threads;          // Wątki
    vector<double> partialResults(numThreads, 0.0); // Wyniki częściowe

    auto startTime = chrono::high_resolution_clock::now(); // Start pomiaru czasu
}

// Podział pracy na wątki
int stepsPerThread = numSteps / numThreads;
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

// Wyświetlanie wyniku
cout << "Przyblizona liczba PI: " << pi << endl;
cout << "Czas obliczen: " << duration.count() << " sekund" << endl;

return 0;
}